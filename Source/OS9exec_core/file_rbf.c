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
/* (c) 1993-2002 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        forsterb@dial.eunet.ch              */
/**********************************************/


/* This file contains the RBF Emulator */
#include "os9exec_incl.h"
#include "filescsi.h"

#define  SegSize 5


/* the RBF device entry itself */			
typedef struct {
			/* common for all types */
			char    name [OS9NAMELEN];	  /* device         name */
			char    name2[OS9NAMELEN];	  /* device 2nd     name */
			char    name3[OS9NAMELEN];	  /* device 3rd     name %%% dirty solution */
			char    alias[OS9PATHLEN];	  /* device's alias name */

			int     nr;					  /* own reference number (array index) */
			ulong   sctSize;			  /* sector size for this device */
			ushort  mapSize;              /* size of allocation map */
			byte    pdtyp;                /* device type: hard disk, floppy */
			ushort  sas;				  /* sector allocation size */
			ulong   root_fd_nr;           /* sector nr of root fd */
			ulong   clusterSize;          /* cluster size (allocation) */
			ulong   totScts;              /* total number of sectors */
			ushort  last_diskID;          /* last disk ID, inherited by new paths */
            ulong   last_alloc;           /* the last allocation was here */
			ulong	currPos;			  /* current position at image */
			ulong   rMiss, rTot,		  /* device statistics */
					wMiss, wTot;
			ushort  sp_img;			      /* syspath number of image file */
			char	img_name[OS9PATHLEN]; /* full path name of image file */
	
			byte*   tmp_sct; 			  /* temporary buffer sector */
			Boolean wProtected;			  /* true, if write  protected */
			Boolean fProtected;			  /* true, if format protected */
			Boolean multiSct;             /* true, if multi sector support */
			
			Boolean isRAM;                /* true, if RAM disk */
            byte*   ramBase;              /* start address of RAM disk */
            
			int	    scsiID;               /* SCSI ID, -1 if NO_SCSI */
			short   scsiAdapt;            /* SCSI Adaptor, -1 if none */
			short   scsiBus;              /* SCSI Bus, normally 0 */
			short   scsiLUN;              /* SCSI LUN, normally 0 */

			Boolean installed;			  /* true, if device is already installed */
		} rbfdev_typ;

/* the RBF devices */
rbfdev_typ  rbfdev[MAXRBFDEV];		



/* OS9exec builtin module, defined as constant array */
const byte RAM_zero[] = {
    0x00,0x20,0x00,0x00,0x04,0x00,0x00,0x01,0x00,0x00,0x05,0x00,0x00,0xbf,0x00,0x00,  // . ...........?..
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x07,0x03,0x14,0x2d,0x52,  // ..............-R
    0x61,0x6d,0x20,0x44,0x69,0x73,0x6b,0x20,0x28,0x43,0x61,0x75,0x74,0x69,0x6f,0x6e,  // am Disk (Caution
    0x3a,0x20,0x56,0x6f,0x6c,0x61,0x74,0x69,0x6c,0x65,0xa9,0x00,0x00,0x00,0x00,0x00,  // : Volatile).....
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // ................
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // ................
    0x43,0x72,0x75,0x7a,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,  // Cruz............
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // ................
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // ................
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // ................
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // ................
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // ................
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // ................
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // ................
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // ................
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00   // ................
};    




/* --- local procedure definitions for object definition ------------------- */
void   init_RBF  ( fmgr_typ* f );

os9err pRopen    ( ushort pid, syspath_typ*, ushort *modeP,   const char* pathname );
os9err pRclose   ( ushort pid, syspath_typ* );
os9err pRread    ( ushort pid, syspath_typ*, ulong  *lenP,    char* buffer );
os9err pRreadln  ( ushort pid, syspath_typ*, ulong  *lenP,    char* buffer );
os9err pRwrite   ( ushort pid, syspath_typ*, ulong  *lenP,    char* buffer );
os9err pRwriteln ( ushort pid, syspath_typ*, ulong  *lenP,    char* buffer );
os9err pRseek    ( ushort pid, syspath_typ*, ulong  *posP );
os9err pRchd     ( ushort pid, syspath_typ*, ushort *modeP,   char* pathname );
os9err pRdelete  ( ushort pid, syspath_typ*, ushort *modeP,   char* pathname );
os9err pRmakdir  ( ushort pid, syspath_typ*, ushort *modeP,   char* pathname );

os9err pRsize    ( ushort pid, syspath_typ*, ulong  *sizeP );
os9err pRopt     ( ushort pid, syspath_typ*,                  byte* buffer );
os9err pRnam     ( ushort pid, syspath_typ*,                  char* volname );
os9err pRpos     ( ushort pid, syspath_typ*, ulong  *posP  );
os9err pRready   ( ushort pid, syspath_typ*, ulong  *n     );
os9err pRgetFD   ( ushort pid, syspath_typ*, ulong  *maxbytP, byte* buffer );
os9err pRgetFDInf( ushort pid, syspath_typ*, ulong  *maxbytP,
                                             ulong  *fdinf,   byte* buffer );
os9err pRdsize   ( ushort pid, syspath_typ*, ulong  *size,    ulong *dtype );

os9err pRsetsz   ( ushort pid, syspath_typ*, ulong  *size );
os9err pRsetatt  ( ushort pid, syspath_typ*, ulong  *attr  );
os9err pRsetFD   ( ushort pid, syspath_typ*,                  byte* buffer );
os9err pRWTrk    ( ushort pid, syspath_typ*, ulong  *trackNr );

void init_RBF_devs();
/* ------------------------------------------------------------------------- */


void init_RBF( fmgr_typ* f )
/* install all procedures of the RBF file manager */
{
    gs_typ* gs= &f->gs;
    ss_typ* ss= &f->ss;
    
    /* main procedures */
    f->open      = (pathopfunc_typ)pRopen;
    f->close     = (pathopfunc_typ)pRclose;
    f->read      = (pathopfunc_typ)pRread;
    f->readln    = (pathopfunc_typ)pRreadln;
    f->write     = (pathopfunc_typ)pRwrite;
    f->writeln   = (pathopfunc_typ)pRwriteln;
    f->seek      = (pathopfunc_typ)pRseek;
    f->chd       = (pathopfunc_typ)pRchd;
    f->delete    = (pathopfunc_typ)pRdelete;
    f->makdir    = (pathopfunc_typ)pRmakdir;
    
    /* getstat */
    gs->_SS_Size = (pathopfunc_typ)pRsize;
    gs->_SS_Opt  = (pathopfunc_typ)pRopt;
    gs->_SS_DevNm= (pathopfunc_typ)pRnam;
    gs->_SS_Pos  = (pathopfunc_typ)pRpos;
    gs->_SS_EOF  = (pathopfunc_typ)pUnimp;    /* not used */
    gs->_SS_Ready= (pathopfunc_typ)pRready;
    gs->_SS_FD   = (pathopfunc_typ)pRgetFD;
    gs->_SS_FDInf= (pathopfunc_typ)pRgetFDInf;
    gs->_SS_DSize= (pathopfunc_typ)pRdsize;   /* get drive size in sectors */

    /* setstat */
    ss->_SS_Size = (pathopfunc_typ)pRsetsz;
    ss->_SS_Opt  = (pathopfunc_typ)pNop;      /* ignored */
    ss->_SS_Attr = (pathopfunc_typ)pRsetatt;
    ss->_SS_FD   = (pathopfunc_typ)pRsetFD;
    ss->_SS_Lock = (pathopfunc_typ)pNop;      /* ignored */
    ss->_SS_WTrk = (pathopfunc_typ)pRWTrk;
    
     init_RBF_devs(); /* init RBF devices */
} /* init_RBF */

/* --------------------------------------------------------- */



void init_RBF_devs()
/* initialize them all to not installed */
{
    rbfdev_typ *dev;
    int  ii;
    for (ii=1; ii<MAXRBFDEV; ii++) {
        dev= &rbfdev[ ii ];
        dev->installed= false;
    }

    for (ii=0; ii<MAXSCSI; ii++) {
        strcpy( scsi[ ii ].name,"" );
    }
} /* init_RBF_devs */



// -----------------------------------------------------------------------

static IsSCSI( rbfdev_typ* dev )
{   return dev->scsiID!=NO_SCSI;
} /* IsSCSI */


static os9err ReadSector( rbfdev_typ* dev, ulong sectorNr, 
                                           ulong nSectors, byte* buffer )
/* Read sectors, either from SCSI or from an RBF image file */
{
    os9err  err= 0;
    Boolean pos_already;
    ulong   pos= sectorNr*dev->sctSize; /* get position and length to read */
    ulong   len= nSectors*dev->sctSize;
    long    cnt;

//  if (sectorNr==0) {
//      debugprintf(dbgFiles,dbgDetail,("# RBF read  sector0\n"));
//  }

    debugprintf(dbgFiles,dbgDetail,("# RBF read  sectorNr: $%06X (n=%d) @ $%08X\n",
                                       sectorNr, nSectors, pos));
    if (sectorNr>0) {
        if (dev->totScts==0)                return E_NOTRDY;
        if (dev->totScts<sectorNr+nSectors) return E_EOF; /* out of valid range */
    }
   
    do {    
        if (dev->isRAM) {
            memcpy( buffer, dev->ramBase+(sectorNr*dev->sctSize), nSectors*dev->sctSize );
            break;
        }
        
        if (IsSCSI(dev)) {
                err= ReadFromSCSI( dev->scsiAdapt, dev->scsiBus, dev->scsiID, dev->scsiLUN, sectorNr,nSectors, len,buffer ); 
            if (err) {
                err= Set_SSize   ( dev->scsiAdapt, dev->scsiBus, dev->scsiID, dev->scsiLUN,                  dev->sctSize ); if (err) break; /* adjust sector size */
                err= ReadCapacity( dev->scsiAdapt, dev->scsiBus, dev->scsiID, dev->scsiLUN,  &dev->totScts, &dev->sctSize ); if (err) break; /* and get new info back */
                err= ReadFromSCSI( dev->scsiAdapt, dev->scsiBus, dev->scsiID, dev->scsiLUN, sectorNr,nSectors, len,buffer );
            }
        }
        else {
            pos_already = (pos==dev->currPos);
            dev->currPos= UNDEF_POS; /* invalidate for case of an error */
    
            if (!pos_already) {       dev->rMiss++;
                err= syspath_seek( 0, dev->sp_img,  pos ); if (err) break;
            }
                                      dev->rTot++;  cnt= len;
                err= syspath_read( 0, dev->sp_img, &cnt, buffer, false ); 
            if (err)                   break;
            if (cnt<len) { err= E_EOF; break; };  /* if not enough read, make EOF */

            dev->currPos= pos+cnt; /* here is now our current read/write position */
        }
    } while (false);

    return err;
} /* ReadSector */



static os9err WriteSector( rbfdev_typ* dev, ulong sectorNr, 
                                            ulong nSectors, byte* buffer )
/* Write sectors, either to SCSI or on an RBF image file */
{
    os9err  err= 0;
    Boolean pos_already;
    ulong   pos= sectorNr*dev->sctSize; /* get position and length to write */
    ulong   len= nSectors*dev->sctSize;
    long    cnt;

    /* take care of write and format protection */
    if (dev->wProtected) return E_WP;
    if (sectorNr==0) {
        if (dev->fProtected) return E_FORMAT;
    }

    debugprintf(dbgFiles,dbgDetail,("# RBF write sectorNr: $%06X (n=%d) @ $%08X\n",
                                       sectorNr, nSectors, pos));
    if (sectorNr>0) {
        if (dev->totScts==0)                return E_NOTRDY;
        if (dev->totScts<sectorNr+nSectors) return E_EOF; /* out of valid range */
    }
        
    do {
        if (dev->isRAM) {
            memcpy( dev->ramBase+(sectorNr*dev->sctSize), buffer, nSectors*dev->sctSize );
            break;
        }
        
        if (IsSCSI(dev))
            err= WriteToSCSI( dev->scsiAdapt, dev->scsiBus, dev->scsiID, dev->scsiLUN, sectorNr,nSectors, len,buffer ); 
        else {
            pos_already = (pos==dev->currPos);
            dev->currPos= UNDEF_POS; /* invalidate for case of an error */
    
            if (!pos_already) {        dev->wMiss++;
                err= syspath_seek ( 0, dev->sp_img,  pos ); if (err) break;
            }
                                       dev->wTot++;  cnt= len;
                err= syspath_write( 0, dev->sp_img, &cnt, buffer, false ); 
            if (err)                   break;
            if (cnt<len) { err= E_EOF; break; }; /* if not enough read, make EOF */

            dev->currPos= pos+cnt; /* here is now our current read/write position */
        }
    } while (false);
    
    return err;
} /* WriteSector */





// ----------------------------------------------------------------
// utility procs

