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



/* I$xxxx calls */
os9err OS9_I_Open  (regs_type *rp, ushort cpid);
os9err OS9_I_Create(regs_type *rp, ushort cpid);
os9err OS9_I_Delete(regs_type *rp, ushort cpid);
os9err OS9_I_MakDir(regs_type *rp, ushort cpid);
os9err OS9_I_ChgDir(regs_type *rp, ushort cpid);
os9err OS9_I_Close (regs_type *rp, ushort cpid);
os9err OS9_I_WritLn(regs_type *rp, ushort cpid);
os9err OS9_I_Write (regs_type *rp, ushort cpid);
os9err OS9_I_ReadLn(regs_type *rp, ushort cpid);
os9err OS9_I_Read  (regs_type *rp, ushort cpid);
os9err OS9_I_Seek  (regs_type *rp, ushort cpid);
os9err OS9_I_SetStt(regs_type *rp, ushort cpid);
os9err OS9_I_GetStt(regs_type *rp, ushort cpid);
os9err OS9_I_Dup   (regs_type *rp, ushort cpid);
os9err OS9_I_Attach(regs_type *rp, ushort cpid);
os9err OS9_I_Detach(regs_type *rp, ushort cpid);
os9err OS9_I_SGetSt(regs_type *rp, ushort cpid);

/* eof */

