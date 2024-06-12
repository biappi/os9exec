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
 *    Revision 1.51  2007/04/17 23:06:10  bfo
 *    FD_ID calling args changed and adapted
 *
 *    Revision 1.50  2007/04/13 10:08:43  bfo
 *    File copy correctly done for ftruncate workaround (MacOSX CW)
 *
 *    Revision 1.49  2007/04/10 22:13:19  bfo
 *    No longer dependent on "Volumes", same treatment MacOSX/Linux
 *
 *    Revision 1.48  2007/04/01 15:04:28  bfo
 *    workaround for MACH ftruncate: copy the file
 *
 *    Revision 1.47  2007/03/31 12:21:07  bfo
 *    - LINKED_HASH support (by using <cond>)
 *    - Set_FileDate supported now for MacOSX
 *
 *    Revision 1.46  2007/03/24 12:47:35  bfo
 *    - <readFlag> introduced (seek when change read -> write)
 *    - ftruncate adaptions (still not working for CW MacOSX)
 *    - check_vod moved to "macfiles"
 *
 *    Revision 1.45  2007/03/10 14:04:29  bfo
 *    MPW adaptions
 *
 *    Revision 1.44  2007/03/10 13:54:52  bfo
 *    MPW adaptions
 *
 *    Revision 1.43  2007/03/10 13:52:30  bfo
 *    MPW adaptions
 *
 *    Revision 1.42  2007/03/10 13:38:29  bfo
 *    Pending things not for 68k
 *
 *    Revision 1.41  2007/03/10 12:46:15  bfo
 *    - No longer GetFPos/SetFPos for every read/readln call
 *    - Use new <readFlag> instead
 *    - ".log" files are TEXT (for MACOS9)
 *    - DeletePath enhanced for MACOS9: Rename, move, delete (later) !
 *
 *    Revision 1.40  2007/02/24 14:28:30  bfo
 *    - All volume/file IDs will be stored as <hashV> now
 *    - General use of "assign_fdsect"
 *    - "/Volumes" adaption for MacOSX w/o Inode use
 *    - MAXTRIES_DEL debugging for directory delete (delay)
 *    - "check_vod" reads back <hashV>, <dirid> and <fName>
 *
 *    Revision 1.39  2007/02/14 20:57:07  bfo
 *    win32 remove will be done now with /A
 *
 *    Revision 1.38  2007/02/11 14:49:01  bfo
 *    "rmdir" adaption
 *
 *    Revision 1.37  2007/01/07 13:57:46  bfo
 *    Using MACOSX instead of __MACH__
 *
 *    Revision 1.36  2007/01/04 20:56:31  bfo
 *    'touchfile' only used for MACFILES
 *
 *    Revision 1.35  2006/10/12 20:00:54  bfo
 *    Several adaptions for Windows directory reading:
 *    - handling EOF correctly
 *    - pDsetatt with additional E_EOF support
 *    - rmdir with /S /Q parameters
 *    - dir flag adaption
 *
 *    Revision 1.34  2006/06/18 14:36:05  bfo
 *    Use 'SITD' instead of 'SIT!' for Mac file type now
 *
 *    Revision 1.33  2006/06/08 08:15:04  bfo
 *    Eliminate causes of signedness warnings with gcc 4.0
 *
 *    Revision 1.32  2006/05/27 00:13:22  bfo
 *    makefile => 'MPS ' / 'TEXT'
 *
 *    Revision 1.31  2006/05/26 01:30:19  bfo
 *    '.pch', '.r', '.x' are CWIE TEXT files now
 *
 *    Revision 1.30  2006/05/16 13:09:57  bfo
 *    Linux full path name adaption / Named pipe adaption
 *
 *    Revision 1.29  2006/02/19 16:17:53  bfo
 *    f->del (instead of f->delete) / type casting
 *
 *    Revision 1.28  2005/07/15 22:13:56  bfo
 *    "fflush" no longer active
 *
 *    Revision 1.27  2005/07/15 10:23:14  bfo
 *    Empty file problem fixed
 *
 *    Revision 1.26  2005/07/10 19:24:07  bfo
 *    Ignore truncate error
 *
 *    Revision 1.25  2005/07/06 21:01:13  bfo
 *    defined UNIX / fSetsz adaptions
 *
 *    Revision 1.24  2005/07/02 14:15:50  bfo
 *    Adapted for Mach-O / Special handling for /Volumes/XXX
 *
 *    Revision 1.23  2005/06/30 11:37:43  bfo
 *    .tmp.X eliminated / Mach-O support
 *
 *    Revision 1.22  2005/05/13 17:23:26  bfo
 *    SetSize access improved for Windows
 *
 *    Revision 1.21  2005/01/22 16:22:30  bfo
 *    SS_Opt: fdPos info for foreign file system (problem with "diff" fixed).
 *
 *    Revision 1.20  2004/12/03 23:56:04  bfo
 *    MacOSX MACH adaptions
 *
 *    Revision 1.19  2004/11/27 12:05:34  bfo
 *    _XXX_ introduced
 *
 *    Revision 1.18  2004/11/20 11:44:07  bfo
 *    Changed to version V3.25 (titles adapted)
 *
 *    Revision 1.17  2004/10/22 22:51:11  bfo
 *    Most of the "pragma unused" eliminated
 *
 *    Revision 1.16  2004/09/15 19:55:46  bfo
 *    C++ extensions cp/cpp for Mac rsc included
 *
 *    Revision 1.15  2003/05/05 17:53:12  bfo
 *    Activate inactive "netatalk" nodes automatically in Mac file system
 *
 *    Revision 1.14  2003/04/25 19:35:47  bfo
 *    "Netatalk" support
 *
 *    Revision 1.13  2002/10/27 23:14:59  bfo
 *    get_mem/release_mem no longer with param <mac_asHandle>
 *
 *    Revision 1.12  2002/10/16 20:06:29  bfo
 *    Make it compatible for gcc 3.2
 *
 *    Revision 1.11  2002/09/22 20:58:47  bfo
 *    Some bugs (at least 5 !!) at getFD fixed: The access to the top directory
 * (Windows32) was seriously wrong. Now it seems to work fine with "mdt".
 *
 *
 */

#include "os9exec_incl.h"

#include <utime.h>

/* Macintosh, PC and Linux File System access */
/* ========================================== */

/* Plain file I/O operations */
/* ------------------------- */

