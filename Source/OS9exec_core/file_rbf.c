//
//    OS9exec,   OS-9 emulator for Mac OS, Windows and Linux
//    Copyright (C) 2002 Lukas Zeller / Beat Forster
//    Available under http://www.synthesis.ch/os9exec
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
/* (c) 1993-2007 by Lukas Zeller, CH-Zuerich  */
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
 *    Revision 1.56  2007/04/17 23:12:36  bfo
 *    FD Cache things added (not yet active)
 *
 *    Revision 1.55  2007/01/07 13:53:23  bfo
 *    Up to date
 *
 *    Revision 1.54  2007/01/04 20:37:22  bfo
 *    Some unused vars eliminated
 *    Boolean return type for 'IsSCSI'
 *    FD_Segment: break loop 'if (scs==0) break;'
 *    DoAccess: Loop until enough sectors allocated ('scsi.r' problem)
 *
 *    Revision 1.53  2006/12/17 00:47:23  bfo
 *    devCopy raw device opening / copy length is OS9PATHLEN
 *
 *    Revision 1.52  2006/12/01 20:03:19  bfo
 *    "MountDev" with <devCopy> support ( -d )
 *
 *    Revision 1.51  2006/11/18 10:07:12  bfo
 *    PrepareRAM adapted:
 *    - Offset to sector 1 is now dev->sctSize, no longer A_Base 0x100
 *    - MaxKB introduced and checked
 *    - <cluSize> must be a power of 2 now
 *    - dev->sas= DD_MINALLOC
 *    - <sctSize>, <cluSize> + root sector calculations correcty
 *    - Params -n and -c added for "mount" command
 *    - bb/coff calculation corrected (for wMode || mlt)
 *
 *    Revision 1.50  2006/11/12 13:30:29  bfo
 *    "ReadFD" visible from outside
 *
 *    Revision 1.49  2006/07/29 08:49:54  bfo
 *    "adaptor" => "adapter"
 *
 *    Revision 1.48  2006/06/13 22:18:47  bfo
 *    SS_EOF implementation added for RBF file manager
 *
 *    Revision 1.47  2006/06/08 08:15:04  bfo
 *    Eliminate causes of signedness warnings with gcc 4.0
 *
 *    Revision 1.46  2006/05/16 13:07:34  bfo
 *    scs>1 bug fixed
 *
 *    Revision 1.45  2006/02/19 16:25:26  bfo
 *    printf commented out / reformatted
 *
 *    Revision 1.44  2005/06/30 11:35:41  bfo
 *    Mount reduced AND full / Mach-O support
 *
 *    Revision 1.43  2005/05/13 17:21:48  bfo
 *    mount -I is supported now
 *
 *    Revision 1.42  2005/04/15 11:13:04  bfo
 *    Reduced size of RBF images is supported now
 *
 *    Revision 1.41  2004/12/03 23:54:51  bfo
 *    MacOSX MACH adaptions
 *
 *    Revision 1.40  2004/11/27 12:04:28  bfo
 *    _XXX_ introduced
 *
 *    Revision 1.39  2004/11/20 11:44:07  bfo
 *    Changed to version V3.25 (titles adapted)
 *
 *    Revision 1.38  2004/10/22 22:51:11  bfo
 *    Most of the "pragma unused" eliminated
 *
 *    Revision 1.37  2004/01/04 02:11:00  bfo
 *    Assign rbf->lastPos & rbf->att also in case of root dir
 *    ("rename" directory cut bug)
 *
 *    Revision 1.36  2003/08/01 11:16:28  bfo
 *    do not create file w/o existing subdir
 *
 *    Revision 1.35  2003/07/31 14:38:56  bfo
 *    Change calculation of PD_FD/PD_DFD (sector size instead of 256)
 *
 *    Revision 1.34  2003/05/05 17:55:07  bfo
 *    Activate most of the ramDisk things even without RAM_SUPPORT
 *
 *    Revision 1.33  2002/10/27 23:31:02  bfo
 *    ReleaseBuffers at pRclose done in every case
 *    get_mem/release_mem without param <mac_asHandle>
 *
 *    Revision 1.32  2002/10/15 18:38:23  bfo
 *    Consider only lobyte at OS9_I_Delete
 *
 *    Revision 1.31  2002/10/09 20:41:16  bfo
 *    uphe_printf => upe_printf
 *
 *    Revision 1.30  2002/10/02 19:21:37  bfo
 *    Correct handling for "mount" without parameters
 *
 *    Revision 1.29  2002/09/21 20:02:52  bfo
 *    E_DIDC problem fixed.
 *
 *    Revision 1.28  2002/09/19 22:00:18  bfo
 *    Disabling "r0" more specific now
 *
 *    Revision 1.27  2002/09/17 00:24:09  bfo
 *    /r0 is disabled as name, if RAM_SUPPORT is disabled.
 *
 *    Revision 1.26  2002/09/14 23:13:38  bfo
 *    Relative paths can be mounted (again).
 *
 *    Revision 1.25  2002/09/11 17:05:55  bfo
 *    Bug at incomplete sector of multisector write fixed
 *
 *
 */

// #define RBF_CACHE

#include <ctype.h>

/* This file contains the RBF Emulator */
#include "os9exec_incl.h"
#include "filescsi.h"

#define FD_Header_Size 16 // size of FD header
#define SegSize 5         // number of bytes per segment

/* the RBF device entry itself */
typedef struct {
    /* common for all types */
    char name[OS9NAMELEN];  /* device         name */
    char name2[OS9NAMELEN]; /* device 2nd     name */
    char name3[OS9NAMELEN]; /* device 3rd     name %%% dirty solution */
    char alias[OS9PATHLEN]; /* device's alias name */

    int    nr;          /* own reference number (array index) */
    ulong  sctSize;     /* sector size for this device */
    ushort mapSize;     /* size of allocation map */
    byte   pdtyp;       /* device type: hard disk, floppy */
    ushort sas;         /* sector allocation size */
    ulong  root_fd_nr;  /* sector nr of root fd */
    ulong  clusterSize; /* cluster size (allocation) */
    ulong  totScts;     /* total   number of sectors */
    ulong  imgScts;     /* current number of sectors at this image */
    ushort last_diskID; /* last disk ID, inherited by new paths */
    ulong  last_alloc;  /* the last allocation was here */
    ulong  currPos;     /* current position at image */
    ulong  rMiss, rTot, /* device statistics */
        wMiss, wTot;
    ushort sp_img;               /* syspath number of image file */
    char   img_name[OS9PATHLEN]; /* full path name of image file */

    addrpair_typ tmp_sct;    /* temporary buffer sector */
    Boolean wProtected; /* true, if write  protected */
    Boolean fProtected; /* true, if format protected */
    Boolean imgMode;    /* true, if reduced img size is allowed/supported */
    Boolean multiSct;   /* true, if multi sector support */

    Boolean isRAM;   /* true, if RAM disk */
    addrpair_typ ramBase; /* start address of RAM disk */

    scsi_dev scsi;      /* SCSI device variables */
    Boolean  installed; /* true, if device is already installed */
} rbfdev_typ;

/* the RBF devices */
rbfdev_typ rbfdev[MAXRBFDEV];

/* OS9exec builtin module, defined as constant array */
const byte RAM_zero[] = {
    0x00, 0x20, 0x00, 0x00, 0x04, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x05, 0x00, 0x00, 0xbf, 0x00, 0x00, // . ...........?..
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x07, 0x03, 0x14, 0x2d, 0x52, // ..............-R
    0x61, 0x6d, 0x20, 0x44, 0x69, 0x73, 0x6b, 0x20,
    0x28, 0x43, 0x61, 0x75, 0x74, 0x69, 0x6f, 0x6e, // am Disk (Caution
    0x3a, 0x20, 0x56, 0x6f, 0x6c, 0x61, 0x74, 0x69,
    0x6c, 0x65, 0xa9, 0x00, 0x00, 0x00, 0x00, 0x00, // : Volatile).....
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
    0x43, 0x72, 0x75, 0x7a, 0x00, 0x00, 0x00, 0x01,
    0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, // Cruz............
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // ................
};

/* --- local procedure definitions for object definition ------------------- */
void init_RBF(fmgr_typ *f);

os9err pRopen(ushort pid, syspath_typ *, ushort *modeP, const char *pathname);
os9err pRclose(ushort pid, syspath_typ *);
os9err pRread(ushort pid, syspath_typ *, ulong *lenP, char *buffer);
os9err pRreadln(ushort pid, syspath_typ *, ulong *lenP, char *buffer);
os9err pRwrite(ushort pid, syspath_typ *, ulong *lenP, char *buffer);
os9err pRwriteln(ushort pid, syspath_typ *, ulong *lenP, char *buffer);
os9err pRseek(ushort pid, syspath_typ *, ulong *posP);
os9err pRchd(ushort pid, syspath_typ *, ushort *modeP, char *pathname);
os9err pRdelete(ushort pid, syspath_typ *, ushort *modeP, char *pathname);
os9err pRmakdir(ushort pid, syspath_typ *, ushort *modeP, char *pathname);

os9err pRsize(ushort pid, syspath_typ *, ulong *sizeP);
os9err pRopt(ushort pid, syspath_typ *, byte *buffer);
os9err pRnam(ushort pid, syspath_typ *, char *volname);
os9err pRpos(ushort pid, syspath_typ *, ulong *posP);
os9err pReof(ushort pid, syspath_typ *);
os9err pRready(ushort pid, syspath_typ *, ulong *n);
os9err pRgetFD(ushort pid, syspath_typ *, ulong *maxbytP, byte *buffer);
os9err pRgetFDInf(ushort pid,
                  syspath_typ *,
                  ulong *maxbytP,
                  ulong *fdinf,
                  byte  *buffer);
os9err pRdsize(ushort pid, syspath_typ *, ulong *size, ulong *dtype);

os9err pRsetsz(ushort pid, syspath_typ *, ulong *size);
os9err pRsetatt(ushort pid, syspath_typ *, ulong *attr);
os9err pRsetFD(ushort pid, syspath_typ *, byte *buffer);
os9err pRWTrk(ushort pid, syspath_typ *, ulong *trackNr);

void init_RBF_devs();
/* ------------------------------------------------------------------------- */

void init_RBF(fmgr_typ *f)
/* install all procedures of the RBF file manager */
{
    gs_typ *gs = &f->gs;
    ss_typ *ss = &f->ss;

    /* main procedures */
    f->open    = pRopen;
    f->close   = pRclose;
    f->read    = pRread;
    f->readln  = pRreadln;
    f->write   = pRwrite;
    f->writeln = pRwriteln;
    f->seek    = pRseek;
    f->chd     = pRchd;
    f->del     = pRdelete;
    f->makdir  = pRmakdir;

    /* getstat */
    gs->_SS_Size  = pRsize;
    gs->_SS_Opt   = pRopt;
    gs->_SS_DevNm = pRnam;
    gs->_SS_Pos   = pRpos;
    gs->_SS_EOF   = pReof;
    gs->_SS_Ready = pRready;
    gs->_SS_FD    = pRgetFD;
    gs->_SS_FDInf = pRgetFDInf;
    gs->_SS_DSize = pRdsize; /* get drive size in sectors */

    /* setstat */
    ss->_SS_Size = pRsetsz;
    ss->_SS_Opt  = IO_Nop; /* ignored */
    ss->_SS_Attr = pRsetatt;
    ss->_SS_FD   = pRsetFD;
    ss->_SS_Lock = IO_Nop; /* ignored */
    ss->_SS_WTrk = pRWTrk;

    init_RBF_devs(); /* init RBF devices */
}

/* --------------------------------------------------------- */

void init_RBF_devs()
/* initialize them all to not installed */
{
    rbfdev_typ *dev;
    int         ii;
    for (ii = 1; ii < MAXRBFDEV; ii++) {
        dev            = &rbfdev[ii];
        dev->installed = false;
    }

    for (ii = 0; ii < MAXSCSI; ii++) {
        strcpy(scsi[ii].name, "");
    }
}

// -----------------------------------------------------------------------

static Boolean IsSCSI(rbfdev_typ *dev) { return dev->scsi.ID != NO_SCSI; }

static inline uint8_t *rambase(rbfdev_typ *dev)
{
    return (uint8_t *)dev->ramBase.host;
}

static inline uint8_t *tmp_sect(rbfdev_typ *dev)
{
    return (uint8_t *)dev->tmp_sct.host;
}

static inline uint8_t *fd_sect(syspath_typ *sp)
{
    return (uint8_t *)sp->fd_sct.host;
}

static inline uint8_t *rw_sect(syspath_typ *sp)
{
    return (uint8_t *)sp->rw_sct.host;
}

static os9err
ReadSector(rbfdev_typ *dev, ulong sectorNr, ulong nSectors, byte *buffer)
/* Read sectors, either from SCSI or from an RBF image file */
{
    os9err  err = 0;
    Boolean pos_already;
    ulong   sect      = dev->sctSize;
    ulong   pos       = sectorNr * sect; // get position and length to read
    ulong   len       = nSectors * sect;
    ulong   img       = dev->imgScts * sect;
    ulong   sectorLim = sectorNr + nSectors; // upper limit
    ulong   blindNr, nBlinds;                // not accessible sectors
    ulong   cnt;

    //  if (sectorNr==0) {
    //      debugprintf(dbgFiles,dbgDetail,("# RBF read  sector0\n"));
    //  }
    //  if (dev->imgScts<dev->totScts) upo_printf( "read  sectors %d..%d
    //  (%d)\n", sectorNr, sectorLim-1, dev->imgScts );

    //  if (dev->imgScts>2097152) upo_printf( "read  sectors %d..%d (%d/%d)\n",
    //                                         sectorNr, sectorLim-1,
    //                                         dev->imgScts, dev->totScts );

    debugprintf(dbgFiles,
                dbgNorm,
                ("# RBF read  sectorNr: $%06X (n=%d) @ $%08X\n",
                 sectorNr,
                 nSectors,
                 pos));
    if (sectorNr > 0) {
        if (dev->totScts == 0)
            return E_NOTRDY;
        if (dev->totScts < sectorLim)
            return E_EOF; /* out of valid range */
        if (dev->imgScts < sectorLim) {
            blindNr = dev->imgScts;
            if (blindNr < sectorNr)
                blindNr = sectorNr;
            //  upo_printf( "read       sectors %d..%d\n", sectorNr, sectorLim-1
            //  ); upo_printf( "read blind sectors %d..%d\n", blindNr,
            //  sectorLim-1 );

            nBlinds = sectorLim - blindNr;
            nSectors -= nBlinds;
            len -= nBlinds * sect;
            memset(buffer + len, 0xEE, nBlinds * sect); // fill rest with 0xEE
            //  upo_printf( "nBlinds=%d nSectors=%d len=%d\n",
            //  nBlinds,nSectors,len );
        }
    }

    do {
        // #ifdef RAM_SUPPORT
        if (dev->isRAM) {
            memcpy(buffer, rambase(dev) + (sectorNr * sect), nSectors * sect);
            break;
        }
        // #endif

        if (IsSCSI(dev)) {
            err = ReadFromSCSI(&dev->scsi, sectorNr, nSectors, len, buffer);
            if (!err)
                break;
            err = Set_SSize(&dev->scsi, sect);
            if (err)
                break; /* adjust sector size */
            err          = ReadCapacity(&dev->scsi, &dev->totScts, &sect);
            dev->sctSize = sect;
            dev->imgScts = dev->totScts;
            if (err)
                break; /* and get new info back */

            err = ReadFromSCSI(&dev->scsi, sectorNr, nSectors, len, buffer);

            //     err= ReadFromSCSI(  dev->scsiAdapt, dev->scsiBus,
            //     dev->scsiID, dev->scsiLUN, sectorNr,nSectors,  len,buffer );
            // if (err) {
            //     err= Set_SSize   (  dev->scsiAdapt, dev->scsiBus,
            //     dev->scsiID, dev->scsiLUN,                   dev->sctSize );

            //     err= ReadCapacity(  dev->scsiAdapt, dev->scsiBus,
            //     dev->scsiID, dev->scsiLUN,  &dev->totScts,  &dev->sctSize );
            //     dev->imgScts= dev->totScts; if (err) break; /* and get new
            //     info back */

            //     err= ReadFromSCSI(  dev->scsiAdapt, dev->scsiBus,
            //     dev->scsiID, dev->scsiLUN, sectorNr,nSectors, len,buffer );
            // }
        }
        else {
            // if (rawMode && sectorNr>=60 && sectorLim<=90) {
            //   if (sectorLim-1>sectorNr) upo_printf( "READ  RAW: %d..%d\n",
            //   sectorNr,sectorLim-1 ); else                      upo_printf(
            //   "READ  RAW: %d\n",     sectorNr );
            // }

            pos_already  = (pos == dev->currPos);
            dev->currPos = UNDEF_POS; /* invalidate for case of an error */

            if (len > 0) {
                if (!pos_already) {
                    dev->rMiss++;
                    err = syspath_seek(0, dev->sp_img, pos);
                    if (err)
                        break;
                    //  if (dev->imgScts>2097152) upo_printf( "seek err=%d\n",
                    //  err );
                }
                dev->rTot++;
                cnt = len;
                err = syspath_read(0, dev->sp_img, &cnt, buffer, false);
                //  if (dev->imgScts>2097152) upo_printf( "read err=%d len=%d
                //  cnt=%d\n", err, len, cnt );
                if (err)
                    break;
                if (cnt < len) {
                    err = E_EOF;
                    break;
                }; /* if not enough read, make EOF */
            }

            // don't set <currPos> behind <img> position
            dev->currPos =
                pos + cnt; /* here is now our current read/write position */
            if (dev->currPos >
                img) { /* upo_printf( "redu %d -> %d\n", dev->currPos,img ); */
                dev->currPos = img;
            }
        }
    } while (false);

    return err;
}

