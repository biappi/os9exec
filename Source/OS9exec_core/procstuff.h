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
/*        beat.forster@ggaweb.ch              */
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