/* --- local procedure definitions for object definition ------------------- */
void   init_File(fmgr_typ *f);
os9err pFopen(ushort pid, syspath_typ *, ushort *modeP, const char *pathname);
os9err pFclose(ushort pid, syspath_typ *);
os9err pFread(ushort pid, syspath_typ *, ulong *n, char *buffer);
os9err pFreadln(ushort pid, syspath_typ *, ulong *n, char *buffer);
os9err pFwrite(ushort pid, syspath_typ *, ulong *n, char *buffer);
os9err pFwriteln(ushort pid, syspath_typ *, ulong *n, char *buffer);
os9err pFseek(ushort pid, syspath_typ *, ulong *posP);
os9err pFdelete(ushort pid, syspath_typ *, ushort *modeP, char *pathname);

os9err pFsize(ushort pid, syspath_typ *, ulong *sizeP);
os9err pFopt(ushort pid, syspath_typ *, byte *buffer);
os9err pHvolnam(ushort pid, syspath_typ *, char *volname);
os9err pFpos(ushort pid, syspath_typ *, ulong *posP);
os9err pFeof(ushort pid, syspath_typ *);
os9err pFready(ushort pid, syspath_typ *, ulong *n);
os9err pHgetFD(ushort pid, syspath_typ *, ulong *maxbytP, byte *buffer);
os9err pHgetFDInf(ushort pid,
                  syspath_typ *,
                  ulong *maxbytP,
                  ulong *fdinf,
                  byte  *buffer);

os9err pFsetsz(ushort pid, syspath_typ *, ulong *sizeP);
os9err pHsetFD(ushort pid, syspath_typ *, byte *buffer);

void   init_Dir(fmgr_typ *f);
os9err pDopen(ushort pid, syspath_typ *, ushort *modeP, const char *pathname);
os9err pDclose(ushort pid, syspath_typ *);
os9err pDread(ushort pid, syspath_typ *, ulong *n, char *buffer);
os9err pDseek(ushort pid, syspath_typ *, ulong *posP);
os9err pDchd(ushort pid, syspath_typ *, ushort *modeP, char *pathname);
os9err pDmakdir(ushort pid, syspath_typ *, ushort *modeP, char *pathname);

os9err pDsize(ushort pid, syspath_typ *, ulong *sizeP);
os9err pDpos(ushort pid, syspath_typ *, ulong *posP);
os9err pDeof(ushort pid, syspath_typ *);
os9err pDsetatt(ushort pid, syspath_typ *, ulong *attr);
/* ------------------------------------------------------------------------- */

void init_File(fmgr_typ *f)
/* install all procedures of the transparent file manager for files */
{
    gs_typ *gs = &f->gs;
    ss_typ *ss = &f->ss;

    /* main procedures */
    f->open    = (pathopfunc_typ)pFopen;
    f->close   = (pathopfunc_typ)pFclose;
    f->read    = (pathopfunc_typ)pFread;
    f->readln  = (pathopfunc_typ)pFreadln;
    f->write   = (pathopfunc_typ)pFwrite;
    f->writeln = (pathopfunc_typ)pFwriteln;
    f->seek    = (pathopfunc_typ)pFseek;
    f->del     = (pathopfunc_typ)pFdelete;
    f->makdir  = (pathopfunc_typ)
        pDmakdir; /* access to directory will be done via fFile */

    /* getstat */
    gs->_SS_Size  = (pathopfunc_typ)pFsize;
    gs->_SS_Opt   = (pathopfunc_typ)pFopt;
    gs->_SS_DevNm = (pathopfunc_typ)pHvolnam;
    gs->_SS_Pos   = (pathopfunc_typ)pFpos;
    gs->_SS_EOF   = (pathopfunc_typ)pFeof;
    gs->_SS_Ready = (pathopfunc_typ)pFready;
    gs->_SS_FD    = (pathopfunc_typ)pHgetFD;
    gs->_SS_FDInf = (pathopfunc_typ)pHgetFDInf;

    /* setstat */
    ss->_SS_Size = (pathopfunc_typ)pFsetsz;
    ss->_SS_Opt  = (pathopfunc_typ)pNop; /* ignored */
    ss->_SS_Attr = (pathopfunc_typ)pNop;
    ss->_SS_FD   = (pathopfunc_typ)pHsetFD;
    ss->_SS_WTrk = (pathopfunc_typ)pUnimp; /* not used */
} /* init_File */

void init_Dir(fmgr_typ *f)
/* install all procedures of the transparent file manager for directories */
{
    gs_typ *gs = &f->gs;
    ss_typ *ss = &f->ss;

    /* main procedures */
    f->open    = (pathopfunc_typ)pDopen;
    f->close   = (pathopfunc_typ)pDclose;
    f->read    = (pathopfunc_typ)pDread;
    f->readln  = (pathopfunc_typ)pDread;   /* the same as read */
    f->write   = (pathopfunc_typ)pBadMode; /* not allowed */
    f->writeln = (pathopfunc_typ)pBadMode; /* not allowed */
    f->seek    = (pathopfunc_typ)pDseek;
    f->chd     = (pathopfunc_typ)pDchd;
    f->makdir  = (pathopfunc_typ)pDmakdir;

    /* getstat */
    gs->_SS_Size  = (pathopfunc_typ)pDsize;
    gs->_SS_Opt   = (pathopfunc_typ)pRBFopt;
    gs->_SS_DevNm = (pathopfunc_typ)pHvolnam;
    gs->_SS_Pos   = (pathopfunc_typ)pDpos;
    gs->_SS_EOF   = (pathopfunc_typ)pDeof;
    gs->_SS_Ready = (pathopfunc_typ)pUnimp; /* not used */
    gs->_SS_FD    = (pathopfunc_typ)pHgetFD;
    gs->_SS_FDInf = (pathopfunc_typ)pHgetFDInf;

    /* setstat */
    ss->_SS_Size = (pathopfunc_typ)pBadMode; /* not allowed */
    ss->_SS_Opt  = (pathopfunc_typ)pNop;     /* ignored */
    ss->_SS_Attr = (pathopfunc_typ)pDsetatt;
    ss->_SS_FD   = (pathopfunc_typ)pHsetFD;
    ss->_SS_Lock = (pathopfunc_typ)pNop;   /* ignored */
    ss->_SS_WTrk = (pathopfunc_typ)pUnimp; /* not used */
} /* init_Dir */

/* -------------------------------------------------------- */