static os9err
WriteSector(rbfdev_typ *dev, ulong sectorNr, ulong nSectors, byte *buffer)
/* Write sectors, either to SCSI or on an RBF image file */
{
    os9err  err = 0;
    Boolean pos_already, extendIt = false;
    ulong   sect      = dev->sctSize;
    ulong   pos       = sectorNr * sect; // get position and length to write
    ulong   len       = nSectors * sect;
    ulong   img       = dev->imgScts * sect;
    ulong   sectorLim = sectorNr + nSectors; // upper limit
    ulong   blindNr;                         // not accessible sectors
    ulong   cnt;
    byte    ee;

    /* take care of write and format protection */
    if (dev->wProtected)
        return E_WP;
    if (sectorNr == 0) {
        if (dev->fProtected)
            return E_FORMAT;
    }

    //  if (dev->imgScts<dev->totScts) upo_printf( "write sectors %d..%d
    //  (%d)\n", sectorNr, sectorLim-1, dev->imgScts ); if
    //  (dev->imgScts>2097152) upo_printf( "write sectors %d..%d (%d/%d)\n",
    //                                         sectorNr, sectorLim-1,
    //                                         dev->imgScts, dev->totScts );

    debugprintf(dbgFiles,
                dbgNorm,
                ("# RBF write sectorNr: $%06X (n=%d) @ $%08X\n",
                 sectorNr,
                 nSectors,
                 pos));
    if (sectorNr > 0) {
        if (dev->totScts == 0)
            return E_NOTRDY;
        if (dev->totScts < sectorLim)
            return E_EOF; /* out of valid range */
        if (dev->imgScts < sectorLim) {
            blindNr = dev->imgScts;
            if (blindNr < sectorNr)
                blindNr = sectorNr;
            // upo_printf( "write       sectors %d..%d\n", sectorNr, sectorLim-1
            // ); upo_printf( "write blind sectors %d..%d\n", blindNr,
            // sectorLim-1 );
            //
            // if (pos==dev->imgScts*dev->sctSize) upo_printf( "perfect fit" );
            // if (pos< dev->imgScts*dev->sctSize) upo_printf( "partly" );

            if (pos > img) {
                pos = img;
                err = syspath_seek(0, dev->sp_img, pos);
                if (err)
                    return err;
                // upo_printf( "fill blind sectors %d..%d\n", dev->imgScts,
                // sectorNr-1 );

                // fill the unused part of the sector
                // this should not happen, as the RBF allocator will ask for the
                // follow up sector
                while (pos < sectorNr * sect) {
                    cnt = sizeof(ee);
                    ee  = 0xFF; // slow because no buffer
                    err = syspath_write(0, dev->sp_img, &cnt, &ee, false);
                    if (err)
                        return err;
                    pos++;
                }
            }

            dev->currPos = UNDEF_POS;
            dev->imgScts = sectorLim;
            img          = dev->imgScts * sect;
            extendIt     = true;
            err          = syspath_setstat(0,
                                  dev->sp_img,
                                  SS_Size,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  &img,
                                  NULL);
        }
    }

    do {
        // #ifdef RAM_SUPPORT
        if (dev->isRAM) {
            memcpy(rambase(dev) + (sectorNr * sect), buffer, nSectors * sect);
            break;
        }
        // #endif

        if (IsSCSI(dev))
            err = WriteToSCSI(&dev->scsi, sectorNr, nSectors, len, buffer);
        else {
            // if (rawMode && sectorNr>=60 && sectorLim<=90) {
            //   if (sectorLim-1>sectorNr) upo_printf( "WRITE RAW: %d..%d\n",
            //   sectorNr,sectorLim-1 ); else                      upo_printf(
            //   "WRITE RAW: %d\n",     sectorNr );
            // }

            pos_already  = (pos == dev->currPos);
            dev->currPos = UNDEF_POS; /* invalidate for case of an error */

            if (!pos_already) {
                dev->wMiss++;
                err = syspath_seek(0, dev->sp_img, pos);
                if (err)
                    break;
            }
            dev->wTot++;
            cnt = len;
            err = syspath_write(0, dev->sp_img, &cnt, buffer, false);
            // upo_printf( "write err=%d cnt=%d\n", err, cnt );
            if (err)
                break;
            if (cnt < len) {
                err = E_EOF;
                break;
            }; /* if not enough read, make EOF */

            dev->currPos =
                pos + cnt; /* here is now our current read/write position */

            if (extendIt) {
                // upo_printf( "extend cnt=%d %02X %02X %02X\n", cnt, buffer[ 0
                // ],buffer[ 1 ],buffer[ 2 ] );
                err = syspath_seek(0, dev->sp_img, 0); // flush the buffer
                // upo_printf( "seek0 err=%d\n", err );
                err = syspath_seek(0, dev->sp_img, dev->currPos);

                //                                     cnt=         sect;
                // upo_printf( "seekN err=%d pos=%d\n", err, dev->currPos );
                // err= syspath_read ( 0, dev->sp_img, &cnt, dev->tmp_sct, false
                // );

                // upo_printf( "size is now %d err=%d pos=%d\n", img, err,
                // dev->currPos ); err= syspath_setstat( 0, dev->sp_img,
                // SS_Size, NULL,NULL, NULL,NULL,&img,NULL ); upo_printf( "size
                // is now %d err=%d pos=%d\n", img, err, dev->currPos );
            }
        }
    } while (false);

    return err;
}

// ----------------------------------------------------------------
// utility procs

static os9err CutOS9Path(char **p, char *cmp_entry)
/* gets the next subpath of <p> into <cmp_entry> */
{
    os9err err = 0;
    int    n   = 1;
    char  *c   = cmp_entry;

    if (ustrcmp(*p, "...") == 0 || /* 3 possibilities */
        ustrncmp(*p, "....", 4) == 0 ||
        ustrncmp(*p, ".../", 4) == 0) { /* support multiple up path: ... */
        strcpy(cmp_entry, "..");
        (*p)++;
        return 0; /* no error */
    }

    while (**p != NUL) {
        if (n++ >= OS9NAMELEN) {
            err = E_BPNAM;
            break;
        }
        if (**p == PSEP) {
            (*p)++;
            break;
        };
        *c = **p;
        c++;
        (*p)++;
    }

    *c = NUL;
    return err;
}

static void GetBuffers(_rbf_, syspath_typ *spP)
{
    // Boolean pp= spP->fd_sct==NULL || spP->rw_sct==NULL;

    // if (pp) upe_printf( "Getbuffers %d %d %08X %08X\n",
    //                      spP->nr, dev->sctSize, spP->fd_sct,spP->rw_sct );

    /* allocate them only once, assume sctSize will not change */
    // if (spP->fd_sct==NULL) spP->fd_sct= get_mem( dev->sctSize, false );
    // if (spP->rw_sct==NULL) spP->rw_sct= get_mem( dev->sctSize, false );

    /* %%% there is currently an allocation bug, use always 2048 bytes */
    if (spP->fd_sct.host == NULL)
        spP->fd_sct = get_mem(2048);
    if (spP->rw_sct.host == NULL)
        spP->rw_sct = get_mem(2048);

    // if (pp) upe_printf( "Getbuffers %d %d %08X %08X\n",
    //                      spP->nr, dev->sctSize, spP->fd_sct,spP->rw_sct );
}

static void ReleaseBuffers(syspath_typ *spP)
{
    // upe_printf( "Relbuffers %d %08X %08X\n", spP->nr,
    // spP->fd_sct,spP->rw_sct);

    if (spP->fd_sct.host != NULL)
        release_mem(spP->fd_sct);
    spP->fd_sct.host  = NULL;
    spP->fd_sct.guest = 0;

    if (spP->rw_sct.host != NULL)
        release_mem(spP->rw_sct);
    spP->rw_sct.host  = NULL;
    spP->rw_sct.guest = 0;
}

static os9err ReleaseIt(ushort pid, rbfdev_typ *dev)
{
    os9err  err       = 0;
    Boolean isRAMDisk = dev->isRAM;
    if (isRAMDisk)
        release_mem(dev->ramBase);

    if (!isRAMDisk && !IsSCSI(dev))
        err = syspath_close(pid, dev->sp_img);

    if (!err) {
        dev->installed = false;
        release_mem(dev->tmp_sct);
        dev->tmp_sct.host  = NULL;
        dev->tmp_sct.guest = 0;
    }

    return err;
}

static os9err DevSize(rbfdev_typ *dev)
/* get the size of the device as numbers of sectors
   and adjust either device's sector size if possible, otherwise
   copy device's current sector size to dev->sctSize */
{
    os9err err;
    ulong  size, ssize;

    /* should be defined already for the RAM disk */
    // #ifdef RAM_SUPPORT
    if (dev->isRAM)
        return 0;
    // #endif

    if (IsSCSI(dev)) { /* try to switch to correct sector size first */
        err = Set_SSize(&dev->scsi, dev->sctSize);
        if (err) {
            // not possible to change sector size to what OS9 expects
            // - get sector size of SCSI device
            err = Get_SSize(&dev->scsi, &ssize);
            if (err)
                return err;
            if (ssize > MIN_TMP_SCT_SIZE)
                return err; // cannot read sector 0 that big
            // - use sector size of SCSI device for now
            dev->sctSize = ssize;
        }
        err          = ReadCapacity(&dev->scsi, &dev->totScts, &dev->sctSize);
        dev->imgScts = dev->totScts;
    }
    else {
        err = syspath_gs_size(0, dev->sp_img, &size);
        //  upo_printf( "DevSize v img=%d/tot=%d \n", dev->imgScts, dev->totScts
        //  );
        dev->imgScts = size / dev->sctSize;
        //  dev->imgScts= dev->totScts;
        //  upo_printf( "DevSize n img=%d/tot=%d \n", dev->imgScts, dev->totScts
        //  );
    }

    return err;
}

static os9err
ChkIntegrity(rbfdev_typ *dev, syspath_typ *spP, byte *mysct, Boolean ignore)
{
    ushort *w = (ushort *)&mysct[14];

    dev->last_diskID = *w;
    if (spP->u.rbf.diskID != *w) { /* doesn't matter big/little endian */
        spP->u.rbf.diskID = *w;
        if (ignore)
            return 0;

        spP->u.rbf.fd_nr = 0; /* do not access any more this fd */
        spP->mustW       = 0; /* don't write this sector */
        DevSize(dev);         /* get new device size */
        return E_DIDC;
    }

    return 0;
}

static os9err GetTop(ushort pid, rbfdev_typ *dev)
{
    os9err err  = 0;
    short  sp   = dev->sp_img;
    ulong  sect = dev->sctSize;
    ulong  map  = dev->mapSize;
    ulong  last = map % sect;
    ulong  len;
    ulong  pos  = ((map - 1) / sect + 1) * sect;
    byte   b    = 0;
    ulong  offs = 0;
    ulong  sv   = dev->imgScts;
    ulong  img;

    // upo_printf( "map=%d sect=%d pos=%d\n", map, sect, pos );

    if (last == 0)
        last = sect;

    // search for the last allocated position at the allocation map
    while (pos >= sect) {
        err = syspath_seek(pid, sp, pos);
        if (err)
            return err;
        len = sect;
        err = syspath_read(pid, sp, &len, dev->tmp_sct.host, false);
        if (err)
            return err;

        while (last > 0) {
            b = tmp_sect(dev)[--last];
            if (b)
                break;
        }

        if (b)
            break;
        pos -= sect;
        last = sect;
    }

    while (b) {
        b = b << 1;
        offs++;
    }

    // upo_printf( "GetTop v img=%d tot=%d\n", dev->imgScts, dev->totScts );
    dev->imgScts = ((pos - sect + last) * BpB + offs) * dev->clusterSize;
    // upo_printf( "GetTop n img=%d tot=%d\n", dev->imgScts, dev->totScts );

    if (sv > dev->imgScts) {
        //  upo_printf( "REDUCE\n" );
        img = dev->imgScts * sect;
        //  upo_printf( "REDUCE img=%d\n",  img );
        err = syspath_setstat(pid,
                              sp,
                              SS_Size,
                              NULL,
                              NULL,
                              NULL,
                              NULL,
                              &img,
                              NULL);
        //  upo_printf( "REDUCED err=%d\n", err );
    }

    // upo_printf( "map=%d sect=%d pos=%d last=%d img=%d rest=%d\n", map, sect,
    // pos, last, img, dev->totScts-img );
    return err;
}

static os9err GetFull(ushort pid, rbfdev_typ *dev)
{
    os9err err  = 0;
    short  sp   = dev->sp_img;
    ulong  sect = dev->sctSize;
    ulong  img  = dev->totScts * sect;

    err = syspath_setstat(pid, sp, SS_Size, NULL, NULL, NULL, NULL, &img, NULL);
    if (!err)
        dev->imgScts = dev->totScts;

    // upo_printf( "FULL AGAIN err=%d\n", err );
    return err;
}

static os9err RootLSN(_pid_, rbfdev_typ *dev, syspath_typ *spP, Boolean ignore)
{
    os9err  err;
    ulong  *l;
    ushort *w;
    ushort  sctSize;
    Boolean cruz;

    // upo_printf( "RootLSN\n" );
    while (true) { /* loop */
        debugprintf(dbgFiles,
                    dbgNorm,
                    ("# RootLSN: sectorsize %d\n", dev->sctSize));

        // get size of device
        if (dev->totScts == 0) {
            err = DevSize(dev);
            if (err)
                return err;
        }
        err = ReadSector(dev, 0, 1, dev->tmp_sct.host);
        if (err)
            return err;

        /* get the sector size of the device */
        char *cruz_s = tmp_sect(dev) + CRUZ_POS;
        cruz         = (strcmp(cruz_s, Cruz_Str) == 0);
        debugprintf(dbgFiles,
                    dbgNorm,
                    ("# RootLSN: sectorsize %d %s\n",
                     dev->sctSize,
                     cruz ? "(cruz)" : ""));

        if (cruz) {
            uint8_t *t = tmp_sect(dev) + SECT_POS;
            w          = (ushort *)t;
            sctSize    = os9_word(*w);
        }
        else {
            if (dev->sctSize == 0)
                sctSize = STD_SECTSIZE;
            else
                sctSize = dev->sctSize; // use existing (luz 2002-02-12)
        }
        if (sctSize == 0)
            sctSize = STD_SECTSIZE; // but avoid 0  (bfo 2002-06-06)

        debugprintf(
            dbgFiles,
            dbgNorm,
            ("# RootLSN: adapt sectorsize %d => %d\n", dev->sctSize, sctSize));
        if (dev->sctSize == sctSize)
            break;              /* it is ok already ? */
        dev->sctSize = sctSize; /* this is the correct sector size */

        /* release buffers of old size */
        release_mem(dev->tmp_sct);
        ReleaseBuffers(spP);

        /* and get the new buffers with the new sector size */
        dev->tmp_sct = get_mem(
            dev->sctSize > MIN_TMP_SCT_SIZE ? dev->sctSize : MIN_TMP_SCT_SIZE);
        GetBuffers(dev, spP);

        dev->totScts    = 0; /* must be set to correct value */
        dev->last_alloc = 0; /* initialize allocater pointer */
    } /* loop */

    l            = (ulong *)(tmp_sect(dev) + TOT_POS);
    dev->totScts = os9_long(*l) >> BpB;
    if (dev->imgScts == 0)
        dev->imgScts = dev->totScts;
    w                = (ushort *)(tmp_sect(dev) + MAP_POS);
    dev->mapSize     = os9_word(*w);
    w                = (ushort *)(tmp_sect(dev) + BIT_POS);
    dev->clusterSize = os9_word(*w);
    l                = (ulong *)(tmp_sect(dev) + DIR_POS);
    dev->root_fd_nr  = os9_long(*l) >> BpB;

    spP->u.rbf.fd_nr = dev->root_fd_nr;
    err              = ChkIntegrity(dev, spP, dev->tmp_sct.host, ignore);
    return err;
}