static os9err CutOS9Path( char** p, char* cmp_entry )
/* gets the next subpath of <p> into <cmp_entry> */
{
    os9err  err= 0;
    int     n  = 1;
    char*   c  = cmp_entry;
    
    if (ustrcmp ( *p,"..."     )==0 || /* 3 possibilities */
        ustrncmp( *p,"....", 4 )==0 ||
        ustrncmp( *p,".../", 4 )==0) { /* support multiple up path: ... */
        strcpy( cmp_entry,"..");
        (*p)++; return 0; /* no error */
    }
    
    while   (**p!=NUL) {
        if  (n++>=OS9NAMELEN) { err= E_BPNAM; break; } /* avoid too long strings */
        if  (**p==PSEP) { (*p)++; break; };
        *c=  **p; 
        c++; (*p)++;
    }
    
    *c= NUL;
    return err;
} /* CutOS9Path */



static void GetBuffers( rbfdev_typ* dev, syspath_typ* spP )
{
    #ifndef linux
    #pragma unused(dev)
    #endif

    Boolean pp= spP->fd_sct==NULL || spP->rw_sct==NULL;
    
//  if (pp) upe_printf( "Getbuffers %d %d %08X %08X\n", 
//                       spP->nr, dev->sctSize, spP->fd_sct,spP->rw_sct );
    /* allocate them only once, assume sctSize will not change */
//  if (spP->fd_sct==NULL) spP->fd_sct= get_mem( dev->sctSize, false );
//  if (spP->rw_sct==NULL) spP->rw_sct= get_mem( dev->sctSize, false );

    /* %%% there is currently an allocation bug, use always 2048 bytes */
    if (spP->fd_sct==NULL) spP->fd_sct= get_mem( 2048, false );
    if (spP->rw_sct==NULL) spP->rw_sct= get_mem( 2048, false );

//  if (pp) upe_printf( "Getbuffers %d %d %08X %08X\n", 
//                       spP->nr, dev->sctSize, spP->fd_sct,spP->rw_sct );
} /* GetBuffers */



static void ReleaseBuffers( syspath_typ* spP )
{
//  upe_printf( "Relbuffers %d %08X %08X\n", spP->nr, spP->fd_sct,spP->rw_sct);
    release_mem( spP->fd_sct, false ); spP->fd_sct= NULL;
    release_mem( spP->rw_sct, false ); spP->rw_sct= NULL;
} /* ReleaseBuffers */



static os9err DevSize( rbfdev_typ* dev )
/* get the size of the device as numbers of sectors
   and adjust either device's sector size if possible, otherwise
   copy device's current sector size to dev->sctSize */
{
    os9err err;
    ulong  size,ssize;
    
    /* should be defined already for the RAM disk */
    if (dev->isRAM) return 0;

    if (IsSCSI(dev)) {  /* try to switch to correct sector size first */
        err= Set_SSize( dev->scsiAdapt, dev->scsiBus, dev->scsiID, dev->scsiLUN,  dev->sctSize );
        if (err) {
            // not possible to change sector size to what OS9 expects
            // - get sector size of SCSI device
            err= Get_SSize( dev->scsiAdapt, dev->scsiBus, dev->scsiID, dev->scsiLUN,  &ssize );
            if (err) return err; 
            if (ssize>MIN_TMP_SCT_SIZE) return err; // cannot read sector 0 that big
            // - use sector size of SCSI device for now
            dev->sctSize=ssize;
        }
        err= ReadCapacity(  dev->scsiAdapt, dev->scsiBus, dev->scsiID, dev->scsiLUN, 
                           &dev->totScts,  &dev->sctSize );
    }
    else { 
        err= syspath_gs_size( 0, dev->sp_img, &size );
        dev->totScts= size/dev->sctSize;
    }

    return err;
} /* DevSize */


static os9err ChkIntegrity( rbfdev_typ* dev, syspath_typ* spP, 
                            byte* mysct, Boolean ignore )
{
    ushort* w= (ushort*)&mysct[14]; 
    
        dev->last_diskID = *w;
    if (spP->u.rbf.diskID!=*w) { /* doesn't matter big/little endian */
        spP->u.rbf.diskID= *w;
        if (ignore) return 0;

        spP->u.rbf.fd_nr= 0; /* do not access any more this fd */
        spP->mustW      = 0; /* don't write this sector */
        DevSize( dev );      /* get new device size */
        return E_DIDC;
    }
    
    return 0;
} /* ChkIntegrity */


static os9err RootLSN( rbfdev_typ* dev, syspath_typ* spP, Boolean ignore )
{   
    os9err  err;
    ulong*  l;
    ushort* w;
    ushort  sctSize;
    Boolean cruz;
    
    while (true) { /* loop */
        debugprintf(dbgFiles,dbgNorm,("# RootLSN: sectorsize %d\n", dev->sctSize ));
        
        // get size of device 
        if (dev->totScts==0) { err= DevSize( dev ); if (err) return err; }
        err= ReadSector( dev, 0,1,  dev->tmp_sct ); if (err) return err;

        /* get the sector size of the device */
        cruz= (strcmp( &dev->tmp_sct[CRUZ_POS],Cruz_Str )==0);
        debugprintf(dbgFiles,dbgNorm,("# RootLSN: sectorsize %d %s\n", dev->sctSize, cruz?"(cruz)":"" ));
        
        if (cruz) {
            w= (ushort*)&dev->tmp_sct[104]; sctSize= os9_word(*w);
        }
        else {
            if (dev->sctSize==0) sctSize= STD_SECTSIZE;
            else                 sctSize= dev->sctSize; // use existing (luz 2002-02-12)
        }   
        if          (sctSize==0) sctSize= STD_SECTSIZE; // but avoid 0  (bfo 2002-06-06)
           
        debugprintf(dbgFiles,dbgNorm,("# RootLSN: adapt sectorsize %d => %d\n",
                                         dev->sctSize, sctSize));
        if (dev->sctSize==sctSize) break; /* it is ok already ? */
        	dev->sctSize= sctSize;        /* this is the correct sector size */
    
        /* release buffers of old size */
        release_mem   ( dev->tmp_sct, false );
        ReleaseBuffers( spP );
        
        /* and get the new buffers with the new sector size */
     // dev->tmp_sct= get_mem( dev->sctSize, false );
        dev->tmp_sct= get_mem( dev->sctSize>MIN_TMP_SCT_SIZE ? dev->sctSize : MIN_TMP_SCT_SIZE, false );
        GetBuffers( dev,spP );

        dev->totScts   = 0; /* must be set to correct value */
        dev->last_alloc= 0; /* initialize allocater pointer */
    } /* loop */
    
    l= (ulong *)&dev->tmp_sct[0]; dev->totScts    = os9_long(*l)>>BpB;
    w= (ushort*)&dev->tmp_sct[4]; dev->mapSize    = os9_word(*w);
    w= (ushort*)&dev->tmp_sct[6]; dev->clusterSize= os9_word(*w);
    l= (ulong *)&dev->tmp_sct[8]; dev->root_fd_nr = os9_long(*l)>>BpB;
    
    spP->u.rbf.fd_nr=  dev->root_fd_nr;
    err= ChkIntegrity( dev,spP, dev->tmp_sct, ignore ); return err;
} /* RootLSN */



static void CutPath( char* s )
{
    int  ii,len= strlen( s );
    for (ii=len-1; ii>=0; ii--) {
        if (s[ii]==PSEP) { strcpy( s,&s[ii+1] ); break; }
    } /* for */
} /* CutPath */


static os9err Open_Image( ushort pid, rbfdev_typ* dev, ptype_typ type, char* pathname, 
                          ushort mode )
{
    os9err  err, cer;
    ushort  sp;
    ulong   size, len;
    char    bb[STD_SECTSIZE]; /* one sector */
    
    do {
        err= syspath_open   ( pid, &sp, type,pathname,mode ); if (err) return err;
        err= syspath_gs_size( pid,  sp, &size );              if (err) break;
        if (size < 8192 || 
           (size % 2048)!=0) { err= E_FNA; break; }
      
                                         len= sizeof(bb);
        err= syspath_read   ( pid,  sp, &len, &bb, false );   if (err) break;
        err= syspath_seek   ( pid,  sp,  0 );                 if (err) break;

        /* Cruzli check */
        if (strcmp( &bb[CRUZ_POS],Cruz_Str )!=0) { err= E_FNA; break; }
                            
        dev->sp_img= sp; /* do this before calling DevSize */
        err= DevSize( dev );
    } while (false);
    
    if (err) { cer= syspath_close( 0, sp ); return err; }
    strcpy( dev->img_name,pathname );
    
    /* if not the complete name at the image */
    if     (ustrcmp( &dev->img_name[1],dev->name )==0) {
            strcpy (  dev->img_name, pathname  );
    }

    if     (ustrcmp( &dev->img_name[1],dev->name )==0) {
        strcpy     (  dev->img_name, startPath );
        MakeOS9Path(  dev->img_name );
        strcat     (  dev->img_name, pathname  );
        
        if   (!FileFound( dev->img_name )) {
            strcpy     (  dev->img_name, strtUPath );
            MakeOS9Path(  dev->img_name );
            strcat     (  dev->img_name, pathname  );
        }
    }
    
    EatBack( dev->img_name );
    return 0;                                                         
} /* Open_Image */



Boolean InstalledDev( const char* os9path, const char* curpath,
                      Boolean fullsearch, ushort *cdv )
/* expect OS-9 notation */
{
    char        tmp[OS9PATHLEN];
    rbfdev_typ* dev;
    char        *p, *dvn;
    int         ii;

    strcpy( tmp,curpath  ); /* create the full pathname */
    strcat( tmp,PSEP_STR );
    if (AbsPath(os9path)) strcpy( tmp,"" );
    strcat( tmp,os9path  );
    
    for (ii=1; ii<MAXRBFDEV; ii++) { /* search for identical image */
            dev= &rbfdev[ ii ];
        if (dev->installed &&     /* test for same path or with sub path */
            SamePathBegin( tmp,dev->img_name )) { *cdv= ii; return true; }
    } /* for */

         p= tmp;
    if (*p==PSEP) p++;
    dvn= p;             /* this is what we are looking for  */
    while  (*p!=NUL ) { /* cut raw device name or sub paths */
        if (*p=='@' ) { *p= NUL; break; }
        if (*p==PSEP) {
            if (fullsearch) { dvn= p+1;       }
            else            { *p= NUL; break; }
        }
        p++;
    } /* while */

    if (*dvn==NUL) return false; /* no device */
    if (fullsearch && *mnt_name!=NUL) 
                  dvn= mnt_name; /* in case of mount, compare to this */

    for (ii=1; ii<MAXRBFDEV; ii++) {     /* search a device */
            dev= &rbfdev[ ii ];          /* get RBF device  */
        if (dev->installed && 
           (ustrcmp( dvn,dev->name  )==0 
         || ustrcmp( dvn,dev->name2 )==0
         || ustrcmp( dvn,dev->name3 )==0) &&
           (SamePathBegin( &tmp[1],dev->name  ) 
         || SamePathBegin( &tmp[1],dev->name2 )
         || SamePathBegin( &tmp[1],dev->name3 )
         || SamePathBegin(  tmp,   dev->img_name ))) { *cdv= ii; return true; }
    } /* for */

    return false;
} /* InstalledDev */



static Boolean MWrong( int cdv )
/* check if already installed on a different device */
{
    rbfdev_typ*  dev;
    int          ii;

	for (ii=1; ii<MAXRBFDEV; ii++) {       
    	    dev= &rbfdev[ ii ];             /* get RBF device  */
    	if (dev->installed && ii!=cdv && (ustrcmp( mnt_name,dev->name  )==0 ||
                                          ustrcmp( mnt_name,dev->name2 )==0 ||
                                          ustrcmp( mnt_name,dev->name3 )==0)) return true;
    } /* for */
    
    return false;
} /* MWrong */



