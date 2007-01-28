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


#include <stdio.h>
#include "native_interface.h"
#include "c_access.h"

#ifdef linux
#define NULL 0
typedef int size_t;
#endif

#define NativeName "hello_world"

#define F_Chain  0x05
#define F_Exit   0x06
#define I_WritLn 0x8C

#define E_MNF     221



// calling interface for a std C program
int main( void );



// --------------------------------------------------------------
#if defined USE_CARBON || defined __MACH__ || defined linux
  // --- temporary implemented locally, because of linking problems
  static size_t strlen( const char* s )
  {
    int       i= 0;
    while (s[ i ]!='\0') i++;
    return    i;
  } // strlen


  static void strcpy( char* s1, const char* s2 )
  {
    for  (int i= 0;; i++) {
          s1[ i ]= s2[ i ];
      if (s1[ i ]=='\0') break;
    } // for
  } // strcpy
#endif



// --------------------------------------------------------------
// The Trap0 callback
short OS9_Call( ushort code, Regs_68k &regs )
{ 
  os9err err= cbP->trap0( code, &regs );
  
  // for internal utilities, F$Exit and F$Chain finish here !!
  if (code==F_Exit ||
      code==F_Chain) {
    throw (ushort)regs.d[ 1 ];
  } // if

  return err;
} /* OS9_Call */



// ---------------------------------------------------------
// Very simple "printf" implementation, w/o any additional param parsing
int os9_printf( const char* str, ... )
{
  #define  StrLen 80
  int      i;
  char     s[ StrLen ];
  Regs_68k regs;
  
  // copy the string and replace LF -> CR
  for (i= 0; i<StrLen; i++) {
        s[ i ]= str[ i ];
    if (s[ i ]==0x0a) 
        s[ i ]= 0x0d;
    if (s[ i ]=='\0') break;
  } // for
           
  regs.d[ 0 ]= 0; // stdout
  regs.a[ 0 ]= (ulong) s;
  regs.d[ 1 ]= strlen( s );

  OS9_Call( I_WritLn, regs );
  return 0;
} // os9_printf


// exit the native OS-9 program
void os9_exit( ushort err )
{
  Regs_68k regs;
  
  regs.d[ 1 ]= err;
  OS9_Call( F_Exit, regs );
} // os9_exit

    
  
// ---------------------------------------------------------
#ifdef MACOSX
#pragma export on
#endif

// The plugin module's version
long Module_Version( void ) { return lVersion(); }



// Get the next native prog
int Next_NativeProg( int* i, char* progName, char* callMode )
{
  if (*i>0) return false; // only one program
     (*i)++;
     
  strcpy( progName, NativeName );
  strcpy( callMode, ""         ); 
  return true;
} // Next_NativeProg



// Check, if <progName> is a native program; bool result
int Is_NativeProg( const char* progName, void** modBase )
{
  const char*       p= progName;
  const char* pure= p + strlen( p ) - 1;
  
  while (pure>p) {  // get the pure file name from eventual abs path name
    if (*pure=='/') { pure++; break; }
         pure--;
  } // while
  
  *modBase= NULL; // there is no assigned module
  return ustrcmp( pure, NativeName )==0;
} // Is_NativeProg



os9err Start_NativeProg( const char* progName, void* nativeInfo )
{
  os9err err= 0;
  void*  modBase;
  
  nativeinfo_typ* ni= (nativeinfo_typ*)nativeInfo;
  cbP=            ni->cbP; // get the callback structure
    
  if (!Is_NativeProg( progName, &modBase )) return E_MNF;

  try {
    err= main();
    os9_exit( err );
  }
  catch( ushort exiterr ) { // the only way to come back from F$Exit
           err= exiterr;
  }
  
  return err;
} // Call_NativeProg


#ifdef MACOSX
#pragma export off
#endif

/* eof */
