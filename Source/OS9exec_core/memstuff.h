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
/* (c) 1993-2006 by Lukas Zeller, CH-Zuerich  */
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
 *    Revision 1.10  2006/03/12 19:31:22  bfo
 *    RealToStrN added
 *
 *    Revision 1.9  2006/02/19 16:33:20  bfo
 *    Some PtoC routines are implemented here now
 *
 *    Revision 1.8  2004/11/20 11:44:07  bfo
 *    Changed to version V3.25 (titles adapted)
 *
 *    Revision 1.7  2003/05/21 20:31:39  bfo
 *    Allocate 512k blocks / Additional parameter <mem_fulldisp> for "show_mem"
 *
 *    Revision 1.6  2003/05/17 10:38:13  bfo
 *    'show_mem' with <mem_unused> parameter
 *
 *    Revision 1.5  2002/10/27 23:12:39  bfo
 *    get_mem/release_mem no longer with param <mac_asHandle>
 *
 *
 */

#if defined __cplusplus && defined __GNUC__
  typedef short Boolean;
  #define true  1
  #define false 0
#endif


#if !defined __cplusplus 
  /* Memory management for OS9 processes */
  void init_all_mem(void);

  void show_mem( ushort pid, Boolean mem_unused, Boolean mem_fulldisp );
  void show_unused   ( void );

  void init_mem( ushort pid );
  void free_mem( ushort pid );

  ushort install_memblock( ushort pid, void* base, ulong size );
  void   release_memblock( ushort pid, ushort memblocknum );

  ulong    max_mem( void );
  void*    get_mem( ulong memsz   );
  void release_mem( void* membase );
#endif


// ----------------------------------------------------------------
#if defined __cplusplus
  extern "C" {
#endif

void* os9malloc( ushort pid,                ulong memsz );
os9err  os9free( ushort pid, void* membase, ulong memsz );


// missing "atof" operation for "cclib", temporary placed here
Boolean StrToReal    ( float*  f, const char* s );
Boolean StrToLongReal( double* f, const char* s );

// missing "sprintf" operations for "cclib", temporary placed here
void IntToStr  ( char* s, int    i );
void IntToStrN ( char* s, int    i, int n );

void UIntToStr ( char* s, unsigned int h );
void UIntToStrN( char* s, unsigned int h, int n );

void BoolToStr ( char* s, Boolean bo );
void BoolToStrN( char* s, Boolean bo, int n );

void RealToStr ( char* s, double d,        int res );
void RealToStrN( char* s, double d, int n, int res );

/*
void RealToStrN( char* s, double d, int n, int res );
*/

#if defined __cplusplus
  } // end extern "C"
#endif

/* eof */