static os9err PrepareRAM( rbfdev_typ* dev, char* cmp )
{
    #define DefaultScts 8192
    #define A_Base      0x100
    #define MaxScts     0x00fffffc
    
    ulong    allocSize, allocN, allocScts;
    ulong*   u;
    ushort*  w;
    byte*    b;
    ulong    f, r, fN, rN;
    int      ii, v;
    byte     pt;
    mod_dev* mod;
    char*    p;
    
        dev->totScts= mnt_ramSize*KByte/dev->sctSize; /* adapt to KBytes */
    if (dev->totScts==0) dev->totScts= DefaultScts;
    
    if ( mnt_ramSize==0 
      && IsDesc( cmp, &mod, &p )  
      && ustrcmp( p,"RBF" )==0 ) {
        p= (char*)mod + os9_word(mod->_mpdev);
        if (ustrcmp( p,"ram" )==0) {
            w= (ushort*)(&mod->_mdtype + PD_SCT);
            dev->totScts= os9_word( *w );
        }
    } /* if */

    if (dev->totScts>MaxScts) dev->totScts= MaxScts;
    
    allocSize= (dev->totScts-1)/(dev->sctSize*BpB) + 1; /* round up */
    allocN   =       allocSize * dev->sctSize*BpB;
    allocScts= 1 +   allocSize + 2;
    
    printf( "%d %s\n", dev->totScts, cmp );
    dev->ramBase= get_mem( dev->sctSize*dev->totScts, false );
    if (dev->ramBase==NULL) return E_NORAM;
    
    memcpy( dev->ramBase,RAM_zero, dev->sctSize );
    
    pt= 0x80;
    for (ii=0; ii<allocN; ii++) {
        if  (ii<allocScts || ii>=dev->totScts) {
          v= ii/BpB;
          b= &dev->ramBase[A_Base+v]; *b |= pt;
        }
        
        pt= pt/2; if (pt==0) pt= 0x80; /* prepare the next pattern */
    } /* for */
    
    f=  1 + allocSize; fN= f*dev->sctSize;
    r=  f + 1;         rN= r*dev->sctSize;
    
    u= &dev->ramBase[   0x00]; *u= os9_long( dev->totScts<<BpB ); /* 0x03 will be overwritten, is 0 anyway */
    printf( "%08X\n", *u );
    w= &dev->ramBase[   0x04]; *w= (ushort) os9_word( (dev->totScts-1)/BpB + 1 );
    u= &dev->ramBase[   0x08]; *u= os9_long( f<<BpB );            /* 0x0b will be overwritten, is 0 anyway */
                        
        dev->ramBase[fN+0x00]= 0xbf; /* prepare the fd sector */
        dev->ramBase[fN+0x08]= 0x01;
        dev->ramBase[fN+0x0C]= 0x40;
    u= &dev->ramBase[fN+0x10]; *u= os9_long( r<<BpB );
        dev->ramBase[fN+0x14]= 0x01;
            
        dev->ramBase[rN+0x00]= 0x2e; /* prepare the directory entry */
        dev->ramBase[rN+0x01]= 0xae;
    w= &dev->ramBase[rN+0x1e]; *w= (ushort) os9_word( f );
        dev->ramBase[rN+0x20]= 0xae;
    w= &dev->ramBase[rN+0x3e]; *w= (ushort) os9_word( f );
    
    strcpy( dev->img_name,cmp );
    return 0;
} /* PrepareRAM */



static os9err DeviceInit( ushort pid, rbfdev_typ** my_dev, syspath_typ* spP, 
                          ushort cdv, char* pathname, char* curpath, ushort mode, Boolean *new_inst )
/* Make Connection to SCSI system or to an TBF image file */ 
{
    os9err       err= 0;
    char         cmp[OS9PATHLEN],
                 ali[OS9PATHLEN],
                 tmp[OS9PATHLEN],
                 ers[10], *q, *p, *v;
    rbfdev_typ*  dev;
    ptype_typ    type;
    int          ii, n;
    Boolean      abs, isSCSI, isRAM, isFolder, wProtect;
    
    process_typ* cp    = &procs[pid];
    
    int          scsiID    = NO_SCSI;
    short        scsiAdapt = -1;
    short        scsiBus   = 0;
    short        scsiLUN   = 0;
    ushort       scsiSsize = STD_SECTSIZE;
    ushort       scsiSas   = 0;
    byte         scsiPDTyp = 0;
    
    Boolean      mock  = *mnt_name!=NUL;
    Boolean      fu    = spP->fullsearch;
    Boolean      fum   = fu && mock;
        
    #ifdef MACFILES
      FSSpec fs, afs;
    #elif defined win_linux
      char rbfname[OS9PATHLEN];
    #endif


    do {
        *new_inst= false;
        if (fu) {
            strcpy( tmp,curpath  ); /* create the full pathname */
            strcat( tmp,PSEP_STR );
            if (AbsPath(pathname)) strcpy( tmp,"" );
            strcat( tmp,pathname );
            strcpy( pathname,tmp );
        }

        strcpy( ali,""       ); /* no alias defined by default */
        strcpy( cmp,pathname ); /* default in case of error */

            abs=        AbsPath( pathname );
        if (abs && InstalledDev( pathname,curpath, fu, &cdv )) {
            dev= &rbfdev[cdv];
     	    debugprintf(dbgFiles,dbgNorm,("# DevInit: cdv=%d path='%s' img='%s'\n", 
     	                                     cdv, pathname,dev->img_name ));
            if                 (!mock 
              ||        ustrcmp( mnt_name,dev->name  )==0
              ||        ustrcmp( mnt_name,dev->name2 )==0
              ||        ustrcmp( mnt_name,dev->name3 )==0
              || !SamePathBegin( pathname,dev->img_name )) break;
            
            /* already installed with a different name */
            if (*dev->name2==NUL) { strcpy( dev->name2,mnt_name ); break; }
            if (*dev->name3==NUL) { strcpy( dev->name3,mnt_name ); break; }
            return E_DEVBSY;
        }
        
        /* existing relative path ? */
        if (!abs && cdv!=0 && mnt_scsi==NO_SCSI) break;

        err= 0;
        if (!fu) {
                 isRAM= RAM_Device( pathname );
            if (!isRAM) {
                #ifdef MACFILES
                  err= GetRBFName( pathname,mode, &isFolder, &fs,&afs );
                #elif defined win_linux
                  err= GetRBFName( pathname,mode, &isFolder, (char*)&rbfname );
                #endif
                
                /* must open it in the right mode */
                if (err==E_FNA && !IsDir(mode)) return err;
            }
        }

        if (isRAM) {
                 isSCSI= false; /* don't forget to set a default */
            GetOS9Dev( pathname, (char*)&cmp );
        }
        else {
                 isSCSI= (err && mnt_scsi!=NO_SCSI);
            if  (isSCSI) {
                scsiID   = mnt_scsi;
                scsiAdapt= mnt_scsiAdapt>=0 ? mnt_scsiAdapt : defSCSIAdaptNo;
                scsiBus  = mnt_scsiBus  >=0 ? mnt_scsiBus   : defSCSIBusNo;    
                scsiLUN  = mnt_scsiLUN  >=0 ? mnt_scsiLUN   : 0;    
            }
            else isSCSI= (err && SCSI_Device( pathname, &scsiAdapt, &scsiBus, &scsiID, &scsiLUN, 
                                                        &scsiSsize, &scsiSas, &scsiPDTyp, &type ));
            if  (isSCSI)           GetOS9Dev( pathname, (char*)&cmp );
            else {
                if (fu) CutPath( cmp );
                else {
                    #ifdef MACFILES
                      /* this is the correct way to get pascal strings back */
                      if (!err) { /* GetRBFName called earlier already */
                          memcpy( &ali, &afs.name, sizeof(ali) );
                          p2cstr ( ali );
                      }
                      else {
                          GetOS9Dev( pathname, (char*)&cmp );
                              err= getFSSpec( 0,cmp, _start, &fs );
                          if (err) return err;
                      }
                  
                      memcpy   ( &cmp, &fs.name, sizeof(cmp) );
                      p2cstr    ( cmp );
                      if (strcmp( cmp,ali )==0) strcpy( ali,"" );
            
                    #elif defined win_linux
                      if (err) return E_UNIT; /* GetRBFName called earlier */
                      strcpy( cmp,rbfname );
              
                    #else
                      /* %%% some fixed devices defined currently */
                      GetOS9Dev( pathname, (char*)&cmp );
                      if      (ustrcmp( cmp,"mt")==0) cdv= 1; 
                      else if (ustrcmp( cmp,"c1")==0) cdv= 2;
                      else if (ustrcmp( cmp,"c2")==0) cdv= 3;
                      else if (ustrcmp( cmp,"c3")==0) cdv= 4;
                      else if (ustrcmp( cmp,"dd")==0) cdv= 5;
                      else return E_UNIT;
            
                      break;
                    #endif
                }
            } /* if isSCSI */
        } /* if !isRAM */

        if (!AbsPath(pathname)) {
            if (IsExec(mode)) strcpy( tmp, cp->x.path );
            else              strcpy( tmp, cp->d.path );
            if (*tmp!=NUL)    strcat( tmp, PSEP_STR ); /* create the full path name */

            strcat( tmp,pathname );
            strcpy( pathname,tmp );
        }

        cdv= 0;                              /* now search a device */
        for (ii=1; ii<MAXRBFDEV; ii++) {       
                 dev= &rbfdev[ ii ];             /* get RBF device  */
            if (!dev->installed) {
                if (cdv==0) cdv= ii;           /* reserve empty one */
            }
            else {
               /* is there already a file with the same file name ? */
     			debugprintf(dbgFiles,dbgNorm,("# DevInit: path='%s' img='%s'\n", 
     			                                 pathname,dev->img_name ));
                if (SamePathBegin( pathname,dev->img_name)) {
                	if (strcmp( cmp,dev->name )!=0) {
                    	strcpy( cmp,dev->name ); /* take existing name */
                    	*new_inst= true;        /* make a new instance */
                    }
                    
                    cdv= ii; break;
                }
            
                /* compare with mnt_name, if available */
                             q= cmp;
                if (mock)    q= mnt_name;
                if (ustrcmp( q,dev->name  )==0
                 || ustrcmp( q,dev->name2 )==0
                 || ustrcmp( q,dev->name3 )==0) { cdv= ii; break; }

                if (isSCSI && IsSCSI( dev ) &&
                    scsiID == dev->scsiID) {
                    if (*dev->name2==NUL) { strcpy( dev->name2,q ); cdv= ii; break; }
                    if (*dev->name3==NUL) { strcpy( dev->name3,q ); cdv= ii; break; }
                    return E_DEVBSY;
                }
            }
        } /* for */

        if (cdv==0) return E_UNIT; /* no valid device found */
    } while (false);

             dev= &rbfdev[ cdv ]; 
    *my_dev= dev;                      /* activate RBF device */
    strcpy ( dev->alias,ali );         /* can change all the time */
    if     (!dev->installed) {
        v= strstr( cmp,"@" ); if (v!=NULL) *v= NUL;
        strcpy( dev->name,cmp );
    }
    
             q= dev->name; /* compare with mnt_name, if available */
    if (fum) q= mnt_name;
    debugprintf(dbgFiles,dbgNorm,("# DevInit: '%s' (%d%s%s)\n", 
                                     q, cdv, dev->installed ? "/installed":"",
                                             *new_inst      ? "/new_inst" :"" ));

    if (dev->installed) {
    	if (!abs) return 0; /* ok, if not absolute */
    	if (mock && MWrong( cdv )) return E_DEVBSY;
    	
    	strcpy  ( tmp,pathname );
    	n= strlen(tmp)-1;
    	while   ( tmp[n]=='@' ) tmp[n--]= NUL;
        p=        tmp+1;
        if (SamePathBegin( tmp,dev->img_name ) ||
            SamePathBegin( p,  dev->name     ) ||
            SamePathBegin( p,  dev->name2    ) ||
            SamePathBegin( p,  dev->name3    )) return 0;
        
        /* already in use !! */    
    //  printf( "path='%s' img='%s'\n", pathname,dev->img_name );
    //  printf( "path='%s'  n1='%s'\n", pathname,dev->name  );
    //  printf( "path='%s'  n2='%s'\n", pathname,dev->name2 );
    //  printf( "path='%s'  n3='%s'\n", pathname,dev->name3 );
        return E_DEVBSY;
	} /* if installed */
	
    dev->nr        = cdv;          /* already done ?? */
    dev->wProtected= mnt_wProtect; /* if not otherwise defined */
    dev->fProtected= false;
    dev->multiSct  = false;        /* not yet supported */
    dev->currPos   = UNDEF_POS;    /* to make access faster: no seeks all the time */
    dev->last_alloc= 0;            /* initialize allocater pointer */
    dev->sp_img    = 0;            /* the image syspath will be connected here later */
    dev->isRAM     = isRAM;        /* RAM disk flag */
    
    strcpy( dev->img_name,"" );    /* no  image name by default */
    strcpy( dev->name2,   "" );    /* no  2nd   name by default */
    strcpy( dev->name3,   "" );    /* no  3rd   name by default */

    dev->rMiss     = 0;            /* reset device statistics */
    dev->rTot      = 0;
    dev->wMiss     = 0;
    dev->wTot      = 0;

    dev->scsiAdapt = scsiAdapt;
    dev->scsiBus   = scsiBus;
    dev->scsiID    = scsiID;
    dev->scsiLUN   = scsiLUN;  
    dev->pdtyp     = scsiPDTyp;
    
    if (IsSCSI(dev)) {
        dev->sctSize= scsiSsize;
        dev->sas    = scsiSas;
    }
    else {
        dev->sctSize= STD_SECTSIZE;       /* as OS-9 <=V2.3: const sectsize 256 bytes */
        if (ustrcmp(dev->name,"dd")==0) dev->sas= DD__MINALLOC; /* use default value  */
        else                            dev->sas= RBF_MINALLOC; /* smaller for others */
    }

    // make sure we get a buffer that is big enough for reading any sector 0 (will be adjusted later)
    dev->tmp_sct= get_mem( dev->sctSize>MIN_TMP_SCT_SIZE ? dev->sctSize : MIN_TMP_SCT_SIZE, false );
    
    debugprintf(dbgFiles,dbgNorm,("# RBF open: trying to open %s \"%s\"\n",
                                     IsSCSI(dev) ? "SCSI":"RBF Image", q));

    /* %%% currently hardwired */
    if (ustrcmp( cmp,"c0")==0) dev->fProtected= false;
    if (ustrcmp( cmp,"c1")==0) dev->fProtected= false;

    do {
        err= 0;                 /* set it as default   */
        dev->installed= true;   /* now it is installed */
        
        if (isRAM) { err= PrepareRAM( dev, &cmp ); break; }  /* no more actions for RAM disk */
        if (IsSCSI(dev))                           break;    /* no more actions for SCSI */
            
        type    = IO_Type( pid, pathname, poRead );
        wProtect= mnt_wProtect;
            
    //  upe_printf( "INSTALL %s %s %d\n", TypeStr(type), pathname, spP->nr );
        /* inherit write protection to sub device */
        if (!wProtect && type==fRBF && InstalledDev( pathname,curpath, false, &cdv ))
             wProtect= rbfdev[cdv].wProtected;

        /* try to open in read/write mode first (if not asking for wProtection */
        /* if not possible, open it readonly */
        if (!wProtect) {
             err= Open_Image( pid,dev, type,pathname, poUpdate ); if (!err) break;
        }
             err= Open_Image( pid,dev, type,pathname, poRead );
        if (!err)                 dev->wProtected= true;
    } while (false);

    do {                               if (err) break;
        err= RootLSN( dev,spP, true ); if (err) break;
        dev->last_diskID= spP->u.rbf.diskID;
        *new_inst= true;
    } while (false);

    if (err) {
        dev->installed= false;
        release_mem( dev->tmp_sct,false );
                     dev->tmp_sct= NULL;
    }
        
    sprintf( ers,"  #000:%03d", err );
    debugprintf(dbgFiles,dbgNorm,("# RBF open: \"%s\" (%d%s%s)\n", 
                                     cmp, cdv, dev->installed ? "/installed":"", 
                                     err ? ers:""));
    return err;
} /* DeviceInit */




