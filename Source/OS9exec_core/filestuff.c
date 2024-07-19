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
 *    Revision 1.37  2007/03/10 12:33:04  bfo
 *    Invalidate correctly at "syspath_close"
 *
 *    Revision 1.36  2007/02/24 14:22:37  bfo
 *    Missing %s (data/exec) added in debugprintf
 *
 *    Revision 1.35  2007/01/28 21:23:03  bfo
 *    err=1: let the path open, this is not an error
 *
 *    Revision 1.34  2007/01/02 11:30:41  bfo
 *    2007 text adaption
 *
 *    Revision 1.33  2006/12/17 00:45:54  bfo
 *    "MountDev" argument ordering corrected
 *
 *    Revision 1.32  2006/12/01 20:03:19  bfo
 *    "MountDev" with <devCopy> support ( -d )
 *
 *    Revision 1.31  2006/11/18 09:56:54  bfo
 *    "MountDev" params <sctSize>, <cluSize> added
 *
 *    Revision 1.30  2006/08/20 18:35:56  bfo
 *    path descriptors completely cleared with memset
 *    (Changes done by Martin Gregorie)
 *
 *    Revision 1.29  2006/07/23 14:30:23  bfo
 *    Up to date
 *
 *    Revision 1.28  2006/06/17 14:17:37  bfo
 *    Display pipe/tty sizes < 1000 not with k
 *
 *    Revision 1.27  2006/06/13 22:21:39  bfo
 *    d1 set correctly for SS_EOF
 *
 *    Revision 1.26  2006/05/16 13:14:09  bfo
 *    pipeDir adaptions
 *
 *    Revision 1.25  2006/02/19 16:16:50  bfo
 *    f->del (instead of f->delete)
 *
 *    Revision 1.24  2005/06/30 11:39:20  bfo
 *    Mach-O support
 *
 *    Revision 1.23  2005/05/13 17:25:13  bfo
 *    Use <imgMode>
 *
 *    Revision 1.22  2005/04/15 11:49:28  bfo
 *    Reduced size of RBF images is supported now
 *
 *    Revision 1.21  2005/01/22 15:56:50  bfo
 *    Renamed to ifdef MACOS9
 *
 *    Revision 1.20  2004/12/03 23:57:26  bfo
 *    MacOSX MACH adaptions
 *
 *    Revision 1.19  2004/11/27 12:12:52  bfo
 *    _XXX_ implemented
 *
 *    Revision 1.18  2004/11/20 11:44:07  bfo
 *    Changed to version V3.25 (titles adapted)
 *
 *    Revision 1.17  2004/10/22 22:51:12  bfo
 *    Most of the "pragma unused" eliminated
 *
 *    Revision 1.16  2003/08/01 11:14:38  bfo
 *    /L2 getstat support
 *
 *    Revision 1.15  2003/07/31 14:40:48  bfo
 *    Additional parameter for sectorsize
 *
 *    Revision 1.14  2003/04/20 22:52:01  bfo
 *    GNam with additional param <d1>
 *
 *    Revision 1.13  2003/04/12 21:50:33  bfo
 *    New functions _SS_Send, _SS_Recv, ... included
 *
 *    Revision 1.12  2002/10/27 23:44:19  bfo
 *    idbg -o has only influence on StdErr
 *
 *    Revision 1.11  2002/10/09 20:39:37  bfo
 *    uphe_printf => upe_printf
 *
 *    Revision 1.10  2002/09/22 20:50:22  bfo
 *    SS_206 (unused setstat call) included.
 *
 *    Revision 1.9  2002/09/19 22:01:32  bfo
 *    "/h0" can be mounted again
 *
 *    Revision 1.8  2002/09/01 17:57:53  bfo
 *    some more variables of the real "procid" record used now
 *
 *    Revision 1.7  2002/08/13 21:24:17  bfo
 *    Some more variables defined at the real procid struct now.
 *
 *    Revision 1.6  2002/07/30 16:47:20  bfo
 *    E-Mail adress bfo@synthesis.ch       is updated everywhere
 *
 *    Revision 1.5  2002/07/06 15:56:42  bfo
 *    ram disks (also other names than /r0) can be mounted/unmounted with
 * "mount" the size -r=<size> is prepared, but not yet active
 *
 *    Revision 1.4  2002/06/25 20:44:33  luz
 *    Added /lp printer support under windows. Not tested or even compiled for
 * Mac
 *
 */

#include <ctype.h>

/* "filestuff.c"
 * Interface to the different output systems
 *
 *
 * 00/04/23 bfo  pNop for poSetOpt fFile included
 * 00/05/13 bfo  dup implications for output implemented
 */

#include "os9exec_incl.h"

/* I/O routines */
/* ============ */

/* --- imported init routines for file managers --- */
void init_None(fmgr_typ *f);

void init_Cons(fmgr_typ *f); /* consio */
void init_NIL(fmgr_typ *f);
void init_SCF(fmgr_typ *f);

void init_Printer(fmgr_typ *f); /* printer */

void init_File(fmgr_typ *f); /* fileaccess */
void init_Dir(fmgr_typ *f);

void init_Pipe(fmgr_typ *f); /* pipefiles */
void init_PTY(fmgr_typ *f);

void init_Net(fmgr_typ *f); /* network */

void init_RBF(fmgr_typ *f); /* file_rbf */

typedef void (*init_func)(fmgr_typ *f);
void conn_FMgr(ptype_typ type, fmgr_typ *f, init_func i);
/* ------------------------------------------------ */

/* routine dispatch table for path operations */
fmgr_typ *fmgr_op[fARRSZ];

void init_None(fmgr_typ *f)
/* initialize all file manager routines to "pUnimp",
   except "open" and "chd", which will be set to "pNoModule"
   NOTE: This is the constructor of the file manager object */
{
    gs_typ *gs = &f->gs;
    ss_typ *ss = &f->ss;

    f->open    = IO_NoModule; /* E_MNF, as in OS-9 */
    f->close   = IO_Unimp;
    f->write   = IO_Unimp;
    f->writeln = IO_Unimp;
    f->read    = IO_Unimp;
    f->readln  = IO_Unimp;
    f->seek    = IO_Unimp;
    f->chd     = IO_NoModule; /* E_MNF, as in OS-9 */
    f->del     = IO_Unimp;
    f->makdir  = IO_Unimp;

    /* driver's getstt */
    gs->_SS_Size  = IO_Unimp;
    gs->_SS_Opt   = IO_Unimp;
    gs->_SS_DevNm = IO_Unimp;
    gs->_SS_Pos   = IO_Unimp;
    gs->_SS_EOF   = IO_Unimp;
    gs->_SS_Ready = IO_Unimp;
    gs->_SS_FD    = IO_Unimp;
    gs->_SS_FDInf = IO_Unimp;
    gs->_SS_DSize = IO_Unimp;

    gs->_SS_PCmd = IO_Unimp; /* network specific function  */

    gs->_SS_LBlink = IO_Unimp; /* /L2 specific function */

    gs->_SS_Undef = IO_Unimp; /* any other getstat */

    /* driver's setstat */
    ss->_SS_Size = IO_Unimp;
    ss->_SS_Opt  = IO_Unimp;
    ss->_SS_Attr = IO_Unimp;
    ss->_SS_FD   = IO_Unimp;
    ss->_SS_Lock = IO_Unimp;
    ss->_SS_WTrk = IO_Unimp;

    ss->_SS_Bind    = IO_Unimp; /* network specific functions */
    ss->_SS_Listen  = IO_Unimp;
    ss->_SS_Connect = IO_Unimp;
    ss->_SS_Accept  = IO_Unimp;
    ss->_SS_Recv    = IO_Unimp;
    ss->_SS_Send    = IO_Unimp;
    ss->_SS_GNam    = IO_Unimp;
    ss->_SS_SOpt    = IO_Unimp;
    ss->_SS_SendTo  = IO_Unimp;
    ss->_SS_PCmd    = IO_Unimp;

    ss->_SS_LBlink = IO_Unimp; /* /L2 specific function */

    ss->_SS_Undef = IO_Unimp; /* any other setstat */
}