static void CutPath(char *s)
{
    size_t ii, len = strlen(s);
    for (ii = len - 1; ii >= 0; ii--) {
        if (s[ii] == PSEP) {
            strcpy(s, &s[ii + 1]);
            break;
        }
    }
}

static os9err Open_Image(ushort      pid,
                         rbfdev_typ *dev,
                         ptype_typ   type,
                         char       *pathName,
                         ushort      mode)
{
#define R0 "/r0"
    os9err  err, cer;
    ushort  sp, sctSize;
    ulong   len, iSize, tSize;
    ulong   imgScts, totScts;
    ulong  *l;
    ushort *w;
    byte    bb[STD_SECTSIZE]; /* one sector */

    do {
#ifndef RAM_SUPPORT
        char *q;
        ulong lr0 = strlen(R0);
        if (ustrcmp(mnt_name, "r0") == 0)
            return E_UNIT;

        len = strlen(pathName);
        if (ustrcmp(pathName, "r0") == 0 || len < lr0)
            return E_UNIT;
        q = pathName + len - lr0;
        if (ustrcmp(q, R0) == 0)
            return E_UNIT;
#endif

        err = syspath_open(pid, &sp, type, pathName, mode);
        if (err)
            return err;
        err = syspath_gs_size(pid, sp, &iSize);
        if (err)
            break;
        if (!RBF_ImgSize(iSize)) {
            err = E_FNA;
            break;
        }

        len = sizeof(bb);
        err = syspath_read(pid, sp, &len, &bb, false);
        if (err)
            break;
        err = syspath_seek(pid, sp, 0);
        if (err)
            break;

        /* Cruzli check */
        if (strcmp((char *)&bb[CRUZ_POS], Cruz_Str) != 0) {
            err = E_FNA;
            break;
        }
        l       = (ulong *)&bb[TOT_POS];
        totScts = os9_long(*l) >> BpB;
        w       = (ushort *)&bb[SECT_POS];
        sctSize = os9_word(*w);
        if (sctSize == 0)
            sctSize = STD_SECTSIZE;

        imgScts = iSize / sctSize;
        // upo_printf( "name1='%s' size=%d %d\n", pathname, imgScts, totScts );

        tSize = totScts * sctSize;
        if ((tSize % 2048) != 0 || tSize < 8192 || (iSize % sctSize) != 0) {
            err = E_FNA;
            break;
        }

        // upo_printf( "name2='%s' size=%d %d\n", pathname, imgScts, totScts );

        dev->sp_img = sp; /* do this before calling DevSize */
        err         = DevSize(dev);
    } while (false);

    if (err) {
        cer = syspath_close(0, sp);
        return err;
    }
    strcpy(dev->img_name, pathName);

    /* if not the complete name at the image */
    if (ustrcmp(&dev->img_name[1], dev->name) == 0) {
        strcpy(dev->img_name, pathName);
    }

    if (ustrcmp(&dev->img_name[1], dev->name) == 0) {
        strcpy(dev->img_name, startPath);
        MakeOS9Path(dev->img_name);
        strcat(dev->img_name, pathName);

        if (!FileFound(dev->img_name)) {
            strcpy(dev->img_name, strtUPath);
            MakeOS9Path(dev->img_name);
            strcat(dev->img_name, pathName);
        }
    }

    EatBack(dev->img_name);
    return 0;
}

Boolean InstalledDev(const char *os9path,
                     const char *curpath,
                     Boolean     fullsearch,
                     ushort     *cdv)
/* expect OS-9 notation */
{
    char        tmp[OS9PATHLEN];
    rbfdev_typ *dev;
    char       *p, *dvn;
    int         ii;

    strcpy(tmp, curpath); /* create the full pathname */
    strcat(tmp, PSEP_STR);
    if (AbsPath(os9path))
        strcpy(tmp, "");
    strcat(tmp, os9path);

    for (ii = 1; ii < MAXRBFDEV; ii++) { /* search for identical image */
        dev = &rbfdev[ii];
        if (dev->installed && /* test for same path or with sub path */
            SamePathBegin(tmp, dev->img_name)) {
            *cdv = ii;
            return true;
        }
    }

    p = tmp;
    if (*p == PSEP)
        p++;
    dvn = p;            /* this is what we are looking for  */
    while (*p != NUL) { /* cut raw device name or sub paths */
        if (*p == '@') {
            *p = NUL;
            break;
        }
        if (*p == PSEP) {
            if (fullsearch) {
                dvn = p + 1;
            }
            else {
                *p = NUL;
                break;
            }
        }
        p++;
    }

    if (*dvn == NUL)
        return false; /* no device */
    if (fullsearch && *mnt_name != NUL)
        dvn = mnt_name; /* in case of mount, compare to this */

    for (ii = 1; ii < MAXRBFDEV; ii++) { /* search a device */
        dev = &rbfdev[ii];               /* get RBF device  */
        if (dev->installed &&
            (ustrcmp(dvn, dev->name) == 0 || ustrcmp(dvn, dev->name2) == 0 ||
             ustrcmp(dvn, dev->name3) == 0) &&
            (SamePathBegin(&tmp[1], dev->name) ||
             SamePathBegin(&tmp[1], dev->name2) ||
             SamePathBegin(&tmp[1], dev->name3) ||
             SamePathBegin(tmp, dev->img_name))) {
            *cdv = ii;
            return true;
        }
    }

    return false;
}

static Boolean MWrong(int cdv)
/* check if already installed on a different device */
{
    rbfdev_typ *dev;
    int         ii;

    for (ii = 1; ii < MAXRBFDEV; ii++) {
        dev = &rbfdev[ii]; /* get RBF device  */
        if (dev->installed && ii != cdv &&
            (ustrcmp(mnt_name, dev->name) == 0 ||
             ustrcmp(mnt_name, dev->name2) == 0 ||
             ustrcmp(mnt_name, dev->name3) == 0))
            return true;
    }

    return false;
}

// #ifdef RAM_SUPPORT
static os9err PrepareRAM(ushort pid, rbfdev_typ *dev, char *cmp)
{
#define DefaultScts 8192
#define MaxKB 0x001ffffe // 2097151 kB = 2047.999 MB
#define SectsPerTrack 0x20

    os9err err, cErr;
    ulong  allocSize, allocN, allocClu, mapSize, f, r, fN, rN, totBits, tracks,
        cluRest, iSize;
    ulong    *u;
    ushort   *w;
    byte     *b;
    int       ii, v, clu = mnt_cluSize;
    byte      pt;
    mod_dev  *mod;
    char     *p;
    Boolean   ok = false;
    ptype_typ type;
    ushort    sp;

    if (strcmp(mnt_devCopy, "") != 0) {
        strcat(mnt_devCopy, "@");
        // upe_printf( "devCopy='%s'\n", mnt_devCopy );
        type = IO_Type(pid, mnt_devCopy, poDir);
        err  = syspath_open(pid, &sp, type, mnt_devCopy, poDir);
        if (err)
            return err;
        err = syspath_gs_size(pid, sp, &iSize);

        if (!err && iSize > 0) {
            // upe_printf( "size=%d\n", iSize );
            dev->ramBase = get_mem(iSize);
            if (dev->ramBase.host == NULL)
                return E_NORAM;
            err = syspath_read(pid, sp, &iSize, dev->ramBase.host, false);
        }

        cErr = syspath_close(pid, sp);
        if (!err)
            err = cErr;
        if (err)
            return err;

        u                = (ulong *)(rambase(dev) + TOT_POS);
        dev->totScts     = os9_long(*u) >> BpB;
        dev->imgScts     = dev->totScts;
        w                = (ushort *)(rambase(dev) + BIT_POS);
        dev->clusterSize = os9_word(*w);
        w                = (ushort *)(rambase(dev) + SECT_POS);
        dev->sctSize     = os9_word(*w);
        dev->sas         = DD__MINALLOC;
        return 0;
    }

    for (ii = 0; ii < 31; ii++) {
        if (1 << ii == clu) {
            ok = true;
            break;
        }
    }
    if (!ok) {
        upe_printf("mount: cluster size must be a power of 2\n");
        return 1;
    }

    if (mnt_ramSize > MaxKB) {
        upe_printf("mount: error - size is too large for this device.\n");
        return 1;
    }

    if (mnt_sctSize > 0)
        dev->sctSize = mnt_sctSize;
    dev->clusterSize = clu;
    dev->sas         = DD__MINALLOC;

    dev->totScts = mnt_ramSize * KByte / dev->sctSize; /* adapt to KBytes */
    tracks       = (dev->totScts - 1) / SectsPerTrack + 1;
    dev->totScts = SectsPerTrack * tracks; /* granulate to tracks */
    totBits      = (dev->totScts - 1) / clu + 1;
    dev->totScts = clu * totBits; /* granulate to clusters */
    if (dev->totScts == 0)
        dev->totScts = DefaultScts;

    if (mnt_ramSize == 0 && IsDesc(cmp, &mod, &p) && ustrcmp(p, "RBF") == 0) {
        p = (char *)mod + os9_word(mod->_mpdev);
        if (ustrcmp(p, "ram") == 0) {
            w            = (ushort *)(&mod->_mdtype + PD_SCT);
            dev->totScts = os9_word(*w);
        }
    }

    dev->imgScts = dev->totScts;

    mapSize = (totBits - 1) / BpB + 1; // rounding up
    if (mapSize > 0xffff) {
        while (mapSize > 0xffff) {
            mapSize = mapSize / 2;
            clu     = clu * 2;
        }
        upe_printf(
            "mount: error - cluster size is too small for this device.\n");
        upe_printf("cluster size must be at least %d.\n", clu);
        return 1;
    }

    allocSize = (totBits - 1) / (dev->sctSize * BpB) +
                1; // nr of allocation sectors, rounded up
    allocN = allocSize * dev->sctSize * BpB; // nr of allocation bits
    allocClu =
        allocSize / clu + 1; // nr of allocated clusters ( including sect 0 )

    dev->ramBase = get_mem(dev->sctSize * dev->totScts);
    if (dev->ramBase.host == NULL)
        return E_NORAM;
    memset(dev->ramBase.host, 0, dev->sctSize * dev->totScts); // clear all
    memcpy(dev->ramBase.host, RAM_zero, dev->sctSize);

    // f= allocClu*clu;

    // f= allocSize + 2; // strategy is a little bit strange
    // f= f / 2;
    // f= f * 2; // odd( f ) => + 1

    f  = allocSize + 1;
    fN = f * dev->sctSize; // root dir fd sector position
    r  = f + 1;
    rN = r * dev->sctSize;

    cluRest = r / clu + 1;
    cluRest = cluRest * clu - r;

    /* Reserve all these bits in the allocation map */
    pt = 0x80;
    for (ii = 0; ii < allocN; ii++) {
        if (ii <= r / clu || ii >= totBits) { // including fd + dir
            v = ii / BpB;
            b = (rambase(dev) + dev->sctSize + v);
            *b |= pt;
        }

        pt = pt / 2;
        if (pt == 0)
            pt = 0x80; /* prepare the next pattern */
    }

    uint8_t *base = rambase(dev);
    u             = (ulong *)(&base[TOT_POS]);
    *u = os9_long(dev->totScts << BpB);    /* 0x03 overwritten, is 0 anyway */
    base[TRK_POS]         = SectsPerTrack; /* number of sector per track */
    w                     = (ushort *)(&base[MAP_POS]);
    *w                    = (ushort)os9_word(mapSize);
    w                     = (ushort *)(&base[BIT_POS]);
    *w                    = (ushort)os9_word(clu);
    u                     = (ulong *)(&base[DIR_POS]);
    *u = os9_long(f << BpB); /* 0x0b overwritten, is 0 anyway */
    w                     = (ushort *)(&base[SECT_POS]);
    *w = (ushort)os9_word(dev->sctSize);

    base[fN]        = 0xbf; /* prepare the fd sector */
    base[fN + 0x08] = 0x01;
    base[fN + 0x0C] = 0x40;
    u               = (ulong *)(&base[fN + 0x10]);
    *u = os9_long(r << BpB); /* fN+0x14 overwritten, is 0 anyway */
    base[fN + 0x14] = cluRest;

    base[rN]                = 0x2e; /* prepare the directory entry */
    base[rN + 0x01]         = 0xae;
    w                       = (ushort *)(&base[rN + 0x1e]);
    *w                      = (ushort)os9_word(f);
    base[rN + 0x20]         = 0xae;
    w                       = (ushort *)(&base[rN + 0x3e]);
    *w                      = (ushort)os9_word(f);

    strcpy(dev->img_name, cmp);
    return 0;
}
// #endif

static os9err DeviceInit(ushort       pid,
                         rbfdev_typ **my_dev,
                         syspath_typ *spP,
                         ushort       cdv,
                         char        *pathname,
                         char        *curpath,
                         ushort       mode,
                         Boolean     *new_inst)
