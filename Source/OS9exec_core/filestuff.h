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



/* main initialisation routine */
void init_fmgrs();

/* I/O routines */
void   show_files        ( ushort pid );
os9err parsepathext      ( ushort pid, char** inp, char* out, Boolean exedir, Boolean *ispath );
os9err parsepath         ( ushort pid, char** inp, char* out, Boolean exedir );

/* Generic */
os9err pUnimp            ( ushort pid, syspath_typ* );
os9err pUnimpOk          ( ushort pid, syspath_typ* );
os9err pBadMode          ( ushort pid, syspath_typ* );
os9err pNop              ( ushort pid, syspath_typ* );
os9err pNoModule         ( ushort pid, syspath_typ* );

/* SCF-like */
os9err pSCFnam           ( ushort pid, syspath_typ*, char* volname );


/* unified I/O */
os9err syspath_new                   ( ushort *sp, ptype_typ type );
os9err usrpath_new       ( ushort pid, ushort *up, ptype_typ type );
os9err syspath_open      ( ushort pid, ushort *sp, ptype_typ type, const char* pathname, ushort mode );
os9err usrpath_open      ( ushort pid, ushort *up, ptype_typ type, const char* pathname, ushort mode );
os9err syspath_close     ( ushort pid, ushort  sp );
os9err usrpath_close     ( ushort pid, ushort  up );
os9err usrpath_link      ( ushort pid, ushort  sp, const char* ident );
os9err syspath_write     ( ushort pid, ushort  sp, ulong *len, void* buffer, Boolean wrln );
os9err usrpath_write     ( ushort pid, ushort  up, ulong *len, void* buffer, Boolean wrln );

void usrpath_printf      ( ushort pid, ushort  up, const char* format, ... );
void     upo_printf                              ( const char* format, ... );
void    upho_printf                              ( const char* format, ... );
void     upe_printf                              ( const char* format, ... );
void    uphe_printf                              ( const char* format, ... );
void    main_printf                              ( const char* format, ... );
void copyright();

os9err syspath_read      ( ushort pid, ushort  sp, ulong *len, void* buffer, Boolean rdln );
os9err usrpath_read      ( ushort pid, ushort  up, ulong *len, void* buffer, Boolean rdln );
os9err syspath_seek      ( ushort pid, ushort  sp, ulong  pos );
os9err usrpath_seek      ( ushort pid, ushort  up, ulong  pos );

os9err syspath_getstat   ( ushort pid, ushort  sp, ushort func, ulong *a0,
                                                                ulong *d0,ulong *d1,ulong *d2,ulong *d3 );
os9err syspath_gs_size   ( ushort pid, ushort  sp, ulong *size );
os9err syspath_gs_devnm  ( ushort pid, ushort  sp, char*  name );
os9err syspath_gs_ready  ( ushort pid, ushort  sp, ulong *cnt  );

os9err usrpath_getstat   ( ushort pid, ushort  up, ushort func, ulong *a0,ulong * 
                                                                       d0,ulong *d1,ulong *d2,ulong *d3 );

os9err syspath_setstat   ( ushort pid, ushort  sp, ushort func, ulong *a0,ulong *a1,
                                                                ulong *d0,ulong *d1,ulong *d2,ulong *d3 );
os9err usrpath_setstat   ( ushort pid, ushort  up, ushort func, ulong *a0,ulong *a1,
                                                                ulong *d0,ulong *d1,ulong *d2,ulong *d3 );


os9err get_locations     ( ushort pid, ptype_typ type, const char* pathname, Boolean doCreate, Boolean *asDir, 
                                                                        ulong *fdP, ulong *dfdP, ulong *dcpP );
os9err change_dir        ( ushort pid, ptype_typ type, const char* pathname, ushort mode );
os9err delete_file       ( ushort pid, ptype_typ type, const char* pathname, ushort mode );
os9err make_dir          ( ushort pid, ptype_typ type, const char* pathname, ushort mode );


void init_syspaths       ( void );
void init_usrpaths       ( ushort pid );
char* fType              ( syspath_typ*, char* field );
syspath_typ* get_syspath ( ushort pid, ushort  sp);
syspath_typ* get_syspathd( ushort pid, ushort  sp);
void close_usrpaths      ( ushort pid );
void close_syspaths      ( void );




/* external consio support routines */
Boolean ConsGetc        ( char *c );
void    ConsPutc        ( char  c );
void    ConsPutcEdit    ( char  c, Boolean alf, char eorch );


/* external RBF    support routines */
os9err  MountDev        ( ushort pid, char* name, char* mnt_dev, 
                          short  adapt, ushort scsibus, 
                          short scsiID, ushort scsiLUN, 
                          int  ramSize, Boolean wProtect );
                             
os9err  int_mount       ( ushort pid, int argc, char **argv );
os9err  int_unmount     ( ushort pid, int argc, char **argv );
void    Disp_RBF_Devs   ( Boolean statistic );


/* external pipe   support routines */
os9err           getPipe( ushort pid, syspath_typ*, ulong buffsize );
os9err       releasePipe( ushort pid, syspath_typ* );
syspath_typ* crossedPath( ushort pid, syspath_typ* );
os9err    ConnectPTY_TTY( ushort pid, syspath_typ* );
void       PutCharsToTTY( ushort pid, syspath_typ*, ulong *lenP, char* buffer, Boolean wrln );
void    CheckInBufferTTY( ttydev_typ* mco );


/* external net    support routines */
os9err        MyInetAddr( ulong *inetAddr );
os9err             pNask( ushort pid, syspath_typ* );

/* eof */

