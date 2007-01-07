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


// ----- C access from PtoC/Plugin side and OS9exec internal use -----
/* OS9 error code */
typedef ushort os9err;

#if defined __cplusplus && defined __GNUC__
  typedef short Boolean;
  #define true  1
  #define false 0
#endif


#if defined __cplusplus
  extern "C" {
#endif

// current process id reference
extern  ushort* curidP;


// The callback function for system calls 
typedef os9err  (*Trap0_Call_Typ )( ushort code, void* rp );
typedef Boolean (*StrToFloat_Typ )( float*  f, const char* s );
typedef Boolean (*StrToDouble_Typ)( double* f, const char* s );

// The callback structure
typedef struct {
  Trap0_Call_Typ  trap0;
  StrToFloat_Typ  strToFloat; 
  StrToDouble_Typ strToDouble;  
} callback_typ;

// the callback structure reference
extern callback_typ* cbP;


// -------------------------------------------------------------------
// Memory block copy
void MoveBlk( void* dst, void* src, ulong size  );


// -------------------------------------------------------------------
// missing "atoi"    operations for "cclib", temporary placed here
Boolean StrToShort( short*  i, const char* s );


// missing "sprintf" operations for "cclib", temporary placed here
void IntToStr  ( char* s, int    i );
void IntToStrN ( char* s, int    i, int n );

void UIntToStr ( char* s, unsigned int h );
void UIntToStrN( char* s, unsigned int h, int n );

void BoolToStr ( char* s, Boolean bo );
void BoolToStrN( char* s, Boolean bo, int n );

void RealToStr ( char* s, double d,        int res );
void RealToStrN( char* s, double d, int n, int res );


#if defined __cplusplus
  } // end extern "C"
#endif

/* eof */