/* Make Connection to SCSI system or to an TBF image file */
{
    os9err err = 0;
    char cmp[OS9PATHLEN], ali[OS9PATHLEN], tmp[OS9PATHLEN], ers[20], *q, *p, *v;
    rbfdev_typ *dev;
    ptype_typ   type;
    size_t      ii, n;
    Boolean     abs, isSCSI, isRAMDisk = false, wProtect;
    Boolean     isFolder;

    process_typ *cp = &procs[pid];

    int    scsiID    = NO_SCSI;
    short  scsiAdapt = -1;
    short  scsiBus   = 0;
    short  scsiLUN   = 0;
    ushort scsiSsize = STD_SECTSIZE;
    ushort scsiSas   = 0;
    byte   scsiPDTyp = 0;

    Boolean mock   = *mnt_name != NUL;
    Boolean fu     = spP->fullsearch;
    Boolean fum    = fu && mock;
    Boolean opened = false;

    char rbfname[OS9PATHLEN];

    do {
        *new_inst = false;
        if (fu) {
            strcpy(tmp, curpath); /* create the full pathname */
            strcat(tmp, PSEP_STR);
            if (AbsPath(pathname))
                strcpy(tmp, "");
            strcat(tmp, pathname);
            strcpy(pathname, tmp);
        }

        strcpy(ali, "");       /* no alias defined by default */
        strcpy(cmp, pathname); /* default in case of error */

        abs = AbsPath(pathname);
        if (abs && InstalledDev(pathname, curpath, fu, &cdv)) {
            dev = &rbfdev[cdv];
            debugprintf(dbgFiles,
                        dbgNorm,
                        ("# DevInit: cdv=%d path='%s' img='%s'\n",
                         cdv,
                         pathname,
                         dev->img_name));
            if (!mock || ustrcmp(mnt_name, dev->name) == 0 ||
                ustrcmp(mnt_name, dev->name2) == 0 ||
                ustrcmp(mnt_name, dev->name3) == 0 ||
                !SamePathBegin(pathname, dev->img_name))
                break;

            /* already installed with a different name */
            if (*dev->name2 == NUL) {
                strcpy(dev->name2, mnt_name);
                break;
            }
            if (*dev->name3 == NUL) {
                strcpy(dev->name3, mnt_name);
                break;
            }
            return E_DEVBSY;
        }

        /* existing relative path ? */
        if (!abs && cdv != 0 && mnt_scsiID == NO_SCSI)
            break;

        err       = 0;
        isRAMDisk = false; /* do it for all conditions */

        if (!fu) {
#ifdef RAM_SUPPORT
            isRAMDisk = RAM_Device(pathname);
#endif

            if (!isRAMDisk) {
                err = GetRBFName(pathname, mode, &isFolder, (char *)&rbfname);

                /* must open it in the right mode */
                if (err == E_FNA && !IsDir(mode))
                    return err;
            }
        }

        if (isRAMDisk) {
            isSCSI = false; /* don't forget to set a default */
            GetOS9Dev(pathname, (char *)&cmp);
        }
        else {
            isSCSI = (err && mnt_scsiID != NO_SCSI);
            if (isSCSI) {
                scsiID    = mnt_scsiID;
                scsiAdapt = mnt_scsiAdapt >= 0 ? mnt_scsiAdapt : defSCSIAdaptNo;
                scsiBus   = mnt_scsiBus >= 0 ? mnt_scsiBus : defSCSIBusNo;
                scsiLUN   = mnt_scsiLUN >= 0 ? mnt_scsiLUN : 0;
            }
            else
                isSCSI = (err && SCSI_Device(pathname,
                                             &scsiAdapt,
                                             &scsiBus,
                                             &scsiID,
                                             &scsiLUN,
                                             &scsiSsize,
                                             &scsiSas,
                                             &scsiPDTyp,
                                             &type));
            if (isSCSI)
                GetOS9Dev(pathname, (char *)&cmp);
            else {
                if (fu)
                    CutPath(cmp);
                else {
                    if (err)
                        return E_UNIT; /* GetRBFName called earlier */
                    strcpy(cmp, rbfname);
                }
            }
        }

        if (!AbsPath(pathname)) {
            if (IsExec(mode))
                strcpy(tmp, cp->x.path);
            else
                strcpy(tmp, cp->d.path);
            if (*tmp != NUL)
                strcat(tmp, PSEP_STR); /* create the full path name */

            strcat(tmp, pathname);
            strcpy(pathname, tmp);
        }

        cdv = 0; /* now search a device */
        for (ii = 1; ii < MAXRBFDEV; ii++) {
            dev = &rbfdev[ii]; /* get RBF device  */
            if (!dev->installed) {
                if (cdv == 0)
                    cdv = ii; /* reserve empty one */
            }
            else {
                /* is there already a file with the same file name ? */
                debugprintf(dbgFiles,
                            dbgNorm,
                            ("# DevInit: path='%s' img='%s'\n",
                             pathname,
                             dev->img_name));
                if (SamePathBegin(pathname, dev->img_name)) {
                    if (strcmp(cmp, dev->name) != 0) {
                        strcpy(cmp, dev->name); /* take existing name */
                        *new_inst = true;       /* make a new instance */
                    }

                    cdv = ii;
                    break;
                }

                /* compare with mnt_name, if available */
                q = cmp;
                if (mock)
                    q = mnt_name;
                if (ustrcmp(q, dev->name) == 0 || ustrcmp(q, dev->name2) == 0 ||
                    ustrcmp(q, dev->name3) == 0) {
                    cdv = ii;
                    break;
                }

                if (isSCSI && IsSCSI(dev) && scsiID == dev->scsi.ID) {
                    if (*dev->name2 == NUL) {
                        strcpy(dev->name2, q);
                        cdv = ii;
                        break;
                    }
                    if (*dev->name3 == NUL) {
                        strcpy(dev->name3, q);
                        cdv = ii;
                        break;
                    }
                    return E_DEVBSY;
                }
            }
        }

        if (cdv == 0)
            return E_UNIT; /* no valid device found */
    } while (false);

    dev     = &rbfdev[cdv];
    *my_dev = dev;           /* activate RBF device */
    strcpy(dev->alias, ali); /* can change all the time */
    if (!dev->installed) {
        v = strstr(cmp, "@");
        if (v != NULL)
            *v = NUL;
        strcpy(dev->name, cmp);
    }

    q = dev->name; /* compare with mnt_name, if available */
    if (fum)
        q = mnt_name;
    debugprintf(dbgFiles,
                dbgNorm,
                ("# DevInit: '%s' (%d%s%s)\n",
                 q,
                 cdv,
                 dev->installed ? "/installed" : "",
                 *new_inst ? "/new_inst" : ""));

    // upo_printf( "DEVICEINIT='%s' img=%d tot=%d %d\n", pathname, dev->imgScts,
    // dev->totScts, dev->installed );
    if (dev->installed) {
        if (!abs)
            return 0; /* ok, if not absolute */
        if (mock && MWrong(cdv))
            return E_DEVBSY;

        strcpy(tmp, pathname);
        n = strlen(tmp) - 1;
        while (tmp[n] == '@')
            tmp[n--] = NUL;
        p = tmp + 1;
        if (SamePathBegin(tmp, dev->img_name) || SamePathBegin(p, dev->name) ||
            SamePathBegin(p, dev->name2) || SamePathBegin(p, dev->name3))
            return 0;

        /* already in use !! */
        //  printf( "path='%s' img='%s'\n", pathname,dev->img_name );
        //  printf( "path='%s'  n1='%s'\n", pathname,dev->name  );
        //  printf( "path='%s'  n2='%s'\n", pathname,dev->name2 );
        //  printf( "path='%s'  n3='%s'\n", pathname,dev->name3 );
        return E_DEVBSY;
    }

    dev->nr         = cdv;          /* already done ?? */
    dev->wProtected = mnt_wProtect; /* if not otherwise defined */
    dev->imgMode    = mnt_imgMode;
    dev->fProtected = false;
    dev->multiSct   = true;   /* is now supported */
    dev->currPos = UNDEF_POS; /* to make access faster: no seeks all the time */
    dev->last_alloc = 0;      /* initialize allocater pointer */
    dev->sp_img     = 0; /* the image syspath will be connected here later */
    dev->imgScts    = 0;
    dev->totScts    = 0; /* set default values */

    // #ifdef RAM_SUPPORT
    dev->isRAM = isRAMDisk; /* RAM disk flag */
                            // #endif

    strcpy(dev->img_name, ""); /* no  image name by default */
    strcpy(dev->name2, "");    /* no  2nd   name by default */
    strcpy(dev->name3, "");    /* no  3rd   name by default */

    dev->rMiss = 0; /* reset device statistics */
    dev->rTot  = 0;
    dev->wMiss = 0;
    dev->wTot  = 0;

    dev->scsi.adapt = scsiAdapt;
    dev->scsi.bus   = scsiBus;
    dev->scsi.ID    = scsiID;
    dev->scsi.LUN   = scsiLUN;
    dev->pdtyp      = scsiPDTyp;

    if (IsSCSI(dev)) {
        dev->sctSize = scsiSsize;
        dev->sas     = scsiSas;
    }
    else {
        dev->sctSize =
            STD_SECTSIZE; /* as OS-9 <=V2.3: const sectsize 256 bytes */
        if (ustrcmp(dev->name, "dd") == 0)
            dev->sas = DD__MINALLOC; /* use default value  */
        else
            dev->sas = RBF_MINALLOC; /* smaller for others */
    }

    // make sure we get a buffer that is big enough for reading any sector 0
    // (will be adjusted later)
    dev->tmp_sct = get_mem(dev->sctSize > MIN_TMP_SCT_SIZE ? dev->sctSize
                                                           : MIN_TMP_SCT_SIZE);

    debugprintf(dbgFiles,
                dbgNorm,
                ("# RBF open: trying to open %s \"%s\"\n",
                 IsSCSI(dev) ? "SCSI" : "RBF Image",
                 q));

    /* %%% currently hardwired */
    if (ustrcmp(cmp, "c0") == 0)
        dev->fProtected = false;
    if (ustrcmp(cmp, "c1") == 0)
        dev->fProtected = false;

    //  if (ustrcmp( cmp,"c0")==0 ||
    //      ustrcmp( cmp,"hx")==0) dev->multiSct= true;

    do {
        err            = 0;    /* set it as default   */
        dev->installed = true; /* now it is installed */
#ifdef RBF_CACHE
        Flush_FDCache(dev->name);
#endif

        // #ifdef RAM_SUPPORT
        if (isRAMDisk) {
            err = PrepareRAM(pid, dev, cmp);
            break; /* no more actions for RAM disk */
        }
        // #endif

        if (IsSCSI(dev))
            break; /* no more actions for SCSI */

        type     = IO_Type(pid, pathname, poRead);
        wProtect = mnt_wProtect;

        /* inherit write protection to sub device */
        if (!wProtect && type == fRBF &&
            InstalledDev(pathname, curpath, false, &cdv))
            wProtect = rbfdev[cdv].wProtected;

        /* try to open in read/write mode first (if not asking for wProtection
         */
        /* if not possible, open it readonly */
        if (!wProtect) {
            err = Open_Image(pid, dev, type, pathname, poUpdate);
            if (!err) {
                opened = true;
                break;
            }
        }
        err = Open_Image(pid, dev, type, pathname, poRead);
        if (!err) {
            opened          = true;
            dev->wProtected = true;
        }
    } while (false);

    do {
        if (err)
            break;
        err = RootLSN(pid, dev, spP, true);
        if (err)
            break;
        dev->last_diskID = spP->u.rbf.diskID;
        *new_inst        = true;
    } while (false);

    if (!err) {
        if ((dev->imgMode == Img_Reduced ||
             (dev->imgMode == Img_Unchanged && dev->imgScts < dev->totScts)) &&
            !dev->isRAM && !IsSCSI(dev)) {
            err = GetTop(pid, dev);
            // upo_printf( "TOPALLOC=%d of %d err=%d\n", dev->imgScts,
            // dev->totScts, err );
        }

        if (dev->imgMode == Img_FullSize) {
            err = GetFull(pid, dev);
        }
    }

    if (err) {
        if (opened)
            ReleaseIt(pid, dev);

        dev->installed = false;
        release_mem(dev->tmp_sct);
        dev->tmp_sct.host  = NULL;
        dev->tmp_sct.guest = 0;
    }

    sprintf(ers, "  #000:%03d", err);
    debugprintf(dbgFiles,
                dbgNorm,
                ("# RBF open: \"%s\" (%d%s%s)\n",
                 cmp,
                 cdv,
                 dev->installed ? "/installed" : "",
                 err ? ers : ""));
    return err;
}

static void mount_usage(char *name, _pid_)
{
    upe_printf("Syntax:   %s <image_file> [<device>]\n", name);
    upe_printf("Function: mount an RBF image file\n");
    upe_printf("Options:  \n");
    upe_printf("    -w           open with write protection on\n");
    upe_printf("    -i           adapt for reduced image size\n");
    upe_printf("    -f           adapt for full    image size\n");

    upe_printf("    -s=<scsiID>  connect to SCSI ID\n");
    upe_printf("    -l=<lun>     connect to LUN (default=0)\n");
    upe_printf("    -r=<size>    create RAM disk with size (in kBytes)\n");
    upe_printf("    -n=<bytes>   sector  size in bytes     for RAM disk\n");
    upe_printf("    -c=<num>     cluster size (default: 1) for RAM disk\n");
    upe_printf("    -d=<device>  create RAM disk as a copy of <device>\n");
}

os9err MountDev(ushort  pid,
                char   *name,
                char   *mnt_dev,
                char   *devCopy,
                short   adapt,
                ushort  scsibus,
                short   scsiID,
                ushort  scsiLUN,
                int     ramSize,
                int     sctSize,
                int     cluSize,
                Boolean wProtect,
                int     imgMode)
{
    os9err    err = 0;
    char      tmp[OS9NAMELEN];
    char      ch;
    int       ii;
    ptype_typ type;
    ushort    sp;

    /* it might come as Mac or DOS path name ... --> OS-9 notation */
    /* commented out: wil be done at "filestuff.c" */
    //  MakeOS9Path( name );

    /* Is there a different name for the mount device ? */
    /* OS9exec can't switch the task in-between */
    /* NOTE: mnt_name is only valid within this context here */
    if (*mnt_dev != NUL || scsiID != NO_SCSI || ramSize > 0 ||
        strcmp(devCopy, "") != 0) {
        mnt_name    = mnt_dev;
        mnt_ramSize = ramSize;
        mnt_sctSize = sctSize;
        mnt_cluSize = cluSize;
        mnt_devCopy = devCopy;

        if (scsiID != NO_SCSI) {
            mnt_name      = name;
            mnt_scsiID    = scsiID;
            mnt_scsiAdapt = adapt;
            mnt_scsiBus   = scsibus;
            mnt_scsiLUN   = scsiLUN;

            if (!AbsPath(name)) { /* make a device path */
                strcpy(tmp, PSEP_STR);
                strncat(tmp, name, OS9NAMELEN - 1);
                name = tmp;
            }
        }

        if (AbsPath(mnt_name))
            mnt_name++;
        for (ii = 0; ii < OS9NAMELEN; ii++) {
            ch = mnt_name[ii];
            if (ch == NUL)
                break; /* ok */
            if (ch == PSEP) {
                err = E_FNA;
                break;
            }
            if (ch < '0' && ch != '.') {
                err = E_BPNAM;
                break;
            }
        }

        if (err)
            name = mnt_name; /* for error output */
    }

    mnt_wProtect = wProtect;
    mnt_imgMode  = imgMode;

    if (!err) {
        type = fRBF;
        if (mnt_scsiID == NO_SCSI)
            type = IO_Type(pid, name, poDir);

        debugprintf(dbgUtils,
                    dbgNorm,
                    ("# mount: name='%s', mnt_name='%s' type='%s'\n",
                     name,
                     mnt_name,
                     TypeStr(type)));
        err = syspath_open(pid, &sp, type, name, poDir);
        debugprintf(dbgUtils,
                    dbgNorm,
                    ("# mount: name='%s', mnt_name='%s' err=%d\n",
                     name,
                     mnt_name,
                     err));
    }

    mnt_wProtect = false; /* switch them off again */
    mnt_imgMode  = Img_Unchanged;
    mnt_name     = "";
    mnt_scsiID   = NO_SCSI;
    mnt_ramSize  = 0;
    mnt_sctSize  = 0;
    mnt_cluSize  = 1;
    mnt_devCopy  = "";

    if (!err)
        err = syspath_close(pid, sp);
    return err;
}