/* --------------------------------------------------------- */

void conn_FMgr(ptype_typ type, fmgr_typ *f, init_func i)
/* connect a specific file manager */
{
    fmgr_op[type] = f; /* assign array field */
    init_None(f);      /* set all routines to 'pUnimp' */
    i(f);              /* just call function <i> with fmgr_typ */
}

void init_fmgrs(void)
{
    ptype_typ type;

    conn_FMgr(fNone,
              &fmgr_none,
              init_None); /* the default system: no operation */
    for (type = fNone; type < fARRSZ; type++)
        fmgr_op[type] = &fmgr_none; /* init all fmgrs to fNone */

#ifdef CON_SUPPORT /* console support */
    conn_FMgr(fCons,
              &fmgr_cons,
              init_Cons); /* connect console and pty  routines (consio)     */
    conn_FMgr(fTTY,
              &fmgr_cons,
              init_Cons); /*    "    tty device       routines     "        */
    conn_FMgr(fNIL,
              &fmgr_nil,
              init_NIL); /*    "    NIL device       routines     "        */
    conn_FMgr(fVMod,
              &fmgr_scf,
              init_SCF); /*    "    /vmod            routines     "        */
    conn_FMgr(fSCF,
              &fmgr_scf,
              init_SCF); /*    "    SCF              routines     "        */
#endif

#ifdef TFS_SUPPORT /* transparent file system */
    conn_FMgr(fFile,
              &fmgr_file,
              init_File); /*    "    transparent file routines (fileaccess) */
    conn_FMgr(fDir,
              &fmgr_dir,
              init_Dir); /*    "    transparent dir  routines     "        */
#endif

#ifdef PIP_SUPPORT /* pipes and PTYs */
    conn_FMgr(fPipe,
              &fmgr_pipe,
              init_Pipe); /*    "    pipe manager     routines (pipefiles)  */
    conn_FMgr(fPTY,
              &fmgr_pty,
              init_PTY); /*    "    pty  manager     routines     "        */
#endif

#ifdef RBF_SUPPORT /* RBF support */
    conn_FMgr(fRBF,
              &fmgr_rbf,
              init_RBF); /*    "    RBF              routines (file_rbf)   */
#endif

#ifdef NET_SUPPORT /* network support */
    conn_FMgr(fNET,
              &fmgr_net,
              init_Net); /*    "    network          routines (network)    */
#endif

#ifdef PRINTER_SUPPORT
    conn_FMgr(fPrinter,
              &fmgr_printer,
              init_Printer); /*    "    printer          routines (printer) */
#endif
}

/* -----------------------------------------------------------------------------
 */

static void disp_line(ushort       pid,
                      ushort       sp,
                      char        *ups,
                      syspath_typ *spP,
                      Boolean      fsspecflag,
                      Boolean      nameflag,
                      char        *typenam)
{
    ulong         n;
    mod_exec     *mod;
    process_typ  *cp = &procs[pid];
    pipechan_typ *p;
    char          idstr[10], pidstr[10], lwstr[10], aa[30], szs[20];
    char          theName[OS9PATHLEN];
    char          devName[OS9NAMELEN];
    char         *mName;

    if (pid == 0) {
        strcpy(idstr, "-");
        strcpy(pidstr, "-");
        mod = NULL;
    }
    else {
        sprintf(idstr, "%c%d", pid == currentpid ? '*' : ' ', pid);
        sprintf(pidstr, "%d", os9_word(cp->pd._pid));
        mod = get_module_ptr(cp->mid).host;
    }

    if (spP->lastwritten_pid == 0)
        strcpy(lwstr, "-");
    else
        sprintf(lwstr, "%d", spP->lastwritten_pid);

    if (pid == currentpid)
        mName = "ipaths";
    else {
        mName = "<none>";
        if (mod != NULL)
            mName = Mod_Name(mod);
    }

    /* this process has a user path corresponding with current syspath */
    upo_printf("%2d %5s %3s %2s %3s %2d %-10s %-5s",
               sp,
               ups,
               idstr,
               lwstr,
               pidstr,
               spP->linkcount,
               mName,
               typenam);

    strcpy(theName, spP->name);
    p = spP->u.pipe.pchP;

    switch (spP->type) {
    case fRBF:
        upo_printf(" %12X", spP->u.rbf.fd_nr);
        break;
    case fTTY:
    case fPTY:
    case fPipe:
        if (p == NULL)
            sprintf(aa, "->NULL");
        else {
            n = Pipe_NReady(p);
            if (p->size >= 1000)
                sprintf(szs, "%1.0fk", (float)(p->size / 1024));
            else
                sprintf(szs, "%3d", (int)p->size);

            sprintf(aa, "%c>%d:%s", p->broken ? '/' : '-', p->sp_lock, szs);
            if (n > 0) {
                char suffix[20];
                snprintf(suffix, sizeof(suffix), ":%lu", n);
                strcat(aa, suffix);
            }
        }

        upo_printf("%-13s", aa);
        break;

    default:
        if (fsspecflag) {
            upo_printf(" %12X",
                       My_FD(spP->fullName)); /* path has fd sector to show */
        }
        else
            upo_printf(" %8s %3s", "", ""); /* no name */
    }

    syspath_gs_devnm(pid, spP->nr, devName);
    upo_printf(" /%s", devName);

    if (ustrcmp(theName, devName) != 0) {
        if (*theName == NUL)
            strcpy(theName, "''");
        if (!nameflag)
            strcpy(theName, "---");
        upo_printf(" -> %s", theName); /* path has a name to show */
    }

    upo_printf("\n"); /* close the line */
}

void show_files(ushort pid)
/* show files, if pid!=MAXPROCESSES only files of given pid */
{
    int          iid, sp, up;
    char        *typenam;
    syspath_typ *spP;
    process_typ *cp;
    Boolean      fsspec, nameflag, stdp, up_found;
    char         ups[10], upc[10];

    upo_printf(
        "sp    up  Id LW PId ln prog       type  parID/refNum /dev -> name\n");
    upo_printf("-- ----- --- -- --- -- ---------- ----- ------------ "
               "--------------------------\n");
    for (sp = 0; sp < MAXSYSPATHS; sp++) {
        spP = &syspaths[sp];
        if (spP->type != fNone) {
            switch (spP->type) {
            case fCons:
            case fTTY:
            case fNIL:
            case fVMod:
            case fSCF:
            case fPipe:
            case fPTY:
            case fRBF:
            case fNET:
                fsspec   = false;
                nameflag = true;
                break;

            case fFile:
            case fDir:
                fsspec   = true;
                nameflag = true;
                break;

            default:
                fsspec   = false;
                nameflag = false;
                break;
            }

            typenam  = spP_TypeStr(spP);
            up_found = false; /* is there any user path with this syspath ? */
            for (iid = 0; iid < MAXPROCESSES; iid++) {
                cp = &procs[iid];
                if (cp->state != pUnused &&
                    (pid >= MAXPROCESSES || pid == iid)) {
                    /* special handling for paths 0..4 */
                    strcpy(upc, "");
                    stdp = false;
                    for (up = 0; up <= 4; up++) {
                        if (cp->usrpaths[up] == sp) {
                            sprintf(ups, "%1d", up);
                            strcat(upc, ups);
                            stdp = true;
                        }
                    }

                    if (stdp) {
                        up_found = true;
                        disp_line(iid, sp, upc, spP, fsspec, nameflag, typenam);
                    }

                    /* doing support for all other user paths */
                    for (up = 5; up < MAXUSRPATHS; up++) {
                        if (cp->usrpaths[up] == sp) {
                            up_found = true;
                            sprintf(ups, "%3d", up);
                            disp_line(iid,
                                      sp,
                                      ups,
                                      spP,
                                      fsspec,
                                      nameflag,
                                      typenam);
                        }
                    }
                }
            }

            if (!up_found && pid >= MAXPROCESSES) {
                strcpy(ups, "-");
                disp_line(0, sp, ups, spP, false, nameflag, typenam);
            }
        }
    }
}

