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


/* General Utility routines */
char* nullterm     ( char* s1,const char* s2, ushort max );
int   ustrcmp( const char* s1,const char* s2 );
int  pustrcmp( const char* s1,const char* s2 );
int  ustrncmp( const char* s1,const char* s2, ushort n );

os9err host2os9err(OSErr hosterr,ushort suggestion);
os9err c2os9err(int cliberr,ushort suggestion);
os9err os9error(os9err err);


ulong j_date  (            int  d,  int  m,  int  y  );
void  g_date  ( ulong jdn, int *dp, int *mp, int *yp );
void  Get_Time( ulong *cTime, ulong *cDate, int *dayOfWk, Boolean asGregorian );


ulong Min( ulong a, ulong b );
ulong Max( ulong a, ulong b );

char* StrBlk_Pt( char* s, int n );

Boolean IsRead ( ushort  mode );
Boolean IsWrite( ushort  mode );
Boolean IsRW   ( ushort  mode );
Boolean IsDir  ( ushort  mode );
Boolean IsExec ( ushort  mode );
Boolean IsCrea ( ushort  mode );
Boolean IsTrDir( ushort umode ); /* transparent file system: unix style */

os9err  pSCFopt     ( ushort pid, syspath_typ*, byte* buffer );
os9err  pRBFopt     ( ushort pid, syspath_typ*, byte* buffer );
void    LastCh_Bit7 ( char* name, Boolean setIt);
void    Console_Name( int term_id, char* consname );
char*   OS9exec_Name( void );


Boolean AbsPath      ( const char* pathname );
Boolean IsWhat       ( const char* pathname, Boolean isRoot );
Boolean IsRoot       ( const char* pathname );
Boolean IsRaw        ( const char* pathname );
Boolean VolInfo      ( const char* pathname, char* volname );
Boolean     OpenTDir ( const char* pathname, DIR** d );
dirent_typ* ReadTDir                       ( DIR*  d );
Boolean PathFound    ( const char* pathname );
Boolean FileFound    ( const char* pathname );
void CutUp                 ( char* pathname );
void    EatBack            ( char* pathname );


#ifdef win_linux
  os9err FD_ID       ( const char* pathname, dirent_typ* dEnt, ulong *id );
  os9err DirNthEntry       ( syspath_typ*, int n );
  os9err RemoveAppledouble ( syspath_typ* );
  void   seekD0            ( syspath_typ* );
  ulong  DirSize           ( syspath_typ* );
#endif


int     stat_        ( const char* pathname, struct stat *buf );
Boolean DirName      ( const char* pathname, ulong fdsect, char* result );
ulong   My_Ino       ( const char* pathname );
void    MakeOS9Path  (       char* pathname );

Boolean SamePathBegin( const char* pathname, const char* cmp );
Boolean InstalledDev ( const char* os9path,  
                       const char* curpath,  Boolean fullsearch, ushort *cdv );
Boolean SCSI_Device( const char* os9path,
                     short *scsiAdapt, short *scsiBus, int *scsiId, short *scsiLUN,
                     ushort *scsiSsize, ushort *scsiSas,
                     ptype_typ *typeP );

#ifdef windows32
  os9err AdjustPath ( const char* pathname, char* adname, Boolean creFile );
#endif

#ifdef win_linux
  void   GetEntry( dirent_typ* dEnt, char* name, Boolean do_2e_conv );
#endif

#ifdef macintosh
  os9err RBF_Rsc   ( FSSpec  *fs );
  os9err GetRBFName( char* os9path, ushort mode, Boolean *isFolder, 
                     FSSpec  *fs, FSSpec *afs );
#elif defined linux
  os9err GetRBFName( char* os9path, ushort mdoe, Boolean *isFolder, 
                     char* rbfname );
#endif


char*    Mod_TypeStr(    mod_exec* mod  );
char*      PStateStr( process_typ* cp   );
char*        TypeStr(   ptype_typ  type );
char*    spP_TypeStr( syspath_typ* );
ptype_typ IO_Type   ( ushort  pid, char* os9path, ushort mode );


/* eof */