os9err int_mount(ushort pid, int argc, char **argv)
/* mount an RBF image partition file */
{
    os9err  err     = 0;
    int     nargc   = 0;
    short   adapt   = defSCSIAdaptNo;
    short   scsibus = defSCSIBusNo;
    short   scsiID  = NO_SCSI;
    short   scsiLUN = 0;
    int     ramSize = 0;
    int     sctSize = 0;
    int     cluSize = 1;
    char    devCopy[OS9PATHLEN];
    Boolean wProtect = false;
    int     imgMode  = Img_Unchanged;
    char   *p;
    int     k;

#define MAXARGS_ 2
    char *nargv[MAXARGS_];
    strcpy(devCopy, ""); // default

    for (k = 1; k < argc; k++) {
        p = argv[k];
        if (*p == '-') {
            p++;
            switch (tolower(*p)) {
            case '?':
                mount_usage(argv[0], pid);
                return 0;

            case 'w':
                wProtect = true;
                break;
            case 'i':
                imgMode = Img_Reduced;
                break;
            case 'f':
                imgMode = Img_FullSize;
                break;

            case 's':
                if (*(p + 1) == '=')
                    p += 2;
                else {
                    k++; /* next arg */
                    if (k >= argc)
                        break;
                    p = argv[k];
                }
                if (sscanf(p, "%hd", &scsiID) < 1)
                    scsiID = NO_SCSI;
                break;

            case 'l':
                if (*(p + 1) == '=')
                    p += 2;
                else {
                    k++; /* next arg */
                    if (k >= argc)
                        break;
                    p = argv[k];
                }

                sscanf(p, "%hd", &scsiLUN);
                break;

            case 'r':
                if (*(p + 1) == '=')
                    p += 2;
                else {
                    k++; /* next arg */
                    if (k >= argc)
                        break;
                    p = argv[k];
                }

                sscanf(p, "%d", &ramSize);
                break;

            case 'n':
                if (*(p + 1) == '=')
                    p += 2;
                else {
                    k++; /* next arg */
                    if (k >= argc)
                        break;
                    p = argv[k];
                }

                sscanf(p, "%d", &sctSize);
                break;

            case 'c':
                if (*(p + 1) == '=')
                    p += 2;
                else {
                    k++; /* next arg */
                    if (k >= argc)
                        break;
                    p = argv[k];
                }

                sscanf(p, "%d", &cluSize);
                break;

            case 'd':
                if (*(p + 1) == '=')
                    p += 2;
                else {
                    k++; /* next arg */
                    if (k >= argc)
                        break;
                    p = argv[k];
                }

                strncpy(devCopy, p, OS9PATHLEN);
                break;

            default:
                upe_printf("Error: unknown option '%c'!\n", *p);
                mount_usage(argv[0], pid);
                return 1;
            }
        }
        else {
            if (nargc >= MAXARGS_) {
                upe_printf("Error: no more than %d arguments allowed\n",
                           MAXARGS_);
                return 1;
            }
            nargv[nargc++] = argv[k];
        }
    }

    if (nargc ==
        0) { /* no param is not really allowed: exception is ramDisk with */
        if (ramSize > 0 || /* size>0 or <devCopy> defined */
            strcmp(devCopy, "") != 0) {
            nargv[0] = "/r0";
            nargc    = 1;
        }
        else
            return _errmsg(E_BPNAM, "can't mount device \"\".\n");
    }

    /* nargv[0] is the name of the image to be mounted */
    /* nargv[1] is the name of the mounted device */
    err = MountDev(pid,
                   nargv[0],
                   nargc < 2 ? "" : nargv[1],
                   devCopy,
                   adapt,
                   scsibus,
                   scsiID,
                   scsiLUN,
                   ramSize,
                   sctSize,
                   cluSize,
                   wProtect,
                   imgMode);
    if (err)
        return _errmsg(err, "can't mount device \"%s\".\n", nargv[0]);
    return err;
}

static void unmount_usage(char *name, _pid_)
{
    upe_printf("Syntax:   %s <device>\n", name);
    upe_printf("Function: unmount an RBF image file\n");
    upe_printf("Options:  None\n");
}

os9err int_unmount(ushort pid, int argc, char **argv)
/* unmount an RBF image partition file */
{
    os9err err   = 0;
    int    nargc = 0, h;
    char  *p, *name;
#define MAXARGS 1
    char *nargv[MAXARGS];

    int          ii;
    syspath_typ *spP;
    rbfdev_typ  *dev;
    char         devName[OS9NAMELEN];

    for (h = 1; h < argc; h++) {
        p = argv[h];
        if (*p == '-') {
            p++;
            switch (tolower(*p)) {
            case '?':
                unmount_usage(argv[0], pid);
                return 0;
            default:
                upe_printf("Error: unknown option '%c'!\n", *p);
                unmount_usage(argv[0], pid);
                return 1;
            }
        }
        else {
            if (nargc >= MAXARGS) {
                upe_printf("Error: no more than %d arguments allowed\n",
                           MAXARGS);
                return 1;
            }
            nargv[nargc++] = argv[h];
        }
    }

    name = nargv[0];
    if (AbsPath(name))
        name++;

    do {
        for (ii = 0; ii < MAXSYSPATHS; ii++) {
            spP = get_syspathd(pid, ii);
            if (spP != NULL) {
                err = syspath_gs_devnm(pid, spP->nr, devName);
                if (err)
                    break;
                if (ustrcmp(devName, name) == 0) {
                    err = E_DEVBSY;
                    break;
                }
            }
        }
        if (err)
            break;

        err = E_UNIT;
        for (ii = 0; ii < MAXRBFDEV; ii++) {
            dev = &rbfdev[ii];
            if (dev->installed) { /* also name2 or name3 can be released now */
                if (ustrcmp(name, dev->name) == 0) {
                    if (*dev->name2 != NUL || *dev->name3 != NUL) {
                        strcpy(dev->name, dev->name2);
                        strcpy(dev->name2, dev->name3);
                        strcpy(dev->name3, "");
                        err = 0;
                        break;
                    }
                    err = ReleaseIt(pid, dev);
                    break;
                }

                if (ustrcmp(name, dev->name2) == 0) {
                    strcpy(dev->name2, dev->name3);
                    strcpy(dev->name3, "");
                    err = 0;
                    break;
                }

                if (ustrcmp(name, dev->name3) == 0) {
                    strcpy(dev->name3, "");
                    err = 0;
                    break;
                }
            }
        }
    } while (false);

    if (err)
        return _errmsg(err, "can't unmount device \"%s\".\n", name);
    return err;
}

static Boolean Mega(long long size, float *r)
{
#define MegaLim 10000.0;
    Boolean m;
    *r = size / KByte;
    m  = *r > MegaLim;
    if (m)
        *r = *r / KByte;
    return m;
} // Mega

static char *Kb(char *v, long long size)
{
    float r;
    char *unit;
    unit = "kB";
    if (Mega(size, &r))
        unit = "MB";

    if (r >= 1000) {
        sprintf(v, "%.0f%s", r, unit);
        return v;
    }
    if (r >= 100) {
        sprintf(v, "%.1f%s", r, unit);
        return v;
    }
    if (r >= 10) {
        sprintf(v, "%.2f%s", r, unit);
        return v;
    }
    sprintf(v, "%.3f%s", r, unit);
    return v;
} // Kb

static void Disp_RBF_DevsLine(rbfdev_typ *rb, char *name, Boolean statistic)
{
    char s[OS9NAMELEN];
    char u[OS9PATHLEN];
    char vI[20], vT[20];
    char v[40], w[20];

    long long sizeI = (long long)rb->imgScts * rb->sctSize;
    long long sizeT = (long long)rb->totScts * rb->sctSize;

    // Boolean isRAMDisk= false;
    // #ifdef RAM_SUPPORT
    //   isRAMDisk= rb->isRAM;
    // #endif

    strcpy(s, name);
    //  sprintf( sc,     "%3d", rb->scsiID   );
    strcpy(u, rb->img_name);

    if (rb->isRAM || *u == NUL)
        strcpy(u, " -");
    if (rb->isRAM)
        strcpy(w, "ram");
    else {
        if (IsSCSI(rb))
            sprintf(w, "SCSI: %d", rb->scsi.ID);
        else
            strcpy(w, "image");
    }

    Kb(vT, sizeT);
    if (sizeI == sizeT)
        sprintf(v, "(%s)", vT);
    else
        sprintf(v, "(%s/%s)", Kb(vI, sizeI), vT);

    upo_printf("%-10s ", StrBlk_Pt(s, 10));

    if (statistic)
        upo_printf("%10d /%10d   %10d /%10d\n",
                   rb->rMiss,
                   rb->rTot,
                   rb->wMiss,
                   rb->wTot);
    else
        upo_printf("%-8s %-7s %2d %4d %-3s %-21s %17s\n",
                   StrBlk_Pt(w, 7),
                   "rbf",
                   rb->nr,
                   rb->sctSize,
                   rb->wProtected ? "yes" : "no",
                   StrBlk_Pt(u, 21),
                   v);
}

void Disp_RBF_Devs(Boolean statistic)
{
    rbfdev_typ *r;

    int ii;
    for (ii = 0; ii < MAXRBFDEV; ii++) {
        r = &rbfdev[ii];
        if (r->installed) {
            Disp_RBF_DevsLine(r, r->name, statistic);
            if (*r->name2 != NUL)
                Disp_RBF_DevsLine(r, r->name2, statistic);
            if (*r->name3 != NUL)
                Disp_RBF_DevsLine(r, r->name3, statistic);
        }
    }
}

/* ------------------------------------------------------------------------------
 */

static void AdaptPath(rbfdev_typ *dev, char **pathP)
{
    char  tmp[OS9PATHLEN];
    char *p    = *pathP;
    size_t   ilen = strlen(dev->img_name);

    if (!AbsPath(p))
        return;

    if (*mnt_name != NUL &&
        (ustrcmp(mnt_name, dev->name) ==
             0 || /* search for all possible names */
         ustrcmp(mnt_name, dev->name2) == 0 ||
         ustrcmp(mnt_name, dev->name3) == 0) &&
        SamePathBegin(p, dev->img_name)) {
        strcpy(tmp, p);
        strcpy(p, PSEP_STR);
        strcat(p, dev->name);
        strcat(p, &tmp[ilen]);
    }

    while (*p != NUL) {   /* search through the string */
        if (*p == PSEP) { /* for a slash */
            if (*(++p) == NUL)
                break;

            /* start with a path delimiter */
            if (SamePathBegin(p, dev->name) || SamePathBegin(p, dev->name2) ||
                SamePathBegin(p, dev->name3) || SamePathBegin(p, dev->alias)) {
                *pathP = p - 1;
                break;
            }
        }

        p++;
    }
}

/* ---------------------------------------------------------------- */
static ulong DirLSN(os9direntry_typ *dir_entry)
{
    return os9_long(dir_entry->fdsect);
}

os9err ReadFD(syspath_typ *spP)
/* read the current file description sector */
{
    rbf_typ    *rbf = &spP->u.rbf;
    ulong       fd  = rbf->fd_nr;
    rbfdev_typ *dev = &rbfdev[rbf->devnr];

    if (fd != 0)
        return ReadSector(dev, fd, 1, spP->fd_sct.host);
    return 0;
}

static os9err WriteFD(syspath_typ *spP)
/* write the current file description sector */
{
    rbf_typ    *rbf = &spP->u.rbf;
    ulong       fd  = rbf->fd_nr;
    rbfdev_typ *dev = &rbfdev[rbf->devnr];

    if (fd != 0)
        return WriteSector(dev, fd, 1, spP->fd_sct.host);
    return 0;
}

static ulong FDSize(syspath_typ *spP)
/* get the file size  */
{
    uint8_t *fdsect = fd_sect(spP);
    ulong   *lp     = (ulong *)(&fdsect[9]);
    return os9_long(*lp);
}

static void Set_FDSize(syspath_typ *spP, ulong size)
/* set the file size  */
{
    uint8_t *fdsect = fd_sect(spP);
    ulong   *lp     = (ulong *)(&fdsect[9]);
    *lp       = os9_long(size);
}

static byte FDAtt(syspath_typ *spP)
/* get the file attributes  */ {
    uint8_t *fdsect = fd_sect(spP);
    return fdsect[0];
}

static void Set_FDAtt(syspath_typ *spP, byte att)
/* set the file attributes  */
/* but only if not root directory */
{
    rbf_typ    *rbf = &spP->u.rbf;
    rbfdev_typ *dev = &rbfdev[rbf->devnr];

    if (rbf->fd_nr != dev->root_fd_nr) {
        uint8_t *fdsect = fd_sect(spP);
        fdsect[0]       = att;
    }
}

static void Set_FDLnk(syspath_typ *spP, byte lnk)
/* set the link count */ {
    uint8_t *fdsect = fd_sect(spP);
    fdsect[8]       = lnk;
}

static os9err FD_Segment(syspath_typ *spP,
                         byte        *attr,
                         ulong       *size,
                         ulong       *totsize,
                         ulong       *sect,
                         ulong       *slim,
                         ulong       *pref)
{
    rbfdev_typ *dev = &rbfdev[spP->u.rbf.devnr];
    ulong      *lp, v, scs, blk, add, pos;
    ushort     *sp;
    int         ii;
    Boolean     done = false;

    *attr = FDAtt(spP);
    *size = FDSize(spP);
    *pref = 0; // preferred sector to allocate: none

    v  = 0;
    ii = FD_Header_Size;
    while (ii + SegSize <= dev->sctSize) {
        uint8_t *fdsect = fd_sect(spP);

        lp  = (ulong *)&fdsect[ii];
        pos = os9_long(*lp) >> BpB;
        sp  = (ushort *)&fdsect[ii + 3];
        scs = os9_word(*sp);
        blk = scs * dev->sctSize;

        if (!done && v + blk > spP->u.rbf.currPos) {
            add   = (spP->u.rbf.currPos - v) / dev->sctSize;
            *sect = pos + add;
            *slim = pos + scs;
            done  = true;
        }

        // if (scs>0) *pref= pos + scs;
        if (scs == 0)
            break;
        *pref = pos + scs;

        v += blk;
        ii += SegSize; // goto next entry
    }

    *totsize = v;
    debugprintf(dbgFiles,
                dbgDetail,
                ("# FD_Segment size/totsize/sect: %d %d $%x\n",
                 *size,
                 *totsize,
                 *sect));
    return 0;
}

static os9err GetThem(rbfdev_typ *dev, ulong pos, ulong scs, Boolean get_them)
/* Allocate the bits at the allocation map */
{
    os9err err;
    ulong  kk;

    ulong pdc  = pos / dev->clusterSize;
    ulong ii   = pdc / BpB;
    ulong jj   = pdc % BpB;
    ulong asct = ii / dev->sctSize + 1;
    ulong apos = ii % dev->sctSize;

    byte  imask, rslt;
    byte *ba;
    byte  mask = 0x80;
    while (jj > 0) {
        mask = mask / 2;
        jj--;
    }

    kk = scs;
    while (kk > 0) { /* use the device's temporary sector */
        debugprintf(dbgFiles,
                    dbgNorm,
                    ("# GetThem (%s) sectorNr: $%x, pos/scs: $%x %d\n",
                     get_them ? "true" : "false",
                     asct,
                     pos,
                     scs));
        err = ReadSector(dev, asct, 1, dev->tmp_sct.host);
        if (err)
            return err;

        while (kk > 0) {
            if (mask == 0) {
                mask = 0x80;
                apos++;
                if (apos == dev->sctSize)
                    break;
            }

            uint8_t *tmpsect = tmp_sect(dev);
            imask            = 255 - mask;     /* invert it */
            ba               = &tmpsect[apos]; /* current buffer field */
            if (get_them)
                rslt = *ba | mask; /* set   bit */
            else
                rslt = *ba & imask; /* clear bit */

            if (rslt == *ba)
                return E_NES;
            *ba = rslt;

            mask = mask / 2;
            kk -= dev->clusterSize;
        }

        err = WriteSector(dev, asct, 1, dev->tmp_sct.host);
        if (err)
            return err;
        asct++;   /* prepare for the next sector */
        apos = 0; /* and don't forget to reset position in sector */
    }             /* outer while */

    return 0;
}

static os9err BlkSearch(rbfdev_typ *dev,
                        ulong       uscs,
                        ulong       mpsct,
                        ulong       mploc,
                        Boolean    *found,
                        ulong      *pos,
                        ulong      *scs)
/* Search for a block with the given size       */
/* start searching in sector <mpsct> at <mploc> */
{
    os9err err;
    ulong  ii, jj;
    byte   vv, mask;
    int    taken;
    ulong  clu    = dev->clusterSize;
    ulong  sOffs  = dev->sctSize * (mpsct - 1) * BpB;
    ulong  posmax = *pos; /* max values are stored */
    ulong  scsmax = *scs;

    /* look for a new block now */
    *pos = clu * (sOffs + mploc * BpB);
    *scs = 0;

    *found = false;
    err    = ReadSector(dev, mpsct, 1, dev->tmp_sct.host);
    if (err)
        return err;

    for (ii = mploc; ii < dev->sctSize; ii++) { /* go through the sector */
        uint8_t *tmpsect = tmp_sect(dev);
        vv               = tmpsect[ii];
        mask = 0x80;

        for (jj = 0; jj <= 7; jj++) {
            taken = vv & mask; /* already taken ? */
            if (taken) {
                *scs = 0;
                *pos = clu * (sOffs + ii * BpB + jj + 1);
            }
            else
                (*scs) += clu;

            if (*scs >= uscs) {
                *found = true;
                return 0;
            }
            if (*scs > scsmax) {
                posmax = *pos;
                scsmax = *scs;
            }
            mask = mask / 2;
        }
    }

    *pos = posmax;
    *scs = scsmax;
    return 0;
}