static void mount_usage(char* name, ushort pid )
{
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    upe_printf( "Syntax:   %s <image_file> [<device>]\n", name );
    upe_printf( "Function: mount an RBF image file\n" );
    upe_printf( "Options:  \n" );
    upe_printf( "    -w         open with write protection on\n" );
    
    #ifdef windows32
    upe_printf( "    -ah        show all SCSI devices on all adaptors and buses\n" );
    upe_printf( "    -a=adapter specify  SCSI adapter\n" );
    upe_printf( "    -b=scsibus specify  SCSI bus\n" );
    #endif
    
    upe_printf( "    -s=scsiID  connect to SCSI ID\n" );
    
    upe_printf( "    -l=lun     connect to LUN (default=0)\n" );
    upe_printf( "    -r=size    create RAM disk with size (in kBytes)\n" );
} /* mount_usage */



os9err MountDev( ushort pid, char* name, char* mnt_dev, 
                             short adapt, ushort scsibus, short scsiID, ushort scsiLUN, 
                             int ramSize, Boolean wProtect )
{
    os9err    err= 0;
    char      tmp[OS9NAMELEN];
    char      ch;
    int       ii;
    ptype_typ type;
    ushort    sp;

    /* it might come as Mac or DOS path name ... --> OS-9 notation */
    MakeOS9Path( name );

    /* Is there a different name for the mount device ? */
    /* OS9exec can't switch the task in-between */
    /* NOTE: mnt_name is only valid within this context here */
    if (*mnt_dev!=NUL || scsiID!=NO_SCSI || ramSize>0) {
        mnt_name   = mnt_dev;
        mnt_ramSize= ramSize;
       
        if (scsiID!=NO_SCSI) {
            mnt_name= name;
            mnt_scsiAdapt = adapt;
            mnt_scsiBus   = scsibus;
            mnt_scsi      = scsiID;
            mnt_scsiLUN   = scsiLUN;
            
            if (!AbsPath(name)) { /* make a device path */
                strcpy ( tmp,PSEP_STR );
                strncat( tmp,name, OS9NAMELEN-1 );
                name=    tmp;
            }
        }
        
        if (AbsPath(mnt_name)) mnt_name++;
        for (ii=0; ii<OS9NAMELEN; ii++) {
                ch= mnt_name[ii];
            if (ch== NUL)                 break;   /* ok */
            if (ch==PSEP) { err= E_FNA;   break; } /* no more path separators allowed */
            if (ch < '0' &&
                ch!= '.') { err= E_BPNAM; break; } /* illegal tokens in path name */
        } /* for */
        
        if (err) name= mnt_name; /* for error output */
    } /* if */

    mnt_wProtect= wProtect;
    if (!err) {                type= fRBF;
        if (mnt_scsi==NO_SCSI) type= IO_Type( pid,name, poDir );
        
        debugprintf(dbgUtils,dbgNorm,("# mount: name='%s', mnt_name='%s' type='%s'\n", 
                                          name, mnt_name,TypeStr(type) ));
        err= syspath_open( pid, &sp,type, name, poDir );
        debugprintf(dbgUtils,dbgNorm,("# mount: name='%s', mnt_name='%s' err=%d\n",
                                          name, mnt_name,err ));
    }
    mnt_wProtect= false; /* switch them off again */
    mnt_name    = "";
    mnt_scsi    = NO_SCSI;
    mnt_ramSize = 0;
    
    if   (!err) err= syspath_close( pid, sp );
    return err;
} /* MountDev */



os9err int_mount( ushort pid, int argc, char **argv )
/* mount an RBF image partition file */
{
    #ifndef linux
    #pragma unused( pid,argc,argv )
    #endif

    os9err    err     = 0;
    int       nargc   = 0;
    short     adapt   = defSCSIAdaptNo;
    short     scsibus = defSCSIBusNo;
    short     scsiID  = NO_SCSI;
    short     scsiLUN = 0;
    int       ramSize = 0;
    
    Boolean   wProtect= false;
    char      *p;
    int       k;
    
    #define     MAXARGS_ 2
    char* nargv[MAXARGS_];
    

    for (k=1; k<argc; k++) {
             p= argv[k];    
        if (*p=='-') { 
            p++;
            switch (tolower(*p)) {
                case '?' :  mount_usage(argv[0],pid); return 0;

                case 'w' :  wProtect= true; break;

				#ifdef windows32
                  case 'a' :  if (*(p+1)=='h') {
                                  upho_printf("Current defaults: Adaptor=SCSI%d, Bus=%d\n\n",
                                               defSCSIAdaptNo,defSCSIBusNo);
                                  scsiadaptor_help();
                                  return 0;    
                              }
                              if (*(p+1)=='=') p+=2;
                              else {  k++; /* next arg */
                                  if (k>=argc) break;
                                  p= argv[k];
                              }
                              sscanf( p,"%hd", &adapt );
                              break;
                            
                  case 'b' :  if (*(p+1)=='=') p+=2;
                              else {  k++; /* next arg */
                                  if (k>=argc) break;
                                  p= argv[k];
                              }
                              sscanf( p,"%hd", &scsibus );
                              break;
                #endif
                            
                case 's' :  if (*(p+1)=='=') p+=2;
                            else {  k++; /* next arg */
                                if (k>=argc) break;
                                p= argv[k];
                            }
                            if (sscanf( p,"%hd", &scsiID )<1) scsiID= NO_SCSI;
                            break;

                case 'l' :  if (*(p+1)=='=') p+=2;
                            else {  k++; /* next arg */
                                if (k>=argc) break;
                                p= argv[k];
                            }
                            sscanf( p,"%hd", &scsiLUN );
                            break;

                case 'r' :  if (*(p+1)=='=') p+=2;
                            else {  k++; /* next arg */
                                if (k>=argc) break;
                                p= argv[k];
                            }
                            sscanf( p,"%d", &ramSize );
                            break;
                            
                default  :  uphe_printf("Error: unknown option '%c'!\n",*p); 
                            mount_usage(argv[0],pid); return 1;
            }   
        }
        else {
            if (nargc>=MAXARGS_) { 
                uphe_printf("Error: no more than %d arguments allowed\n",MAXARGS_); 
                return 1;
            }
            nargv[nargc++]= argv[k];
        }
    } /* for */

    /* nargv[0] is the name of the image to be mounted */
    /* nargv[1] is the name of the mounted device */
        err= MountDev( pid, nargv[0], nargc<2 ? "":nargv[1], adapt, scsibus, scsiID, scsiLUN, ramSize, wProtect );
    if (err) return _errmsg( err, "can't mount device \"%s\".\n", nargv[0] );
    return 0;
} /* int_mount */




static void unmount_usage( char* name, ushort pid )
{
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    upe_printf( "Syntax:   %s <device>\n", name );
    upe_printf( "Function: unmount an RBF image file\n" );
    upe_printf( "Options:  None\n" );
} /* unmount_usage */


static os9err ReleaseIt( ushort pid, rbfdev_typ* dev )
{
    os9err err= 0;
    
    if (dev->isRAM) release_mem( dev->ramBase, false );
    else {
        if (!IsSCSI(dev)) err= syspath_close( pid, dev->sp_img ); 
    }
    
    if (!err) {       dev->installed= false;
         release_mem( dev->tmp_sct,   false );
                      dev->tmp_sct= NULL;
    }
    
    return err;
} /* ReleaseIt */


os9err int_unmount( ushort pid, int argc, char **argv )
/* unmount an RBF image partition file */
{
    #ifndef linux
    #pragma unused( pid,argc,argv )
    #endif

    os9err err= 0;
    int nargc=0, h;
    char *p, *name;
    #define     MAXARGS 1
    char *nargv[MAXARGS];

    int          ii;
    syspath_typ* spP;
    rbfdev_typ*  dev;
    char         devName[OS9NAMELEN];

    for (h=1; h<argc; h++) {
        p= argv[h];    
        if (*p=='-') { 
            p++;
            switch (tolower(*p)) {
                case '?' :  unmount_usage(argv[0],pid); return 0;
                default  :  uphe_printf("Error: unknown option '%c'!\n",*p); 
                            unmount_usage(argv[0],pid); return 1;
            }   
        }
        else {
            if (nargc>=MAXARGS) { 
                uphe_printf("Error: no more than %d arguments allowed\n",MAXARGS); return 1;
            }
            nargv[nargc++]= argv[h];
        }
    }   

    name= nargv[0]; if (AbsPath(name)) name++;
    
    do {
        for (ii=0; ii<MAXSYSPATHS; ii++) {
                spP= get_syspathd( pid,ii );
            if (spP!=NULL) {
                err= syspath_gs_devnm( pid, spP->nr, devName ); if (err) break;
                if (ustrcmp(devName,name)==0) { err= E_DEVBSY; break; }
            } 
        } /* for */
        if (err) break;
        
        err = E_UNIT;
        for (ii=0; ii<MAXRBFDEV; ii++) {
                dev= &rbfdev[ii]; 
            if (dev->installed) { /* also name2 or name3 can be released now */
                if (ustrcmp( name,dev->name )==0) {
                    if (*dev->name2!=NUL ||
                        *dev->name3!=NUL)  { 
                          strcpy( dev->name, dev->name2 );
                          strcpy( dev->name2,dev->name3 ); 
                          strcpy( dev->name3,"" ); err= 0; break;
                    }
                    err= ReleaseIt( pid,dev );             break;
                }

                if (ustrcmp( name,dev->name2 )==0) {
                          strcpy( dev->name2,dev->name3 ); 
                          strcpy( dev->name3,"" ); err= 0; break;
                }

                if (ustrcmp( name,dev->name3 )==0) {
                          strcpy( dev->name3,"" ); err= 0; break;
                }
            }
        } /* for */
    } while (false);
        
    if    (err) return _errmsg( err, "can't unmount device \"%s\".\n", name );
    return err;
} /* int_unmount */



static Boolean Mega( long long size, float *r )
{
    #define MegaLim 10000.0;
    Boolean m;
                           *r= size/KByte;
    m = *r>MegaLim; if (m) *r= *r  /KByte;
    return m;
}