static void CheckH0(char *name, char *p, char **p3)
{
#ifdef RBF_SUPPORT
    os9err  err;
    Boolean ish0 = ustrncmp(p, "h0", 2) == 0;
    char   *q;

    if (FileFound(name) || PathFound(name)) {
        *p3 = name;
        return;
    }
    if (!ish0)
        return; /* do it for "h0" only */

    q  = name + strlen(name) - 2;
    *q = NUL; /* cut "h0" again */
    strcat(name, "dd");
    MakeOS9Path(
        name); /* it might come as Mac or DOS path name ... --> OS-9 notation */
    err = MountDev(0,
                   name,
                   p,
                   "",
                   0,
                   0,
                   NO_SCSI,
                   0,
                   0,
                   0,
                   1,
                   false,
                   Img_Unchanged);
    if (err)
        return;

    strcpy(name, "/h0");
    *p3 = name;

#else
#endif
}

static void TwoCharDev(char *p, char **p3, char *tmp)
/* --- two-char device name */
{
    char    envnam[15];
    char   *q;
    char    sv1[OS9PATHLEN];
    char    sv2[OS9PATHLEN];
    Boolean isDD = ustrncmp(p, "dd", 2) == 0;

    if (isDD)
        *p3 = egetenv("OS9DISK"); /* default device */
    else if (tolower(*p) == 'h' && (*(p + 2) == PATHDELIM || *(p + 2) == NUL)) {
        strcpy(envnam, "OS9H");
        strncat(envnam, (p + 1), 1);
        *p3 = egetenv(envnam); /* get OS9Hx */
        if (*p3 != NULL && **p3 == NUL)
            *p3 = NULL; /* invalidate again */

        if (*p3 == NULL) {
            strcpy(tmp, startPath);
            q = tmp + strlen(tmp) - 1;
            if (*q != PATHDELIM)
                strcat(tmp, PATHDELIM_STR);
            strncat(tmp, p, 2);
            if (FileFound(tmp) || PathFound(tmp))
                *p3 = tmp;
            strcpy(sv1, tmp);
        }

        if (*p3 == NULL) {
            strcpy(tmp, startPath);

            q = tmp + strlen(tmp) - 1;
            if (*q == PATHDELIM)
                q--;
            while (*q != PATHDELIM)
                q--;
            *(++q) = NUL; /* cut the string after delimiter */
            strncat(tmp, p, 2);
            if (FileFound(tmp) || PathFound(tmp))
                *p3 = tmp;
        }

        if (*p3 == NULL) {
            strcpy(sv2, tmp);
            strcpy(tmp, sv1);
            CheckH0(tmp, p, p3);
        }

        if (*p3 == NULL) {
            strcpy(tmp, sv2);
            CheckH0(tmp, p, p3);
        }
    }

    if (*p3 != NULL && **p3 == NUL)
        *p3 = NULL; /* invalidate again */
    debugprintf(dbgFiles, dbgNorm, ("# TwoCharDev: path='%s'\n", *p3));
}

os9err
parsepathext(ushort pid, char **inp, char *out, Boolean exedir, Boolean *ispath)
/* macintosh:
 * parses path, converts it into MacOS format: / and .. are replaced by
 * appropriate number of :) and updates pointer to end of path. The path ends
 * when a control char is found (<=SPACE)
 * - MPW shell variable names in {} will be substituted by their value
 * - the absolute pathlist "/dd" will be substituted by the contents of the
 * OS9DISK shell variable.
 * - The process' directory will be made the default, (only if the path is
 * relative)
 * - ResolveAliasFile will be called to resolve aliases (in all known ways).
 * - isPath returns true only if result is a pathlist
 *
 * NOT macintosh:
 * Convert it to the operating system specific format
 */
{
    char       *p, *p2, *p3, *op;
    static char pathbuf[OS9PATHLEN]; /* static buffer to allow path display in
                                        error tracebacks */
    char         tmp[OS9PATHLEN];
    Boolean      firstElemChar, firstElem;
    process_typ *cp = &procs[pid];
    int          k;

    char   *defDir_s;
    Boolean absolute;
    Boolean addIt;
    char    vv[OS9PATHLEN];

    if (debugcheck(dbgFiles, dbgNorm)) {
        nullterm((char *)&tmp, *inp, OS9PATHLEN);
        uphe_printf("parsepathext  Input: '%s'\n", tmp);
    }

    /* NOT MACINOTSH */
    /* --- first set the process' default dir string */
    if (exedir)
        defDir_s = cp->x.path;
    else
        defDir_s = cp->d.path;

    debugprintf(
        dbgFiles,
        dbgNorm,
        ("# parsepathext (%s): '%s'\n", exedir ? "exec" : "data", defDir_s));

    /* --- now analyze the path */
    op = pathbuf; /* intermediate buffer */
    p  = *inp;
    trigcheck("parsepathext (full path)", p);
    firstElem = true;
    *ispath   = false;

    absolute = (*p == '/');

    if (!absolute) {
        /* --- it is a relative path: copy in default dir first (must end with
         * PATHDELIM) */
        p2 = defDir_s;
        if (*p2 == NUL)
            p2 = "./"; /* special condition, if undefined */

        while (*p2 != NUL)
            *op++ = *p2++; /* do it until end of string (not including NUL) */

        op--;
        if (*op != PATHDELIM && *p != NUL && ustrcmp(p, ".") != 0)
            *++op = PATHDELIM; /* add a slash at the end */
        *++op = NUL;           /* terminate the string */

        p             = *inp; /* go back to start */
        firstElemChar = true; /* next char is first char of element */
    }                         /* relative pathlist or file */

    else {
        /* --- absolute pathlist */
        *ispath = true;
        firstElemChar =
            false; /* parent dirs of absolute paths (devices) are not allowed */

        p++; /* skip slash at beginning */
        if (*(p + 1) == '/')
            p++; /* eat Linux specific double slash at the beginning */
        p--;

        debugprintf(dbgFiles, dbgNorm, ("# parsepathext  (abs): '%s'\n", p));

        /* --- check for special device translation */
        p3 = NULL; /* no substitution found yet */
        if (strlen(p) >= 3 && (*(p + 3) == '/' || *(p + 3) <= ' ')) {
            p++;
            addIt = false;

            /* --- two-char device name */
            TwoCharDev(p, &p3, (char *)&tmp);
            if (p3 == NULL) { /* no substitution, just use start dir */
                p3 = startPath;
                strcpy(vv, p3);
                strcat(vv, PATHDELIM_STR);
                strcat(vv, p);
                if (!FileFound(vv))
                    p3 = strtUPath;

                addIt = true;
                if (*p3 == NUL)
                    p3 = NULL; /* only valid if not empty */
            }

            if (p3 != NULL) {
                /* --- replace /dd or /hx by OS9DISK/OS9Gx environment variable
                 * if it is defined */
                while (*p3 != NUL)
                    *op++ = *p3++;

                if (addIt) {
                    *op++ = PATHDELIM;
                    *op++ = *p++;
                    *op++ = *p++;
                }
                else
                    p += 2; /* skip "dd" "hx" part */

                firstElem = false; /* after substitution, don't count this as
                                      "first" elem any more */
            }
        }

        if (p3 != NULL)
            debugprintf(dbgFiles,
                        dbgNorm,
                        ("# parsepathext (subs): '%s' '%s'\n", p, p3));

#ifdef WINFILES
        /* no subsititution for first element of absolute path occurred */
        if (p3 == NULL) {
            if ((*(p + 1) == '/') || (*(p + 1) <= ' ')) {
                /* single char name: use as drive letter like "C:\" */
                *op++ = *p++;
                if (*p == '/')
                    p++; /* skip slash, too, if pathlist continues */
                *op++ = ':';
                *op++ = PATHDELIM;
            }
        }
#endif
    }

    *op = NUL; /* make it readeable */
    debugprintf(dbgFiles, dbgNorm, ("# pathbuf '%s' '%s'\n", pathbuf, p));

    while (*p > ' ') {
        /* check for current and parent directories */
        if (firstElemChar) {
            trigcheck("parsepathext (path element)", p);
            firstElemChar = false; /* not any more */
            k             = 0;
            p3            = p;
            while (*p3 == '.') {
                k++;
                p3++;
            }

            if ((*p3 == '/') || (*p3 <= ' ')) {
                /* path element consists of periods only */
                if (*p3 == '/') {
                    /* path continues */
                    p = p3 + 1;
                    firstElemChar =
                        true; /* it's the first char of the next element! */
                    firstElem = false; /* we need no more additional colons */

                    while (k-- > 1) {
                        /* for each period more than 1 append one '..\' or '../'
                         * sequence */
                        *op++ = '.';
                        *op++ = '.';
                        *op++ = PATHDELIM;
                    }
                    continue; /* check next char */
                }
                else {
                    /* path does not continue */
                    p = p3;
                    while (k-- > 1) {
                        /* for each period more than 1 append one '..\' or '../'
                         * sequence */
                        *op++ = '.';
                        *op++ = '.';
                        *op++ = PATHDELIM;
                    }
                    continue; /* check next char */
                }
            }
        }

        /* normal copy */
        if (*p == '/') {
            *op           = PATHDELIM;
            firstElem     = false; /* not first element any more */
            firstElemChar = true;  /* next character is first of an element */
        }
        else {
            *op = *p;
        }

        p++;
        op++;
    }

    *op  = NUL; /* set terminator */
    *inp = p;   /* return updated pointer */

    /* remember for error tracebacks */
    lastpathparsed = pathbuf;
    strncpy(out, pathbuf, OS9PATHLEN);

    /* show result */
    debugprintf(dbgFiles, dbgNorm, ("# parsepathext Output: '%s'\n", out));
    return 0; /* no error (no bad pathlists possible) */
}