#ifndef USE_UAEMU
static void assert(Boolean b)
{
    if (!b)
        printf("Assert Error\n");
}
#endif

/* input from file */
os9err pFread(_pid_, syspath_typ *spP, ulong *n, char *buffer)
{
    long      cnt, k;
    file_typ *f = &spP->u.disk.u.file;

    assert(buffer != NULL);

    if (spP->rawMode) {
        if (spP->rawPos > STD_SECTSIZE)
            return E_EOF; /* finished */
        if (spP->rawPos + *n > STD_SECTSIZE)
            *n = STD_SECTSIZE - spP->rawPos;

        memcpy(buffer, spP->rw_sct + spP->rawPos, *n);
        spP->rawPos += *n;
        return 0;
    } // if

    cnt = *n;
    if (cnt == 0)
        return 0; /* null read returns w/o error */

    cnt         = fread((void *)buffer, 1, cnt, spP->stream);
    f->readFlag = true;

    if (cnt == 0 && feof(spP->stream))
        return os9error(E_EOF);

    /* show read for debug */
    if (debugcheck(dbgFiles, dbgDetail)) {
        uphe_printf("%08X: ", cnt);
        for (k = 0; k < 16 && k < cnt; k = k + 2) {
            upe_printf("%02X", (byte)buffer[k]);
            if (k + 1 < cnt)
                upe_printf("%02X ", (byte)buffer[k + 1]);
        }
        upe_printf("...\n");
    }

    /* ok, return # of chars read to caller */
    *n = cnt;
    return 0;
} /* pFread */

/* input line from file */
os9err pFreadln(_pid_, syspath_typ *spP, ulong *n, char *buffer)
{
    long      cnt;
    file_typ *f = &spP->u.disk.u.file;

    char *p;
    char  c;

    assert(buffer != NULL);

    /* input not more than one line from a FILE */
    /* we cannot rely on fgets() to stop on 0x0D on non-Mac */
    /* single char for now */
    p = buffer;

    cnt = 0;
    while (cnt < *n) {
        c = fgetc(spP->stream);
        if (c == EOF) {
            *n = cnt;
            return cnt == 0 ? os9error(E_EOF)
                            : 0; /* return EOF only if on first char */
        }                        // if

        *p++ = c; /* save in the buffer */
        cnt++;
        if (c == CR)
            break; /* abort on CR */
    }              // while

    f->readFlag = true;

    debugprintf(dbgFiles,
                dbgDetail,
                ("# pFreadLn: requested=%ld, returned=%ld\n", *n, cnt));

    *n = cnt;
    return 0;
} /* pFreadln */

/* output to file */
os9err pFwrite(_pid_, syspath_typ *spP, ulong *n, char *buffer)
{
    file_typ *f = &spP->u.disk.u.file;

    long   cnt;
    fpos_t tmp_pos;

    assert(buffer != NULL);

    if (f->readFlag) {
        fgetpos(spP->stream, &tmp_pos); /* save current position */
        fsetpos(spP->stream, &tmp_pos); /* restore position */
        f->readFlag = false;
    } // if

    /* output to a FILE */
    cnt = fwrite((void *)buffer, 1, *n, spP->stream);
    fflush(spP->stream); /* don't forget this */

    debugprintf(dbgFiles,
                dbgDeep,
                ("# pFwrite: requested=%ld, written=%ld, ferror=%d, errno=%d\n",
                 *n,
                 cnt,
                 ferror(spP->stream),
                 errno));
    if (cnt < 0)
        return c2os9err(errno, E_WRITE); /* default: general write error */

    *n = cnt;
    return 0;
} /* pFwrite */

/* output to file */
os9err pFwriteln(_pid_, syspath_typ *spP, ulong *n, char *buffer)
{
    file_typ *f = &spP->u.disk.u.file;

    long   cnt, ii;
    fpos_t tmp_pos;

    assert(buffer != NULL);

    if (f->readFlag) {
        fgetpos(spP->stream, &tmp_pos); /* save current position */
        fsetpos(spP->stream, &tmp_pos); /* restore position */
        f->readFlag = false;
    } // if

    /* output line to a FILE */
    for (ii = 0; ii < *n; ii++)
        if (buffer[ii] == CR) {
            ++ii;
            break;
        }
    cnt = fwrite((void *)buffer, 1, ii, spP->stream);
    fflush(spP->stream); /* don't forget this */

    if (cnt < 0)
        return c2os9err(errno, E_WRITE); /* default: general write error */

    *n = cnt;
    return 0;
} /* pFwriteln */

os9err pFopt(ushort pid, syspath_typ *spP, byte *buffer)
{
    os9err err = pRBFopt(pid, spP, buffer);
    ulong  fdID;
    ulong *l;

    dirtable_entry *mP = NULL;
    dirent_typ      dEnt;
    strcpy(dEnt.d_name, spP->name);
    FD_ID(spP->fullName, &dEnt, &fdID, &mP);
// upe_printf( "FD_ID '%s' '%s' fdPos=%08X\n", spP->fullName, dEnt.d_name, fdpos
// );

    l  = (ulong *)&buffer[PD_FD];
    *l = os9_long(fdID) << BpB; /* LSN of file */
    return err;
} /* pFopt */

/* check ready */
os9err pFready(_pid_, _spP_, ulong *n)
{
    *n = 1;
    return 0;
} /* pFready */

/* get device name from HFS object */
os9err pHvolnam(_pid_, syspath_typ *spP, char *volname)
{
    int ii; // get the current top path as name
    for (ii = 0; ii < strlen(spP->fullName); ii++) {
        volname[ii] = spP->fullName[ii + 1];
        if (ii > 0 && volname[ii] == '/') {
            volname[ii] = NUL;
            break;
        }
    } // for

    // strcpy( volname,spP->fullName ); /* none for Linux, top directory
    // structure is different */

    //  #elif defined MACOSX
    /* MacOSX has a very special structure */
    //    strcpy( volname, "Volumes" );

    /*
    #define VVP "Volumes"
    #define VV "/Volumes/"

    char* w= &spP->fullName;
    char* v= w;

    if (ustrncmp( w,VV, strlen(VV) )==0) {
      strcpy( volname, VVP );

    //if  (*v==PATHDELIM) v++; // cut slash at the beginning
    //strcpy( volname, v );
    //
    //w= volname + strlen(VV)-1;
    //v= strstr( w, PATHDELIM_STR );
    //if (v!=NULL) *v= NUL;   // Keep "/Volumes/XXX"
    } // if
    else
      strcpy( volname, "" );
    */

    // v= &volname[ strlen(volname)-1 ];
    // if (*v==PATHDELIM) *v= NUL; // cut slash at the end

    /*
    char* v;
    strcpy( volname, "" );
    v=     &volname[ strlen(volname)-1 ];
    if (*v==PATHDELIM) *v= NUL; // cut slash at the end
    */


    return 0;
} /* pHvolnam*/