static void Disp_RBF_DevsLine( rbfdev_typ* rb, char* name, Boolean statistic )
{
//  char  sc[20];
    char  s [OS9NAMELEN];
    char  u [OS9PATHLEN];
    char  v [20];
    char  w [20];
    char* unit;
    long long size= (long long)rb->totScts * rb->sctSize;
    float r;

                         unit= "kB";
    if (Mega( size,&r )) unit= "MB";

    strcpy ( s,             name         );
//  sprintf( sc,     "%3d", rb->scsiID   );
    strcpy ( u,             rb->img_name ); 
    
    if     (rb->isRAM || *u==NUL) strcpy( u," -" );
    if     (rb->isRAM)  strcpy ( w, "ram" );
    else {
        if (IsSCSI(rb)) sprintf( w, "SCSI: %d", rb->scsiID );
        else            strcpy ( w, "image" );
    }
    
                 sprintf( v, "(%1.2f%s)", r,unit );
    if (r>=  10) sprintf( v, "(%2.1f%s)", r,unit );
    if (r>= 100) sprintf( v," (%3.0f%s)", r,unit );
    if (r>=1000) sprintf( v, "(%4.0f%s)", r,unit );
    
    
    upo_printf( "%-10s ", StrBlk_Pt( s,10 ) );
            
    if (statistic)
        upo_printf( "%10d /%10d   %10d /%10d\n",
                     rb->rMiss, rb->rTot,
                     rb->wMiss, rb->wTot );
    else 
        upo_printf( "%-10s %-7s %2d %4d  %-4s %-26s %s\n", 
                     w,
                     "rbf", 
                     rb->nr,
                     rb->sctSize,
                     rb->wProtected ? "yes":"no",
                     StrBlk_Pt( u,26 ),
                     v );
} /* Disp_RBF_DevsLine */



void Disp_RBF_Devs( Boolean statistic )
{
    rbfdev_typ* r;
    
    int  ii;
    for (ii=0; ii<MAXRBFDEV; ii++) {
            r= &rbfdev[ii]; 
        if (r->installed) {     Disp_RBF_DevsLine( r, r->name,  statistic );
            if (*r->name2!=NUL) Disp_RBF_DevsLine( r, r->name2, statistic );
            if (*r->name3!=NUL) Disp_RBF_DevsLine( r, r->name3, statistic );
        }
    }
} /* Disp_RBF_Devs */



/* ------------------------------------------------------------------------------ */

static void AdaptPath( rbfdev_typ* dev, char** pathP )
{
    char  tmp[OS9PATHLEN];
    char* p   = *pathP;
    int   ilen= strlen(dev->img_name); 

    if (!AbsPath(p)) return;

    if           (*mnt_name!=NUL
      && (ustrcmp( mnt_name,dev->name  )==0 || /* search for all possible names */
      	  ustrcmp( mnt_name,dev->name2 )==0 ||
		  ustrcmp( mnt_name,dev->name3 )==0)
      &&  SamePathBegin ( p,dev->img_name )) {
        strcpy( tmp,p );
        strcpy( p,PSEP_STR   );
        strcat( p,dev->name  );
        strcat( p,&tmp[ilen] );
    }

    while  (*p!=NUL)  { /* search through the string */
        if (*p==PSEP) { /* for a slash */
            if (*(++p)==NUL) break;

            /* start with a path delimiter */
            if (SamePathBegin( p,dev->name  ) ||
                SamePathBegin( p,dev->name2 ) ||
                SamePathBegin( p,dev->name3 ) ||
                SamePathBegin( p,dev->alias )) { *pathP= p-1; break; }
        } /* if */

        p++;
    } /* while */
} /* AdaptPath */




/* ---------------------------------------------------------------- */
static ulong DirLSN( os9direntry_typ* dir_entry )
{   return os9_long( dir_entry->fdsect );
} /* DirLSN */


static os9err ReadFD( syspath_typ* spP )
/* read the current file description sector */
{   
    rbf_typ*    rbf= &spP->u.rbf;
    ulong       fd =         rbf->fd_nr;
    rbfdev_typ* dev= &rbfdev[rbf->devnr];
    
    if   (fd!=0) return  ReadSector( dev, fd,1, spP->fd_sct );
    return 0;
} /* ReadFD */


static os9err WriteFD( syspath_typ* spP )
/* write the current file description sector */
{   
    rbf_typ*    rbf= &spP->u.rbf;
    ulong       fd =         rbf->fd_nr;
    rbfdev_typ* dev= &rbfdev[rbf->devnr];

    if   (fd!=0) return WriteSector( dev, fd,1, spP->fd_sct );
    return 0;
} /* WriteFD */



static ulong FDSize( syspath_typ* spP )
/* get the file size  */
{   ulong* lp= (ulong*)&spP->fd_sct[9]; return os9_long(*lp);   
} /* FDSize */


static void Set_FDSize( syspath_typ* spP, ulong size )
/* set the file size  */
{   ulong* lp= (ulong*)&spP->fd_sct[9]; *lp= os9_long(size);    
} /* Set_FDSize */


static byte FDAtt( syspath_typ* spP )
/* get the file attributes  */
{   return spP->fd_sct[0];  
} /* FDAtt */


static void Set_FDAtt( syspath_typ* spP, byte att )
/* set the file attributes  */
/* but only if not root directory */
{   
    rbf_typ*    rbf= &spP->u.rbf;
    rbfdev_typ* dev= &rbfdev[rbf->devnr];

    if (rbf->fd_nr!=dev->root_fd_nr) spP->fd_sct[0]= att;   
} /* Set_FDAtt */


static void Set_FDLnk( syspath_typ* spP, byte lnk )
/* set the link count */
{   spP->fd_sct[8]= lnk;    
} /* Set_FDLnk */




static os9err FD_Segment( syspath_typ* spP, byte *attr, ulong *size, ulong *totsize, 
                                           ulong *sect, ulong *pref )
{
    rbfdev_typ* dev= &rbfdev[spP->u.rbf.devnr];
    ulong       *lp, v, scs, blk, add, pos;
    ushort      *sp;
    int         ii;
    Boolean     done= false;

    *attr= FDAtt ( spP );
    *size= FDSize( spP );   
    *pref= 0; /* preferred sector to allocate: none */
    
    v= 0;  ii= 16;
    while (ii+SegSize <= dev->sctSize) {
        lp = (ulong *)&spP->fd_sct[ii  ]; pos= os9_long(*lp)>>BpB;
        sp = (ushort*)&spP->fd_sct[ii+3]; scs= os9_word(*sp);
        blk=                              scs* dev->sctSize;
        
        if (!done && v+blk>spP->u.rbf.currPos) {
            add  =        (spP->u.rbf.currPos-v)/dev->sctSize;
            *sect= pos + add;
            done = true;
        }
        
        if (scs>0) 
            *pref= pos+scs;
            
        v += blk;
        ii+= SegSize; /* goto next entry */
    } /* while */
    
    *totsize= v;
    debugprintf(dbgFiles,dbgDetail,("# FD_Segment size/totsize/sect: %d %d $%x\n",
                                       *size,*totsize,*sect ));
    return 0;
} /* FD_Segment */



static os9err GetThem( rbfdev_typ* dev, ulong pos, ulong scs, Boolean get_them )
/* Allocate the bits at the allocation map */
{
    os9err err;
    ulong  kk;
    
    ulong pdc = pos/dev->clusterSize;
    ulong ii  = pdc / BpB;
    ulong jj  = pdc % BpB;
    ulong asct= ii  / dev->sctSize + 1;
    ulong apos= ii  % dev->sctSize;
    
    byte           imask, rslt;
    byte*          ba;
    byte           mask= 0x80;
    while (jj>0) { mask= mask/2; jj--; } /* search first mask */
    
           kk= scs;
    while (kk>0) {     /* use the device's temporary sector */
        debugprintf(dbgFiles,dbgNorm,("# GetThem (%s) sectorNr: $%x, pos/scs: $%x %d\n",
                                         get_them ? "true":"false", asct, pos,scs ));
        err= ReadSector ( dev, asct,1, dev->tmp_sct ); if (err) return err;
    
        while (kk>0) {
            if (mask==0) { 
                mask= 0x80; apos++;
                if         (apos==dev->sctSize) break; 
            }

            imask= 255-mask; /* invert it */
            ba= &dev->tmp_sct[apos]; /* current buffer field */
            if  (get_them) rslt= *ba |  mask;   /* set   bit */     
            else           rslt= *ba & imask;   /* clear bit */

            if  (rslt==*ba) return E_NES;
            *ba= rslt;

            mask= mask / 2;
            kk -= dev->clusterSize;
        } /* inner while */
    
        err= WriteSector( dev, asct,1, dev->tmp_sct ); if (err) return err;
        asct++;  /* prepare for the next sector */
        apos= 0; /* and don't forget to reset position in sector */
    } /* outer while */
        
    return 0;
} /* GetThem */



static os9err BlkSearch( rbfdev_typ* dev, ulong uscs, ulong mpsct, ulong mploc,
                         Boolean *found,  ulong *pos, ulong *scs )
/* Search for a block with the given size       */
/* start searching in sector <mpsct> at <mploc> */
{
    os9err err;
    ulong  ii, jj;
    byte   vv, mask;
    int    taken;
    ulong  clu   = dev->clusterSize;
    ulong  sOffs = dev->sctSize*(mpsct-1)*BpB;
    ulong  posmax= *pos; /* max values are stored */
    ulong  scsmax= *scs;

    /* look for a new block now */
    *pos= clu*( sOffs+mploc*BpB ); 
    *scs= 0;

    *found= false;
    err= ReadSector( dev, mpsct,1, dev->tmp_sct ); if (err) return err;

    for (ii=mploc; ii<dev->sctSize; ii++ ) { /* go through the sector */
        vv  = dev->tmp_sct[ii];
        mask= 0x80;
        
        for (jj=0; jj<=7; jj++) {
                taken= vv & mask;       /* already taken ? */
            if (taken) { *scs= 0; *pos= clu*( sOffs+ii*BpB + jj+1 ); } 
            else        (*scs)+=        clu;
            
            if (*scs>=  uscs) { *found= true; return 0; }
            if (*scs> scsmax) { posmax= *pos; scsmax= *scs; }
            mask= mask/2;
        } /* for */
    } /* for */

    *pos= posmax;
    *scs= scsmax;
    return 0;
} /* BlkSearch */



static os9err AllocateBlocks( syspath_typ* spP, ulong uscs, ulong *posP, ulong *scsP,
                                                ulong prefpos )
/* allocate <uscs> sectors and. Result is <posP>,<scsP>. */
{
    os9err      err;
    rbfdev_typ* dev  = &rbfdev[spP->u.rbf.devnr];
    ulong       mxsct= (dev->mapSize-1)/dev->sctSize + 1;
    ulong       mpsct, s1;
    ulong       pdc  = prefpos        /dev->clusterSize/BpB;  
    ulong       lst  = dev->last_alloc/dev->clusterSize/BpB;  
    ulong       mploc;
    Boolean     first, found;

    *posP= 0;
    *scsP= 0;

    if (prefpos!=0) {
            mpsct= 1 + pdc / dev->sctSize;
            mploc=     pdc % dev->sctSize;
        if (mpsct>=mxsct) mpsct= 1;

            err= BlkSearch( dev, uscs, mpsct,mploc, &found, posP,scsP ); 
//      printf( "Ask=%5X get=%4X pos=%06X fnd=%d err=%d\n", uscs,*scsP,*posP, found, err );
        if (err) return err;
    }

    if (*scsP==0 || *posP!=prefpos) { /* no preferred allocation */
            mpsct= 1 + lst / dev->sctSize; 
            mploc=     lst % dev->sctSize;
        if (mpsct>mxsct) mpsct= 1;
        s1= mpsct;

        first= true;
        while (true) { /* run through for one whole round */
                err= BlkSearch( dev, uscs, mpsct,mploc, &found, posP,scsP );
//          printf( "ask=%5X get=%4X pos=%06X fnd=%d err=%d\n", uscs,*scsP,*posP, found, err );
            if (err)   return err;
            if (found) break;

            if (!first &&    mpsct==s1) break; /* all done */
            if (mpsct<mxsct) mpsct++;
            else             mpsct= 1;

            first= false;
            mploc= 0; /* from now on search from beginning */
        } /* while (true) */
    }

    if (*scsP==0) return E_FULL;

//  printf( "FND=%5X get=%4X pos=%06X\n", uscs,*scsP,*posP );
    dev->last_alloc=     *posP + *scsP; /* next search position */
    return GetThem( dev, *posP,  *scsP, true );
} /* AllocateBlocks */



static os9err DeallocateBlocks( syspath_typ* spP )
{
    rbfdev_typ* dev= &rbfdev[spP->u.rbf.devnr];
    ulong       fd =         spP->u.rbf.fd_nr;
    int         ii;
    ulong       *lp, pos;
    ushort      *sp, scs;
    
    for (ii=16; ii+SegSize <= dev->sctSize; ii+=SegSize) {
        lp= (ulong *)&spP->fd_sct[ii  ];  pos= os9_long(*lp)>>BpB;
        sp= (ushort*)&spP->fd_sct[ii+3];  scs= os9_word(*sp);

        /* if the fd sector is allocated alone -> dealloc it alone */
        if (ii==16 && fd+1!=pos) GetThem( dev, fd, 1, false );

        if (scs>0) {
            debugprintf(dbgFiles,dbgNorm,("# Dealloate ii/pos/scs: %4d $%x %d\n", 
                                             ii, pos, scs ));
            if (ii==16 && fd+1==pos) { pos--; scs++; } /* add the fd sector */
            GetThem( dev, pos, scs, false ); /* release elements */

            if (dev->last_alloc>pos) dev->last_alloc= pos; /* next pos to search */
        } /* if */
    } /* for */
    
    return 0;
} /* DeallocateBlocks */



