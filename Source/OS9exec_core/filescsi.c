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
/* (c) 1993-2006 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        bfo@synthesis.ch                    */
/**********************************************/

/*
 *  CVS:
 *    $Author$
 *    $Date$
 *    $Revision$
 *    $Source$
 *    $State$
 *    $Name$ (Tag)
 *    $Locker$ (who has reserved checkout)
 *  Log:
 *    $Log$
 *    Revision 1.17  2006/02/19 15:42:23  bfo
 *    Header changed to 2006
 *
 *    Revision 1.16  2005/06/30 11:38:36  bfo
 *    SCSI not yet supported for Mach-O
 *
 *    Revision 1.15  2005/04/15 11:16:09  bfo
 *    SCSI params are combined in one struct now
 *
 *    Revision 1.14  2005/01/22 15:50:31  bfo
 *    Renamed to ifdef MACOS9
 *
 *    Revision 1.13  2004/12/03 23:56:46  bfo
 *    MacOSX MACH adaptions
 *
 *    Revision 1.12  2004/11/20 11:44:07  bfo
 *    Changed to version V3.25 (titles adapted)
 *
 *    Revision 1.11  2003/05/08 23:12:14  bfo
 *    MacOSX: RBF activated for Carbon
 *
 *    Revision 1.10  2002/10/27 23:17:46  bfo
 *    get_mem/release_mem no longer with param <mac_asHandle>
 *
 *    Revision 1.9  2002/09/11 17:15:13  bfo
 *    Definition changed from "ulong* dat_buf" ==> "byte* dat_buf"
 *
 *
 */

/* This file contains the RBF Emulator */
#include "os9exec_incl.h"
#include "filescsi.h"

#define CmdRead 0x08
#define CmdWrite 0x0a
#define CmdSel 0x15
#define CmdSense 0x1a
#define CmdCapacity 0x25

#define CB_Size 6
#define CB_SizeExt 10
#define SizeCmd 12

#define NTries 30

/* the SCSI lowlevel call */
static os9err SCSIcall(short   scsiAdapt,
                       ushort  scsiBus,
                       ushort  scsiID,
                       ushort  scsiLUN,
                       byte   *cb,
                       ulong   cb_size,
                       byte   *dat_buf,
                       ulong   len,
                       Boolean doWrite)
{
    os9err err;
    char  *s;

    // linux or others???
    err = E_UNIT; // just fail

    if (debugcheck(dbgFiles, dbgDetail)) {
        switch (cb[0]) {
        case CmdSel:
            s = "CmdSel";
            break;
        case CmdSense:
            s = "CmdSense";
            break;
        case CmdRead:
            s = "CmdRead";
            break;
        case CmdWrite:
            s = "CmdWrite";
            break;
        case CmdCapacity:
            s = "CmdCapacity";
            break;
        default:
            s = "<none>";
        }

        upe_printf("# SCSIcall: %s (err=%d)\n", s, err);
    } 

    return err;
} /* SCSIcall */

os9err Set_SSize(scsi_dev *scsi, ulong sctSize)
/* Set the SCSI sector size */
{
    byte  cb[CB_Size];
    ulong dat_buf[3];

    cb[0] = CmdSel;
    cb[1] = (scsi->LUN & 0x07) << 5;
    cb[2] = 0;
    cb[3] = 0;
    cb[4] = SizeCmd;
    cb[5] = 0;

    dat_buf[0] = os9_long(8); /* block descriptor */
    dat_buf[1] = os9_long(0);
    dat_buf[2] = os9_long(sctSize);

    return SCSIcall(scsi->adapt,
                    scsi->bus,
                    scsi->ID,
                    scsi->LUN,
                    (byte *)&cb,
                    CB_Size,
                    (byte *)&dat_buf,
                    sizeof(dat_buf),
                    true);
} /* Set_SSize */

os9err Get_SSize(scsi_dev *scsi, ulong *sctSize)
/* Get the SCSI sector size */
{
    os9err err;
    byte   cb[CB_Size];
    ulong  dat_buf[3];

    cb[0] = CmdSense;
    cb[1] = (scsi->LUN & 0x07) << 5;
    cb[2] = 0;
    cb[3] = 0;
    cb[4] = SizeCmd;
    cb[5] = 0;

    err      = SCSIcall(scsi->adapt,
                   scsi->bus,
                   scsi->ID,
                   scsi->LUN,
                   (byte *)&cb,
                   CB_Size,
                   (byte *)&dat_buf,
                   sizeof(dat_buf),
                   false);
    *sctSize = os9_long(dat_buf[2]);
    return err;
} /* Set_SSize */

