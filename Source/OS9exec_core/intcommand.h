// 
// Ê ÊOS9exec,   OS-9 emulator for Mac OS, Windows and Linux 
// Ê ÊCopyright (C) 2002 Ê[ Lukas Zeller / Beat Forster ]
//	  Available under http://www.synthesis.ch/os9exec
// 
// Ê ÊThis program is free software; you can redistribute it and/or 
// Ê Êmodify it under the terms of the GNU General Public License as 
// Ê Êpublished by the Free Software Foundation; either version 2 of 
// Ê Êthe License, or (at your option) any later version. 
// 
// Ê ÊThis program is distributed in the hope that it will be useful, 
// Ê Êbut WITHOUT ANY WARRANTY; without even the implied warranty of 
// Ê ÊMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// Ê ÊSee the GNU General Public License for more details. 
// 
// Ê ÊYou should have received a copy of the GNU General Public License 
// Ê Êalong with this program; if not, write to the Free Software 
// Ê ÊFoundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
//

/**********************************************/
/*             O S 9 E x e c / NT             */
/*  Cooperative-Multiprocess OS-9 emulation   */
/*         for Apple Macintosh and PC         */
/*                                            */
/* (c) 1993-2002 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        forsterb@dial.eunet.ch              */
/**********************************************/



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
os9err _errmsg( os9err err, char* format, ...);
int    isintcommand( char *name );
os9err prepArgs    ( char *arglist, ushort *argcP, char*** argP);
os9err callcommand ( char *name,    ushort pid, int argc,     char **argv );
os9err call_hostcmd( char *cmdline, ushort pid, int moreargs, char **argv );

#endif

/* eof */