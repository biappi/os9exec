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
 *
 */

#include "os9exec_incl.h"
#define MAXINTEGER 32767

// current process id reference
ushort* curidP;

// the callback structure reference
callback_typ* cbP;


void MoveBlk( void* dst, void* src, ulong size )
// copy the block with <size> form <src> to <dst
// forward and backward mode supported (in case of overlapping structures
{
  ulong n;
  byte* s;
  byte* d;
    
  if (src>=dst) {    // condition for forward/backward copy
    s= (byte*)src;   // normal forward copy
    d= (byte*)dst;
    for ( n=0; n<size; n++ ) { *d= *s; s++; d++; }
  }
  else {             // reverse ordered copy 
    s= (byte*)( (ulong)src + size-1 );
    d= (byte*)( (ulong)dst + size-1 );
    for ( n=0; n<size; n++ ) { *d= *s; s--; d--; }
  } // if
} // MoveBlk



// -------------------------------------------------------------------------------------------------------
static Boolean IsZeroI( const char* s )
{
  if    (*s==NUL) return false;
  while (*s!=NUL) {
    if  (*s <'0' && *s >'9' &&
         *s!='-' && *s!='+') return false;
    s++;
  } // while
  
  return true;
} // IsZeroI


Boolean StrToShort( short* i, const char* s )
{
  *i= atoi( s );
  return *i==0 && !IsZeroI( s );
} // StrToShort



// -------------------------------------------------------------------------------------------------------
// missing "sprintf" operations for "cclib", temporary placed here
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


void BoolToStr ( char* s, Boolean bo ) {
  sprintf( s, "%s", bo ? "TRUE":"FALSE" );
} // BoolToStr


void BoolToStrN( char* s, Boolean bo, int n )
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