/* same as parsepathext, but uses default settings for file-only paths */
os9err parsepath(ushort pid, char **inp, char *out, Boolean exedir)
{
    Boolean isPath;
    return parsepathext(pid, inp, out, exedir, &isPath);
}

/* Generic I/O routines */
/* -------------------- */

/* unimplemented function */
os9err pUnimp(_pid_, _spP_) { return os9error(E_UNKSVC); }

/* unimplemented function, but that's OS9-conformant and therefore not to be
 * alerted */
os9err pUnimpOk(_pid_, _spP_) { return E_UNKSVC + E_OKFLAG; }

/* bad mode */
os9err pBadMode(_pid_, _spP_) { return os9error(E_BMODE); }

/* no operation (allowed, but useless) */
os9err pNop(_pid_, _spP_) { return 0; }

/* unavailable 'open' function, as in OS-9 */
os9err pNoModule(_pid_, _spP_) { return os9error(E_MNF); }


/* get SCF device name from file */
os9err pSCFnam(_pid_, syspath_typ *spP, char *volname)
{
    char *p;

    switch (spP->type) {
    case fNIL:
        p = "nil";
        break;
    case fVMod:
        p = "vmod";
        break;

    case fCons:
        p = volname;
        Console_Name(spP->term_id, p);
        break;

    case fPipe:
        p = "pipe";
        break;
    default:
        p = spP->name;
        break;
    }

    if (volname == p)
        return 0;

    strcpy(volname, p);
    return 0;
}

/* Unified I/O routines */
/* ==================== */

/* initialize system paths */
void init_syspaths()
{
    int          err;
    syspath_typ *spP;
    process_typ *cp = &procs[0];

    ttydev_typ *mco;

    int k;
    for (k = 0; k < MAXSYSPATHS; k++) {
        memset(&syspaths[k], 0, sizeof(syspath_typ));
        spP            = &syspaths[k];
        spP->type      = fNone; /* no paths yet */
        spP->linkcount = 0;
        spP->fd_sct.host  = NULL;
        spP->fd_sct.guest = 0;
        spP->rw_sct.host  = NULL;
        spP->rw_sct.guest = 0;
        syspth[k]      = 0; /* structure as it is in "real" OS-9 (bfo) */
    }

    /* --- prepare default paths */
    /* start numbering with 1 because of 'procs' compatibility (bfo) */
    /* see also system path definitions at 'os9exec_nt.h' */

    /* stdin */
    spP       = &syspaths[sysStdin];
    spP->nr   = sysStdin;
    spP->type = fCons;

    spP->stream = stdin;

    spP->term_id = 0;
    err          = pSCFnam(0, spP, spP->name);
    err          = pSCFopt(0, spP, (byte *)&spP->opt);

    spP->linkcount = 3; /* do not close at the end !! */

    mco             = &main_mco;
    mco->installed  = true;
    mco->spP        = spP;
    mco->inBufUsed  = 0;
    mco->holdScreen = false;

    /* stdnil */
    spP            = &syspaths[sysStdnil];
    spP->nr        = sysStdnil;
    spP->type      = fNIL;
    err            = pSCFnam(0, spP, spP->name);
    spP->linkcount = 1;

    /* vmod */
    spP            = &syspaths[sysVMod];
    spP->nr        = sysVMod;
    spP->type      = fVMod;
    err            = pSCFnam(0, spP, spP->name);
    spP->linkcount = 1;

    /* title output thru process 0 */
    cp->usrpaths[usrStdin]  = sysStdin;
    cp->usrpaths[usrStdout] = sysStdin;
    cp->usrpaths[usrStderr] = sysStdin;
}

syspath_typ *get_syspathd(ushort pid, ushort sp)
/* get_syspath */
/* get pointer to valid syspath entry, return NULL if none, debug mode */
{
    syspath_typ *spP;
    process_typ *cp;
    Boolean      notW;

    if (sp == 0 || sp >= MAXSYSPATHS)
        return NULL;
    spP = &syspaths[sp];

    notW = true;
    if (pid < MAXPROCESSES) {
        cp   = &procs[pid];
        notW = (cp->state != pWaitRead);
    }

    if (!in_recursion && notW && debugcheck(dbgFMgrType, dbgDetail)) {
        in_recursion = true;
        uphe_printf("spP->type: %s / spP->linkcount: %d\n",
                    spP_TypeStr(spP),
                    spP->linkcount);
        in_recursion = false;
    }

    if (spP->type == fNone)
        return NULL;
    return spP;
}

syspath_typ *get_syspath(ushort pid, ushort sp)
/* get_syspath */
/* get pointer to valid syspath entry, return NULL if none */
{
    syspath_typ *spP;
    Boolean      sv = in_recursion;

    in_recursion = true;
    spP          = get_syspathd(pid, sp);
    in_recursion = sv;

    return spP;
}