static os9err ReleaseBlocks( syspath_typ* spP, ulong lastPos )
{
    os9err      err;
    int         ii;
    rbfdev_typ* dev= &rbfdev[spP->u.rbf.devnr];
    ulong       fd =         spP->u.rbf.fd_nr;
    ulong       cmp= lastPos/dev->sctSize + 1; /* including fd sector for cluster allocation */
    ulong       tps= 0;
    ulong       *lp, pos, diff;
    ushort      *sp, scs;
    Boolean     broken= false;

    /* go through the fs segment list */
    for (ii=16; ii+SegSize <= dev->sctSize; ii+=SegSize) { 
        lp= (ulong *)&spP->fd_sct[ii  ]; pos= os9_long(*lp)>>BpB;
        sp= (ushort*)&spP->fd_sct[ii+3]; scs= os9_word(*sp);
        
        if (scs==0) broken= true;
        if (broken) {
            *lp= 0;
            *sp= 0;
            if (scs>0) GetThem( dev, pos, scs, false );
        }
        else {  tps+= scs;
            if (tps>cmp) {
                    diff= tps-cmp; /* keep track of granularity */
                    diff= diff/dev->clusterSize*dev->clusterSize;
                if  (diff>=scs) {
                    diff= scs;
                    *lp = 0;
                    *sp = 0;
                }
                else *sp = os9_word(scs-diff);
                
                broken= true;
                GetThem( dev, (pos+scs)-diff, diff, false );
            }
        } /* if (scs>0) */
    } /* for */
    
    err= WriteFD( spP ); return err;
} /* ReleaseBlocks */



static os9err AdaptAlloc_FD( syspath_typ* spP, ulong pos, ulong scs )
{
    #define     LimScsPerSegment 0x8000 /* number of sectors per segment must be less than this */
    #define     First  16
    #define     Second First+SegSize
    os9err      err;
    rbfdev_typ* dev= &rbfdev[spP->u.rbf.devnr];
    int         ii;
    ulong       *lp,  prev_l,  blk, mx;
    ushort      *sp, *prev_sp, psp, size= 0;
    ulong       lpos= pos; /* treat them locally, because 'Get_Them' uses it also */
    ushort      lscs= scs;

    prev_sp= &size; /* initial value */
    for (ii=First; ii+SegSize <= dev->sctSize; ii+=SegSize) {
        lp= (ulong *)&spP->fd_sct[ii  ];
        sp= (ushort*)&spP->fd_sct[ii+3];

        if (*sp==0) {          /* zero is zero for big/little endian */
                psp= os9_word(*prev_sp);
            if (psp>0 && prev_l+psp==lpos) {  /* combine it */
                if (ii==Second)    mx= LimScsPerSegment-1; /* assuming clusterSize is divideable */
                else               mx= LimScsPerSegment-dev->clusterSize;
                blk= Min( psp+lscs,mx ); /* but not larger than the max allowed size */
                *prev_sp= os9_word(blk);
                
                lpos+= blk-psp;
                lscs-= blk-psp; /* adjust the number of blocks */
            }
                
            if (lscs>0) {
                if (ii==First) mx= LimScsPerSegment-1; /* assuming clusterSize is divideable */
                else           mx= LimScsPerSegment-dev->clusterSize;
                blk= Min( lscs,mx );
                                
                *lp= os9_long(lpos<<BpB);
                *sp= os9_word(blk); /* one byte overlapping */

                lpos+= blk;
                lscs-= blk;
            }
            
            if (lscs==0) {
                err= WriteFD( spP ); if (err) return err;
                return 0;
            }
        } /* if */
        
        prev_l = os9_long(*lp)>>BpB;
        prev_sp= sp; 
    } /* for */

    GetThem( dev, pos,scs, false );
    return E_SLF;
} /* AdaptAlloc_FD */



static os9err DoAccess( syspath_typ* spP, ulong *lenP, byte* buffer, 
                 Boolean lnmode, Boolean wMode )
/* this is the main read routine for the RBF emulator */
/* it is working for read and readln */
{
    os9err      err   =  0;
    rbf_typ*    rbf   = &spP->u.rbf;
    rbfdev_typ* dev   = &rbfdev[rbf->devnr];
    ulong       bstart=  rbf->currPos;
    ulong       boffs =  0;
    ulong       remain= *lenP;
    ulong*      mw    = &spP->mustW;
    ulong       ma    = Max( dev->sas,dev->clusterSize );
    ushort*     w;
    ulong       sect, offs, size, totsize, maxc, pos, scs, *rs, pref, coff, sv;
    byte*       bb;
    byte        attr;
    byte        *bp;
    int         ii;
    Boolean     done= false;               /* break condition for readln  */
    Boolean     rOK = (remain==0);         /* is true, if nothing to read */

    debugprintf( dbgFiles,dbgDetail,("# >DoAccess (%s): n=%d\n", wMode ? "write":"read", *lenP ));
    sv= rbf->currPos;
    
    do {          /* do this loop for every sector to be read into buffer */
        if (spP->rawMode) {
            sect= rbf->currPos / dev->sctSize;       /* get raw sector nr */    
            size= -1; rs= &size; /* no upper limit */
        }
        else {                           /* get sector nr of segment info */
            FD_Segment( spP, &attr,&size,&totsize,&sect, &pref );
                                        rs= &totsize; /* the relevant size */
            if (!wMode && size<totsize) rs= &size;
            
            if (totsize<=rbf->lastPos)      rbf->lastPos= totsize;
            if (*rs    <=rbf->lastPos) *rs= rbf->lastPos;

            if (!wMode) { /* reduce to currect ammount */
                if     (*rs<rbf->currPos+remain) 
                remain= *rs-rbf->currPos;
            }
            
            if (rbf->currPos>=*rs) {
                if (wMode) {
                    err= AllocateBlocks( spP,ma, &pos,&scs, pref ); if (err) break;
                    err= AdaptAlloc_FD ( spP,     pos, scs       ); if (err) break;
                    
                    FD_Segment( spP, &attr,&size,&totsize,&sect, &pref );
                                             rs= &totsize;
                    if (rbf->currPos>=*rs) {
                        debugprintf( dbgFiles,dbgDetail,("# DoAccess (%s) EOF: n=%d\n", 
                                                            wMode ? "write":"read", *lenP ));
                        return E_EOF;
                    }               
                }
                else {
                    if (rOK)    break; /* reading is ok so far */
                    err= E_EOF; break;
                }
            }
        }
    
        offs= rbf->currPos % dev->sctSize; /* get offset within this sector */
        if (spP->rw_nr==0     /* read only if different one */
         || spP->rw_nr!=sect) {
            if (*mw!=0) {
                err= WriteSector( dev,  *mw,1, spP->rw_sct ); if (err) break;
                *mw =0; /* now it is written */
            }

            err=      ReadSector( dev, sect,1, spP->rw_sct ); if (err) break;
            spP->rw_nr= sect;
            if (sect==0) 
                err= ChkIntegrity( dev, spP,   spP->rw_sct,true ); if (err) break;
        }   

        rOK = true;
        debugprintf(dbgFiles,dbgDeep,("# RBF %s: \"%s\" $%x bytes, sect: $%x, size: $%x\n", 
                    wMode ? "write":"read", dev->name, remain, sect, *rs));
        
            maxc= dev->sctSize-offs;  /* calculate the max number of bytes to be read */
        if (maxc>remain) maxc= remain;
        
        if (lnmode) {       /* depends on read or write */
            if (wMode) { bb=      buffer; coff= boffs; }
            else       { bb= spP->rw_sct; coff=  offs; }

            for (ii=coff; ii<coff+maxc; ii++) {
                if (bb[ii]==CR) { done= true; maxc= ii-coff+1; break; }
            }
        }
    
        /* copy to/from the buffer */   
        if (wMode) {
            memcpy(spP->rw_sct+ offs, buffer+boffs, maxc);

            /* if sector in raw mode */
            if (spP->rawMode) {
                    if (sect==0) {
                        w= (ushort*)&spP->rw_sct[14]; /* get the new disk ID */
                        dev->last_diskID = *w;        /* must be done */
                        rbf->diskID=       *w;
                    }

                    if (sect==1) dev->last_alloc= 0; /* reset after "format" */
                    err= WriteSector( dev, sect,1, spP->rw_sct ); if (err) break;
                    spP->rw_nr= sect;
            }
            else {
                if (*mw!=0 && *mw!=sect) { /* if sector nr has changed */
                    err= WriteSector( dev,  *mw,1, spP->rw_sct ); if (err) break;
                }
                *mw= sect;
            }
        }
        else memcpy(buffer+boffs, spP->rw_sct+ offs, maxc);

                      
        rbf->currPos+= maxc; /* calculate the new position */
    
        bp   = (byte *) rbf->currPos - bstart;
        *lenP= (ulong) bp;
        
        if (done) {
            maxc = remain; /* loop is finished now because CR has been found */
        }
        else if (rbf->currPos > *rs) {         /* remaining byte calculation */
            bp   = (byte *) *lenP  +  *rs - rbf->currPos;
            *lenP= (ulong) bp;
            rbf->currPos= *rs;
        }
        
        remain-= maxc; /* adapt them for the next loop */
        boffs += maxc;
    } while (remain>0);
    
    if (err==E_FULL) {
        rbf->currPos= sv;
        rbf->lastPos= sv;
    }
    
    if (rbf->lastPos< rbf->currPos) /* adapt lastpos */
        rbf->lastPos= rbf->currPos;
    
    debugprintf( dbgFiles,dbgDetail,("# <DoAccess (%s): n=%d\n", wMode ? "write":"read", *lenP ));
    return err;
} /* DoAccess */



static os9err Create_FD( syspath_typ* spP, byte att, ulong size )
{
    rbfdev_typ* dev= &rbfdev[spP->u.rbf.devnr];
    
    int  ii;
    for (ii=0; ii<dev->sctSize; ii++) { /* clear sector */
          spP->fd_sct[ii]= NUL;
    }

    Set_FDAtt     ( spP,  att ); /* attributes */
    Set_FDLnk     ( spP,    1 ); /* the link count */
    Set_FDSize    ( spP, size ); /* file size  */
    return WriteFD( spP );       /* write FD sector */
} /* Create_FD */



static os9err OpenDir( rbfdev_typ* dev, ulong dfd, ushort *sp )
{
    os9err       err;
    syspath_typ* spP;
    rbf_typ*     rbf;

    err= syspath_new( sp,fRBF ); if (err) return err;

    spP         = &syspaths[*sp];
    spP->fd_sct = NULL;           /* prepare them locally */
    spP->rw_sct = NULL;
    spP->rw_nr  = 0;              /* undefined */
    GetBuffers  ( dev,spP );      /* dev must be assigned before */

    rbf         = &spP->u.rbf;
    rbf->currPos= 0;              /* initialize position to 0 */
    rbf->wMode  = true;           /* by default it can be written */
    rbf->devnr  = dev->nr;
    rbf->fd_nr  = dfd;            /* the directory's sector */

        err=   ReadFD        ( spP );            /* IMPORTANT !! */
    if (err)   ReleaseBuffers( spP );
    else rbf->lastPos= FDSize( spP );      /* get the file size  */

    return err; 
} /* OpenDir */


static os9err CloseDir( ushort sp )
{
    os9err       err;
    syspath_typ* spP= &syspaths[sp];

    spP->u.rbf.currPos= 0; /* don't release blocks */
    err= syspath_close( 0,sp );
    return err;
} /* CloseDir */



static void Fill_DirEntry( os9direntry_typ* dir_entry, char* name, ulong fd )
{
    byte* b= (byte*)dir_entry;
    int   ii;

    if (*name==NUL) {          /* if entry must be cleared */
        *dir_entry->name= NUL; /* clear string: do it the same way as OS-9 */
    }
    else {
        for (ii=0; ii<DIRENTRYSZ; ii++) {
            *b= NUL; b++; /* Clear block */
        }

        /* write it only if new entry */
        strcpy     ( dir_entry->name, name );
        LastCh_Bit7( dir_entry->name, true );
                     dir_entry->fdsect= os9_long( fd );
    }
} /* Fill_DirEntry */