static os9err AllocateBlocks(syspath_typ *spP,
                             ulong        uscs,
                             ulong       *posP,
                             ulong       *scsP,
                             ulong        prefpos)
/* allocate <uscs> sectors and. Result is <posP>,<scsP>. */
{
    os9err      err;
    rbfdev_typ *dev   = &rbfdev[spP->u.rbf.devnr];
    ulong       mxsct = (dev->mapSize - 1) / dev->sctSize + 1;
    ulong       mpsct, s1;
    ulong       pdc = prefpos / dev->clusterSize / BpB;
    ulong       lst = dev->last_alloc / dev->clusterSize / BpB;
    ulong       mploc;
    Boolean     first, found;

    *posP = 0;
    *scsP = 0;

    if (prefpos != 0) {
        mpsct = 1 + pdc / dev->sctSize;
        mploc = pdc % dev->sctSize;
        if (mpsct >= mxsct)
            mpsct = 1;

        err = BlkSearch(dev, uscs, mpsct, mploc, &found, posP, scsP);
        //  printf( "Ask=%5X get=%4X pos=%06X fnd=%d err=%d\n",
        //  uscs,*scsP,*posP, found, err );
        if (err)
            return err;
    }

    if (*scsP == 0 || *posP != prefpos) { /* no preferred allocation */
        mpsct = 1 + lst / dev->sctSize;
        mploc = lst % dev->sctSize;
        if (mpsct > mxsct)
            mpsct = 1;
        s1 = mpsct;

        first = true;
        while (true) { /* run through for one whole round */
            err = BlkSearch(dev, uscs, mpsct, mploc, &found, posP, scsP);
            //    printf( "ask=%5X get=%4X pos=%06X fnd=%d err=%d\n",
            //    uscs,*scsP,*posP, found, err );
            if (err)
                return err;
            if (found)
                break;

            if (!first && mpsct == s1)
                break; /* all done */
            if (mpsct < mxsct)
                mpsct++;
            else
                mpsct = 1;

            first = false;
            mploc = 0; /* from now on search from beginning */
        }              /* while (true) */
    }

    if (*scsP == 0)
        return E_FULL;

    // printf( "FND=%5X get=%4X pos=%06X\n", uscs,*scsP,*posP );
    dev->last_alloc = *posP + *scsP; /* next search position */
    err             = GetThem(dev, *posP, *scsP, true);
    // printf( "GetThem err=%d\n", err );
    return err;
}

static os9err DeallocateBlocks(syspath_typ *spP)
{
    rbfdev_typ *dev = &rbfdev[spP->u.rbf.devnr];
    ulong       fd  = spP->u.rbf.fd_nr;
    int         ii;
    ulong      *lp, pos;
    ushort     *sp, scs;

    for (ii = 16; ii + SegSize <= dev->sctSize; ii += SegSize) {
        uint8_t *fdsect = fd_sect(spP);
        lp              = (ulong *)&fdsect[ii];
        pos = os9_long(*lp) >> BpB;
        sp              = (ushort *)&fdsect[ii + 3];
        scs = os9_word(*sp);

        /* if the fd sector is allocated alone -> dealloc it alone */
        if (ii == 16 && fd + 1 != pos)
            GetThem(dev, fd, 1, false);

        if (scs > 0) {
            debugprintf(dbgFiles,
                        dbgNorm,
                        ("# Dealloate ii/pos/scs: %4d $%x %d\n", ii, pos, scs));
            if (ii == 16 && fd + 1 == pos) {
                pos--;
                scs++;
            }                              /* add the fd sector */
            GetThem(dev, pos, scs, false); /* release elements */

            if (dev->last_alloc > pos)
                dev->last_alloc = pos; /* next pos to search */
        }
    }

    return 0;
}

static os9err ReleaseBlocks(syspath_typ *spP, ulong lastPos)
{
    os9err      err;
    int         ii;
    rbfdev_typ *dev = &rbfdev[spP->u.rbf.devnr];
    // ulong       fd =         spP->u.rbf.fd_nr;
    ulong cmp = lastPos / dev->sctSize +
                1; /* including fd sector for cluster allocation */
    ulong   tps = 0;
    ulong  *lp, pos, diff;
    ushort *sp, scs;
    Boolean broken = false;

    // go through the fs segment list
    for (ii = 16; ii + SegSize <= dev->sctSize; ii += SegSize) {
        uint8_t *fdsect = fd_sect(spP);

        lp  = (ulong *)&fdsect[ii];
        pos = os9_long(*lp) >> BpB;
        sp  = (ushort *)&fdsect[ii + 3];
        scs = os9_word(*sp);

        if (scs == 0)
            broken = true;
        if (broken) {
            *lp = 0;
            *sp = 0;
            if (scs > 0)
                GetThem(dev, pos, scs, false);
        }
        else {
            tps += scs;
            if (tps > cmp) {
                diff = tps - cmp; /* keep track of granularity */
                diff = diff / dev->clusterSize * dev->clusterSize;
                if (diff >= scs) {
                    diff = scs;
                    *lp  = 0;
                    *sp  = 0;
                }
                else {
                    *sp = os9_word(scs - diff);
                }

                broken = true;
                GetThem(dev, (pos + scs) - diff, diff, false);
            }
        }
    }

    err = WriteFD(spP);
    return err;
}

static os9err AdaptAlloc_FD(syspath_typ *spP, ulong pos, ulong scs)
{
#define LimScsPerSegment                                                       \
    0x8000 /* number of sectors per segment must be less than this */
#define First 16
#define Second First + SegSize
    os9err      err;
    rbfdev_typ *dev = &rbfdev[spP->u.rbf.devnr];
    int         ii;
    ulong      *lp, prev_l, blk, mx;
    ushort     *sp, *prev_sp, psp, size = 0;
    ulong  lpos = pos; /* treat them locally, because 'Get_Them' uses it also */
    ushort lscs = scs;

    prev_sp = &size; /* initial value */
    for (ii = First; ii + SegSize <= dev->sctSize; ii += SegSize) {
        uint8_t *fdsect = fd_sect(spP);
        lp              = (ulong *)&fdsect[ii];
        sp              = (ushort *)&fdsect[ii + 3];

        if (*sp == 0) { /* zero is zero for big/little endian */
            psp = os9_word(*prev_sp);
            if (psp > 0 && prev_l + psp == lpos) { /* combine it */
                if (ii == Second)
                    mx = LimScsPerSegment -
                         1; /* assuming clusterSize is divideable */
                else
                    mx = LimScsPerSegment - dev->clusterSize;
                blk      = Min(psp + lscs,
                          mx); /* but not larger than the max allowed size */
                *prev_sp = os9_word(blk);

                lpos += blk - psp;
                lscs -= blk - psp; /* adjust the number of blocks */
            }

            if (lscs > 0) {
                if (ii == First)
                    mx = LimScsPerSegment -
                         1; /* assuming clusterSize is divideable */
                else
                    mx = LimScsPerSegment - dev->clusterSize;
                blk = Min(lscs, mx);

                *lp = os9_long(lpos << BpB);
                *sp = os9_word(blk); /* one byte overlapping */

                lpos += blk;
                lscs -= blk;
            }

            if (lscs == 0) {
                err = WriteFD(spP);
                if (err)
                    return err;
                return 0;
            }
        }

        prev_l  = os9_long(*lp) >> BpB;
        prev_sp = sp;
    }

    GetThem(dev, pos, scs, false);
    return E_SLF;
}

static os9err DoAccess(syspath_typ *spP,
                       ulong       *lenP,
                       char        *buffer,
                       Boolean      lnmode,
                       Boolean      wMode)
/* this is the main read routine for the RBF emulator */
/* it is working for read and readln */
{
    os9err      err    = 0;
    rbf_typ    *rbf    = &spP->u.rbf;
    rbfdev_typ *dev    = &rbfdev[rbf->devnr];
    ulong       bstart = rbf->currPos;
    ulong       boffs  = 0;
    ulong       remain = *lenP;
    ulong      *mw     = &spP->mustW;
    ulong       ma     = Max(dev->sas, dev->clusterSize);
    ushort     *w;
    ulong sect, slim, offs, size, totsize, maxc, pos, scs, *rs, pref, coff, sv,
        req;
    byte   *bb;
    byte    attr;
    byte   *bp;
    size_t  ii;
    Boolean done  = false;         // break condition for readln
    Boolean rOK   = (remain == 0); // is true, if nothing to read
    Boolean first = true;
    Boolean mlt, mltFirst;
    ulong   n, n0, d;
    byte   *b;

    debugprintf(dbgFiles,
                dbgDetail,
                ("# >DoAccess (%s): n=%d\n", wMode ? "write" : "read", *lenP));
    sv = rbf->currPos;

    do { // do this loop for every sector to be read into buffer
        if (spP->rawMode) {
            sect = rbf->currPos / dev->sctSize; // get raw sector nr
            slim = -1;
            size = -1;
            rs   = &size; // no upper limit
        }
        else { // get sector nr of segment info
            FD_Segment(spP, &attr, &size, &totsize, &sect, &slim, &pref);
            rs = &totsize; // the relevant size
            if (!wMode && size < totsize)
                rs = &size;

            if (totsize <= rbf->lastPos)
                rbf->lastPos = totsize;
            if (*rs <= rbf->lastPos)
                *rs = rbf->lastPos;

            if (!wMode) { // reduce to currect ammount
                if (*rs < rbf->currPos + remain)
                    remain = *rs - rbf->currPos;
            }

            if (rbf->currPos >= *rs) {
                if (wMode) {
                    req = rbf->currPos - *rs;
                    if (req > 0)
                        req =
                            (req - 1) / dev->sctSize + 1; // at least requested

                    while (true) {
                        scs = Max(ma, req); // alloc size might be larger
                        err = AllocateBlocks(spP, scs, &pos, &scs, pref);
                        if (err)
                            break;
                        err = AdaptAlloc_FD(spP, pos, scs);
                        if (err)
                            break;

                        if (scs >= req)
                            break;
                        req -= scs; // still asking for some more sectors
                    }               // loop
                    if (err)
                        break;

                    FD_Segment(spP,
                               &attr,
                               &size,
                               &totsize,
                               &sect,
                               &slim,
                               &pref);
                    rs = &totsize;
                    if (rbf->currPos >= *rs) {
                        debugprintf(dbgFiles,
                                    dbgDetail,
                                    ("# DoAccess (%s) EOF: n=%d\n",
                                     wMode ? "write" : "read",
                                     *lenP));
                        return E_EOF;
                    }
                }
                else {
                    if (rOK)
                        break; /* reading is ok so far */
                    err = E_EOF;
                    break;
                }
            }
        }

        offs = rbf->currPos % dev->sctSize; /* get offset within this sector */

        mlt = (dev->multiSct && sect != 0 && offs == 0 &&
               remain >= 2 * dev->sctSize);
        if (first) {
            mltFirst = mlt;
            first    = false;
        }

        if (mlt) {
            maxc = *rs;
            if (maxc > remain)
                maxc = remain;
            n = maxc / dev->sctSize;
            d = maxc % dev->sctSize;
            b = (byte *)(buffer + boffs);

            if (sect + n > slim)
                n = slim - sect; /* not more than a segment   */
            if (n > 255)
                n = 255; /* command for <=256 sectors */

            n0 = n;
            if (d == 0)
                n0--;
            maxc = dev->sctSize * n; /* don't take the last sector, if
                                        incomplete: could cause overflow */
        }
        else {
            maxc = dev->sctSize -
                   offs; /* calculate the max number of bytes to be read */
            if (maxc > remain)
                maxc = remain;
            n  = 1;
            n0 = 0;
            d  = 0;
            b  = spP->rw_sct.host;
        }

        // if (spP->rawMode && sect>=60 && sect<=90) {
        //   upo_printf( "DoAccess rw_nr=%d sect=%d *mw=%d mlt=%d\n",
        //   spP->rw_nr, sect, *mw, mlt );
        // }

        if (spP->rw_nr == 0 ||                /* read only if different one */
            spP->rw_nr != sect || mltFirst) { /* it is important to decide about
                                                 <mlt> of the first run */
            if (*mw != 0) {
                //  if (dev->multiSct) upe_printf( "Tsct slm n n0 offs d len %7d
                //  %7d %7d %7d %7d %7d %7d\n", sect,slim,n,n0,offs,d,*lenP );
                err = WriteSector(dev, *mw, 1, spP->rw_sct.host);
                if (err)
                    break;
                *mw = 0; /* now it is written */
            }

            if (!mlt || !wMode) {
                err = ReadSector(dev, sect, n, b);
                if (err)
                    break;
                //  if (dev->multiSct) upe_printf( "Rsct slm n n0 offs d len %7d
                //  %7d %7d %7d %7d %7d %7d\n", sect,slim,n,n0,offs,d,*lenP );
                spP->rw_nr = sect + n0;

                if (mlt) {
                    if (n > n0) { /* the full sector has been read */
                        memcpy(spP->rw_sct.host,
                               b + n0 * dev->sctSize,
                               dev->sctSize); /* update rw_sct */
                    }
                    else
                        spP->rw_nr = 0; /* there must be a follow up */
                }
                else {
                    if (sect == 0) {
                        err = ChkIntegrity(dev, spP, b, true);
                        if (err)
                            break;
                    }
                }
            }
        }

        rOK = true;
        debugprintf(dbgFiles,
                    dbgDeep,
                    ("# RBF %s: \"%s\" $%x bytes, sect: $%x, size: $%x\n",
                     wMode ? "write" : "read",
                     dev->name,
                     remain,
                     sect,
                     *rs));
        // if (sect==0) upe_printf( "Write 0 raw=%d\n", spP->rawMode );

        if (lnmode) { /* depends on read or write */
            if (wMode || mlt) {
                bb   = (byte *)buffer;
                coff = boffs;
            }
            else {
                bb   = spP->rw_sct.host;
                coff = offs;
            }

            for (ii = coff; ii < coff + maxc; ii++) {
                if (bb[ii] == CR) {
                    done = true;
                    maxc = ii - coff + 1;
                    break;
                }
            }
        }

        /* copy to/from the buffer */
        if (wMode) {
            if (!mlt) {
                memcpy(spP->rw_sct.host + offs, buffer + boffs, maxc);
            }

            if (*mw != 0 && *mw != sect) { /* if sector nr has changed */
                err = WriteSector(dev, *mw, 1, spP->rw_sct.host);
                if (err)
                    break;
                spP->rw_nr = *mw;
            }

            /* if sector in raw mode */
            if (spP->rawMode) {
                if (sect == 0) {
                    uint8_t *rwsect = rw_sect(spP);
                    w = (ushort *)&rwsect[14];      /* get the new disk ID */
                    dev->last_diskID = *w;          /* must be done */
                    rbf->diskID      = *w;

                    /* write sector 0 always */
                    err = WriteSector(dev, sect, 1, spP->rw_sct.host);
                    if (err)
                        break;
                }

                if (sect == 1)
                    dev->last_alloc = 0; /* reset after "format" */
            }

            *mw = sect;
            if (mlt) {
                err = WriteSector(dev, sect, n, b);
                if (err)
                    break;
                spP->rw_nr = sect + n0;
                *mw        = 0; /* already written */
            }
        }
        else {
            if (!mlt)
                memcpy(buffer + boffs, rw_sect(spP) + offs, maxc);
        }

        rbf->currPos += maxc; /* calculate the new position */

        bp    = (byte *)rbf->currPos - bstart;
        *lenP = (ulong)bp;

        if (done) {
            maxc = remain; /* loop is finished now because CR has been found */
        }
        else if (rbf->currPos > *rs) { /* remaining byte calculation */
            bp           = (byte *)*lenP + *rs - rbf->currPos;
            *lenP        = (ulong)bp;
            rbf->currPos = *rs;
        }

        remain -= maxc; /* adapt them for the next loop */
        boffs += maxc;
    } while (remain > 0);

    if (err == E_FULL) {
        rbf->currPos = sv;
        rbf->lastPos = sv;
    }

    if (rbf->lastPos < rbf->currPos) /* adapt lastpos */
        rbf->lastPos = rbf->currPos;

    debugprintf(dbgFiles,
                dbgDetail,
                ("# <DoAccess (%s): n=%d\n", wMode ? "write" : "read", *lenP));
    return err;
}