static void showbuff(syspath_typ *spP, byte *buffer, ulong len)
{
    int  k, ii, sv;
    char c;

    if (in_recursion)
        return;

    /* avoid multiple calling */
    in_recursion = true;
    uphe_printf("'%s' (%s)\n", spP->name, spP_TypeStr(spP));

    for (k = 0; k < len; k++) {
        if (k % 16 == 0) {
            uphe_printf("%08x: ", k);
            sv = k;
        }

        upe_printf("%02x", buffer[k]);
        if (k % 2 == 1)
            upe_printf(" ");

        if (k >= 16 && k + 1 == len) {
            ii = k + 1;
            while (ii % 16 != 0) {
                upe_printf("  ");
                if (ii % 2 == 1)
                    upe_printf(" ");
                ii++;
            }
        }

        if (k % 16 == 15 || k + 1 == len) {
            upe_printf(" \"");
            for (ii = sv; ii <= k; ii++) {
                c = buffer[ii] & 0x7f;
                upe_printf("%c", c < ' ' ? '.' : c);
            }
            upe_printf("\"\n");
        }
    }

    in_recursion = false;
}

os9err syspath_close(ushort pid, ushort sp)
/* close system path
 * Note: if pid is invalid, all paths will be closed
 *       independent of link count
 */
{
    os9err       err;
    syspath_typ *spP = get_syspathd(pid, sp);
    if (spP == NULL)
        return os9error(E_BPNUM);

    /* --- now check if real close or only unlink */
    if (spP->linkcount > 0)
        spP->linkcount--; /* only decrement link */
    if (spP->linkcount > 0 && pid != MAXPROCESSES) {
        debugprintf(dbgFiles,
                    dbgNorm,
                    ("# syspath_close: close of syspath=%d only unlinks, "
                     "linkcount=%d\n",
                     sp,
                     spP->linkcount));
        return 0; /* not yet really closed */
    }

    if (sp == procs[0].usrpaths[usrStdout] ||
        sp == procs[0].usrpaths[usrStderr]) {
        /* don't close this path because this is used for main system's output
         */
        debugprintf(
            dbgFiles,
            dbgNorm,
            ("# syspath_close: don't close syspath=%d\n", sp, spP->linkcount));
        return 1; /* let the path open, this is not an error */
    }

    /* --- must close path */
    debugprintf(dbgFiles,
                dbgNorm,
                ("# syspath_close: close of syspath=%d really closes path\n",
                 sp,
                 spP->linkcount));

    err = (fmgr_op[spP->type]->close)(pid, spP);
    if (err == 1)
        return 0; /* err=1: let the path open, this is not an error */
                  // if (err)    return err;   /* do not invalidate, if error */

    spP->type = fNone; /* invalidate the path descriptor */
    strcpy(spP->name, "");

    return err;
}

// close user path
os9err usrpath_close(ushort pid, ushort up)
{
    os9err       err;
    int          ll;
    ushort      *sp  = &procs[pid].usrpaths[up];
    syspath_typ *spP = get_syspathd(pid, *sp);

    if (spP == NULL)
        ll = 0;
    else
        ll = spP->linkcount - 1;

    debugprintf(dbgFMgrType,
                dbgNorm,
                ("# %s pid/up/sp-lnk: %2d  %2d %2d-%2d %s\n",
                 "close",
                 pid,
                 up,
                 *sp,
                 ll,
                 spP_TypeStr(spP)));

    if (up >= MAXUSRPATHS)
        return os9error(E_BPNUM);
    err = syspath_close(pid, *sp);
    debugprintf(
        dbgFiles,
        dbgNorm,
        ("# usrpath_close: pid=%d, up=%d, sp=%d, err=%d\n", pid, up, *sp, err));

    if (!err)
        *sp = 0; // invalidate if no error
    if (err == 1)
        err = 0; // err=1: let the path open, this is not an error
    return err;
} // usrpath_close

os9err usrpath_link(ushort pid, ushort up, const char *ident)
/* link system path (when a new user path number is created for the same
 * syspath) */
{
    ushort       sp  = procs[pid].usrpaths[up];
    syspath_typ *spP = get_syspath(pid, sp);
    int          ll;

    if (spP == NULL)
        ll = 0;
    else
        ll = spP->linkcount + 1;

    debugprintf(dbgFMgrType,
                dbgNorm,
                ("# %s pid/up/sp-lnk: %2d  %2d %2d-%2d %s\n",
                 ident,
                 pid,
                 up,
                 sp,
                 ll,
                 spP_TypeStr(spP)));

    if (spP == NULL)
        return os9error(E_BPNUM);

    spP->linkcount++;
    debugprintf(dbgFiles,
                dbgNorm,
                ("# usrpath_link: up=%d, sp=%d, new linkcount=%d\n",
                 up,
                 sp,
                 spP->linkcount));
    return 0;
}

/* allocate a new syspath number */
os9err syspath_new(ushort *sp, ptype_typ type)
{
    ushort       k;
    syspath_typ *spK;

    /* prepare new syspath record, don't use syspath 0 (bfo) */
    for (k = 1; k < MAXSYSPATHS; k++) {
        spK = &syspaths[k];
        if (spK->type == fNone) {   /* this is a free syspath */
            spK->type      = type;  /* assign new type */
            spK->linkcount = 1;     /* link it (once) */
            spK->mh.host      = NULL;  /* no underlying module by default */
            spK->mh.guest     = 0;
            spK->nr        = k;     /* store it also in itself */
            spK->rawMode   = false; /* by default it is not raw mode */
            spK->mustW     = 0;     /* last sector to be written */
            spK->fd_sct.host          = NULL;
            spK->fd_sct.guest         = 0;
            spK->rw_sct.host          = NULL;
            spK->rw_sct.guest         = 0;
            *sp            = k; /* return new syspath's number */

            spK->signal_to_send  = 0; /* send signal on data ready: none */
            spK->set_evId        = 0; /* set event   on data ready: none */
            spK->lastwritten_pid = 0;
            spK->fullsearch      = false;

            spK->u.pipe.pchP = NULL; /* no pipe available by default */
            spK->u.pipe.i_svd_pchP =
                NULL; /* saved pipe during internal commands, not yet used */
            spK->u.pipe.locked_open = false;

            debugprintf(dbgFiles,
                        dbgDetail,
                        ("# syspath_new: allocated new sp=%d\n", k));
            return 0;
        }
    }

    return os9error(E_PTHFUL); /* no more system paths */
}

os9err usrpath_new(ushort pid, ushort *up, ptype_typ type)
{
    os9err  err;
    ushort  spnum, k;
    ushort *upN;

    /* gets a new path */
    for (k = 0; k < MAXUSRPATHS; k++) {
        upN = &procs[pid].usrpaths[k];
        if (*upN == 0 || *upN >= MAXSYSPATHS) {
            /* --- free entry, try to open a syspath */
            /* prepare new syspath record */
            err = syspath_new(&spnum, type);
            if (err)
                return err;

            /* this is a free syspath */
            *upN = spnum; /* assign system path number */
            *up  = k;
            return 0;
        }
    }

    return os9error(E_PTHFUL); /* no more user paths available */
}

os9err syspath_open(ushort      pid,
                    ushort     *sp,
                    ptype_typ   type,
                    const char *pathname,
                    ushort      mode)
/* open system path */
{
    os9err       err = 0;
    syspath_typ *spP;

    if (type == fNIL ||
        type == fVMod) { /* don't waste new paths for NIL/vmod */
        switch (type) {
        case fNIL:
            *sp = sysStdnil;
            break;
        case fVMod:
            *sp = sysVMod;
            break;
        default:; /* do nothing */
        }         // switch

        spP = &syspaths[*sp];
        spP->linkcount++; /* and hold it */
    }
    else {
        /* prepare new syspath record */
        err = syspath_new(sp, type);
        if (err)
            return err;
        spP          = &syspaths[*sp]; /* this is a free syspath */
        spP->mode    = mode;           /* store this as well */
        spP->fileAtt = procs[pid].fileAtt;
        err          = (fmgr_op[spP->type]->open)(pid,
                                         spP,
                                         (void *)&mode,
                                         pathname); /* specific */

        /* successful ? */
        if (err) { /* in E_PLINK case, syspath open routine */
            if (err == E_PLINK) {
                *sp = mode;
                err = 0;
            }
            spP->type =
                fNone; /* not sucessfully opened, in case of E_PLINK: no need */
        }
    }

    debugprintf(dbgFiles,
                dbgNorm,
                ("# syspath_open: '%s' %s; sp=%d, type=%d, err=%d\n",
                 pathname,
                 err ? "failed" : "ok",
                 *sp,
                 type,
                 err));
    return err;
}