os9err ReadCapacity(scsi_dev *scsi, ulong *totScts, ulong *sctSize)
/* Get the SCSI device size */
{
    os9err err;
    byte   cb[CB_SizeExt];
    ulong  dat_buf[3];

    dat_buf[0] = 0;
    dat_buf[1] = 0;
    dat_buf[2] = 0;

    cb[0] = CmdCapacity;
    cb[1] = (scsi->LUN & 0x07) << 5;
    cb[2] = 0;
    cb[3] = 0;
    cb[4] = 0;
    cb[5] = 0;
    cb[6] = 0;
    cb[7] = 0;
    cb[8] = 0;
    cb[9] = 0;

    err = SCSIcall(scsi->adapt,
                   scsi->bus,
                   scsi->ID,
                   scsi->LUN,
                   (byte *)&cb,
                   CB_SizeExt,
                   (byte *)&dat_buf,
                   sizeof(dat_buf),
                   false);

    *totScts = os9_long(dat_buf[0]) +
               1; /* +1, because capacity returns latest sector */
    *sctSize = os9_long(dat_buf[1]);

    return err;
} /* ReadCapacity */

os9err Get_DSize(scsi_dev *scsi, ulong *totScts)
/* Get the SCSI device size */
{
    os9err err;
    byte   cb[CB_Size];
    ulong  dat_buf[3];
    ulong  sctSize;

    cb[0] = CmdSense;
    cb[1] = (scsi->LUN & 0x07) << 5;
    cb[2] = 0;
    cb[3] = 0;
    cb[4] = SizeCmd;
    cb[5] = 0;

    err      = SCSIcall(scsi->adapt,
                   scsi->bus,
                   scsi->ID,
                   scsi->LUN,
                   (byte *)&cb,
                   CB_Size,
                   (byte *)&dat_buf,
                   sizeof(dat_buf),
                   false);
    *totScts = os9_long(dat_buf[1]);
    sctSize  = os9_long(dat_buf[2]);

    /* Iomega JAZ does not support the mode sense command */
    if (*totScts == 0) {
        err = ReadCapacity(scsi, totScts, &sctSize);
    } // if

    return err;
} /* Get_DSize */

os9err ReadFromSCSI(scsi_dev *scsi,
                    ulong     sectorNr,
                    ulong     nSectors,
                    ulong     len,
                    byte     *buffer)
{
    os9err err;
    byte   cb[CB_Size];
    ulong *l;
    int    ii;

    for (ii = 0; ii < NTries; ii++) {
        l     = (ulong *)&cb[0];
        *l    = os9_long(sectorNr); /* cb0,cb1,cb2,cb3 */
        cb[0] = CmdRead;            /* and overwrite cb0 field */
        cb[1] =
            (cb[1] & 0x1F) | ((scsi->LUN & 0x07) << 5); // upper 3 bits are LUN
        cb[4] = nSectors;
        cb[5] = 0;

        err = SCSIcall(scsi->adapt,
                       scsi->bus,
                       scsi->ID,
                       scsi->LUN,
                       (byte *)&cb,
                       CB_Size,
                       buffer,
                       len,
                       false);
        //      if (err) printf( "Read  %008X %d %d\n", sectorNr,ii,err );
        if (err != E_UNIT && err != E_SEEK)
            break;

    } 

    if (err == E_UNIT)
        err = E_READ;
    return err;
} /* ReadFromSCSI */

os9err WriteToSCSI(scsi_dev *scsi,
                   ulong     sectorNr,
                   ulong     nSectors,
                   ulong     len,
                   byte     *buffer)
{
    os9err err;
    byte   cb[CB_Size];
    ulong *l;
    int    ii;

    for (ii = 0; ii < NTries; ii++) {
        l     = (ulong *)&cb[0];
        *l    = os9_long(sectorNr); /* cb0,cb1,cb2,cb3 */
        cb[0] = CmdWrite;           /* and overwrite cb0 field */
        cb[1] =
            (cb[1] & 0x1F) | ((scsi->LUN & 0x07) << 5); // upper 3 bits are LUN
        cb[4] = nSectors;
        cb[5] = 0;

        err = SCSIcall(scsi->adapt,
                       scsi->bus,
                       scsi->ID,
                       scsi->LUN,
                       (byte *)&cb,
                       CB_Size,
                       buffer,
                       len,
                       true);
        //      if (err) printf( "Write %008X %d %d\n", sectorNr,ii,err );
        if (err != E_UNIT && err != E_SEEK)
            break;

    } 

    if (err == E_UNIT)
        err = E_WRITE;
    return err;
} /* WriteToSCSI */

/* eof */
