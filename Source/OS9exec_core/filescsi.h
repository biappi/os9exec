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


/* Low level SCSI interface */

os9err Set_SSize( short scsiAdapt, ushort scsiBus, ushort scsiID, ushort scsiLUN,
                  ulong sctSize );
os9err Get_SSize( short scsiAdapt, ushort scsiBus, ushort scsiID, ushort scsiLUN,
                  ulong *sctSize );

os9err Get_DSize( short scsiAdapt, ushort scsiBus, ushort scsiID, ushort scsiLUN,
                  ulong *totScts );

os9err ReadFromSCSI( short scsiAdapt, ushort scsiBus, ushort scsiID, ushort scsiLUN,
                     ulong sectorNr, ulong nSectors,
                     ulong len, byte* buffer );

os9err WriteToSCSI( short scsiAdapt, ushort scsiBus, ushort scsiID, ushort scsiLUN,
                    ulong sectorNr, ulong nSectors,
                    ulong len,      byte* buffer );

#ifdef windows32
void scsiadaptor_help(void);
void scsi_finddefaults(void);
#endif
/* eof */