os9err usrpath_open(ushort      pid,
                    ushort     *up,
                    ptype_typ   type,
                    const char *pathname,
                    ushort      mode)
/* open new user path */
{
    os9err  err;
    ushort  spnum, k;
    ushort *upN;

    for (k = 0; k < MAXUSRPATHS; k++) {
        upN = &procs[pid].usrpaths[k];
        if (*upN == 0 || *upN >= MAXSYSPATHS) {
            /* --- free entry, try to open a syspath */
            err = syspath_open(pid, &spnum, type, pathname, mode);
            if (!err) {
                *upN = spnum; /* assign system path number */
                *up  = k;     /* return user path number to caller */
                debugprintf(dbgFiles,
                            dbgNorm,
                            ("# usrpath_open: '%s' pid=%d, up=%d, sp=%d, "
                             "link=%d, err=%d\n",
                             pathname,
                             pid,
                             k,
                             spnum,
                             syspaths[spnum].linkcount,
                             err));
            }

            return err;
        }
    }

    debugprintf(
        dbgFiles,
        dbgNorm,
        ("# usrpath_open: no more userpaths can be opened: E_PTHFUL\n"));
    return os9error(E_PTHFUL); /* no more user paths */
}

/* write to a syspath */
os9err
syspath_write(ushort pid, ushort spnum, ulong *len, void *buffer, Boolean wrln)
{
    os9err         err;
    procid        *pd = &procs[pid].pd;
    fmgr_typ      *f;
    os9err (*wproc)(ushort pid, syspath_typ *spP, uint32_t *n, char *buffer);
    syspath_typ   *spP = get_syspathd(pid, spnum);

    if (spP == NULL) {
        if (in_recursion) {
            spnum = 1;

            spP = get_syspathd(pid, spnum);
        }

        if (spP == NULL) {
            errpid =
                pid; /* this loop must be broken because it would lead to */
            return E_BPNUM; /* infinit recursion in case of debugprintf */
        }
    }

#ifdef TERMINAL_CONSOLE
    gLastwritten_pid = pid; /* save this info in terminal interface system */
#endif

    f = fmgr_op[spP->type];
    if (wrln)
        wproc = f->writeln;
    else
        wproc = f->write;
    err = (wproc)(pid, spP, len, (char *)buffer);

    if (!err)
        os9_long_inc(&pd->_wbytes, *len); /* for statistics*/
    if (!err && debugcheck(dbgSysCall, dbgDetail))
        showbuff(spP, buffer, *len);

    debugprintf(
        dbgFiles,
        dbgDeep,
        ("# syspath_write: pid=%d, type=%d, writeln=%d, written=%ld, err=%d\n",
         pid,
         spP->type,
         wrln,
         *len,
         err));
    return err;
}

/* write to a usrpath */
os9err
usrpath_write(ushort pid, ushort up, ulong *len, void *buffer, Boolean wrln)
{
    if (up >= MAXUSRPATHS)
        return os9error(E_BPNUM);
    return syspath_write(pid, procs[pid].usrpaths[up], len, buffer, wrln);
}

/* print to user path */
static void usrpath_puts(ushort pid, ushort up, char *s, Boolean direct)
{
    ulong        n, c, base;
    ushort       sp;
    int          ii;
    char        *b;
    Boolean      sv           = in_recursion;
    int          sv_ConsoleID = gConsoleID;
    syspath_typ *sv_spP       = g_spP;
    Boolean      no_o         = direct || dbgOut == -1;

    ii = 0; /* replace LF by CR */
    while (s[ii] != NUL) {
        if (s[ii] == LF)
            s[ii] = CR;
        ii++;
    }

    n = strlen(s);

    in_recursion = true; /* break recursion loop */
    base         = 0;
    for (ii = 0; ii < n; ii++) {
        if (s[ii] == CR || ii == n - 1) {
            b = &s[base];
            c = n - base;

            if (up < MAXUSRPATHS && no_o)
                usrpath_write(pid, up, &c, b, true);
            else {
                sp = no_o ? 0 : dbgOut;
                syspath_write(pid, sp, &c, b, true);
            }

            base = ii + 1;
        }
    }

    gConsoleID   = sv_ConsoleID;
    g_spP        = sv_spP;
    in_recursion = sv;
}

void usrpath_printf(ushort pid, ushort up, const char *format, ...)
/* printf to OS9 user path */
{
    char    buffer[MAXPRINTFLEN];
    va_list vp;
    va_start(vp, format);
    vsprintf(buffer, format, vp);
    va_end(vp);

    usrpath_puts(pid, up, buffer, true);
}

void upo_printf(const char *format, ...)
{
    char    buffer[MAXPRINTFLEN];
    va_list vp;
    va_start(vp, format);
    vsprintf(buffer, format, vp);
    va_end(vp);

    usrpath_puts(currentpid, usrStdout, buffer, true);
}

void upho_printf(const char *format, ...)
/* usr path with hash std output printing */
{
    char  buffer[MAXPRINTFLEN];
    char *b = &buffer[2];

    va_list vp;
    va_start(vp, format);
    vsprintf(b, format, vp);
    va_end(vp);

    buffer[0] = '#';
    buffer[1] = ' ';
    usrpath_puts(currentpid, usrStdout, buffer, true);
}

void upe_printf(const char *format, ...)
{
    char    buffer[MAXPRINTFLEN];
    va_list vp;
    va_start(vp, format);
    vsprintf(buffer, format, vp);
    va_end(vp);

    usrpath_puts(currentpid, usrStderr, buffer, false);
}

void uphe_printf(const char *format, ...)
/* usr path with hash error output printing */
{
    char  buffer[MAXPRINTFLEN];
    char *b = &buffer[2];

    va_list vp;
    va_start(vp, format);
    vsprintf(b, format, vp);
    va_end(vp);

    buffer[0] = '#';
    buffer[1] = ' ';
    usrpath_puts(currentpid, usrStderr, buffer, false);
}

void main_printf(const char *format, ...)
/* main path error output printing */
{
    char    buffer[MAXPRINTFLEN];
    va_list vp;
    va_start(vp, format);
    vsprintf(buffer, format, vp);
    va_end(vp);

    usrpath_puts(currentpid, MAXUSRPATHS, buffer, true);
}

/* copyright notice */
void copyright(void)
{
    upe_printf(
        "(c) 1993-2007 by luz/bfo (luz@synthesis.ch / bfo@synthesis.ch)\n");
}

/* read from a syspath */
os9err
syspath_read(ushort pid, ushort spnum, uint32_t *len, void *buffer, Boolean rdln)
{
    os9err         err, cer;
    procid        *pd = &procs[pid].pd;
    int            prev;
    fmgr_typ      *f;
    os9err (*rproc)(ushort pid, syspath_typ *spP, uint32_t *n, char *buffer);
    syspath_typ   *spP = get_syspathd(pid, spnum);

    if (spP == NULL)
        return os9error(E_BPNUM);

    f = fmgr_op[spP->type];
    if (rdln)
        rproc = f->readln;
    else
        rproc = f->read;
    err = (rproc)(pid, spP, len, (char *)buffer);

    if (!err)
        os9_long_inc(&pd->_rbytes, *len); /* for statistics */
    if (!err && debugcheck(dbgSysCall, dbgDetail))
        showbuff(spP, buffer, *len);

    if (spP->signal_to_send) {
        send_signal(pid, spP->signal_to_send);
        spP->signal_to_send = 0; /* and reset it */
    }

    if (spP->set_evId && *len > 0)
        cer = evSet(spP->set_evId, 1, &prev);

    return err;
}

