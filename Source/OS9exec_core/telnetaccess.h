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
/*         for Apple Macintosh und PC         */
/*                                            */
/* (c) 1993-2002 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        beat.forster@ggaweb.ch              */
/**********************************************/


/*      Telnet/TTY interface Routines
**      by Beat Forster (beat.forster@ggaweb.ch)
*/

void               InitTTYs(void );
void  CheckInputBuffersTTY( void );

short           InstallTTY       ( syspath_typ*, int consoleID );
void             RemoveTTY                     ( int consoleID );
long      ReadCharsFromPTY( char *buffer,long n, int consoleID );
long       WriteCharsToPTY( char *buffer,long n, int consoleID, Boolean do_lf );
Boolean        DevReadyTTY( long *count,         int consoleID );

void           HandleEvent( void );
void     CheckInputBuffers( void );
void           WindowTitle( char *title, Boolean vmod );
long ReadCharsFromTerminal( char *buffer,long n, ttydev_typ* mco );
Boolean   DevReadyTerminal( long *count,         ttydev_typ* mco );

/* eof */