static os9err Create_FD(syspath_typ *spP, byte att, ulong size)
{
    rbfdev_typ *dev = &rbfdev[spP->u.rbf.devnr];
    uint8_t    *fdsect = fd_sect(spP);

    for (int ii = 0; ii < dev->sctSize; ii++) {
        /* clear sector */
        fdsect[ii] = 0;
    }

    Set_FDAtt(spP, att);   /* attributes */
    Set_FDLnk(spP, 1);     /* the link count */
    Set_FDSize(spP, size); /* file size  */
    return WriteFD(spP);   /* write FD sector */
}

static os9err OpenDir(rbfdev_typ *dev, ulong dfd, ushort *sp)
{
    os9err       err;
    syspath_typ *spP;
    rbf_typ     *rbf;

    err = syspath_new(sp, fRBF);
    if (err)
        return err;

    spP         = &syspaths[*sp];
    spP->fd_sct.host  = NULL; /* prepare them locally */
    spP->fd_sct.guest = 0;
    spP->rw_sct.host  = NULL;
    spP->rw_sct.guest = 0;
    spP->rw_nr  = 0;      /* undefined */
    GetBuffers(dev, spP); /* dev must be assigned before */

    rbf          = &spP->u.rbf;
    rbf->currPos = 0;    /* initialize position to 0 */
    rbf->wMode   = true; /* by default it can be written */
    rbf->devnr   = dev->nr;
    rbf->fd_nr   = dfd; /* the directory's sector */

    err = ReadFD(spP); /* IMPORTANT !! */
    if (err)
        ReleaseBuffers(spP);
    else
        rbf->lastPos = FDSize(spP); /* get the file size  */

    return err;
}

static os9err CloseDir(ushort sp)
{
    os9err       err;
    syspath_typ *spP = &syspaths[sp];

    spP->u.rbf.currPos = 0; /* don't release blocks */
    err                = syspath_close(0, sp);
    return err;
}

static void Fill_DirEntry(os9direntry_typ *dir_entry, char *name, ulong fd)
{
    byte *b = (byte *)dir_entry;
    int   ii;

    if (*name == NUL) {         /* if entry must be cleared */
        *dir_entry->name = NUL; /* clear string: do it the same way as OS-9 */
    }
    else {
        for (ii = 0; ii < DIRENTRYSZ; ii++) {
            *b = NUL;
            b++; /* Clear block */
        }

        /* write it only if new entry */
        strcpy(dir_entry->name, name);
        LastCh_Bit7(dir_entry->name, true);
        dir_entry->fdsect = os9_long(fd);
    }
}

static os9err
Access_DirEntry(rbfdev_typ *dev, ulong dfd, ulong fd, char *name, ulong *deptr)
{
    os9err          err, cer;
    ulong           dir_len;
    os9direntry_typ dir_entry;
    ushort          sp;
    syspath_typ    *spP;
    Boolean         found;
    Boolean         deleteIt = (fd == 0);
    char           *cmp, *fill;

    if (deleteIt) {
        cmp  = name;
        fill = "";
    }
    else {
        cmp  = "";
        fill = name;
    }

    err = OpenDir(dev, dfd, &sp);
    if (err)
        return err;
    spP = &syspaths[sp];

    while (true) {
        dir_len = DIRENTRYSZ; /* read 1 dir entry */
        err     = DoAccess(spP, &dir_len, (char *)&dir_entry, false, false);
        LastCh_Bit7(dir_entry.name, false);
        if (err) {
            if (err != E_EOF)
                break;
            if (deleteIt) {
                err = E_PNNF;
                break;
            }
        }

        found = (ustrcmp(cmp, dir_entry.name) == 0);
        if (found) /* make something with the entry */
            spP->u.rbf.currPos = spP->u.rbf.currPos - dir_len; /* seek back */

        if (err == E_EOF || found) {
            *deptr = spP->u.rbf.currPos;
            Fill_DirEntry(&dir_entry, fill, fd);
            dir_len = DIRENTRYSZ;
            err     = DoAccess(spP, &dir_len, (char *)&dir_entry, false, true);
            break;
        }
    }

    cer = CloseDir(sp);
    if (!err)
        err = cer;
    return err;
}

static os9err Delete_DirEntry(rbfdev_typ *dev, ulong fd, char *name)
{            /* file sector 0 deletes the file */
    ulong d; /* no interrest for this variable here */
    return Access_DirEntry(dev, fd, 0, name, &d);
}

static os9err touchfile_RBF(syspath_typ *spP, Boolean creDat)
{
    struct tm tim; /* Important Note: internal use of <tm> as done in OS-9 */

    GetTim(&tim);
    uint8_t *fdsect = fd_sect(spP);
    fdsect[3]       = tim.tm_year;
    fdsect[4]       = tim.tm_mon + 1; /* somewhat different month notation */
    fdsect[5]       = tim.tm_mday;
    fdsect[6]       = tim.tm_hour;
    fdsect[7]       = tim.tm_min;

    if (creDat) {
        fdsect[13] = tim.tm_year;
        fdsect[14] = tim.tm_mon + 1; /* somewhat different month notation */
        fdsect[15] = tim.tm_mday;
    }

    return WriteFD(spP);
}

static os9err
CreateNewFile(syspath_typ *spP, byte fileAtt, char *name, ulong csize)
{
    os9err      err;
    rbf_typ    *rbf = &spP->u.rbf;
    rbfdev_typ *dev = &rbfdev[rbf->devnr];
    ulong       sct = dev->sctSize;
    ulong       clu = dev->clusterSize;
    ulong       dfd = rbf->fd_nr;
    ulong      *d   = &rbf->deptr;
    ulong       fd, scs, ascs, sTmp;

    if (strlen(name) > DIRNAMSZ)
        return E_BPNAM;

    /* 1 fd + sectors according to cluster size */
    scs = (csize + sct - 1) / sct + 1;
    scs = (scs - 1) / clu + 1; /* and adapt for cluster granularity */
    scs = scs * clu;
    // printf( "a) scs=%d\n", scs );
    err = AllocateBlocks(spP, scs, &fd, &ascs, 0);
    if (err)
        return err;
    // printf( "c) err=%d, ascs=%d\n", err, ascs );
    spP->u.rbf.fd_nr = fd; /* access them correctly */
    spP->u.rbf.fddir = dfd;
    err              = Create_FD(spP, fileAtt, 0);
    if (err)
        return err;
    err = Access_DirEntry(dev, dfd, fd, name, d);
    if (err)
        return err;

    if (scs > 1) {   // Adaption for scs>1 only
        sTmp = ascs; // get a copy before changing
        fd++;
        ascs--; // get the remaining part
        err = AdaptAlloc_FD(spP, fd, ascs);
        if (err)
            return err;

        while (sTmp < scs) {
            // printf( "remania=%d\n", scs-sTmp );
            err = AllocateBlocks(spP, scs - sTmp, &fd, &ascs, 0);
            if (err)
                return err;
            err = AdaptAlloc_FD(spP, fd, ascs);
            if (err)
                return err;
            sTmp += ascs;
        }
    }

    err = touchfile_RBF(spP, true);
    if (!err)
        strcpy(spP->name, name); /* assign file name, if everything is ok */
    return err;
}

static os9err ConvertToDir(syspath_typ *spP)
{
    Set_FDAtt(spP, 0xbf); /* as directory */
    return WriteFD(spP);
}

/* ------------------------------------------------------------ */
/* visible procs */

os9err pRopen(ushort pid, syspath_typ *spP, ushort *modeP, const char *name)
{
    char            tmp[OS9PATHLEN];
    char           *pathname;
    rbf_typ        *rbf = &spP->u.rbf;
    rbfdev_typ     *dev;
    os9err          err = 0;
    ulong           sect, slim, size, totsize, dir_len, pref;
    os9direntry_typ dir_entry;
    char            cmp_entry[OS9NAMELEN];
    int             root, isFileEntry;
    char           *p;
    byte            attr;
    ushort          cdv;
    long            ls;
    process_typ    *cp     = &procs[pid];
    Boolean         isExec = IsExec(*modeP);
    Boolean         isFile = (*modeP & 0x80) == 0;
    Boolean         cre    = IsCrea(*modeP);
    // byte            mode  =       (*modeP & 0x47); /* attributes to be used
    // */
    char   *co = cre ? "create" : "open";
    char   *fo = isFile ? "file" : "dir";
    char   *curpath;
    Boolean new_inst;
    Boolean isAbs;

#ifdef RBF_CACHE
    // char            vvv[OS9PATHLEN];
    ulong           fd_hash;
    dirtable_entry *mP = NULL;
#endif

    strncpy(tmp, name, OS9PATHLEN);
    pathname = tmp;
    debugprintf(dbgFiles, dbgNorm, ("# RBF %s: '%s' (%s)\n", co, pathname, fo));

    rbf->currPos      = 0; /* initialize position to 0 */
    rbf->lastPos      = 0;
    rbf->flushFDCache = false;

    root = IsRoot(pathname); /* root path must be a directory */
    if (root && isFile)
        return E_FNA;

    /* get pointers to execution/current path */
    if (isExec) {
        cdv     = cp->x.dev;
        ls      = cp->x.lsn;
        curpath = cp->x.path;
    }
    else {
        cdv     = cp->d.dev;
        ls      = cp->d.lsn;
        curpath = cp->d.path;
    }

    err = DeviceInit(pid, &dev, spP, cdv, pathname, curpath, *modeP, &new_inst);
    if (err)
        return err;
    debugprintf(dbgFiles,
                dbgNorm,
                ("# RBF before adapt '%s' '%s'\n", pathname, mnt_name));
    AdaptPath(dev, &pathname); /* adapt to offical name */
    debugprintf(dbgFiles,
                dbgNorm,
                ("# RBF after  adapt '%s' '%s'\n", pathname, mnt_name));

    /* now the path could be a new root */
    root = IsRoot(pathname); /* root path must be a directory */
    if (root && isFile)
        return E_FNA;
    if (root && new_inst && *mnt_name != NUL) {
        strcpy(pathname, PSEP_STR);
        strcat(pathname, mnt_name);  /* the mount name can be different */
        strcpy(dev->name, mnt_name); /* to the root path name */
    }

    debugprintf(dbgFiles,
                dbgNorm,
                ("# RBF mount  adapt '%s' '%s' %d %d\n",
                 pathname,
                 mnt_name,
                 root,
                 new_inst));

    rbf->devnr  = dev->nr;
    rbf->diskID = dev->last_diskID;
    rbf->wMode  = IsWrite(*modeP);
    //  printf( "GetBuffers %08X %08X\n", spP->fd_sct, spP->rw_sct );
    GetBuffers(dev, spP); /* get the internal buffer structures now */
    spP->rw_nr = 0;       /* undefined */

    do {
        /* take care of write protection */
        if (cre && dev->wProtected) {
            err = E_WP;
            break;
        }

        p = pathname; /* that's it for raw mode */
        if (IsRaw(p)) {
            if (rbf->wMode) {
                rbf->flushFDCache =
                    true; // flush it in fact when closing the path
                // main_printf( "Flush RBF cache: pathname='%s' id=%d\n",
                // pathname, pid );
            }

            spP->rawMode = true;
            strcpy(spP->name, pathname);
            if (cre) {
                err = E_CEF;
                break;
            }
            return 0;
        } // if (IsRaw)

        isAbs = AbsPath(p); /* if abs path -> search from the root */

#ifdef RBF_CACHE
        if (isAbs || strcmp(curpath, "") == 0) {
            strcpy(spP->fullName, pathname);
        }
        else {
            strcpy(spP->fullName, curpath);
            strcat(spP->fullName, PSEP_STR);
            strcat(spP->fullName, pathname);
        }

        err = FD_ID(spP->fullName, NULL, &fd_hash, &mP);
        if (err)
            break;
// main_printf( "'%s' hash=%d fd=%06X\n", vvv, fd_rbf, mP->dirid );
#endif

        if (isAbs) {
            err = RootLSN(pid, dev, spP, false);
            if (err == E_DIDC)
                err = 0; /* changes recognized */
            if (err)
                break; /* for all other errors: break */

            err = ReadFD(spP);
            if (err)
                break;
            if (root) {
                strcpy(spP->name, pathname + 1);
                err = FD_Segment(spP,
                                 &attr,
                                 &size,
                                 &totsize,
                                 &sect,
                                 &slim,
                                 &pref);
                if (err)
                    break;
                rbf->lastPos = size; /* last pos is the filesize */
                rbf->att     = attr; /* save attributes */
                return 0;
            }
            p++; /* cut root path */
            err = CutOS9Path(&p, (char *)&cmp_entry);
            if (err)
                break;
        }
        else { /* if string is empty */
            if (*pathname == NUL) {
                err = E_FNA;
                break;
            }
            rbf->fd_nr = ls; /* take current path */
            err        = ReadFD(spP);
            if (err)
                break;
        }
    } while (false);
    if (err)
        return err;

    rbf->fddir = rbf->fd_nr;
    rbf->deptr = 0;
    err        = CutOS9Path(&p, (char *)&cmp_entry);
    if (err)
        return err;

    /* this is the recursion loop for directory entries */
    while (true) {
        dir_len = DIRENTRYSZ; /* read 1 dir entry */
        err     = DoAccess(spP, &dir_len, (char *)&dir_entry, false, false);
        if (err) {
            if (err == E_EOF) { /* do not create new sub paths !! */
                if (cre && strcmp(p, "") == 0) { /* create it ? */
                    err          = CreateNewFile(spP,
                                        procs[pid].fileAtt,
                                        (char *)&cmp_entry,
                                        procs[pid].cre_initsize);
                    rbf->currPos = 0; /* initialize position to 0 */
                    rbf->lastPos = 0;
                }
                else
                    err = E_PNNF; /* OS-9 expects E_PNNF, if entry not found */
            }
            break; /* leave loop also, if file has been correctly created */
        }

        debugprintf(
            dbgFiles,
            dbgDetail,
            ("# RBF path : \"%s\" \"%s\"\n", cmp_entry, dir_entry.name));
        LastCh_Bit7(dir_entry.name, false); /* normalize name, allow compare */
        if (ustrcmp(dir_entry.name, cmp_entry) ==
            0) { /* now the entry has been found */
            debugprintf(dbgFiles,
                        dbgNorm,
                        ("# RBF path found : \"%s\"\n", dir_entry.name));
            rbf->fddir   = rbf->fd_nr;
            rbf->deptr   = rbf->currPos - DIRENTRYSZ;
            rbf->currPos = 0; /* initialize position to 0 */
            rbf->lastPos = 0; /* last       position is 0 */

            rbf->fd_nr = DirLSN(&dir_entry);
            err        = ReadFD(spP);
            if (err)
                break;
            err = FD_Segment(spP, &attr, &size, &totsize, &sect, &slim, &pref);
            if (err)
                break;
            rbf->lastPos = size;                  /* last pos is the filesize */
            rbf->att     = attr;                  /* save attributes */
            isFileEntry  = (attr & 0x80) == 0x00; /* recognized as file entry */

            err = CutOS9Path(&p, (char *)&cmp_entry);
            if (err)
                break;

            if (*cmp_entry == NUL) { /* no more sub directories */
                strcpy(spP->name, dir_entry.name);

                if (isFileEntry) { /* if it is a file entry */
                    if (isFile) {
                        if (cre)
                            err = E_CEF; /* already there */
                        else
                            err = 0; /* is there as file -> ok */
                    }
                    else {
                        err = E_FNA; /* is path, should be file */
                        if (RBF_ImgSize(size) &&
                            !spP->fullsearch) { /* but try it again in special
                                                   mode */
                            spP->fullsearch = true;
                            debugprintf(
                                dbgFiles,
                                dbgNorm,
                                ("# not a path >>fullsearch '%s'\n", pathname));
                            err = pRopen(pid, spP, modeP, pathname);
                            debugprintf(
                                dbgFiles,
                                dbgNorm,
                                ("# not a path <<fullsearch '%s', err=%d\n",
                                 pathname,
                                 err));
                            spP->fullsearch = false;
                        }
                    }
                }
                else { /* if it is a dir  entry */
                    if (isFile)
                        err = E_FNA; /* is file, should be path */
                    else
                        err = 0; /* is there path -> ok */
                }

                if (!err && rbf->wMode && !dev->wProtected)
                    err = touchfile_RBF(spP, false);
                break;
            }
            else { /* more sub directories ? */
                if (isFileEntry) {
                    err = E_FNA;
                    break;
                }
            }
        }
    }

#ifdef RBF_CACHE
    if (!err) {
        if ((mP->dirid != 0 && mP->dirid != rbf->fd_nr) ||
            strcmp(spP->fullName, "") == 0) {
            main_printf("wrong '%s' %06X <> %06X\n",
                        spP->fullName,
                        mP->dirid,
                        rbf->fd_nr);
        }

        mP->dirid = rbf->fd_nr;
    }
#endif

    //  printf( "RelBuffers %08X %08X %d\n", spP->fd_sct, spP->rw_sct, err );
    if (err)
        ReleaseBuffers(spP);
    return err;
}