static void Set_FileDate(syspath_typ *spP, time_t t)
/* Set (Windows/Linux) file date */
{
    struct utimbuf buf;

    buf.actime  = t;
    buf.modtime = t;
    utime(spP->fullName, &buf);
} /* Set_FileDate */

os9err pFopen(ushort pid, syspath_typ *spP, ushort *modeP, const char *pathname)
{
    /* open/create file */
    os9err    err;
    char     *pastpath;
    char      hostpath[OS9PATHLEN];
    char      ploc[OS9PATHLEN];
    Boolean   isW    = IsWrite(*modeP);
    Boolean   exedir = IsExec(*modeP);
    Boolean   cre    = IsCrea(*modeP);
    file_typ *f      = &spP->u.disk.u.file;
    char     *p;
    char     *pp;
    char     *vn;
    int       len;

    FILE *stream;

    char adapted[OS9PATHLEN];

    strcpy(ploc, pathname);
    if (*ploc == NUL)
        return E_BPNAM;
    while (IsRaw(ploc)) {
        spP->rawMode  = true;
        len           = strlen(ploc);
        ploc[len - 1] = NUL;
    } /* while */

    pastpath = ploc;
    err      = parsepath(pid, &pastpath, hostpath, exedir);
    if (err)
        return err;
    pp = hostpath;

    if (spP->rawMode) { /* rawmode allows only reading of 1st sector */
        spP->rw_sct = get_mem(STD_SECTSIZE); /* for some info procs */
        spP->rawPos = 0;
        vn          = (char *)&spP->rw_sct[31];
        VolInfo(pp, vn); /* this is the correct position */

        return 0;
    } /* if IsRaw */

    f->readFlag = false;

    debugprintf(dbgFiles,
                dbgNorm,
                ("# pFopen: trying to %s '%s', mode=$%04hX\n",
                 cre ? "create" : "open",
                 pp,
                 *modeP));

    err = AdjustPath(pp, adapted, cre);
    if (err)
        return err;
    pp = adapted;

    if (cre) {
        /* --- create */
        /* check if file exists */
        if (FileFound(pp))
            return os9error(E_CEF); /* create existing file not allowed */

        stream =
            fopen(pp, "wb+"); /* create for update, use binary mode (bfo) ! */
        if (stream == NULL)
            return c2os9err(
                errno,
                E_FNA); /* default: file not accessible in this mode */
    }
    else {
/* --- open */
        /* object exists, but make sure it is not a directory */

        if (PathFound(pp))
            return os9error(E_FNA);


        stream = fopen(pp, "rb"); /* try to open for read, use binary mode */
        if (stream == NULL)
            return os9error(E_PNNF); /* file not found */

        if (isW) {                     /* open (also) for write */
            fclose(stream);            /* close the read-only path again */
            stream = fopen(pp, "rb+"); /* open for update, use binary mode */
            if (stream == NULL) {
                return c2os9err(
                    errno,
                    E_FNA); /* default: file no access in this mode */
            }
        }
    }

    // spP->rw_sct= get_mem( BUFSIZ );
    // err= setvbuf( stream, spP->rw_sct, _IOFBF, BUFSIZ ); // make buffered I/O
    spP->stream = stream; // assign stream

    spP->dDsc = NULL; /* this is not a directory */
    strcpy(spP->fullName, pp);

    p = (char *)ploc + strlen(ploc) -
        1; /* only the filename itself, no path name */
    while (p >= ploc && *p != PSEP)
        p--;
    p++;

    strcpy(spP->name, p);

    f->moddate_changed = false;

    return 0;
} /* pFopen */

/* close a file */
os9err pFclose(_pid_, syspath_typ *spP)
{

    file_typ *f = &spP->u.disk.u.file;

    if (spP->rawMode) {
        release_mem(spP->rw_sct);
        spP->rw_sct  = NULL;
        spP->rawMode = false;
        return 0;
    }

    if (fclose(spP->stream) < 0)
        return c2os9err(errno, E_WRITE);
    if (f->moddate_changed)
        Set_FileDate(spP, f->moddate);

    // release_mem( spP->rw_sct ); // don't use I/O buffer anymore
    //              spP->rw_sct= NULL;

    return 0;
} /* pFclose */

os9err pFseek(_pid_, syspath_typ *spP, ulong *posP)
/* seek within a file */
{
    int fildes;

    if (spP->rawMode) {
        if (*posP > STD_SECTSIZE)
            return E_SEEK;
        spP->rawPos = *posP;
        return 0;
    } /* if */

    if (fseek(spP->stream, (long)*posP, SEEK_SET) == 0)
        return 0;

    debugprintf(
        dbgFiles,
        dbgDetail,
        ("# pFseek: tried to seek to $%08lX, got errno=%d\n", *posP, errno));

    if (errno != 0) /* %%% rude version, if seek fails, assume that we must
                       enlarge the file */
    {
        /* try extending file */
        debugprintf(
            dbgFiles,
            dbgDetail,
            ("# pFseek: Trying to extend file to size=$%08lX\n", *posP));
        fflush(spP->stream); /* unbuffer everything */
        fildes = fileno(spP->stream);

        return c2os9err(errno,
                        E_SEEK); /* %%% FIOSETEOF is not available in MSL !! */

        if (fseek(spP->stream, (long int)*posP, SEEK_SET) != 0)
            return c2os9err(errno, E_SEEK);
    }
    else
        return c2os9err(errno, E_SEEK);

    fflush(spP->stream);
    return 0;
} /* pFseek */

os9err pFdelete(ushort pid, _spP_, ushort *modeP, char *pathname)
{
    os9err  err;
    OSErr   oserr = 0;
    char   *pastpath;
    char    pp[OS9PATHLEN];
    Boolean exedir = IsExec(*modeP);

    char adapted[OS9PATHLEN];

    pastpath = pathname;
    err      = parsepath(pid, &pastpath, pp, exedir);
    if (err)
        return err;
    pathname = pp;

    debugprintf(dbgFiles,
                dbgNorm,
                ("# I$Delete: pathname=%s, err=%d\n", pathname, err));
    if (err)
        return err;

    err = AdjustPath(pathname, adapted, false);
    if (err)
        return err;
    pathname = adapted;

    oserr = remove(pathname);


    return host2os9err(oserr, E_SHARE);
} /* pFdelete */