static os9err Access_DirEntry( rbfdev_typ* dev, ulong dfd,  ulong fd, 
                                                char* name, ulong *deptr )
{
    os9err          err, cer;
    ulong           dir_len;
    os9direntry_typ dir_entry;
    ushort          sp;
    syspath_typ*    spP;
    Boolean         found;
    Boolean         deleteIt= (fd==0);
    char            *cmp, *fill;
         
    if (deleteIt)  { cmp= name; fill= "";   }
    else           { cmp= "";   fill= name; }
            
    err= OpenDir( dev,dfd, &sp ); if (err) return err;
    spP= &syspaths[sp];
    
    while (true) {               dir_len= DIRENTRYSZ; /* read 1 dir entry */
            err= DoAccess( spP, &dir_len, (byte*)&dir_entry, false,false );
                                     LastCh_Bit7( dir_entry.name,  false );
        if (err) {
            if (err!=E_EOF)              break;
            if (deleteIt) { err= E_PNNF; break; } /* E_EOF for del: not found */
        }
        
            found= (ustrcmp( cmp, dir_entry.name )==0);
        if (found)                       /* make something with the entry */
            spP->u.rbf.currPos= spP->u.rbf.currPos-dir_len;  /* seek back */
        
        if (err==E_EOF || found) {
            *deptr= spP->u.rbf.currPos;
            Fill_DirEntry               ( &dir_entry, fill, fd   );
                                 dir_len= DIRENTRYSZ;
            err= DoAccess( spP, &dir_len, (byte*)&dir_entry, false,true ); 
            break;
        }
    } /* loop */
    
    cer= CloseDir( sp ); if (!err) err= cer;
    return err;
} /* Access_DirEntry */



static os9err Delete_DirEntry( rbfdev_typ* dev, ulong fd,  char* name )
{   /* file sector 0 deletes the file */
    ulong  d; /* no interrest for this variable here */
    return Access_DirEntry( dev, fd, 0, name, &d );
} /* Delete_DirEntry */



static os9err touchfile_RBF( syspath_typ* spP, Boolean creDat )
{
    struct tm tim;  /* Important Note: internal use of <tm> as done in OS-9 */

    GetTim       ( &tim );
    spP->fd_sct[3]= tim.tm_year;
    spP->fd_sct[4]= tim.tm_mon+1; /* somewhat different month notation */
    spP->fd_sct[5]= tim.tm_mday;
    spP->fd_sct[6]= tim.tm_hour;
    spP->fd_sct[7]= tim.tm_min;
    
    if (creDat) {
        spP->fd_sct[13]= tim.tm_year;
        spP->fd_sct[14]= tim.tm_mon+1; /* somewhat different month notation */
        spP->fd_sct[15]= tim.tm_mday;
    }
    
    return WriteFD( spP );
} /* touchfile_RBF */



static os9err CreateNewFile( syspath_typ* spP, byte fileAtt, char* name, ulong csize )
{
    os9err      err;
    rbf_typ*    rbf= &spP->u.rbf;
    rbfdev_typ* dev= &rbfdev[rbf->devnr];
    ulong       sct=  dev->sctSize;
    ulong       clu=  dev->clusterSize;
    ulong       dfd=  rbf->fd_nr;
    ulong*      d  = &rbf->deptr;
    ulong       fd, scs, ascs;

    if (strlen(name)>DIRNAMSZ) return E_BPNAM;
    
         /* 1 fd + sectors according to cluster size */
    scs= (csize+sct-1)/sct+1;
    scs=       (scs-1)/clu+1; /* and adapt for cluster granularity */
    scs=        scs   *clu;

    err=     AllocateBlocks ( spP, scs, &fd,  &ascs, 0 ); if (err) return err;
             spP->u.rbf.fd_nr=           fd; /* access them correctly */
             spP->u.rbf.fddir=     dfd;     
    err=     Create_FD      ( spP,          fileAtt, 0 ); if (err) return err;
    err=     Access_DirEntry( dev, dfd,  fd,   name, d ); if (err) return err;

    if  (scs>1) { /* get the remaining part */
        err= AdaptAlloc_FD  ( spP,       fd+1, ascs-1  ); if (err) return err;
    }
    
           err= touchfile_RBF( spP,true );
    if   (!err) strcpy( spP->name, name ); /* assign file name, if everything is ok */
    return err;
} /* CreateNewFile */



static os9err ConvertToDir( syspath_typ* spP )
{
    Set_FDAtt     ( spP, 0xbf );  /* as directory */    
    return WriteFD( spP );
} /* ConvertToDir */




/* ------------------------------------------------------------ */
/* visible procs */

os9err pRopen( ushort pid, syspath_typ* spP, ushort *modeP, const char* name )
{
    char            tmp[OS9PATHLEN];
    char*           pathname;
    rbf_typ*        rbf= &spP->u.rbf;
    rbfdev_typ*     dev;
    os9err          err= 0;
    ulong           sect, size, totsize, dir_len, pref;
    os9direntry_typ dir_entry;
    char            cmp_entry[OS9NAMELEN];
    int             root, isFileEntry;    
    char*           p;
    byte            attr;
    ushort          cdv;
    long            ls;
    process_typ*    cp    = &procs[pid];
    Boolean         isExec= IsExec(*modeP);
    Boolean         isFile=       (*modeP & 0x80)==0;
    Boolean         cre   = IsCrea(*modeP);
    byte            mode  =       (*modeP & 0x47); /* attributes to be used */
    char*           co    = cre    ? "create":"open";
    char*           fo    = isFile ? "file"  :"dir";
    char*           curpath;
    Boolean         new_inst;



    strncpy ( tmp,name, OS9PATHLEN );
    pathname= tmp;
    debugprintf(dbgFiles,dbgNorm,("# RBF %s: '%s' (%s)\n" , co, pathname, fo ));
    
    rbf->currPos= 0; /* initialize position to 0 */
    rbf->lastPos= 0;

        root= IsRoot(pathname); /* root path must be a directory */
    if (root && isFile) return E_FNA;

    /* get pointers to execution/current path */
    if (isExec) { cdv= cp->x.dev; ls= cp->x.lsn; curpath= cp->x.path; }
    else        { cdv= cp->d.dev; ls= cp->d.lsn; curpath= cp->d.path; }

    err= DeviceInit( pid, &dev, spP, cdv, pathname,curpath, *modeP, &new_inst ); if (err) return err;
    debugprintf(dbgFiles,dbgNorm,("# RBF before adapt '%s' '%s'\n" , pathname,mnt_name ));
    AdaptPath            ( dev,          &pathname ); /* adapt to offical name */
    debugprintf(dbgFiles,dbgNorm,("# RBF after  adapt '%s' '%s'\n" , pathname,mnt_name ));

    /* now the path could be a new root */
        root= IsRoot(pathname); /* root path must be a directory */
    if (root && isFile) return E_FNA;
    if (root && new_inst && *mnt_name!=NUL) {
        strcpy( pathname,  PSEP_STR );
        strcat( pathname,  mnt_name ); /* the mount name can be different */
        strcpy( dev->name, mnt_name );           /* to the root path name */
    }
    
    debugprintf(dbgFiles,dbgNorm,("# RBF mount  adapt '%s' '%s' %d %d\n" , pathname,mnt_name, root,new_inst ));
    
    rbf->devnr = dev->nr;
    rbf->diskID= dev->last_diskID;
    rbf->wMode = IsWrite(*modeP);
    GetBuffers ( dev,spP ); /* get the internal buffer structures now */
    spP->rw_nr = 0;         /* undefined */

    do {
        /* take care of write protection */     
        if (cre && dev->wProtected) { err= E_WP; break; }

                  p= pathname; /* that's it for raw mode */   
        if (IsRaw(p)) { 
        	        spP->rawMode= true;
            strcpy( spP->name,pathname ); if (cre) { err= E_CEF; break; }
            return 0; 
        } /* if (IsRaw) */

        if (AbsPath(p)) { /* if abs path -> search from the root */                     
            err= RootLSN( dev, spP, false );   if (err) break;
            err= ReadFD      ( spP );          if (err) break;
            if (root)   { 
                strcpy( spP->name,pathname+1 );
                return 0; 
            }
                           
                              p++; /* cut root path */
            err= CutOS9Path( &p, (char*)&cmp_entry ); if (err) break;
        }
        else {  /* if string is empty */
            if (*pathname==NUL) { err= E_FNA; break; }
            rbf->fd_nr= ls; /* take current path */
            err= ReadFD( spP );      if (err) break;
        }
    } while (false);
    if (err) return err;


    rbf->fddir= rbf->fd_nr;
    rbf->deptr= 0;
    err= CutOS9Path( &p, (char*)&cmp_entry ); if (err) return err;
    
    /* this is the recursion loop for directory entries */
    while (true) {               dir_len= DIRENTRYSZ; /* read 1 dir entry */
            err= DoAccess( spP, &dir_len, (byte*)&dir_entry, false,false ); 
        if (err) {
            if (err==E_EOF) {
                if (cre) {  /* create it ? */
                    err= CreateNewFile( spP, procs[pid].fileAtt,
                          (char*)&cmp_entry, procs[pid].cre_initsize );
                    rbf->currPos= 0;  /* initialize position to 0 */
                    rbf->lastPos= 0;
                }
                else err= E_PNNF; /* OS-9 expects E_PNNF, if entry not found */

            }
            break; /* leave loop also, if file has been correctly created */
        } /* if */


        debugprintf(dbgFiles,dbgDetail,("# RBF path : \"%s\" \"%s\"\n", 
                                           cmp_entry,dir_entry.name ));
        LastCh_Bit7(dir_entry.name,false );        /* normalize name, allow compare */
        if (ustrcmp(dir_entry.name,cmp_entry)==0) { /* now the entry has been found */
            debugprintf(dbgFiles,dbgNorm,("# RBF path found : \"%s\"\n", dir_entry.name ));
            rbf->fddir= rbf->fd_nr;
            rbf->deptr= rbf->currPos-DIRENTRYSZ;
            rbf->currPos= 0;                      /* initialize position to 0 */
            rbf->lastPos= 0;                      /* last       position is 0 */

                             rbf->fd_nr= DirLSN( &dir_entry );
            err= ReadFD    ( spP );                                    if (err) break;  
            err= FD_Segment( spP, &attr,&size,&totsize,&sect, &pref ); if (err) break;
            rbf->lastPos= size;                   /* last pos is the filesize */
            rbf->att    = attr;                   /* save attributes */
            isFileEntry= (attr & 0x80)==0x00;     /* recognized as file entry */
            
            err= CutOS9Path( &p, (char*)&cmp_entry ); if (err) break;

            if (*cmp_entry==NUL) {                /* no more sub directories */
                strcpy( spP->name, dir_entry.name );
                
                if     (isFileEntry) {            /* if it is a file entry */
                    if (isFile) {
                        if (cre)  err= E_CEF;     /* already there */
                        else      err= 0;         /* is there as file -> ok */
                    }
                    else        { err= E_FNA;     /* is path, should be file */
                        if (size >= 8192     && 
                           (size %  2048)==0 &&
                            !spP->fullsearch) {   /* but try it again in special mode */
                             spP->fullsearch= true;
                             debugprintf(dbgFiles,dbgNorm,("# not a path >>fullsearch '%s'\n",         
                                                              pathname ));
                             err= pRopen( pid, spP, modeP,    pathname );
                             debugprintf(dbgFiles,dbgNorm,("# not a path <<fullsearch '%s', err=%d\n",
                                                              pathname,err ));
                             spP->fullsearch= false;
                        } /* if */
                    }
                }
                else {                            /* if it is a dir  entry */
                    if (isFile) err= E_FNA;       /* is file, should be path */
                    else        err= 0;           /* is there path -> ok */
                }
                
                if (!err &&  rbf->wMode
                         && !dev->wProtected) err= touchfile_RBF( spP, false );
                break;
            }
            else {                                      /* more sub directories ? */
                if (isFileEntry) { err= E_FNA; break; } /* must be a directory */
            }
        } /* if entry found */
    } /* while (true) */    
    
    return err;
} /* pRopen */



os9err pRclose( ushort pid, syspath_typ* spP )
{
    os9err      err=  0;
    rbf_typ*    rbf= &spP->u.rbf;
    rbfdev_typ* dev= &rbfdev[rbf->devnr];
    ulong       crp=  rbf->currPos;
    ulong       lsp=  rbf->lastPos;
    ulong       v;

    debugprintf(dbgFiles,dbgNorm,("# RBF close (pid=%d) wMode/rawMode/mustW: %d %d $%x\n", 
                                     pid, rbf->wMode,spP->rawMode, spP->mustW ));

    if (rbf->wMode==0 || /* no change in read or raw mode */
        spP->rawMode) return 0;
        
    if (spP->mustW) { /* write the last cached sector */
        err= WriteSector( dev, spP->mustW,1, spP->rw_sct ); if (err) return err;
    }

    /* set file size at close */
        v= FDSize( spP ); 
    if (v< lsp) { 
        v= lsp; Set_FDSize( spP,v ); /* new file size */
        err=       WriteFD( spP ); if (err) return err;
    }

    /* release remaining part, if pointer is not at the end of file */
    if (crp!=0 &&
        crp==lsp) {
        Set_FDSize        ( spP,crp ); /* new file size */        
        err= ReleaseBlocks( spP,crp );
    }
    
    ReleaseBuffers( spP );
    return err;
} /* pRclose */