os9err pRclose(ushort pid, syspath_typ *spP)
{
    os9err      err = 0;
    rbf_typ    *rbf = &spP->u.rbf;
    rbfdev_typ *dev = &rbfdev[rbf->devnr];
    ulong       crp = rbf->currPos;
    ulong       lsp = rbf->lastPos;
    ulong       v;

    debugprintf(dbgFiles,
                dbgNorm,
                ("# RBF close (pid=%d) wMode/rawMode/mustW: %d %d $%x\n",
                 pid,
                 rbf->wMode,
                 spP->rawMode,
                 spP->mustW));

    do {
        if (rbf->wMode == 0)
            break; /* no change in read mode */

        if (spP->mustW) { /* write the last cached sector */
            err = WriteSector(dev, spP->mustW, 1, spP->rw_sct.host);
            if (err)
                break;
        }

        if (spP->rawMode)
            break; // no more things to do

        /* set file size at close */
        v = FDSize(spP);
        if (v < lsp) {
            v = lsp;
            Set_FDSize(spP, v); /* new file size */
            err = WriteFD(spP);
            if (err)
                break;
        }

        /* release remaining part, if pointer is not at the end of file */
        if (crp != 0 && crp == lsp) {
            Set_FDSize(spP, crp); /* new file size */
            err = ReleaseBlocks(spP, crp);
        }
    } while (false);

#ifdef RBF_CACHE
    if (rbf->flushFDCache)
        Flush_FDCache(dev->name);
#endif

    ReleaseBuffers(spP);
    return err;
}

os9err pRread(_pid_, syspath_typ *spP, ulong *lenP, char *buffer)
{
    return DoAccess(spP, lenP, buffer, false, false);
}

os9err pRreadln(_pid_, syspath_typ *spP, ulong *lenP, char *buffer)
{
    return DoAccess(spP, lenP, buffer, true, false);
}

os9err pRwrite(_pid_, syspath_typ *spP, ulong *lenP, char *buffer)
{
    return DoAccess(spP, lenP, buffer, false, true);
}

os9err pRwriteln(_pid_, syspath_typ *spP, ulong *lenP, char *buffer)
{
    return DoAccess(spP, lenP, buffer, true, true);
}

os9err pRseek(_pid_, syspath_typ *spP, ulong *posP)
/* seek to new file position <posP> */
{
    rbfdev_typ *dev = &rbfdev[spP->u.rbf.devnr];
    debugprintf(dbgFiles,
                dbgNorm,
                ("# RBF seek: dev='%s' file='%s' pos: %x\n",
                 dev->name,
                 spP->name,
                 *posP));
    spP->u.rbf.currPos = *posP;

    // if (*posP==0x2a26) // debugging special alloc problem
    //   upe_printf( "hallo\n" );

    return 0;
}

os9err pRchd(ushort pid, syspath_typ *spP, ushort *modeP, char *pathname)
{
    os9err       err;
    ushort       path;
    long        *xD;
    short       *xV;
    Boolean      exedir = IsExec(*modeP);
    process_typ *cp     = &procs[pid];
    rbfdev_typ  *dev;
    size_t       n;
    char        *curpath;
    char         tmp[OS9PATHLEN];

    /* get current default path */
    if (exedir) {
        xV      = (short *)&cp->x.dev;
        xD      = (long *)&cp->x.lsn;
        curpath = cp->x.path;
    }
    else {
        xV      = (short *)&cp->d.dev;
        xD      = (long *)&cp->d.lsn;
        curpath = cp->d.path;
    }

    err = usrpath_open(pid, &path, fRBF, pathname, *modeP);
    if (err)
        return err;
    spP = get_syspathd(pid, cp->usrpaths[path]);
    if (spP == NULL)
        return os9error(E_BPNUM);

    *xV = spP->u.rbf.devnr;
    dev = &rbfdev[*xV];
    *xD = spP->u.rbf.fd_nr;

    if (AbsPath(pathname))
        strcpy(curpath, "");
    else
        strcat(curpath, PSEP_STR);
    strcat(curpath, pathname);

    n = strlen(dev->img_name);
    if (SamePathBegin(curpath, dev->img_name)) {
        strcpy(tmp, curpath);
        strcpy(curpath, PSEP_STR); /* get a str staring with dev->name instead
                                      of dev->img_name */
        strcat(curpath, dev->name);
        strcat(curpath, &tmp[n]);
    }
    EatBack(curpath);

    err = usrpath_close(pid, path);
    if (err)
        return err;

    if (exedir)
        cp->x.type = fRBF; /* adapt the type */
    else
        cp->d.type = fRBF;

    return 0;
}

os9err pRdelete(ushort pid, syspath_typ *spP, ushort *modeP, char *pathname)
{
    os9err      err, cer;
    rbfdev_typ *dev;
    ushort      path;
    ulong       dfd;

#ifdef RBF_CACHE
    ulong           fd_hash;
    dirtable_entry *mP = NULL;
#endif

    err = usrpath_open(pid, &path, fRBF, pathname, *modeP);
    if (err)
        return err;
    spP =
        get_syspath(pid, procs[pid].usrpaths[path]); /* get spP for fd sects */
    if (spP == NULL)
        return os9error(E_BPNUM);

    dev = &rbfdev[spP->u.rbf.devnr]; /* can't be assigned earlier */
    dfd = spP->u.rbf.fddir;

    do {
        err = Delete_DirEntry(dev, dfd, (char *)&spP->name);
        if (err)
            break;
        err = DeallocateBlocks(spP);
        if (err)
            break;

#ifdef RBF_CACHE
        err = FD_ID(spP->fullName, NULL, &fd_hash, &mP);
        if (err)
            break;
        mP->dirid = 0; // invalidate;
#endif

        spP->u.rbf.currPos =
            0; /* seek back to zero, avoid release remaining part */
    } while (false);

    cer = usrpath_close(pid, path);
    if (!err)
        err = cer;
    return err;
}

os9err pRmakdir(ushort pid, syspath_typ *spP, _modeP_, char *pathname)
{
    os9err          err;
    ulong           size = 2 * DIRENTRYSZ;
    os9direntry_typ dirblk[2];
    ushort          path;

    procs[pid].fileAtt      = 0xBF;
    procs[pid].cre_initsize = 0; /* don't use mode at the moment */
    err = usrpath_open(pid, &path, fRBF, pathname, 0x03 | poCreateMask);
    if (err)
        return err;

    spP =
        get_syspath(pid, procs[pid].usrpaths[path]); /* get spP for fd sects */
    if (spP == NULL)
        return os9error(E_BPNUM);

    /* prepare the contents */
    Fill_DirEntry(&dirblk[0], "..", spP->u.rbf.fddir);
    Fill_DirEntry(&dirblk[1], ".", spP->u.rbf.fd_nr);

    err = usrpath_write(pid, path, &size, &dirblk, false);
    if (err)
        return err;
    spP->u.rbf.currPos =
        0; /* seek back to zero, avoid release remaining part */
    err = ConvertToDir(spP);
    if (err)
        return err;
    err = usrpath_close(pid, path);
    return err;
}

os9err pRpos(_pid_, syspath_typ *spP, ulong *posP)
/* get current file position <posP> */
{
    rbf_typ    *rbf = &spP->u.rbf;
    rbfdev_typ *dev = &rbfdev[rbf->devnr];

    *posP = rbf->currPos;
    debugprintf(dbgFiles, dbgNorm, ("# RBF pos: '%s' %d\n", dev->name, *posP));
    return 0;
}

os9err pReof(_pid_, syspath_typ *spP)
/* get current file position <posP> */
{
    rbf_typ    *rbf   = &spP->u.rbf;
    rbfdev_typ *dev   = &rbfdev[rbf->devnr];
    Boolean     isEOF = rbf->currPos >= rbf->lastPos;

    debugprintf(dbgFiles,
                dbgNorm,
                ("# RBF eof: '%s' %s\n", dev->name, isEOF ? "true" : "false"));

    if (isEOF)
        return os9error(E_EOF);
    else
        return 0;
}

/* get options for RBF file */
os9err pRopt(ushort pid, syspath_typ *spP, byte *buffer)
{
    os9err      err  = pRBFopt(pid, spP, buffer);
    rbf_typ    *rbf  = &spP->u.rbf;
    rbfdev_typ *dev  = &rbfdev[rbf->devnr];
    ulong       sSct = dev->sctSize;
    byte       *b;
    ushort     *w;
    ulong      *l;
    char       *c;

    if (sSct == 0)
        sSct = STD_SECTSIZE; /* the std way how to handle unknow sector sizes */

    /* and fill some specific RBF path values */
    b  = (byte *)&buffer[PD_TYP];
    *b = dev->pdtyp;
    w  = (ushort *)&buffer[PD_SAS];
    *w = os9_word(dev->sas); /* sector alloc size */
    w  = (ushort *)&buffer[PD_SSize];
    *w = os9_word(sSct); /* phys sect size    */
    b  = (byte *)&buffer[PD_CtrlrID];
    *b = dev->scsi.ID;
    b  = (byte *)&buffer[PD_ATT];
    *b = rbf->att;

    l  = (ulong *)&buffer[PD_FD];
    *l = os9_long(rbf->fd_nr * sSct); /* pos of file       */
    l  = (ulong *)&buffer[PD_DFD];
    *l = os9_long(rbf->fddir * sSct); /* pos of its dir    */

    l  = (ulong *)&buffer[PD_DCP];
    *l = os9_long(rbf->deptr); /* dir entry pointer */
    l  = (ulong *)&buffer[PD_DVT];
    l  = (ulong *)&buffer[PD_SctSiz];
    *l = os9_long(sSct); /* logical sect size */
    c  = (char *)&buffer[PD_NAME];
    strcpy(c, spP->name); /* name */

    return err;
}

os9err pRready(_pid_, _spP_, ulong *n)
/* check ready */
{
    *n = 1;
    return 0;
}

os9err pRgetFD(_pid_, syspath_typ *spP, ulong *maxbytP, byte *buffer)
/* get the current FD sector of the opened path */
{
    debugprintf(
        dbgFiles,
        dbgNorm,
        ("# RBF getFD (fd/bytes): $%x %d\n", spP->u.rbf.fd_nr, *maxbytP));

    memcpy(buffer, spP->fd_sct.host, *maxbytP); /* copy to the buffer */
    return 0;
}

os9err
pRgetFDInf(_pid_, syspath_typ *spP, ulong *maxbytP, ulong *fdinf, byte *buffer)
/* get any FD sector ( using variable <fdinf> ) */
{
    os9err      err;
    rbfdev_typ *dev = &rbfdev[spP->u.rbf.devnr];

    debugprintf(dbgFiles,
                dbgNorm,
                ("# RBF getFDInf (fd/bytes): $%x %d\n", *fdinf, *maxbytP));

    err = ReadSector(dev, *fdinf, 1, dev->tmp_sct.host);
    if (err)
        return err;
    memcpy(buffer, dev->tmp_sct.host, *maxbytP); /* copy to the buffer */
    return 0;
}

os9err pRsetFD(_pid_, syspath_typ *spP, byte *buffer)
/* set the current FD sector */
{
    ulong maxbyt = 16;
    debugprintf(dbgFiles,
                dbgNorm,
                ("# RBF setFD (fd/bytes): $%x %d\n", spP->u.rbf.fd_nr, maxbyt));

    memcpy(spP->fd_sct.host, buffer, maxbyt); /* copy to the buffer */
    return WriteFD(spP);
}

os9err pRsize(ushort pid, syspath_typ *spP, ulong *sizeP)
/* get the size of a file */
{
    os9err      err;
    rbf_typ    *rbf = &spP->u.rbf;
    rbfdev_typ *dev = &rbfdev[rbf->devnr];
    byte        attr;
    ulong       sect, slim, totsize, sv, pref;

    if (spP->rawMode) {
        *sizeP = dev->totScts * dev->sctSize;
        return 0;
    }

    sv           = rbf->currPos;
    rbf->currPos = 0; /* initialize position to 0 */

    err = FD_Segment(spP, &attr, sizeP, &totsize, &sect, &slim, &pref);

    rbf->currPos = sv; /* get saved position back */
    if (rbf->lastPos > *sizeP)
        *sizeP = rbf->lastPos;

    debugprintf(dbgFiles, dbgNorm, ("# RBF size (pid=%d): %d\n", pid, *sizeP));
    return err;
}

os9err pRdsize(ushort pid, syspath_typ *spP, ulong *size, ulong *dtype)
/* get the size of the device as numbers of sectors */
/* the <dtype> field will be returned as 0, to avoid problems with "castype" */
{
    os9err      err;
    rbfdev_typ *dev = &rbfdev[spP->u.rbf.devnr];

    err = DevSize(dev);
    if (err)
        return err;
    *size  = dev->totScts;
    *dtype = 0;

    debugprintf(dbgFiles, dbgNorm, ("# RBF dsize (pid=%d): %d\n", pid, *size));
    return 0;
}

os9err pRsetsz(_pid_, syspath_typ *spP, ulong *size)
/* set the size of a file */
{
    rbf_typ *rbf = &spP->u.rbf;

    rbf->lastPos = *size; /* set position      to new max */
    if (rbf->currPos > *size)
        rbf->currPos = *size; /* set position back to new max */

    Set_FDSize(spP, *size); /* new file size */
    return WriteFD(spP);
}

os9err pRsetatt(_pid_, syspath_typ *spP, ulong *attr)
/* set the attributes of a file */
{
    Set_FDAtt(spP, (byte)*attr); /* byte ordering is already correct */
    return WriteFD(spP);
}

os9err pRnam(ushort pid, syspath_typ *spP, char *volname)
/* get device name of RBF device */
{
    process_typ *cp  = &procs[pid];
    rbfdev_typ  *dev = &rbfdev[spP->u.rbf.devnr];

    strcpy(volname, dev->name);
    if (SamePathBegin(&cp->d.path[1], dev->name2))
        strcpy(volname, dev->name2);
    if (SamePathBegin(&cp->d.path[1], dev->name3))
        strcpy(volname, dev->name3);

    debugprintf(dbgFiles, dbgNorm, ("# RBF name: %s\n", volname));
    return 0;
}

os9err pRWTrk(ushort pid, syspath_typ *spP, ulong *trackNr)
/* get device name of RBF device */
{
    os9err      err;
    rbfdev_typ *dev = &rbfdev[spP->u.rbf.devnr];
    int         ii;
    ulong       sctNr, scts, dtype;

    uint8_t *tmpsect = tmp_sect(dev);
    for (ii = 0; ii < dev->sctSize; ii++)
        tmpsect[ii] = 0xE5; /* fill with formatting pattern */

    err = pRdsize(pid, spP, &scts, &dtype);

    for (ii = 0; ii < scts; ii++) {
        sctNr = *trackNr * DEFAULT_SCT + ii;
        err   = WriteSector(dev, sctNr, 1, dev->tmp_sct.host);
        if (err)
            return err;
    }

    return 0;
}

/* eof */
