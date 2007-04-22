// 
//    OS9exec,   OS-9 emulator for Mac OS, Windows and Linux 
//    Copyright (C) 2002 Lukas Zeller / Beat Forster
//	  Available under http://www.synthesis.ch/os9exec
// 
//    This program is free software; you can redistribute it and/or 
//    modify it under the terms of the GNU General Public License as 
//    published by the Free Software Foundation; either version 2 of 
//    the License, or (at your option) any later version. 
// 
//    This program is distributed in the hope that it will be useful, 
//    but WITHOUT ANY WARRANTY; without even the implied warranty of 
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//    See the GNU General Public License for more details. 
// 
//    You should have received a copy of the GNU General Public License 
//    along with this program; if not, write to the Free Software 
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
//

/**********************************************/
/*             O S 9 E x e c / NT             */
/*  Cooperative-Multiprocess OS-9 emulation   */
/*         for Apple Macintosh and PC         */
/*                                            */
/* (c) 1993-2007 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        bfo@synthesis.ch                    */
/**********************************************/

/*
 *  CVS:
 *    $Author$
 *    $Date$
 *    $Revision$
 *    $Source$
 *    $State$
 *    $Name$ (Tag)
 *    $Locker$ (who has reserved checkout)
 *  Log:
 *    $Log$
 *    Revision 1.8  2007/04/19 21:00:56  bfo
 *    Version changed to V3.38
 *
 *    Revision 1.7  2007/03/11 17:17:37  bfo
 *    Version changed to V3.37
 *
 *    Revision 1.6  2007/02/22 22:58:28  bfo
 *    Version changed to V3.36
 *
 *    Revision 1.5  2007/01/29 23:19:36  bfo
 *    import "Resource.h"
 *
 *    Revision 1.4  2007/01/29 23:12:08  bfo
 *    trying to make it clean for MPW (again)
 *
 *    Revision 1.3  2007/01/29 22:48:29  bfo
 *    'Handle' def imported
 *
 *    Revision 1.2  2007/01/28 21:47:57  bfo
 *    - __GLOBDEF added / Boolean => int /
 *    - 'nativeinfo_typ' added
 *    - getversion/lVersion implemented here now
 *    - 'ustrcmp' implemented here now
 *
 *    Revision 1.1  2007/01/07 13:25:33  bfo
 *    *** NEW MODULE ADDED ***
 *
 *
 */


// Don't import all other things ...
#if defined macintosh && !defined __MACH__
  #define MACOS9
  #include <ToolUtils.h>
  #include <Resources.h>
#endif

#include "c_access.h"


// the callback structure reference
callback_typ* cbP;


// obtain os9exec version
void getversion( unsigned short *ver,
                 unsigned short *rev )
{
  #ifdef MACOS9
    Handle versH;
  #endif
	
  *ver= 0; 
  *rev= 0;
	
  #ifdef MACOS9
    // obtain os9exec version
        versH= GetResource( 'vers',2 );
    if (versH!=NULL) {
      *ver= *( (byte*)*versH+0 );
      *rev= *( (byte*)*versH+1 );
      ReleaseResource( versH );
    } // if
  #else
    // simply hardwired for all other platforms
    *ver=    3;
    *rev= 0x39; /* V3.39 */
  #endif
} // getversion


// obtain os9exec version in long format
long lVersion()
{
  unsigned short ver,  rev;
  getversion  ( &ver, &rev );
  return   ( 256*ver + rev )*256*256;
} // lVersion



// -------------------------------------------------------------------------------------------------------
// copy the block with <size> form <src> to <dst
// forward and backward mode supported (in case of overlapping structures
void MoveBlk( void* dst, void* src, unsigned long size )
{
  unsigned long  n;
  unsigned char* s;
  unsigned char* d;
    
  if (src>=dst) {    // condition for forward/backward copy
    s= (unsigned char*)src;   // normal forward copy
    d= (unsigned char*)dst;
    for ( n=0; n<size; n++ ) { *d= *s; s++; d++; }
  }
  else {             // reverse ordered copy 
    s= (unsigned char*)( (unsigned long)src + size-1 );
    d= (unsigned char*)( (unsigned long)dst + size-1 );
    for ( n=0; n<size; n++ ) { *d= *s; s--; d--; }
  } // if
} // MoveBlk



// case insensitive version of strcmp
//  Input  : <s1>, <s2> = strings to be compared
//  Result : -1: s1<s2,
//            0: s1=s2,
//            1: s1>s2
int ustrcmp( const char *s1,const char *s2 )
{
  int  diff;
  char c;

  do {
                   c=           *(s1++);
    diff= toupper( c )-toupper( *(s2++) );

    if (diff!=0) return diff>0 ? 1 : -1;
  } while ( c!='\0' );

  return 0; // strings are equal
} // ustrcmp



// -------------------------------------------------------------------------------------------------------
// missing "sprintf" operations for "cclib", temporary placed here
#define MAXINTEGER 32767


void IntToStr( char* s, int i ) {
  sprintf( s, "%d", i );
} // IntToStr



void IntToStrN( char* s, int i, int n )
{
  char form[ 20 ]; // format string
  
  if (n==MAXINTEGER) { IntToStr( s, i ); return; }

  sprintf   ( form, "%s%dd", "%", n );
  sprintf( s, form, i );
} // IntToStrN



void UIntToStr( char* s, unsigned int i ) {
  sprintf( s, "%X", i );
} // UIntToStr



void UIntToStrN( char* s, unsigned int i, int n )
{
  char form[ 20 ]; // format string
  int  r= 0;
  
  if (n==MAXINTEGER) { UIntToStr( s, i ); return; }
  
  if (n>8) { r= n-8; n= 8; } // keep spaces for the remaining part of > 8 
  sprintf   ( form, "%s%ds%s0%dX", "%", r, "%", n );
  sprintf( s, form, "", i );
} // UIntToStrN



void BoolToStr ( char* s, int bo ) {
  sprintf( s, "%s", bo ? "TRUE":"FALSE" );
} // BoolToStr



void BoolToStrN( char* s, int bo, int n )
{
  char form[ 20 ]; // format string
  
  if (n==MAXINTEGER) { BoolToStr( s, bo ); return; }

  sprintf   ( form, "%s%ds", "%", n );
  sprintf( s, form, bo ? "TRUE":"FALSE" );
} // BoolToStrN



void RealToStr( char* s, double d, int res )
{
  char form[ 20 ]; // format string
  
  switch (res) {
    case MAXINTEGER: sprintf( form, "%sE",    "%" ); break;
    case 0         : sprintf( form, "%s.0f.", "%" ); break;
    default        : sprintf( form, "%s.%df", "%", res );
  } // switch
  
  sprintf( s, form, d );
} // RealToStr



void RealToStrN( char* s, double d, int n, int res )
{
  char form[ 20 ]; // format string
  
  switch (res) {
    case MAXINTEGER: sprintf( form, "%s%dE",    "%", n ); break;
    case 0         : sprintf( form, "%s%d.0f.", "%", n ); break;
    default        : sprintf( form, "%s%d.%df", "%", n, res );
  } // switch
  
  sprintf( s, form, d );
} // RealToStr


/* eof */
