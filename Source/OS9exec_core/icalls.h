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