/* read from a usrpath */
os9err
usrpath_read(ushort pid, ushort up, uint32_t *len, void *buffer, Boolean rdln)
{
    if (up >= MAXUSRPATHS)
        return os9error(E_BPNUM);
    return syspath_read(pid, procs[pid].usrpaths[up], len, buffer, rdln);
}

os9err syspath_seek(ushort pid, ushort spnum, ulong pos)
/* seek from a syspath */
{
    syspath_typ *spP = get_syspathd(pid, spnum);
    if (spP == NULL)
        return os9error(E_BPNUM);

    return (fmgr_op[spP->type]->seek)(pid, spP, &pos);
}

os9err usrpath_seek(ushort pid, ushort up, ulong pos)
/* read from a usrpath */
{
    if (up >= MAXUSRPATHS)
        return os9error(E_BPNUM);
    return syspath_seek(pid, procs[pid].usrpaths[up], pos);
}

static os9err etc_path(_pid_, _spP_, _d2_, byte *a0)
/* %%% this is a very straight forward implementation for OS9TCP/inetd */
{
    ulong *u;
    char  *p;

    u  = (ulong *)a0;
    *u = 0x0064;
    u  = (ulong *)a0 + 0x04;
    *u = 0x0014;
    u  = (ulong *)a0 + 0x08;
    *u = 0x03e8;
    u  = (ulong *)a0 + 0x0c;
    *u = 0x0001;
    u  = (ulong *)a0 + 0x10;
    *u = 0x0001;
    p  = (char *)a0 + 0x14;
    strcpy(p, "mac");
    u  = (ulong *)a0 + 0x34;
    *u = 0x14;
    p  = (char *)a0 + 0x38;
    strcpy(p, "/dd/ETC");

    return 0;
}

os9err syspath_getstat(ushort pid,
                       ushort sp,
                       ushort func,
                       uint32_t *a0,
                       uint32_t *d0,
                       uint32_t *d1,
                       uint32_t *d2,
                       uint32_t *d3)
/* GetStat from syspath */
{
    os9err       err;
    fmgr_typ    *f;
    gs_typ      *g;
    syspath_typ *spP = get_syspathd(pid, sp);

    if (spP == NULL)
        return os9error(E_BPNUM);

    debugprintf(dbgFiles,
                dbgDetail,
                ("# syspath_getstat %s (%d): sp=%d, type=%s\n",
                 get_stat_name(func),
                 func,
                 sp,
                 spP_TypeStr(spP)));
    f = fmgr_op[spP->type];
    g = &f->gs;

    /* call appropriate service */
    switch (func) {
    case SS_Size:
        err = (g->_SS_Size)(pid, spP, d2);
        break;
    case SS_Opt:
        err = (g->_SS_Opt)(pid, spP, get_pointer(*a0));
        break;
    case SS_DevNm:
        err = (g->_SS_DevNm)(pid, spP, get_pointer(*a0));
        break;
    case SS_Pos:
        err = (g->_SS_Pos)(pid, spP, d2);
        break;
    case SS_EOF:
        err = (g->_SS_EOF)(pid, spP);
        *d1 = 0;
        break;
    case SS_Ready:
        err       = (g->_SS_Ready)(pid, spP, d1);
        arbitrate = true;
        break;
    case SS_FD:
        err = (g->_SS_FD)(pid, spP, d2, get_pointer(*a0));
        break;
    case SS_FDInf:
        err = (g->_SS_FDInf)(pid, spP, d2, d3, get_pointer(*a0));
        break;
    case SS_DSize:
        err = (g->_SS_DSize)(pid, spP, d2, d3);
        break;

    /* $7A protocol direct command */
    case SS_PCmd:
        err = (g->_SS_PCmd)(pid, spP, get_pointer(*a0));
        break;

    /* $80 + 32: "/L2" specific */
    case SS_LBlink:
        err = (g->_SS_LBlink)(pid, spP, d2);
        break;

    /* get ETC path name and more */
    case SS_Etc:
        err = etc_path(pid, spP, d2, get_pointer(*a0));
        break;

    /* don't know yet what is it good for, used by mgratrap */
    case SS_201:
        err = 0;
        break;

    /* undefined */
    default:
        err = (g->_SS_Undef)(pid, spP, d1, d2);
        break;
    }

    return err;
}

os9err syspath_gs_size(ushort pid, ushort sp, ulong *size)
{
    return syspath_getstat(pid, sp, SS_Size, NULL, NULL, NULL, size, NULL);
}

os9err syspath_gs_devnm(ushort pid, ushort sp, char *name)
{
    return syspath_getstat(pid,
                           sp,
                           SS_DevNm,
                           (ulong *)&name,
                           NULL,
                           NULL,
                           NULL,
                           NULL);
}

os9err syspath_gs_ready(ushort pid, ushort sp, ulong *cnt)
{
    return syspath_getstat(pid, sp, SS_Ready, NULL, NULL, cnt, NULL, NULL);
}

os9err usrpath_getstat(ushort pid,
                       ushort up,
                       ushort func,
                       uint32_t *a0,
                       uint32_t *d0,
                       uint32_t *d1,
                       uint32_t *d2,
                       uint32_t *d3)
/* GetStat from usrpath */
{
    if (up >= MAXUSRPATHS)
        return os9error(E_BPNUM);
    return syspath_getstat(pid,
                           procs[pid].usrpaths[up],
                           func,
                           a0,
                           d0,
                           d1,
                           d2,
                           d3);
}

