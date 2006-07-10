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
 *    Revision 1.6  2006/02/19 16:28:56  bfo
 *    'Is_PtoC' visible from ouside / PtoC + thread support added
 *
 *    Revision 1.5  2004/11/20 11:44:07  bfo
 *    Changed to version V3.25 (titles adapted)
 *
 *    Revision 1.4  2003/05/17 11:25:21  bfo
 *    (CVS header included)
 *
 *
 */


/* OS9exec/nt internal commands */
#ifndef __intcommand_h
#define __intcommand_h

/* public vars */
extern ushort icmpid; /* current internal command's PID */

/* internal commands (in separate files) */
os9err int_dir   ( ushort pid, int argc, char **argv );
os9err int_rename( ushort pid, int argc, char **argv );
os9err int_move  ( ushort pid, int argc, char **argv );
os9err int_help  ( ushort pid, int argc, char **argv );

#ifdef windows32
os9err int_wincmd( ushort pid, int argc, char **argv );
#endif


/* utility */
os9err  _errmsg( os9err err, char* format, ...);
Boolean Is_PtoC     ( char* name );
int     isintcommand( char* name );
os9err  prepArgs    ( char* arglist, ushort *argcP, char*** argP);
os9err  callcommand ( char* name,    ushort pid, ushort parentid,
                                                 int argc,     char** argv, Boolean* asThread );
os9err  call_hostcmd( char* cmdline, ushort pid, int moreargs, char** argv );

#endif

/* eof */