/* get file position */
os9err pFpos(_pid_, syspath_typ *spP, ulong *posP)
{
    *posP = (ulong)ftell(spP->stream);
    // fgetpos( spP->stream,  posP );   /* save current position */
    return 0;
} /* pFpos */

/* get file size */
os9err pFsize(_pid_, syspath_typ *spP, ulong *sizeP)
{
    os9err err = 0;

    int         fd = fileno(spP->stream);
    struct stat info;

    err = fstat(fd, &info);
    if (err)
        return E_SEEK;
    *sizeP = info.st_size; // for MACOSX <st_size> is unfortunately 0

    return err;
} /* pFsize */

/* set file size */
os9err pFsetsz(ushort pid, syspath_typ *spP, ulong *sizeP)
{
    os9err err = 0;
    char   b   = 0;
    ulong  n;
    ulong  p;
    ulong  tmp_pos = 0;

    long curSize;

    int   fd, i, j, cnt;
    OSErr oserr = 0;
    char  tmpName[OS9PATHLEN];
    FILE *tmp__stream;
#define BUFFSIZE 1024
    byte buffer[BUFFSIZE];

    tmp_pos = ftell(spP->stream); /* make it compatible for gcc >= 3.2 */
    err     = fseek(spP->stream, 0, SEEK_END);
    if (err)
        return err; /* go to EOF */

    curSize = (ulong)ftell(spP->stream); /* get position now = file size */
    // upo_printf( "curSize=%d\n", curSize );
    // upe_printf( "pos=%d size=%d newSize=%d\n", tmp_pos, curSize, *sizeP );

    if (*sizeP < curSize) {
        err = fseek(spP->stream, 0, SEEK_SET);
        if (err)
            return err;           // go to beginning
        fd = fileno(spP->stream); // <fd> used for "ftruncate"

        if (fd != 0)
            err = ftruncate(fd, *sizeP); // cut the file at <*sizeP>
        else {
            // problems with Mach ...
            // upe_printf( "problems_with_mach\n" );
            strcpy(tmpName, spP->fullName);
            i = strlen(tmpName);
            while (i >= 0 && tmpName[i] != '/')
                i--;
            tmpName[i + 1] = NUL;
            sprintf(tmpName, "%s.tmpfile.%d", tmpName, pid);

            err = fclose(spP->stream);
            err = remove(tmpName);
            // upe_printf( "remove '%s' err=%d\n", tmpName, err );
            err = rename(spP->fullName, tmpName);
            // upe_printf( "rename err=%d\n", err );

            spP->stream = fopen(spP->fullName, "wb+");
            tmp__stream = fopen(tmpName, "rb+");

            err = E_FNA;
            if (spP->stream && tmp__stream) {
                err = 0;

                i = *sizeP;
                while (i > 0) {
                    j = i;
                    if (j > BUFFSIZE)
                        j = BUFFSIZE;
                    cnt = fread((void *)buffer, 1, j, tmp__stream);
                    cnt = fwrite((void *)buffer, 1, cnt, spP->stream);
                    i -= cnt;
                } // while

                err = fclose(tmp__stream);
                err = remove(tmpName);
            } // if
        }     // if
    }         // if

    /*
    //fflush     ( spP->stream );                             // unbuffer
  everything fd = fileno( spP->stream );                       // <fd> used for
  "ftruncate" upe_printf( "fd=%d errno=%d fdGOOD=%d\n", fd, errno,
  spP->stream->_file );

      err= fclose( spP->stream );
      upe_printf( "close err=%d\n", err );

    //upe_printf( "name='%s' fd=%d\n", spP->fullName, fd );
      upe_printf( "name='%s' size=%d\n", spP->fullName, *sizeP );
      err= truncate( spP->fullName, *sizeP  );              // cut the file at
  <*sizeP>
    //err= SetEOF( spP->stream->handle );
      upe_printf( "err=%d errno=%d\n", err, errno );

          spP->stream= fopen( spP->fullName,"rb+" ); // open for update, use
  binary mode if (spP->stream) err= 0; else             err= E_FNA; upe_printf(
  "stream=%08X err=%d\n", spP->stream, err );

    //if (fd!=0) err= ftruncate( fd, *sizeP  );             // cut the file at
  <*sizeP>
      // problems with Mach ...

    //upe_printf( "TRUncate=%d fd=%d err=%d\n", *sizeP, fd, err );
    //fflush        ( spP->stream );
    //fseek         ( spP->stream, *sizeP,SEEK_SET );
    //fflush        ( spP->stream );
    //upe_printf( "NOW=%d\n", ftell( spP->stream ) );

    //             fclose( spP->stream );
    //spP->stream= fopen ( spP->fullName,"rb+" ); // create for update, use
  binary mode (bfo) !
    //fseek              ( spP->stream, *sizeP,SEEK_SET );
    }  // if

  //            curSize= *sizeP;
  //if (tmp_pos>curSize)            tmp_pos= curSize;
  //fsetpos         ( spP->stream, &tmp_pos );   // restore position
    */

    /* just read and write back one char at the end to get the correct size */
    do { // do not change the content if smaller -> read it first
        if (*sizeP == 0) {
            fclose(spP->stream);
            oserr = remove(spP->fullName);
            spP->stream =
                fopen(spP->fullName,
                      "wb+"); /* create for update, use binary mode (bfo) ! */
            if (spP->stream == NULL)
                return c2os9err(
                    errno,
                    E_FNA); /* default: file not accessible in this mode */
        }

        /*
        if (*sizeP>0) {
          #if defined win_unix || defined MACFILES
            if (*sizeP<=curSize) {              n= sizeof(b);
                                      p= *sizeP-n;
              err= pFseek( pid, spP, &p );               if (err) break;
              err= pFread( pid, spP,           &n, &b ); if (err) break;
            } // if
          #endif
                                             n= sizeof(b);
                                   p= *sizeP-n;
          err= pFseek ( pid, spP, &p );               if (err) break;
          err= pFwrite( pid, spP,           &n, &b ); if (err) break;
        } // if
        */

        if (*sizeP > curSize) {
            n   = sizeof(b);
            p   = *sizeP - n;
            err = pFseek(pid, spP, &p);
            if (err)
                break;
            err = pFwrite(pid, spP, &n, &b);
            if (err)
                break;
        } // if

        if (tmp_pos > *sizeP)
            tmp_pos = *sizeP; // adapt current pos, if now smaller
        err = pFseek(pid, spP, &tmp_pos);

        // upe_printf( "'%s' new  pos=%d err=%d\n", spP->name, tmp_pos, err );
        // fflush ( spP->stream );
    } while (false);

    if (err == E_EOF)
        err = E_FULL;
    return err;
} /* pFsetsz */