/* SetStat from syspath */
os9err syspath_setstat(ushort pid,
                       ushort path,
                       ushort func,
                       uint32_t *a0,
                       uint32_t *a1,
                       uint32_t *d0,
                       uint32_t *d1,
                       uint32_t *d2,
                       uint32_t *d3)
{
    os9err    err;
    fmgr_typ *f;
    ss_typ   *s;
    byte    **a;
    ulong     n;

    syspath_typ *spP = get_syspathd(pid, path);
    if (spP == NULL)
        return os9error(E_BPNUM);

    debugprintf(dbgFiles,
                dbgDetail,
                ("# syspath_setstat %s (%d): sp=%d, type=%s\n",
                 get_stat_name(func),
                 func,
                 path,
                 spP_TypeStr(spP)));
    f = fmgr_op[spP->type];
    s = &f->ss;
    a = (byte **)a0;

    /* call appropriate service */
    switch (func) {
    case SS_Size:
        err = (s->_SS_Size)(pid, spP, d2);
        break;
    case SS_Opt:
        err = (s->_SS_Opt)(pid, spP, get_pointer(*a));
        break;
    case SS_Attr:
        err = (s->_SS_Attr)(pid, spP, d2);
        break;
    case SS_FD:
        err = (s->_SS_FD)(pid, spP, get_pointer(*a));
        break;
    case SS_Lock:
        err = (s->_SS_Lock)(pid, spP, d0, d1);
        break; /* $11 */

    case SS_SSig:
        spP->signal_to_send =
            loword(*d2);       /* $1A: sends  signal on data ready */
        spP->signal_pid = pid; /* if ready, send immediately */
        err             = (f->gs._SS_Ready)(pid, spP, &n);
        if (!err) {
            send_signal(spP->signal_pid, spP->signal_to_send);
            spP->signal_to_send = 0;
        }
        err = 0;
        break;

    case SS_Relea:
        err                 = 0; /* $1B: clears signal on data ready */
        spP->signal_to_send = 0;
        spP->signal_pid     = 0;
        break;

    case SS_SEvent:
        err           = 0;
        spP->set_evId = *d2;
        break; /* $3A set event on data ready */
    case SS_Reset:
        err = 0; /* do nothing */
        break;
    case SS_WTrk:
        err = (s->_SS_WTrk)(pid, spP, d2 /*, get_pointer(*a) */);
        break;

    case SS_LBlink:
        err = (s->_SS_LBlink)(pid, spP, d2);
        break; /* $80 + 32: "/L2" specific */

    /* socket connections */
    case SS_Bind:
        err = (s->_SS_Bind)(pid, spP, d2, get_pointer(*a));
        break; /* $6C */
    case SS_Listen:
        err = (s->_SS_Listen)(pid, spP /*, d2, get_pointer(*a)*/);
        break; /* $6D */
    case SS_Connect:
        err = (s->_SS_Connect)(pid, spP, d2, get_pointer(*a));
        break; /* $6E */
    case SS_Resv:
        err = 0; /* do nothing at the moment */
        break;   /* $6F */
    case SS_Accept:
        err = (s->_SS_Accept)(pid, spP, d1 /*, get_pointer(*a)*/);
        break; /* $70 */
    case SS_Recv:
        err = (s->_SS_Recv)(pid, spP, d1, d2, get_pointer(*a));
        break; /* $71 */
    case SS_Send:
        err = (s->_SS_Send)(pid, spP, d1, d2, get_pointer(*a));
        break; /* $72 */
    case SS_GNam:
        err = (s->_SS_GNam)(pid, spP, d1, d2, get_pointer(*a));
        break; /* $73 */
    case SS_SOpt:
        err = (s->_SS_SOpt)(pid, spP, d1 /*, d2*/);
        break; /* $74 set socket option   */
    case SS_SendTo:
        err = (s->_SS_SendTo)(pid, spP, d1, d2, get_pointer(*a));
        break; /* $77 */
    case SS_PCmd:
        err = (s->_SS_PCmd)(pid, spP, get_pointer(*a));
        break; /* $7A protocol direct cmd */

    /* general block read */
    case SS_BlkRd: /* normal sw goes automatically to read/write if error */
        err = syspath_read(pid, path, d2, get_pointer(*a0), false);
        *d1 = *d2; /* return param must be here */
        break;

    /* general block write */
    case SS_BlkWr:
        err = syspath_write(pid, path, d2, get_pointer(*a0), false);
        *d1 = *d2; /* return param must be here */
        break;

    case SS_204: /* don't know yet what is it good for, used by mgratrap */
    case SS_206:
        //                 spC= crossedPath( pid,spP );
        //              p= spC->u.pipe.pchP;
        //          if (p!=NULL) p->sp_win= *a0;
        //
        //          debugprintf( dbgAnomaly,dbgNorm,("# 204 %s d0=%x d1=%x d2=%x
        //          a0=%x (%d %s)\n",
        //                       spP_TypeStr(spP), *d0,*d1,*d2,*a0,
        //                       pid,spP->name ));
        err = 0;
        break;

    /* undefined */
    default:
        err = (s->_SS_Undef)(pid, spP, d2, *a);
        break;
    }

    return err;
}

/* SetStat from usrpath */
os9err usrpath_setstat(ushort pid,
                       ushort up,
                       ushort func,
                       uint32_t *a0,
                       uint32_t *a1,
                       uint32_t *d0,
                       uint32_t *d1,
                       uint32_t *d2,
                       uint32_t *d3)
{
    if (up >= MAXUSRPATHS)
        return os9error(E_BPNUM);
    return syspath_setstat(pid,
                           procs[pid].usrpaths[up],
                           func,
                           a0,
                           a1,
                           d0,
                           d1,
                           d2,
                           d3);
}

os9err get_locations(ushort      pid,
                     ptype_typ   type,
                     const char *pathname,
                     Boolean     doCreate,
                     Boolean    *asDir,
                     ulong      *fdP,
                     ulong      *dfdP,
                     ulong      *dcpP,
                     ulong      *sSct)
{ /* try as file first, then as dir */
    os9err err;
    ulong  a0, *l;
    byte   opt_buff[OPTSECTSIZE];
    ushort modeF = 0x01;
    ushort modeD = 0x81;
    ushort path;

    if (doCreate) {
        modeF = modeF | poCreateMask;
        modeD = modeD | poCreateMask;

        procs[pid].fileAtt      = 0x03; /* avoid wrong attributes at "move" */
        procs[pid].cre_initsize = 0;
    }
    err    = usrpath_open(pid, &path, type, pathname, modeF);
    *asDir = false;
    if (err == E_FNA) {
        err    = usrpath_open(pid, &path, type, pathname, modeD);
        *asDir = true;
    }
    if (err)
        return err;

    /* do it the same way as the OS-9 rename */
    a0  = (ulong)opt_buff;
    err = usrpath_getstat(pid, path, SS_Opt, &a0, NULL, NULL, NULL, NULL);
    if (err)
        return err;

    l     = (ulong *)&opt_buff[PD_FD];
    *fdP  = os9_long(*l); /* position of file    (not LSN) */
    l     = (ulong *)&opt_buff[PD_DFD];
    *dfdP = os9_long(*l); /* position of its dir (not LSN) */
    l     = (ulong *)&opt_buff[PD_DCP];
    *dcpP = os9_long(*l); /* dir entry pointer */
    l     = (ulong *)&opt_buff[PD_SctSiz];
    *sSct = os9_long(*l); /* dir entry pointer */
    err   = usrpath_close(pid, path);
    return err;
}

static os9err doCmd(
    os9err (*cmd)(ushort pid, syspath_typ *spP, ushort *modeP, char *pathname),
    ushort      pid,
    ptype_typ   type,
    ushort      mode,
    char       *pathname,
    const char *txt)
{
    os9err err = (cmd)(pid, NULL, &mode, pathname); /* specific call */
    debugprintf(
        dbgFiles,
        dbgNorm,
        ("# %s '%s' (type=%s) err=%d\n", txt, pathname, TypeStr(type), err));
    return err;
}

os9err change_dir(ushort pid, ptype_typ type, char *pathname, ushort mode)
/* Change a directory */
{
    return doCmd(fmgr_op[type]->chd, pid, type, mode, pathname, "change_dir");
}

os9err delete_file(ushort pid, ptype_typ type, char *pathname, ushort mode)
/* Delete a file */
{
    return doCmd(fmgr_op[type]->del, pid, type, mode, pathname, "delete_file");
}

os9err make_dir(ushort pid, ptype_typ type, char *pathname, ushort mode)
/* Create a directory */
{
    return doCmd(fmgr_op[type]->makdir, pid, type, mode, pathname, "make_dir");
}

void init_usrpaths(ushort pid)
/* initialize process' list of open paths */
{
    int k;
    for (k = 0; k < MAXUSRPATHS; k++)
        procs[pid].usrpaths[k] = 0; /* no paths yet */
}

void close_usrpaths(ushort pid)
/* close all process <pid>'s open paths */
{
    ushort *spN;
    int     k;
    for (k = 0; k < MAXUSRPATHS; k++) {
        spN = &procs[pid].usrpaths[k];
        if (*spN > 0 && *spN < MAXSYSPATHS)
            usrpath_close(pid, k); /* close this path */
        //  *spN= 0;                     /* and invalidate to avoid double
        //  closing */
    }
}

void close_syspaths(void)
/* close all OS9exec's system paths */
{
    int k;

    debugprintf(dbgFiles,
                dbgDetail,
                ("# close_syspaths: now closing globally left-open paths\n"));

    for (k = 0; k < MAXSYSPATHS; k++) {
        syspath_close(MAXPROCESSES, k); /* close this path unconditionally */
    }

#ifdef TERMINAL_CONSOLE
    gConsoleID = NO_CONSOLE; /* final condition for no console */
    g_spP      = NULL;
#endif
}

/* eof */