os9err pRread( ushort pid, syspath_typ* spP, ulong *lenP, char* buffer )
{   
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    return DoAccess( spP, lenP,buffer, false,false );
} /* pRread */


os9err pRreadln ( ushort pid, syspath_typ* spP, ulong *lenP, char* buffer )
{   
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    return DoAccess( spP, lenP,buffer, true,false );
} /* pRreadln */


os9err pRwrite  ( ushort pid, syspath_typ* spP, ulong *lenP, char* buffer )
{   
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    return DoAccess( spP, lenP,buffer, false,true );
} /* pRwrite */


os9err pRwriteln( ushort pid, syspath_typ* spP, ulong *lenP, char* buffer )
{   
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    return DoAccess( spP, lenP,buffer, true,true );
} /* pRwriteln */



os9err pRseek( ushort pid, syspath_typ* spP, ulong *posP )
/* seek to new file position <posP> */
{   
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    rbfdev_typ* dev= &rbfdev[spP->u.rbf.devnr];
    
    debugprintf(dbgFiles,dbgNorm,("# RBF seek: dev='%s' file='%s' pos: %x\n", 
                                     dev->name, spP->name, *posP));
    spP->u.rbf.currPos= *posP;
    return 0;
} /* pRseek */



os9err pRchd( ushort pid, syspath_typ* spP, ushort *modeP, char* pathname )
{
    os9err       err;
    ushort       path;
    long*        xD;
    short*       xV;
    Boolean      exedir= IsExec(*modeP);
    process_typ* cp= &procs[pid];
    rbfdev_typ*  dev;
    int          n;
    char*        curpath;
    char         tmp[OS9PATHLEN];
    
      /* get current default path */
    if (exedir) { xV= &cp->x.dev; xD= &cp->x.lsn; curpath= cp->x.path; }
    else        { xV= &cp->d.dev; xD= &cp->d.lsn; curpath= cp->d.path; }
        
    err=     usrpath_open( pid,&path, fRBF, pathname,*modeP); if (err) return err;
        spP= get_syspathd( pid, cp->usrpaths[path] );
    if (spP==NULL) return os9error(E_BPNUM);
    
    *xV= spP->u.rbf.devnr; dev= &rbfdev[ *xV ];   
    *xD= spP->u.rbf.fd_nr;
    
    if (AbsPath(pathname)) strcpy( curpath,"" );
    else                   strcat( curpath,PSEP_STR );
    strcat( curpath,pathname );
      
    n= strlen(dev->img_name);
    if (SamePathBegin( curpath,dev->img_name )) {
        strcpy( tmp,   curpath );
        strcpy( curpath,PSEP_STR  ); /* get a str staring with dev->name instead of dev->img_name */
        strcat( curpath,dev->name );
        strcat( curpath,&tmp[n]   );
    }     
    EatBack( curpath );
    
    err= usrpath_close( pid, path ); if (err) return err;

    if (exedir) cp->x.type= fRBF; /* adapt the type */
    else        cp->d.type= fRBF;
    
    return 0;
} /* pRchd */



os9err pRdelete( ushort pid, syspath_typ* spP, ushort *modeP, char* pathname )
{
    os9err      err;
    rbfdev_typ* dev;
    ushort      path;
    ulong       dfd;
    
    err=    usrpath_open( pid,&path, fRBF, pathname,*modeP ); if (err) return err;
        spP= get_syspath( pid, procs[pid].usrpaths[path] ); /* get spP for fd sects */
    if (spP==NULL) return os9error(E_BPNUM);
    
    dev= &rbfdev[spP->u.rbf.devnr]; /* can't be assigned earlier */
    dfd=         spP->u.rbf.fddir;

    err= Delete_DirEntry ( dev, dfd, (char*)&spP->name ); if (err) return err;
    err= DeallocateBlocks( spP );                         if (err) return err;
    spP->u.rbf.currPos= 0;   /* seek back to zero, avoid release remaining part */

    err= usrpath_close( pid, path ); return err;
} /* pRdelete */



os9err pRmakdir( ushort pid, syspath_typ* spP, ushort *modeP, char* pathname )
{
    #ifndef linux
    #pragma unused(modeP)
    #endif

    os9err err;
    ulong  size= 2*DIRENTRYSZ;
    os9direntry_typ dirblk[ 2 ];
    ushort path;
    
    procs[pid].fileAtt     = 0xBF;
    procs[pid].cre_initsize= 0;   /* don't use mode at the moment */
    err= usrpath_open( pid,&path, fRBF, pathname, 0x03 | poCreateMask ); if (err) return err;

        spP= get_syspath( pid, procs[pid].usrpaths[path] ); /* get spP for fd sects */
    if (spP==NULL) return os9error(E_BPNUM);
    
    /* prepare the contents */
    Fill_DirEntry( &dirblk[0], "..", spP->u.rbf.fddir );    
    Fill_DirEntry( &dirblk[1], "." , spP->u.rbf.fd_nr );    
    
    err= usrpath_write( pid,path, &size, &dirblk, false ); if (err) return err;
    spP->u.rbf.currPos= 0;   /* seek back to zero, avoid release remaining part */
    err= ConvertToDir      ( spP );                        if (err) return err;
    err= usrpath_close(pid, path);                                  return err;
} /* pRmakdir */



os9err pRpos( ushort pid, syspath_typ* spP, ulong *posP )
/* get current file position <posP> */
{
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    rbf_typ*    rbf= &spP->u.rbf;
    rbfdev_typ* dev= &rbfdev[rbf->devnr];
    
    *posP= rbf->currPos;
    debugprintf(dbgFiles,dbgNorm,("# RBF pos: '%s' %d\n", dev->name, *posP ));
    return 0;
} /* pRpos */



/* get options for RBF file */
os9err pRopt(ushort pid, syspath_typ* spP, byte *buffer)
{
    os9err      err= pRBFopt( pid,spP, buffer );
    rbf_typ*    rbf= &spP->u.rbf;
    rbfdev_typ* dev= &rbfdev[rbf->devnr];
    byte*       b;
    ushort*     w;
    ulong*      l;
    char*       c;

    /* and fill some specific RBF path values */
    b= (byte  *)&buffer[ PD_TYP    ]; *b=          dev->pdtyp;
    w= (ushort*)&buffer[ PD_SAS    ]; *w= os9_word(dev->sas);        /* sector alloc size */
    w= (ushort*)&buffer[ PD_SSize  ]; *w= os9_word(dev->sctSize);    /* phys sect size    */
    b= (byte  *)&buffer[ PD_CtrlrID]; *b=          dev->scsiID;
    b= (byte  *)&buffer[ PD_ATT    ]; *b=          rbf->att;
    l= (ulong *)&buffer[ PD_FD     ]; *l= os9_long(rbf->fd_nr<<BpB); /* LSN of file       */
    l= (ulong *)&buffer[ PD_DFD    ]; *l= os9_long(rbf->fddir<<BpB); /* LSN of its dir    */
    l= (ulong *)&buffer[ PD_DCP    ]; *l= os9_long(rbf->deptr);      /* dir entry pointer */
    l= (ulong *)&buffer[ PD_DVT    ]; 
    l= (ulong *)&buffer[ PD_SctSiz ]; *l= os9_long(dev->sctSize);    /* logical sect size */
    c= (char  *)&buffer[ PD_NAME   ];  strcpy( c,  spP->name );      /* name */

    return err;
} /* pRopt */



os9err pRready( ushort pid, syspath_typ* spP, ulong *n )
/* check ready */
{
    #ifndef linux
    #pragma unused(pid,spP)
    #endif

    *n= 1; return 0;
} /* pRready */




os9err pRgetFD(ushort pid, syspath_typ* spP, ulong *maxbytP, byte *buffer)
/* get the current FD sector of the opened path */
{
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    debugprintf(dbgFiles,dbgNorm,("# RBF getFD (fd/bytes): $%x %d\n", 
                                     spP->u.rbf.fd_nr, *maxbytP ));

    memcpy( buffer, spP->fd_sct, *maxbytP); /* copy to the buffer */
    return 0;
} /* pRgetFD */



os9err pRgetFDInf(ushort pid, syspath_typ* spP, ulong *maxbytP, 
                                                ulong *fdinf, byte *buffer)
/* get any FD sector ( using variable <fdinf> ) */
{
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    os9err      err;
    rbfdev_typ* dev= &rbfdev[spP->u.rbf.devnr];

    debugprintf(dbgFiles,dbgNorm,("# RBF getFDInf (fd/bytes): $%x %d\n", 
                                     *fdinf, *maxbytP ));

    err= ReadSector( dev,*fdinf,1, dev->tmp_sct ); if (err) return err;
    memcpy               ( buffer, dev->tmp_sct, *maxbytP); /* copy to the buffer */
    return 0;
} /* pRgetFDInf */



os9err pRsetFD(ushort pid, syspath_typ* spP, byte *buffer)
/* set the current FD sector */
{
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    ulong maxbyt= 16;
    debugprintf(dbgFiles,dbgNorm,("# RBF setFD (fd/bytes): $%x %d\n", 
                                     spP->u.rbf.fd_nr, maxbyt ));

    memcpy( spP->fd_sct, buffer, maxbyt );  /* copy to the buffer */
    return WriteFD( spP );
} /* pRsetFD */



os9err pRsize( ushort pid, syspath_typ* spP, ulong *sizeP )
/* get the size of a file */
{
    os9err      err;
    rbf_typ*    rbf= &spP->u.rbf;
    rbfdev_typ* dev= &rbfdev[rbf->devnr];
    byte        attr;
    ulong       sect, totsize, sv, pref;

    if (spP->rawMode) {
        *sizeP= dev->totScts*dev->sctSize;
        return 0;
	}

    sv = rbf->currPos;
         rbf->currPos= 0;  /* initialize position to 0 */
    
    err= FD_Segment( spP, &attr,sizeP,&totsize,&sect, &pref );

         rbf->currPos= sv; /* get saved position back */
    if  (rbf->lastPos>*sizeP) *sizeP= rbf->lastPos;

    debugprintf( dbgFiles,dbgNorm,("# RBF size (pid=%d): %d\n", pid, *sizeP ));
    return err;
} /* pRsize */



os9err pRdsize(ushort pid, syspath_typ* spP, ulong* size, ulong* dtype )
/* get the size of the device as numbers of sectors */
/* the <dtype> field will be returned as 0, to avoid problems with "castype" */
{
    os9err      err;
    rbfdev_typ* dev= &rbfdev[spP->u.rbf.devnr];
    
    err   = DevSize( dev ); if (err) return err;
    *size = dev->totScts;
    *dtype= 0;

    debugprintf( dbgFiles,dbgNorm,("# RBF dsize (pid=%d): %d\n", pid, *size ));
    return 0;
} /* pRdsize */



os9err pRsetsz( ushort pid, syspath_typ* spP, ulong *size )
/* set the size of a file */
{
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    rbf_typ* rbf= &spP->u.rbf;
    
        rbf->lastPos= *size; /* set position      to new max */
    if (rbf->currPos> *size)
        rbf->currPos= *size; /* set position back to new max */

    Set_FDSize    ( spP,*size ); /* new file size */
    return WriteFD( spP );
} /* pRsetsz */



os9err pRsetatt( ushort pid, syspath_typ* spP, ulong *attr )
/* set the attributes of a file */
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    Set_FDAtt     ( spP, (byte)*attr ); /* byte ordering is already correct */
    return WriteFD( spP );
} /* pRsetatt */



os9err pRnam( ushort pid, syspath_typ* spP, char* volname )
/* get device name of RBF device */
{
    process_typ* cp = &procs [pid];
    rbfdev_typ*  dev= &rbfdev[spP->u.rbf.devnr];
    
                     strcpy( volname, dev->name  );
    if (SamePathBegin( &cp->d.path[1],dev->name2 )) strcpy( volname, dev->name2 );
    if (SamePathBegin( &cp->d.path[1],dev->name3 )) strcpy( volname, dev->name3 );
    
    debugprintf( dbgFiles,dbgNorm,("# RBF name: %s\n", volname ));
    return 0;
} /* pRnam*/


os9err pRWTrk( ushort pid, syspath_typ* spP, ulong* trackNr )
/* get device name of RBF device */
{
    os9err      err;
    rbfdev_typ* dev= &rbfdev[spP->u.rbf.devnr]; 
    int         ii;
    ulong       sctNr, scts, dtype;
    
    for (ii=0; ii<dev->sctSize; ii++)
        dev->tmp_sct[ii]= 0xE5; /* fill with formatting pattern */
    
    err= pRdsize( pid,spP, &scts, &dtype );
    
    for (ii=0; ii<scts; ii++) { sctNr= *trackNr*DEFAULT_SCT + ii;
      err=    WriteSector( dev, sctNr,1, dev->tmp_sct ); if (err) return err;
    }
    
    return 0;
} /* pRWTrk*/



/* eof */