os9err pFeof(_pid_, syspath_typ *spP)
/* check for EOF */
{
    if (feof(spP->stream))
        return os9error(E_EOF);

    return 0;
} /* pFeof */

// attempt to improve things a little
#define NEW_LUZ_FD_IMPL 1

/* prepare a FD from a cipb */
static void getFD(void *fdl, ushort maxbyt, byte *buffer)
{
#define FDS 16
    byte      fdbeg[FDS];                 /* buffer for preparing FD */
    byte     *att   = (byte *)&fdbeg[0];  /* the position of the attr field */
    ulong    *sizeP = (ulong *)&fdbeg[9]; /* the position of the size field */
    Boolean   isFolder = false;
    ulong     u        = 0;
    struct tm tim;

    char       *pathname;
    struct stat info;
    syspath_typ spRec;
    Boolean     ok;

#if defined MACOSX
    mode_t v;
#else
    __mode_t v;
#endif


    /* fill up with 0 as far as needed, used as default */
    memset(buffer, 0, maxbyt);
    memset(fdbeg, 0, FDS);

    // conventional ifdef haystack :-)

    /* fill in constants */
    *att = 0x3F; /* default: peprpwerw (exe always set for now, %%% later: check
                    file type!) */

    pathname = (char *)fdl;

    // upe_printf( "'%s' %s\n", pathname, isFolder ? "folder":"file" );
    *att = 0x00;
    if (!isFolder) {
        stat_(pathname, &info);
        v = info.st_mode;

        if (v & S_IRUSR)
            *att |= poRead;
        if (v & S_IWUSR)
            *att |= poWrite;
        /* if (v & S_IXUSR) */ *att |= poExec; /* always */

        if (v & S_IROTH)
            *att |= 0x08;
        if (v & S_IWOTH)
            *att |= 0x10;
        /* if (v & S_IXOTH) */ *att |= 0x20; /* always */

        isFolder = IsTrDir(v);
    }

    if (isFolder)
        *att |= 0x3F; /* all attr for directory */

    if (isFolder)
        *att |= 0x80; /* set if it is a directory */

    fdbeg[1] = 0;
    fdbeg[2] = 0; /* owner = superuser */

/* file dates */
    u = info.st_mtime;


    TConv(u, &tim);
    //  tp = localtime( (time_t*)&u );
    //  tim= *tp; /* copy it, as it might be overwritten */

    fdbeg[3] = tim.tm_year;
    fdbeg[4] = tim.tm_mon + 1; /* somewhat different month notation */
    fdbeg[5] = tim.tm_mday;
    fdbeg[6] = tim.tm_hour;
    fdbeg[7] = tim.tm_min;

    fdbeg[8] = 1; /* link count = 1 */
    debugprintf(dbgFiles,
                dbgNorm,
                ("# getFD: %02d/%02d/%02d %02d:%02d\n",
                 tim.tm_year % 100,
                 tim.tm_mon + 1,
                 tim.tm_mday,
                 tim.tm_hour,
                 tim.tm_min));


    u = info.st_ctime;

    TConv(u, &tim);
    //  tp = localtime( (time_t*)&u );
    //  tim= *tp; /* copy it, as it might be overwritten */

    fdbeg[13] = tim.tm_year;
    fdbeg[14] = tim.tm_mon + 1;
    fdbeg[15] = tim.tm_mday;

    /* file length */
    //  *sizeP= 0; /* by default */

    if (isFolder) {
        *sizeP = 0; /* by default */

        ok = (pathname != NULL && ustrcmp(pathname, "") != 0);
        if (ok) {
            strcpy(spRec.fullName, (char *)fdl);
            ok = OpenTDir(spRec.fullName, &spRec.dDsc);
            if (ok) {
                *att   = 0x80 | 0x3F;
                *sizeP = os9_long(2 * DIRENTRYSZ); /* if no entries */

                if (spRec.dDsc != NULL) {
                    *sizeP = os9_long(DirSize(&spRec));
                    closedir(spRec.dDsc);
                }
            }
        } /* if (ok) */
    }
    else
        *sizeP = os9_long(info.st_size);

        //    printf( "%d %10d '%s'\n", isFolder, os9_long(*sizeP), pathname );

    /* copy FD beginning to caller's buffer */
    memcpy(buffer, fdbeg, maxbyt > FDS ? FDS : maxbyt);

    //  /* fill up with 0 as far as needed */
    //  if (maxbyt>16) {
    //      memset( &buffer[16],0, maxbyt-16 );
    //  }
} /* getFD */

static void setFD(syspath_typ *spP, void *fdl, byte *buffer)
/* adapt cipb with info of FD */
// void setFD(CInfoPBRec *cipbP, ushort maxbyt, byte *buffer)
{
    byte      fdbeg[16]; /* buffer for preparing FD */
    struct tm tim;
    time_t    u;

    memcpy(fdbeg, buffer, 16); /* the size IS 16 */
    tim.tm_year = fdbeg[3];
    tim.tm_mon  = fdbeg[4] - 1; /* the month correction */
    tim.tm_mday = fdbeg[5];
    tim.tm_hour = fdbeg[6];
    tim.tm_min  = fdbeg[7];
    tim.tm_sec  = 0; /* no seconds supported */

    u = UConv(&tim);

    spP->u.disk.u.file.moddate         = u;
    spP->u.disk.u.file.moddate_changed = true;

    Set_FileDate(spP, u);

    // printf( "Set_FileDate was1\n" );

    if (fdbeg[13] == 0)
        return; /* don't change it for invalid year */

    tim.tm_year = fdbeg[13];
    tim.tm_mon  = fdbeg[14] - 1; /* the month correction */
    tim.tm_mday = fdbeg[15];
    tim.tm_hour = 0;
    tim.tm_min  = 0;
    tim.tm_sec  = 0; /* no seconds supported */

    // printf( "Set_FileDate was2\n" );
    u = UConv(&tim);

} /* setFD */

