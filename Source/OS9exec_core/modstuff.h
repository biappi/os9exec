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
/* (c) 1993-2002 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        forsterb@dial.eunet.ch              */
/**********************************************/



/* Memory modules */

mod_exec* os9mod      ( int k );
char*     Mod_Name    ( mod_exec* mod );

void      MoveBlk     ( byte *dst, byte *src, ulong size );
Boolean   SameBlk     ( byte *a,   byte *b,   ulong size );
ulong     DatMod_Size ( ulong namsize, ulong datsize );
void      FillTemplate( mod_exec* m, short access, short tylan, short attrev );

int   NextFreeModuleId( char* name );
void      show_modules( char* cmp  );
void      init_modules( void );
void   release_module ( ushort mid, Boolean modOK );

mod_exec*  get_module_ptr( int   mid     );
int        get_mid       ( void *modptr );

int       find_mod_id            ( const char* name );
int       link_mod_id                  ( char* name );
os9err    load_module( ushort pid,       char* name, ushort *midP, Boolean exedir, Boolean linkstyle );
os9err    link_module( ushort pid, const char* name, ushort *midP );
os9err    link_load  ( ushort pid,       char* name, ushort *midP );
void    unlink_module( ushort mid );
void      free_modules();

void init_exceptions  ( ushort pid );
void init_traphandlers( ushort pid );

ushort calc_parity( ushort*   p, ushort numwords );
ulong  calc_crc   ( byte*     p, ulong size, ulong accum );
void    mod_crc   ( mod_exec* m );

os9err prepData(ushort pid, mod_exec* theModule, ulong memplus, ulong *msiz, byte **mp);

os9err install_traphandler(ushort pid, ushort trapidx, char *mpath, ulong addmem, traphandler_typ **traphandler);
os9err release_traphandler(ushort pid, ushort trapidx);
void   unlink_traphandlers(ushort pid);

/* eof */
