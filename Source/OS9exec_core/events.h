// 
// � �OS9exec,   OS-9 emulator for Mac OS, Windows and Linux 
// � �Copyright (C) 2002 �[ Lukas Zeller / Beat Forster ]
//	  Available under http://www.synthesis.ch/os9exec
// 
// � �This program is free software; you can redistribute it and/or 
// � �modify it under the terms of the GNU General Public License as 
// � �published by the Free Software Foundation; either version 2 of 
// � �the License, or (at your option) any later version. 
// 
// � �This program is distributed in the hope that it will be useful, 
// � �but WITHOUT ANY WARRANTY; without even the implied warranty of 
// � �MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// � �See the GNU General Public License for more details. 
// 
// � �You should have received a copy of the GNU General Public License 
// � �along with this program; if not, write to the Free Software 
// � �Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
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



/* OS-9 events */
/* the definitions */
#define Ev_Link     0x00
#define Ev_UnLnk    0x01
#define Ev_Creat    0x02
#define Ev_Delet    0x03
#define Ev_Wait     0x04
#define Ev_WaitR    0x05
#define Ev_Read     0x06
#define Ev_Info     0x07
#define Ev_Signl    0x08
#define Ev_Pulse    0x09
#define Ev_Set      0x0A
#define Ev_SetR     0x0B



/* the routines */
void   init_events(void);

os9err evLink ( char* evName,                                      ulong *evId );
os9err evUnLnk                                                   ( ulong  evId );
os9err evCreat( char* evName, int evValue, short wInc, short sInc, ulong *evId );
os9err evDelet( char* evName );
os9err evWait ( ulong evId,   int minV, int maxV, int  *evValue );
os9err evSignl( ulong evId );
os9err evSet  ( ulong evId,   int evValue,        int *prvValue );

/* eof */