/* get file descriptor for object */
os9err pHgetFD(_pid_, syspath_typ *spP, ulong *maxbytP, byte *buffer)
{
    void *fdl;

    fdl = &spP->fullName; /* use this for linux */


    getFD(fdl, loword(*maxbytP), buffer);
    debugprintf(dbgFiles, dbgNorm, ("# pHgetFD: no longer alive\n"));
    return 0;
} /* pHgetFD */

/* set file descriptor for object */
/* %%% currently only the file date will be set */
os9err pHsetFD(_pid_, syspath_typ *spP, byte *buffer)
{
    os9err err = 0;
    void  *fdl;

    fdl = NULL;


    setFD(spP, fdl, buffer);

    return err;
} /* pHsetFD */
/* adapt time/date of a file */

/* get file descriptor for file specified by "sector" */
os9err
pHgetFDInf(_pid_, syspath_typ *spP, ulong *maxbytP, ulong *fdinf, byte *buffer)
{
    void  *fdl;
    os9err err = 0;

    err = FD_Name(*fdinf, (char **)&fdl);
    if (err)
        return err;

        /*
        if      (DirName( spP->fullName, *fdinf, (char*)&result, false )) {
            strcpy( name, spP->fullName );
            strcat( name, PATHDELIM_STR );
            strcat( name, result );
            fdl=   &name; // use this for windows and linux
        }
        */


    getFD(fdl, loword(*maxbytP), buffer);
    return 0;
} /* pHgetFDInf */

/* Directory file emulation */
/* ------------------------ */

/* open directory */
os9err pDopen(ushort pid, syspath_typ *spP, ushort *modeP, const char *pathname)
{
    os9err  err = 0;
    char   *pastpath;
    char    hostpath[OS9PATHLEN];
    char    ploc[OS9PATHLEN];
    Boolean exedir = IsExec(*modeP);
    char   *p;
    char   *pp;

    DIR    *d;
    char    adapted[OS9PATHLEN];
    Boolean ok;

    /* if (strlen(pathname)>DIRNAMSZ) return os9error(E_BPNAM); */

    strcpy(ploc, pathname);
    if (*ploc == NUL)
        return E_BPNAM;
    pastpath = ploc;
    err      = parsepath(pid, &pastpath, hostpath, exedir);
    pp       = hostpath;
    debugprintf(dbgFiles, dbgNorm, ("# parsepath='%s' err=%d\n", pp, err));
    if (err)
        return err;

    debugprintf(dbgFiles, dbgNorm, ("# pp (v)='%s'\n", pp));
    err = AdjustPath(pp, adapted, false);
    if (err)
        return err;
    pp = adapted;
    debugprintf(dbgFiles, dbgNorm, ("# pp (n)='%s'\n", pp));

    ok = OpenTDir(pp, &d);
    if (!ok) {
        return E_FNA;
    } // if

    spP->dDsc = d;
    strcpy(spP->fullName, adapted); /* for later use with stat function */


    spP->u.disk.u.dir.pos = 0; /* rewind it ! */

    spP->svD_n = 0;

    p = (char *)ploc + strlen(ploc) -
        1; /* only the filename itself, no path name */
    while (p >= ploc && *p != PSEP)
        p--;
    p++;

    strcpy(spP->name, p);
    return err;
} /* pDopen */

/* not used for Mac */
os9err pDclose(_pid_, syspath_typ *spP)
{
    os9err err = 0;

    DIR *d = spP->dDsc;
    if (d != NULL) {
        err       = closedir(d);
        spP->dDsc = NULL;
    }
    debugprintf(dbgFiles,
                dbgNorm,
                ("# pDclose: '%s' err=%d\n", spP->fullName, err));

    return err;
} /* pDclose */

/* read from (simulated) directory file */
os9err pDread(_pid_, syspath_typ *spP, ulong *n, char *buffer)
{
    os9err err;

    ulong *pos   = &spP->u.disk.u.dir.pos; /* current file position */
    ulong  offs  = *pos & 0x1F;            /* offset    to start */
    ushort index = *pos >> 5;              /* dir index to start */
    char  *myb   = buffer;

    ulong           cnt, nbytes;
    os9direntry_typ os9dirent;

    dirent_typ     *dEnt;
    dirtable_entry *mP = NULL;
    int             len;
    ulong           fdpos;
    Boolean         topFlag = false;


    // printf( "pos=%8d n=%4d '%s'\n", *pos, *n, spP->name );
    debugprintf(dbgFiles, dbgDetail, ("# pDread: requests $%lX bytes\n", *n));
    cnt = *n;
    if (*n == 0)
        return 0;

    /* now copy remaining entries (if any) */
    /* now do it in one single step (bfo)  */
    while (cnt > 0) {
        memset(&os9dirent, 0, DIRENTRYSZ); /* clear before using */

        err = DirNthEntry(spP, index, &dEnt);

        if (err) {
            if (err == E_EOF && cnt < *n)
                break; // EOF can be handled !!
            return err;
        } // if

        if (*pos - offs == 0) { /* if 1st entry, ".." expected */

            if (topFlag)
                strcpy(dEnt->d_name, "..");
        } // if

        if (dEnt != NULL) {
            GetEntry(dEnt, os9dirent.name, true);
            FD_ID(spP->fullName, dEnt, &fdpos, &mP);
            if (topFlag) {
                seekD0(spP);
                topFlag = false;
            }
        }
        else
            err = E_EOF;

        if (!err) {
            len = strlen(os9dirent.name);
            os9dirent.name[len - 1] |= 0x80; /* set old-style terminator */
            os9dirent.fdsect = os9_long(fdpos);
        } // if


        if (!err) {
            nbytes = (offs + cnt) > DIRENTRYSZ ? DIRENTRYSZ - offs : cnt;
            memcpy(myb, (byte *)&os9dirent + offs, nbytes);

            cnt -= nbytes;
            myb += nbytes;
            offs = (offs + nbytes) & 0x1F;
            *pos += nbytes;
            if (offs == 0)
                index++;
        }
        else {
            if (err == E_EOF) {
                if (cnt == *n)
                    return err; /* nothing to be read */
                /* ok, no more to read, but something was there */
                break;
            }
            else
                return err;
        } // if
    }     /* while */

    /* sucessful, set number of bytes actually read */
    *n -= cnt; /* adjust to show actually read # of bytes */
    debugprintf(dbgFiles, dbgDetail, ("# pDread: returned $%lX bytes\n", *n));

    /*
    printf( "buf=%8d n=%4d '%s'\n", buffer, *n, spP->name );
    for (ii=0; ii<*n; ii++) {
      printf( "%02X ", (byte)buffer[ii] );
      if (ii%16==15) printf( "\n" );
    } // for
    */

    return 0;
} /* pDread */

