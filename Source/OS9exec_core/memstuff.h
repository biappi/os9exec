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



/* Memory management for OS9 processes */
void show_mem(ushort pid);
void init_mem(ushort pid);
void free_mem(ushort pid);

ushort install_memblock(ushort pid, void *base, ulong size);
void   release_memblock(ushort pid, ushort memblocknum);

ulong    max_mem( void );
void    *get_mem( ulong memsz,   Boolean mac_asHandle );
void release_mem( void* membase, Boolean mac_asHandle );

void *os9malloc(ushort pid, ulong memsz);
os9err  os9free(ushort pid, void* membase, ulong memsz);

/* eof */

