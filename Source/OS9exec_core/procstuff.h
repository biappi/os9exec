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



/* process routines */
void   show_processes(void);
void   init_processes(void);
void   kill_processes(void);

os9err  new_process( ushort parentid, ushort *newpid, ushort numpaths );
os9err kill_process( ushort pid );
os9err send_signal ( ushort pid, ushort signal );
os9err sigmask     ( ushort pid, int    level  );

void do_arbitrate(void);
void stop_os9exec(void);
void lw_pid( ttydev_typ* mco );

os9err setprior( ushort pid, ushort newprior);
os9err prepFork( ushort pid,      char* mpath,    ushort mid,
                 byte*  paramptr, ulong paramsiz, ulong memplus, 
                 ushort numpaths, ushort grp, ushort usr, ushort prior,
                 Boolean *intCmd );

/* eof */