/* get pointer position */
os9err pDpos(_pid_, syspath_typ *spP, ulong *posP)
{
    *posP = spP->u.disk.u.dir.pos;
    return 0;
} /* pDpos */

/* get size of directory file */
os9err pDsize(_pid_, syspath_typ *spP, ulong *sizeP)
{
    os9err err = 0;

    *sizeP = DirSize(spP);


    return err;
} /* pDsize */

/* set read position */
os9err pDseek(ushort pid, syspath_typ *spP, ulong *posP)
{
    int         cnt, n;
    dirent_typ *dEnt;

    seekD0(spP); /* start at the beginning */

    n   = 0;
    cnt = *posP / DIRENTRYSZ;
    while (n < cnt) { /* search for the nth entry */
        dEnt       = ReadTDir(spP->dDsc);
        spP->svD_n = 0; /* catch again */

        /* special handling for the root directory */

        if (dEnt == NULL)
            return os9error(E_SEEK);
        if (ustrcmp(dEnt->d_name, AppDo) != 0)
            cnt--; /* ignore ".AppleDouble" */
        n++;
    } /* while */


    spP->u.disk.u.dir.pos = *posP;
    return 0;
} /* pDseek */

os9err pDchd(ushort pid, _spP_, ushort *modeP, char *pathname)
{
    os9err       err = 0;
    char        *pastpath;
    char         p[OS9PATHLEN];
    Boolean      exedir = IsExec(*modeP); /* check if it is chd or chx */
    process_typ *cp     = &procs[pid];
    char        *defDir_s;

    char adapted[OS9PATHLEN];


    /* now obtain volume and directory of new path */
    pastpath = pathname;
    err      = parsepath(pid, &pastpath, p, exedir);
    pathname = p;

    debugprintf(dbgFiles,
                dbgNorm,
                ("# I$ChgDir: try to %s to '%s', err=%d\n",
                 exedir ? "chx" : "chd",
                 pathname,
                 err));
    if (err)
        return err;

    /* get current default path */
    if (exedir)
        defDir_s = cp->x.path;
    else
        defDir_s = cp->d.path;

    /* now do a OS changedir to "resolve" (and verify) location */
    err = AdjustPath(pathname, adapted, false);
    if (err)
        return err;
    pathname = adapted;

    if (!PathFound(pathname))
        return E_FNA;
    strcpy(defDir_s, pathname);

    if (exedir)
        cp->x.type = fDir; /* adapt the type */
    else
        cp->d.type = fDir;

    return 0; /* ok */
} /* pDchd */

os9err pDmakdir(ushort pid, _spP_, ushort *modeP, char *pathname)
{
    os9err  err;
    OSErr   oserr = 0;
    char   *pastpath;
    char    p[OS9PATHLEN];
    Boolean exedir = IsExec(*modeP); /* check if it is chd or chx */
                                     // process_typ* cp= &procs[pid];

    char adapted[OS9PATHLEN];

    pastpath = pathname;
    err      = parsepath(pid, &pastpath, p, exedir);
    if (err)
        return err;
    pathname = p;

    err = AdjustPath(pathname, adapted, true);
    if (err)
        return err;

    debugprintf(dbgFiles, dbgNorm, ("# I$MakDir: Linux path=%s\n", adapted));
    if (PathFound(adapted) || FileFound(adapted))
        return E_CEF;

    if (mkdir(adapted, 0x01c0) == 0)
        return 0; /* rwx------ */
    return E_BPNAM;


    return host2os9err(oserr, E_WRITE);
} /* pDmakdir */

os9err pDsetatt(ushort pid, syspath_typ *spP, ulong *attr)
{
    os9err err = 0;

    OSErr oserr = 0;

    ushort mode;
    char  *pp = spP->fullName;

#if defined windows32 || defined MACOSX
    char cmd[OS9PATHLEN];
#endif


    if (*attr & 0x80)
        return 0; /* it is already a directory */

    err = RemoveAppledouble(spP); /* because this file is not visible */
    if (err == E_EOF)
        err = 0; // this is not an error !!
    if (err)
        return err;

    /* can't change attributes in Mac/Linux/Windows */
    /* But remove and recreate as file is possible */
    err = pDclose(pid, spP);
    if (err)
        return err;
        // err= syspath_close( pid, spP->nr ); if (err) return err;

#if defined MACOSX
    sprintf(cmd, "rmdir %s", pp);
    err = call_hostcmd(cmd, pid, 0, NULL);
    if (err)
        return err;

#else
    //#elif defined UNIX
    oserr = remove(pp);
    if (oserr)
        err = host2os9err(oserr, E_DNE);
#endif

    debugprintf(dbgFiles,
                dbgNorm,
                ("# pDsetatt: '%s' remove err=%d\n", pp, err));
    if (err)
        return err;

    spP->type = fFile; /* change the file type now */

    mode = 0x03 | poCreateMask;
    err  = pFopen(pid,
                 spP,
                 &mode,
                 pp); // and open it as file again on the same system path

    /*
    #ifdef MACOS9
      for (k=0; k<MAXTRIES_DEL; k++) {
        if  (err!=E_CEF) break;
                                     mode= 0x03;
             err= pFopen( pid, spP, &mode, pp ); // try do open it
        if (!err) break;

                                     mode= 0x03 | poCreateMask;
        err=      pFopen( pid, spP, &mode, pp );
      } // for
    #endif
    */

    debugprintf(dbgFiles, dbgNorm, ("# pDsetatt: '%s' end err=%d\n", pp, err));
    return err;
} /* pDsetatt */

/* check for EOF */
os9err pDeof(ushort pid, syspath_typ *spP)
{
    ulong  n;
    os9err err = pDsize(pid, spP, &n);
    if (err)
        return err;
    if (n <= spP->u.disk.u.dir.pos)
        return os9error(E_EOF);

    return 0;
} /* pDeof */

/* eof */
