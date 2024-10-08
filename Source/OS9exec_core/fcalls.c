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
 *    Revision 1.56  2007/04/10 22:09:05  bfo
 *    retword( d[ 0 ] )= newpid added
 *
 *    Revision 1.55  2007/04/07 09:10:18  bfo
 *    - do while section for prcess setup
 *    - don't remove pDead processes in F$Fork anymore
 *    - 'AssignNewChild' called at F$Wait
 *
 *    Revision 1.54  2007/02/04 20:10:14  bfo
 *    "Plugin_Possible"/"Native_Possible" are more orthogonal now
 *
 *    Revision 1.53  2007/01/28 21:16:02  bfo
 *    'Native_Possible()' added as condition for <nativeActive>
 *
 *    Revision 1.52  2007/01/07 13:32:22  bfo
 *    <pmem> adaptions
 *    F$SetSys D_IPID: Bit 10..11 additional info for plugins added
 *
 *    Revision 1.51  2007/01/04 20:17:58  bfo
 *    Some unused vars eliminated
 *    text formatting
 *
 *    Revision 1.50  2007/01/02 11:33:32  bfo
 *    Show <ptocActive> as D_IPID bit 9
 *
 *    Revision 1.49  2006/12/02 12:08:57  bfo
 *    global <lastsyscall> eliminated
 *
 *    Revision 1.48  2006/11/12 13:28:58  bfo
 *    "load_module" <linkstyle> parameter eliminated
 *
 *    Revision 1.47  2006/10/22 17:24:53  bfo
 *    sleep_x==1 special treatment reactivated
 *
 *    Revision 1.46  2006/10/20 08:32:53  bfo
 *    idle load of OS9exec nearly reduced to 0 %
 *
 *    Revision 1.45  2006/10/01 15:33:43  bfo
 *    MAX_SLEEP adaption for Sleep
 *
 *    Revision 1.44  2006/08/26 23:53:35  bfo
 *    OS9_F_Wait for internal commands returns the err correctly now
 *
 *    Revision 1.43  2006/08/04 18:39:33  bfo
 *    Comment changes
 *
 *    Revision 1.42  2006/07/29 08:59:59  bfo
 *    currentpid will be correctly adapted for int utils now
 *
 *    Revision 1.41  2006/07/23 14:26:07  bfo
 *    Initialize <pBlocked> and <masklevel>
 *
 *    Revision 1.40  2006/07/21 07:21:07  bfo
 *    Assign <newpid> early enough (for intUtils)
 *
 *    Revision 1.39  2006/06/16 22:29:07  bfo
 *    Some comments adapted
 *
 *    Revision 1.38  2006/06/16 15:59:30  bfo
 *    F$Exit for built-in later / avoid bad <sp> manipulation for built-in
 *
 *    Revision 1.37  2006/06/11 22:06:45  bfo
 *    set_os9_state with 3rd param <callingProc>
 *
 *    Revision 1.36  2006/06/08 08:15:04  bfo
 *    Eliminate causes of signedness warnings with gcc 4.0
 *
 *    Revision 1.35  2006/06/02 18:58:37  bfo
 *    Option -g activated
 *
 *    Revision 1.34  2006/06/01 21:06:22  bfo
 *    g_ipAddr things added
 *
 *    Revision 1.33  2006/05/29 10:15:37  bfo
 *    "Int Sleep" commented out
 *
 *    Revision 1.32  2006/05/19 17:59:33  bfo
 *    Use only procid_adapted
 *
 *    Revision 1.31  2006/02/19 16:22:35  bfo
 *    use <isIntUtil> / OS9_F_Exit thread save
 *
 *    Revision 1.30  2005/07/10 19:23:04  bfo
 *    go back to the old way
 *
 *    Revision 1.29  2005/06/30 11:30:47  bfo
 *    Mach-O support / use sig_mask (name clash) / FPU support info
 *
 *    Revision 1.28  2004/11/27 12:03:49  bfo
 *    _XXX_ introduced
 *
 *    Revision 1.27  2004/11/20 11:44:06  bfo
 *    Changed to version V3.25 (titles adapted)
 *
 *    Revision 1.26  2004/10/22 22:51:12  bfo
 *    Most of the "pragma unused" eliminated
 *
 *    Revision 1.25  2004/09/15 19:57:13  bfo
 *    Up to date
 *
 *    Revision 1.24  2004/01/04 02:07:52  bfo
 *    F$UnLink returns ok, as long as module found
 *
 *    Revision 1.23  2003/05/25 10:35:04  bfo
 *    <totalMem> can't be any more smaller than <memsz>
 *
 *    Revision 1.22  2003/05/21 20:25:54  bfo
 *    Use "procid_special.h" with *except[10] instead of *_except[10]
 *
 *    Revision 1.21  2003/01/10 21:00:50  bfo
 *    pWaitRead problems fixed
 *
 *    Revision 1.20  2003/01/09 21:56:45  bfo
 *    Some F$RTE adaptions
 *
 *    Revision 1.19  2003/01/02 14:34:22  bfo
 *    Some type castinmg things fixed
 *
 *    Revision 1.18  2003/01/02 12:19:17  bfo
 *    RTE registers correctly saved and restored; remaining ticks (F$Sleep)
 *
 *    Revision 1.17  2002/11/06 22:08:14  bfo
 *    OS9_F_Load <mode> problem is now really fixed !
 *
 *    Revision 1.16  2002/11/06 20:15:14  bfo
 *    Colored memory dependency at OS9_F_Load avoided.
 *    lastsignal->pd._signal/icptroutine->pd._sigvec (directly defined at pd
 * struct)
 *
 *    Revision 1.15  2002/10/27 23:25:13  bfo
 *    module system on Mac no longer implemented with handles
 *    get_mem/release_mem without param <mac_asHandle>
 *
 *    Revision 1.14  2002/10/15 18:40:05  bfo
 *    Consider only loword at OS9_F_CmpNam / type casting fixed
 *
 *    Revision 1.13  2002/10/02 19:19:48  bfo
 *    Create the real mdir structure for OS9_F_SetSys D_ModDir
 *
 *    Revision 1.12  2002/09/01 17:57:40  bfo
 *    some more variables of the real "procid" record used now
 *
 *    Revision 1.11  2002/08/13 21:55:47  bfo
 *    grp,usr and prior at the real prdsc now
 *
 *    Revision 1.10  2002/08/13 21:21:12  bfo
 *    Some more variables defined at the real procid struct now.
 *
 *    Revision 1.9  2002/08/13 15:15:37  bfo
 *    The state <dead> will be handled correctly now (state=0x9100)
 *
 *    Revision 1.8  2002/08/09 22:39:21  bfo
 *    New procedure set_os9_state introduced and adapted everywhere
 *
 *    Revision 1.7  2002/08/08 21:53:57  bfo
 *    F$SetSys extended with D_PrcDBT support
 *
 *    Revision 1.6  2002/08/08 20:42:04  bfo
 *    sleep 0 bug for 1/256th sec fixed
 *
 *
 */

#include <ctype.h>

#include "os9exec_incl.h"

/* OS-9 system call routines */
/* ========================= */

os9err OS9_F_Exit(regs_type *rp, ushort cpid)
/* F$Exit
 * Input:   d1.w=exit status
 * Output:  none, process does not continue
 *
 */
{
    // -- save exit code
    process_typ   *cp      = &procs[cpid];
    unsigned short exiterr = loword(rp->regs[REGS_D + 1]);

    // -- kill the process
    arbitrate   = true;
    cp->exiterr = exiterr;
    sig_mask(cpid, 0); // activate queued intercepts
    kill_process(cpid);

    debugprintf(dbgProcess,
                dbgNorm,
                ("# pid=%d (%s) exited thru F$Exit, exit-code=%d\n",
                 cpid,
                 PStateStr(cp),
                 cp->exiterr));
    return 0;
}

os9err OS9_F_Load(regs_type *rp, ushort cpid)
{
    /* F$Load:
     * Input:   d0.w=access mode (E bit is relevant)
     *          (a0)=module name string (or module path)
     * Output:  d0.w=actual module type/language
     *          d1.w=module attributes/revision level
     *          (a0)=updated past module name
     *          (a1)=Module execution entry point
     *          (a2)=Module pointer
     * Error:   Carry set
     *          d1.w = error
     *             E$BMID: module does not start with MODSYNC code
     *             E$MNF: module not found
     *             E$DIRFUL: module list full
     */
    os9err    err;
    char      mpath[OS9PATHLEN], *p;
    ushort    mid;
    ushort    mode   = loword(rp->regs[REGS_D + 0]); /* attributes */
    Boolean   exedir = IsExec(mode) || (mode == 0) || (mode == 0x80);
    /* mode=0 is a strange default, */
    /* but seems to be correct as default for exedir in 'load' */
    /* Attention !!! Colored memory (bit 7) is %%% ignored. */

    os9ptr PTR = rp->regs[REGS_A + 0];
    char *ptr = get_pointer(PTR);
    char *ret = p = nullterm(mpath, ptr, OS9PATHLEN);

    debugprintf(dbgModules,
                dbgNorm,
                ("# F$Load: requested %sload of '%s', mode=$%04X\n",
                 exedir ? "exec " : "",
                 mpath,
                 mode));

    /* --- really load module, anyway */
    err = load_module(cpid, mpath, &mid, exedir);
    if (err)
        return err;

    addrpair_typ module = get_module_ptr(mid);
    mod_exec *module_host = module.host;

    retword(rp->regs[REGS_D + 0]) = os9_word(module_host->_mh._mtylan);
    retword(rp->regs[REGS_D + 1]) = os9_word(module_host->_mh._mattrev);

    rp->regs[REGS_A + 0] = PTR + (uint32_t)(ret - ptr);
    rp->regs[REGS_A + 2] = module.guest;
    rp->regs[REGS_A + 1] = module.guest + os9_long(module_host->_mexec);
    return 0;
}

os9err OS9_F_Link(regs_type *rp, ushort cpid)
/* F$Link:
 * Input:   d0.w=desired type/language
 *          (a0)=module name string (or module path)
 * Output:  d0.w=actual module type/language
 *          d1.w=module attributes/revision level
 *          (a0)=updated past module name
 *          (a1)=Module execution entry point
 *          (a2)=Module pointer
 * Error:   Carry set
 *          d1.w = error
 *             E$BMID: module does not start with MODSYNC code
 *             E$MNF: module not found
 *             E$DIRFUL: module list full
 */
{
    os9err    err;
    char      mname[OS9NAMELEN], *p;
    ushort    mid;
    ushort    tylan = loword(rp->regs[REGS_D + 0]); /* wanted attrs */

    os9ptr PTR = rp->regs[REGS_A + 0];
    char *ptr = get_pointer(PTR);
    char *ret = p = nullterm(mname, ptr, OS9NAMELEN);
    debugprintf(
        dbgModules,
        dbgNorm,
        ("# F$Link: requested link to '%s', type/lang=$%04X\n", mname, tylan));

    /* --- really link (that is, load without path, and always from exe dir) */
    err = link_module(cpid, mname, &mid);
    if (err)
        return err; /* link-style errors */

    addrpair_typ module = get_module_ptr(mid);
    mod_exec *module_host = module.host;

    retword(rp->regs[REGS_D + 0]) = os9_word(module_host->_mh._mtylan);
    debugprintf(dbgModules,
                dbgNorm,
                ("# F$Link: actual type/lang=$%04X\n", loword(rp->regs[REGS_D + 0])));

    /* module does not match request, forget it */
    if (tylan != 0 && tylan != os9_word(module_host->_mh._mtylan))
        return os9error(E_MNF); /* no such module found */

    retword(rp->regs[REGS_D + 1]) = os9_word(module_host->_mh._mattrev);
    rp->regs[REGS_A + 0]          = PTR + (uint32_t)(ret - ptr);
    rp->regs[REGS_A + 2]          = module.guest;
    rp->regs[REGS_A + 1]          = module.guest + os9_long(module_host->_mexec);

    return 0;
}

os9err OS9_F_UnLink(regs_type *rp, _pid_)
/* F$UnLink:
 * Input:   (a2)=pointer to module
 * Output:  none
 * Error:   Carry set
 *          d1.w = error
 *             none possible
 */
{
    void *ptr = get_pointer(rp->regs[REGS_A + 2]);
    ushort mid = get_mid(ptr);
    
    debugprintf(dbgModules,
                dbgNorm,
                ("# F$Unlink: Module at $%08lX has mid=%d%s\n",
                 rp->regs[REGS_A + 2],
                 mid,
                 mid < MAXMODULES ? "" : "=MAXMODULES"));

    if (mid >= MAXMODULES)
        return E_MNF;

    unlink_module(mid);
    debugprintf(
        dbgModules,
        dbgNorm,
        ("# F$Unlink: module found at mdir entry #%d at $%08lX, newlink=%d\n",
         mid,
         rp->regs[REGS_A + 2],
         os9modules[mid].linkcount));
    return 0; /* returns ok, as long as module found */
}

/* F$UnLoad:
 * Input:   d0.w=type/language
 *              (a0)=module name pointer
 * Output:  none
 * Error:   Carry set
 *          d1.w = error
 *             none possible
 */
os9err OS9_F_UnLoad(regs_type *rp, _pid_)
{
    char mname[OS9NAMELEN];

    os9ptr PTR = rp->regs[REGS_A + 0];
    char *src = get_pointer(PTR);
    char *p   = nullterm(mname, src, OS9NAMELEN);
    int   mid = find_mod_id(mname);

    if (mid >= MAXMODULES)
        return os9error(E_MNF); /* module not found */

    rp->regs[REGS_A + 0] = PTR + (uint32_t)(p - src);
    unlink_module(mid);
    debugprintf(dbgModules,
                dbgNorm,
                ("# F$UnLoad: Unloaded mid=%d, '%s', link now=%d\n",
                 mid,
                 mname,
                 os9modules[mid].linkcount));
    return 0;
}

/* F$SRqMem:
 * Input:   d0.l=block size requested
 * Output:  d0.l=actual block size
 *          (a2)=pointer to block
 * Error:   Carry set
 *          d1.w = error
 *             E$NORAM: no memory free
 */
os9err OS9_F_SRqMem(regs_type *rp, ushort cpid)
{
    uint32_t memsz = rp->regs[REGS_D + 0];

    if (memsz == 0xFFFFFFFF) { /* get max mem */
        memsz = 0x00800000;    /* %%% a large portion */
    }

    memsz = (memsz + 15) & 0xFFFFFFF0; /* round up to next 16-byte boundary */

    addrpair_typ bp = os9malloc(cpid, memsz);

    /* %%% E_MEMFUL is never returned, even if only pointer list is full */
    if (bp.host == NULL)
        return os9error(E_NORAM);    /* not enough RAM */
    rp->regs[REGS_D + 0] = memsz;    /* return actual block size */
    rp->regs[REGS_A + 2] = bp.guest; /* return block pointer */
    return 0;
}

/* F$SRtMem:
 * Input:   d0.l=block size
 *          (a2)=pointer to block
 * Error:   Carry set
 *          d1.w = error
 *           None possible
 * Restrictions:  memory will not actually be returned if block size does not
 * match the size of the block previously allocated
 */
os9err OS9_F_SRtMem(regs_type *rp, ushort cpid)
{
    os9ptr bp;
    uint32_t memsz;

    memsz = rp->regs[REGS_D + 0];
    bp    = rp->regs[REGS_A + 2];
    memsz = (memsz + 15) & 0xFFFFFFF0; /* round up to next 16-byte boundary */
    return os9free(cpid, bp, memsz);
}

/* F$STrap:
 * Input:   (A0)=stack to use for exception handler (or 0 to use current)
 *          (A1)=Pointer to initialisation table
 * Output:  none
 * Error:   Carry set
 *          d1.w = error
 * Restrictions:  only BusErr..Privilege exceptions can be handled, Line A and F
 * will be ignored, as well as all FPU exceptions.
 */
os9err OS9_F_STrap(regs_type *rp, ushort cpid)
{
    os9ptr    itab_guest = rp->regs[REGS_A + 1];
    uint16_t *itab_host  = get_pointer(itab_guest);
    uint16_t  vect;

    process_typ *cp = &procs[cpid];

    while (*itab_host != 0xFFFF) {
        /* get vector number */
        vect = *itab_host >> 2;

        if ((vect >= FIRSTEXCEPTION) &&
            (vect < FIRSTEXCEPTION + NUMEXCEPTIONS)) {

            /* installable vector routine */
            if (*(itab_host + 1) == 0) {
                /* deinstall handler */
                cp->ErrorTraps[vect - FIRSTEXCEPTION].handleraddr = 0;

                debugprintf(
                            dbgTrapHandler,
                            dbgNorm,
                            ("De-Installed handler for vector number $%02X\n", vect));
            }
            else {
                /* install routine pointer */
                cp->ErrorTraps[vect - FIRSTEXCEPTION].handleraddr =
                *(itab_host + 1) + itab_guest;
                /* stack */
                cp->ErrorTraps[vect - FIRSTEXCEPTION].handlerstack =
                rp->regs[REGS_A + 0];
                debugprintf(
                    dbgTrapHandler,
                    dbgNorm,
                    ("Installed handler at $%08lX for vector number $%02X\n",
                     cp->ErrorTraps[vect - FIRSTEXCEPTION].handleraddr,
                     vect));
            }
        }
        else
            debugprintf(
                dbgTrapHandler,
                dbgNorm,
                ("Cannot install handler for vector number $%02X\n", vect));

        /* advance two words, next table entry */
        itab_host  += 2;
        itab_guest += 2;
    }
        /* end of table */

    return 0;
}

os9err OS9_F_Time(regs_type *rp, _pid_)
/* F$Time:
 * Input:   d0.w=time format (0=gregorian, 1=julian, 2/3=same with ticks)
 * Output:  d0.l=current time
 *          d1.l=current date
 *          d2.w=day of week
 *          d3.l=tick rate/current tick (if requested)
 * Error:   Carry set
 *          d1.w = error
 * Restrictions: - ticks are not necessarily related to seconds
 *               - other restrictions that apply to Mac ticks (lost ticks, etc.)
 */
{
    ushort  mode        = loword(rp->regs[REGS_D + 0]);
    Boolean asGregorian = (mode & 0x1) == 0;
    Boolean withTicks   = (mode & 0x2) != 0;
    uint32_t aTime, aDate;
    int     dayOfWk, currentTick;

    Get_Time(&aTime, &aDate, &dayOfWk, &currentTick, asGregorian, withTicks);

    rp->regs[REGS_D + 0]          = aTime;
    rp->regs[REGS_D + 1]          = aDate;
    retword(rp->regs[REGS_D + 2]) = dayOfWk; /* day of week, 0=sunday, 1=monday... */

    if (withTicks) {
        /* we need some ticks, too */
        hiword(rp->regs[REGS_D + 3]) = TICKS_PER_SEC;
        loword(rp->regs[REGS_D + 3]) = currentTick;
    }

    return 0;
}

os9err OS9_F_STime(regs_type *rp, ushort cpid)
/* F$STime:
 * Input:   d0.l=current time (00hhmmss)
 *          d1.l=current date (yyyymmdd)
 * Output:  Time/date is set
 * Error:   Carry set
 *          d1.w = error
 */
{
    ulong secs = rp->regs[REGS_D + 0];                      // get time
    long  days = rp->regs[REGS_D + 1] - j_date(1, 1, 1904); // get date
    OS9_F_Julian(rp, cpid);

    if (days < 0) {
        days = 0;
        secs = 0;
    }

    secs += SecsPerDay * days;

    return 0;
}

os9err OS9_F_Event(regs_type *rp, ushort cpid)
{
    os9err       err    = 0;
    os9ptr       p_guest = rp->regs[REGS_A + 0];
    char        *p_host  = get_pointer(p_guest);
    short        evCode = loword(rp->regs[REGS_D + 1]);
    process_typ *cp     = &procs[cpid];

    int   evValue;
    short wIncr, sIncr;
    int   minV, maxV;
    uint32_t evId;

    switch (evCode) {
    case Ev_Link:
        err = evLink(p_host, &evId);
        if (!err)
            rp->regs[REGS_D + 0] = evId;
        break;

    case Ev_UnLnk:
        evId = rp->regs[REGS_D + 0];
        err  = evUnLnk(evId);
        break;

    case Ev_Creat:
        evValue = rp->regs[REGS_D + 0];
        wIncr   = loword(rp->regs[REGS_D + 2]);
        sIncr   = loword(rp->regs[REGS_D + 3]);

        err = evCreat(p_host, evValue, wIncr, sIncr, &evId);
        if (err)
            break;

        rp->regs[REGS_A + 0] = p_guest + (uint32_t)strlen(p_host);
        rp->regs[REGS_D + 0] = evId;
        break;

    case Ev_Delet:
        err      = evDelet(p_host);
        rp->regs[REGS_A + 0] = p_guest + (uint32_t)strlen(p_host);
        break;

    case Ev_Wait:
        evId = rp->regs[REGS_D + 0];
        minV = rp->regs[REGS_D + 2];
        maxV = rp->regs[REGS_D + 3];

        if (cp->state == pWaitRead)
            set_os9_state(cpid, cp->saved_state, "OS9_F_Event");

        err = evWait(evId, minV, maxV, &evValue);
        if (err) {
            cp->saved_state = cp->state;
            set_os9_state(cpid, pWaitRead, "OS9_F_Event");
        }

        rp->regs[REGS_D + 1] = evValue;
        break;

    case Ev_Signl:
        evId = rp->regs[REGS_D + 0];
        err  = evSignl(evId);
        break;

    default:
        err = E_UNKSVC;
    }

    return err;
}

/* F$Julian:
 * Input:   d0.l=current time (00hhmmss)
 *          d1.l=current date (yyyymmdd)
 * Output:  d0.l=time (seconds since midnight)
 *          d1.l=julian date (days since january 1st, 4713 BC)
 * Error:   Carry set
 *          d1.w = error
 */
os9err OS9_F_Julian(regs_type *rp, _pid_)
{
    union {
        uint8_t tc[4];
        uint32_t tcp;
    } t;

    t.tcp = os9_long(rp->regs[REGS_D + 0]);                /* get time */
    rp->regs[REGS_D + 0] = t.tc[1] * 3600 + t.tc[2] * 60 + t.tc[3]; /* seconds since midnight */

    t.tcp     = os9_long(rp->regs[REGS_D + 1]); /* get date */
    rp->regs[REGS_D + 1] = j_date(t.tc[3], t.tc[2], hiword(rp->regs[REGS_D + 1]));

    return 0;
}

os9err OS9_F_Gregor(regs_type *rp, _pid_)
/* F$Gregor:
 * Input:   d0.l=time (seconds since midnight)
 *          d1.l=julian date (days since january 1st, 4713 BC)
 * Output:  d0.l=current time (00hhmmss)
 *          d1.l=current date (yyyymmdd)
 * Error:   Carry set
 *          d1.w = error
 */
{
    byte   tc[4];
    ulong *tcp = (ulong *)&tc[0];
    int    d, m, y;

    tc[0]    = 0;
    tc[1]    = rp->regs[REGS_D + 0] / 3600;      /* hours   */
    tc[2]    = (rp->regs[REGS_D + 0] / 60) % 60; /* minutes */
    tc[3]    = rp->regs[REGS_D + 0] % 60;        /* seconds */
    rp->regs[REGS_D + 0] = os9_long(*tcp);

    g_date(rp->regs[REGS_D + 1], &d, &m, &y); /* convert the date */
    tc[0]    = y / 256;
    tc[1]    = y % 256;
    tc[2]    = m;
    tc[3]    = d;
    rp->regs[REGS_D + 1] = os9_long(*tcp);

    return 0;
}

os9err OS9_F_ID(regs_type *rp, ushort cpid)
/* F$ID:
 * Input:   none
 *
 * Output:  d0.w=Current process ID
 *          d1.l=Current process group / user number
 *          d2.w=Current process priority
 * Error:   none
 */
{
    procid *pd = &procs[cpid].pd;

    rp->regs[REGS_D + 0] = cpid; /* return current process' ID */
    rp->regs[REGS_D + 1] = os9_word(pd->_group) << BpB | os9_word(pd->_user);
    rp->regs[REGS_D + 2] = os9_word(pd->_prior);
    return 0;
}

os9err OS9_F_SUser(regs_type *rp, ushort cpid)
/* F$SUser:
 * Input:   d1.l=Desired group / user ID number
 *
 * Output:  None
 * Error:   Appropriate error code
 */
{
    procid *pd = &procs[cpid].pd;

    pd->_group = os9_word(hiword(rp->regs[REGS_D + 1]));
    pd->_user  = os9_word(loword(rp->regs[REGS_D + 1]));
    return 0;
}

os9err OS9_F_Send(regs_type *rp, ushort cpid)
/* F$Send:
 * Input:   d0.w=intended receiver's process ID (0 is NOT all here!)
 *          d1.w=signal code to send
 *
 * Output:  none
 * Error:   E_IPRCID, E_USIGP
 *
 * Restrictions: Killing all processes with ID=0 is not implemented
 *   because pid=0 is a valid process ID in os9exec/nt.
 */
{
    debugprintf(dbgProcess,
                dbgNorm,
                ("# F$Send: pid=%d is sending signal %d to pid=%d\n",
                 cpid,
                 loword(rp->regs[REGS_D + 1]),
                 loword(rp->regs[REGS_D + 0])));
    return send_signal(loword(rp->regs[REGS_D + 0]), loword(rp->regs[REGS_D + 1]));
}

os9err OS9_F_Icpt(regs_type *rp, ushort cpid)
/* F$Icpt:
 * Input:   (a0)=address of intercept routine
 *          (a6)=address to pass to intercept routine in A6
 *
 * Output:  none
 * Error:   none
 *
 * Restrictions: does not work, as signal handling is not yet implemented (%%%)
 */
{
    process_typ *cp = &procs[cpid];

    /* set address of intercept routine */
    cp->pd._sigvec = os9_long(rp->regs[REGS_A + 0]);

    /* set data pointer for intercept routine */
    cp->icpta6 = rp->regs[REGS_A + 6];

    debugprintf(dbgProcess,
                dbgNorm,
                ("# F$Icpt: set intercept of pid=%d to pc=$%08lx, a6=$%08lx\n",
                 cpid,
                 os9_long((ulong)cp->pd._sigvec),
                 cp->icpta6));
    return 0;
}

os9err OS9_F_RTE(_rp_, ushort cpid)
/* F$RTE:
 * Input:   none
 *
 * Output:  none
 * Error:   none
 *
 * Restrictions: will kill process if called when procs[].lastsignal==0
 */
{
    os9err       err = 0;
    process_typ *cp  = &procs[cpid];
    save_type   *svd;
    short        pds;
    uint32_t     syt;

    if (cp->pd._signal == 0) {
        /* fatal: kill process */
        debugprintf(dbgAnomaly,
                    dbgNorm,
                    ("# F$RTE: *** Fatal: called by pid=%d not from an "
                     "intercept routine\n",
                     cpid));
        cp->exiterr = os9error(
            E_DELSP); /* no close match, but signals strange condition */
        kill_process(cpid);
    }
    else {
        /* ok, terminate processing of intercept routine */
        set_os9_state(cpid, cp->rtestate, "OS9_F_RTE");
        debugprintf(
            dbgProcess,
            dbgNorm,
            ("# F$RTE: end of intercept in pid=%d, signal was %d; state=%s\n",
             cpid,
             os9_word(cp->pd._signal),
             PStateStr(cp)));
        memcpy((void *)&cp->os9regs,
               (void *)&cp->rteregs,
               sizeof(regs_type)); /* restore regs */
        cp->vector = cp->rtevector;
        cp->func   = cp->rtefunc;

        if (cp->state == pWaitRead) { /* make the save status ready again */
            svd = &cp->savread;       /* pointer to process' saved registers */
            memcpy((void *)&svd->r, (void *)&cp->rteregs, sizeof(regs_type));
            svd->vector = cp->rtevector;
            svd->func   = cp->rtefunc;

            /* but the process will be woken up after intercept !!! */
            pds = os9_word(cp->pd._signal);
            if (pds > 0 && pds <= 32) {
                cp->os9regs.regs[REGS_D + 1] = pds;
                cp->os9regs.sr |= CARRY;
                set_os9_state(cpid,
                              pActive,
                              "OS9_F_RTE"); /* only for some cases */
            }
            else
                arbitrate = true;
        }

        cp->pd._signal = 0; /* intercept done */
        if (cp->os9regs.sr & CARRY)
            err = cp->os9regs.regs[REGS_D + 1];

        /* in case of F_Sleep, return remaining ticks */
        if (cp->func == F_Sleep) {
            cp->os9regs.regs[REGS_D + 0] = 0;
            if (cp->wakeUpTick < MAX_SLEEP) {
                syt = GetSystemTick();
                if (cp->wakeUpTick > syt)
                    cp->os9regs.regs[REGS_D + 0] = cp->wakeUpTick - syt;
            }
        }
    }

    sig_mask(cpid, 0); /* disable signal mask */
    return err;
}

os9err OS9_F_GPrDBT(regs_type *rp, _pid_)
/* F$GPrDBT:
 * Input:   d1.l = maximum number of bytes to copy
 *          (a0) = Buffer pointer
 *
 * Output:  d1.l = Actual number of bytes copied
 * Error:   d1.w = Appropriate error code
 *
 */
{
    int    k;
    uint32_t *ptr, *lim;
    int16_t *s, *sl;

    os9ptr PTR = rp->regs[REGS_A + 0];
    ptr = get_pointer(PTR);
    os9err LIM = rp->regs[REGS_D + 1] + PTR;
    lim = get_pointer(LIM);

    s  = (void *)ptr;
    sl = (void *)lim;

    if (s < sl) {
        *s = os9_word(MAXPROCESSES - 1);
        s++;
        PTR++;
    } /* no process 0 */

    if (s < sl) {
        *s = os9_word(2048);
        s++;
        PTR++;
    } /* the size of the real descriptor */

    ptr = (uint32_t *)s; /* start with process nr 1 */
    for (k = 1; (k < MAXPROCESSES) && (ptr < lim); k++) {
        if (procs[k].state == pUnused) {
            *ptr = 0;
        }
        else {
            *ptr = os9_long((ulong)&procs[k]);
        }

        ptr++;
        PTR++;
    }

    rp->regs[REGS_D + 1] = PTR - rp->regs[REGS_A + 0];
    return 0;
}

os9err OS9_F_GPrDsc(regs_type *rp, ushort cpid)
/* F$GPrDsc:
 * Input:   d0.w = requested PID
 *          d1.w = number of bytes to copy
 *          (a0) = Buffer pointer
 *
 * Output:  none
 * Error:   E$IPrcID
 *
 * Restrictions: Some of the fields are not implemented
 */
{
    procid pd; // this is a local construction buffer for the Process descriptor
    int    k;
    ulong  memsz;
    ushort id           = (ushort)loword(rp->regs[REGS_D + 0]);
    process_typ     *cp = &procs[id];
    pmem_typ        *cm = &pmem[id];
    traphandler_typ *tp;
    errortrap_typ   *ep;

    if (cp->state == pUnused)
        return E_IPRCID; // this is not a valid process

    memcpy(&pd, &cp->pd, sizeof(procid));

    pd._usp = get_pointer(rp->regs[REGS_A + 7]);

    // <_state> and <queueid> will be assigned directly
    if (id == cpid)
        pd._queueid = '*';

    pd._scall  = os9_byte(cp->lastsyscall);
    pd._pmodul = (mod_exec *)os9_long((ulong)os9mod(cp->mid).host);
    // upe_printf( "pmodul1=%08X\n", os9_long( (ulong)pd._pmodul ) );

    // get the list of the currently connected trap handlers (bfo)
    for (k = 0; k < NUMTRAPHANDLERS; k++) {
        tp            = &cp->TrapHandlers[k];
        pd._traps[k]  = os9_long(tp->trapmodule.guest);
        pd._trpmem[k] = os9_long(tp->trapmem.guest);
        pd._trpsiz[k] = 0;
    }

    // get the list of the currently installed error trap handlers (LuZ)
    for (k = 0; k < NUMEXCEPTIONS; k++) {
        ep = &cp->ErrorTraps[k];

        pd.except[k] = os9_long(ep->handleraddr);
        pd._exstk[k] = os9_long(ep->handlerstack);
    }

    // get the list of the currently opened paths
    for (k = 0; k < MAXUSRPATHS; k++) {
        pd._path[k] = os9_word(cp->usrpaths[k]);
    }

    // ... and get the total size from the first segment
    memsz = 0;

    for (k = 0; k < MAXMEMBLOCKS; k++) {
        if (cm->m[k].base.host != NULL) {
            pd._memimg[0] =
                (unsigned char *)os9_long((ulong)cm->m[k].base.guest);
            memsz += cm->m[k].size;
        }
    }
    pd._blksiz[0] = os9_long(memsz);

    uint32_t a0 = rp->regs[REGS_A + 0];
    uint16_t d1w = loword(rp->regs[REGS_D + 1]);
    memcpy(get_pointer(a0), &pd, d1w);
    // upe_printf( "pmodul2=%08X\n", os9_long( (ulong)pd._pmodul ) );
    return 0;
} // OS9_F_GPrDsc

os9err OS9_F_GBlkMp(regs_type *rp, _pid_)
/* F$GBlkMp:
 * Input:   d0.l= address to begin reporting segments
 *          d1.l= size of buffer in bytes
 *          (a0)= buffer pointer
 * Output:  d0.l= system min memory allocation size
 *          d1.l= number of memory fragments in system
 *          d2.l= total RAM found at startup
 *          d3.l= current total free RAM available
 *          (a0)= memory fragment information
 * Error:   Carry set
 *          d1.w = error
 */
{
    uint32_t *b, memsz;

    //  #ifdef powerc
    //  Gestalt( gestaltPhysicalRAMSize, &totalMem ); /* not all defs visible
    //  for MPW ... */ Gestalt( FOUR_CHAR_CODE('ram '), &totalMem ); #endif

    rp->regs[REGS_D + 0] = OS9MINSYSALLOC;
    rp->regs[REGS_D + 1] = 0;

    /* MaxBlock() will not be reduced on Mac OS X */
    /* Take the larger one, to avoid > Mem 100%   */
    memsz = max_mem();
    if (memsz > totalMem)
        totalMem = memsz;

    rp->regs[REGS_D + 2] = totalMem;
    rp->regs[REGS_D + 3] = memsz;

    b  = get_pointer(rp->regs[REGS_A + 0]);
    *b = 0; /* no segments available */
    return 0;
}

os9err OS9_F_SetSys(regs_type *rp, ushort cpid)
/* F$SetSys:
 * Input:   d0.w=offset
 *          d1.l=size (1,2 or 4 bytes, negative if read, positive if write)
 *          d2.l=value (if write)
 *
 * Output:  d2.l=original value (before write)
 * Error:
 *
 * Restrictions: This is a half-dummy, nearly always returns 0
 */
{
#define D_ID 0x0000     /* set to modsync code after coldboot has finished */
#define D_Init 0x0020   /* pointer to 'init' module */
#define D_TckSec 0x0028 /* ticks per second */
#define D_68881 0x002F
#define D_ModDir 0x003C
#define D_ModDir_L 0x0040
#define D_PrcDBT 0x0044 /* process descriptor block table pointer   */
#define D_PthDBT 0x0048 /* path    descriptor block table pointer   */
#define D_Ticks 0x0054  /* current tick count                       */
#define D_TotRAM 0x006C /* total RAM available at startup           */
#define D_MinBlk 0x0070 /* process minimum allocatable block size   */
#define D_FreMem 0x0074 /* system free memory list head             */
#define D_FreMem_L 0x0078
#define D_BlkSiz 0x007C /* system  minimum allocatable block size   */
#define D_DevTbl 0x0080 /* I/O device table ptr                     */
#define D_MPUTyp 0x03C8 /* MPU type, also supported for PowerPC     */
#define D_IPID 0x040C   /* os9exec/nt identification!               */

#define D_ScreenW 0x1000  /* Width in pixels of this system's screen  */
#define D_ScreenH 0x1004  /* Hight  "   "    "    "    "         "    */
#define D_ScreenW1 0x1008 /* Width in pixels from OS9exec's option -x */
#define D_ScreenH1 0x100C /* Hight  "   "    from OS9exec's option -y */
#define D_UserOpt 0x1010  /* User defined option -u                   */
#define D_IPAddr 0x1014   /* Open MGR screen at IP address: option -g */

    ulong        offs = loword(rp->regs[REGS_D + 0]);
    int          size = (int)rp->regs[REGS_D + 1];
    ulong        b    = (ulong)&mdirField;
    process_typ *cp   = &procs[cpid];

    uint32_t  v;
    ulong *ptr;
    int    k;

    switch (offs) {
    case D_ID:
        v = MODSYNC;
        break;
    case D_Init:
        v = init_module.guest;
        break;
    case D_TckSec:
        v = TICKS_PER_SEC;
        break;

    case D_68881:
        v = 1;   /* still some problems ? */
        break;

    case D_ModDir:
        // WIL: shared buffers between host and guest?!
        // v = b;
        v = 0x3eadbeef;
        Update_MDir();
        break;
    case D_ModDir_L:
        // WIL: shared buffers between host and guest?!
        // v = b + sizeof(mdirField);
        v = 0x2eadbeef;
        Update_MDir();
        break;
    case D_PrcDBT:
        // WIL: shared buffers between host and guest?!
        // v = (ulong)prDBT;
        v = 0x1eadbeef;

        ptr = &prDBT[1]; /* start with process nr 1 */
        for (k = 1; k < MAXPROCESSES; k++) {
            if (procs[k].state == pUnused)
                *ptr = 0;
            else
                *ptr = os9_long(
                    (ulong)&procs[k]); /* is now the right ptr, but ... */

            ptr++;
        }
        break; /* prc table image  */

    case D_PthDBT:
        // WIL: shared buffers between host and guest?!
        //v = (ulong)syspth;
        v = 0x0eadbeef;
        break; /* pth table image  */
    case D_Ticks:
        v = GetSystemTick();
        break; /* system heartbeat */
    case D_TotRAM:
        v = max_mem();
        break;
    case D_MinBlk:
        v = 16;
        break; /* as on real OS-9 systems */
    case D_FreMem:
        v = (ulong)NULL;
        break; /* no list available */
    case D_FreMem_L:
        v = (ulong)NULL;
        break;
    case D_BlkSiz:
        v = OS9MINSYSALLOC;
        break; /* as on real OS-9 systems */
    case D_DevTbl:
        // WIL: shared buffers between host and guest?!
        // v = (ulong)&devs;
        v = 0x4eadbeef;
        break; /* I/O device table ptr */

    case D_MPUTyp:
#if defined powerc && !defined MACOSX
        /* Gestalt( gestaltNativeCPUtype,   &v ); not all defs visible for MPW
         * ... */
        Gestalt(FOUR_CHAR_CODE('cput'), &v);
#else
        v = 4; /* just a fix value for 68040 */
#endif

        switch (v) {
        case 0: /* gestaltCPU68000 */
            v = 68000;
            break; /* Various 68k CPUs...    */
        case 1:    /* gestaltCPU68010 */
            v = 68010;
            break;
        case 2: /* gestaltCPU68020 */
            v = 68020;
            break;
        case 3: /* gestaltCPU68030 */
            v = 68030;
            break;
        case 4: /* gestaltCPU68040 */
            v = 68040;
            break;
        case 0x0101: /* gestaltCPU601   */
            v = 601;
            break;   /* IBM 601 */
        case 0x0103: /* gestaltCPU603   */
            v = 683;
            break;
        case 0x0104: /* gestaltCPU604   */
            v = 604;
            break;
        case 0x0106: /* gestaltCPU603e  */
            v = 603;
            break;
        case 0x0107: /* gestaltCPU603ev */
            v = 603;
            break;
        case 0x0108: /* gestaltCPU750   */
            v = 750;
            break;   /* Also 740 - "G3" */
        case 0x0109: /* gestaltCPU604e  */
            v = 604;
            break;
        case 0x010A: /* gestaltCPU604ev */
            v = 604;
            break; /* Mach 5, 250Mhz and up */
        default:
            v = 0;
        }

        break;

    case D_IPID:
        v = 42; // allow programs to identify as os9exec/nt environment
        if (cp->isIntUtil)
            v += 0x00010000; // special modes for internal utilities
        if (Native_Possible(false))
            v += 0x00020000; // b2: not to disturb SPOT V5.50
        if (cp->isPlugin)
            v += 0x00040000;
        if (Plugin_Possible(false))
            v += 0x00080000;
        break;

    case D_ScreenW:
        v = GetScreen('w');
        break; // not according to OS-9
    case D_ScreenH:
        v = GetScreen('h');
        break;
    case D_ScreenW1:
        v = screenW;
        break; // -x option of OS9exec
    case D_ScreenH1:
        v = screenH;
        break; // -y option of OS9exec
    case D_UserOpt:
        v = userOpt;
        break; // -u option of OS9exec
    case D_IPAddr:
        // WIL: shared buffers between host and guest?!
        // v = g_ipAddr;
        v = 0x5eadbeef;
        break; // -g option of OS9exec, no os9_long needed here

    default:
        v = 0;
        if (debug[dbgNorm] & dbgAnomaly)
            upe_printf("F$SetSys: unimplemented %04X (size=%X)\n", offs, size);
    } // switch

    debugprintf(dbgPartial,
                dbgNorm,
                ("# F$SetSys: %04lX %x %d\n", offs, size, v));

    switch (size) {
    case -1:
        rp->regs[REGS_D + 2] = v << 24;
        break; /* two different ways to read them */
    case -2:
        rp->regs[REGS_D + 2] = v << 16;
        break;
    case -4:
    case 1:
    case 2:
    case 4:
    case 0x80000001:
    case 0x80000002:
    case 0x80000004:
        rp->regs[REGS_D + 2] = v;
        break;
    }

    return 0;
}

os9err OS9_F_GModDr(regs_type *rp, _pid_)
/* F$GModDr:
 * Input:   d1.l = Maximum number of bytes to copy
 *          (a0) = Buffer pointer
 * Output:  d1.l = Actual number of bytes copied.
 *
 */
{
    uint32_t b   = rp->regs[REGS_A + 0];
    uint32_t cnt = rp->regs[REGS_D + 1];
    uint32_t mx  = (uint32_t)sizeof(mdirField);
    if (cnt > mx)
        cnt = mx;

    Update_MDir();
    MoveBlk(get_pointer(b), (byte *)&mdirField, cnt);

    debugprintf(dbgProcess, dbgNorm, ("# F$GModDr: get module directory\n"));

    rp->regs[REGS_D + 1] = cnt; /* return parameter */
    return 0;
}

os9err OS9_F_CpyMem(regs_type *rp, _pid_)
/* F$CpyMem:
 * Input:   d0.w = process ID of external memory's owner
 *          d1.l = number of bytes to copy
 *          (a0) = address of memory in external process to copy
 *          (a1) = caller's destination buffer pointer
 * Output:  none
 *
 */
{
    os9ptr   src = rp->regs[REGS_A + 0];
    os9ptr   dst = rp->regs[REGS_A + 1];
    uint32_t cnt = rp->regs[REGS_D + 1];

    MoveBlk(get_pointer(dst), get_pointer(src), cnt);
    debugprintf(
        dbgMemory,
        dbgDeep,
        ("# F$CpyMem: copied %ld bytes from $%lX to %$lX\n", cnt, src, dst));
    return 0;
}

os9err OS9_F_TLink(regs_type *rp, ushort cpid)
/* F$TLink
 * Input:   d0.w=User trap number
 *          d1.l=Optional memory override
 *          (a0)=Module name pointer (if NULL or *NULL, Trap handler will be
 * removed) d2-d7,a3-a5 are passed to the trap handler's init routine Output:
 * (a0)=updated past module name (a1)=Trap library execution entry point
 *          (a2)=Trap module pointer
 *          Other registers might be changed by the trap handler's init routine
 */
{
    os9err           err;
    ushort           trapidx;
    char             mpath[OS9PATHLEN], *p;
    traphandler_typ *tp;
    process_typ     *cp = &procs[cpid];

    trapidx = rp->regs[REGS_D + 0] - 1;
    if (trapidx >= NUMTRAPHANDLERS)
        return os9error(E_ITRAP); /* invalid trap code */

    os9ptr PTR = rp->regs[REGS_A + 0];
    char *ptr = get_pointer(PTR);

    if ((rp->regs[REGS_A + 0] != 0) && (*ptr != 0)) {
        /* install trap handler */
        char *res = p = nullterm(mpath, ptr, OS9PATHLEN);

        err = install_traphandler(cpid, trapidx, mpath, rp->regs[REGS_D + 1], &tp);
        if (!err) {
            mod_trap *trapmodule_host = tp->trapmodule.host;

            /* --- D0.W is still the trap no, D1.L is the additional memory */
            rp->regs[REGS_A + 0] = PTR + (uint32_t)(res - ptr);
            rp->regs[REGS_A + 2] = tp->trapmodule.guest; /* pointer to the trap module */
            rp->regs[REGS_A + 1] = tp->trapmodule.guest + os9_long(trapmodule_host->progmod._mexec);

            /* --- now modify stack and PC to return through trapinit routine to
             * program */

            os9ptr SP = rp->regs[REGS_A + 7]; // get current stack pointer
            uint32_t *sp = get_pointer(SP);

            // workaround for built-in utilities: not really used
            if (!cp->isIntUtil) {
                // save PC pointing to instruction after F$TLink
                *(--sp) = os9_long(rp->pc);
                *(--sp)  = 0; // save two dummy null words
                *(--sp)  = os9_long(rp->regs[REGS_A + 6]); // save "caller's A6"
                rp->regs[REGS_A + 7] = SP - (3 * sizeof(uint32_t));          // update stack pointer
            }

            /* --- modify registers to continue execution in traphandler's init
             * routine */
            rp->pc = tp->trapmodule.guest + os9_long(trapmodule_host->_mtrapinit);

            /* set pointer to traphandler's data with offset */
            rp->regs[REGS_A + 6] = tp->trapmem.guest + 0x8000;

            debugprintf(dbgTrapHandler,
                        dbgNorm,
                        ("# F$TLink: About to launch trapinit of vector #%d "
                         "(pid=%d) with the following regs:\n",
                         trapidx + 1,
                         cpid));
        }
        return err;
    }
    else {
        /* remove trap handler */
        err = release_traphandler(cpid, trapidx);
        debugprintf(dbgTrapHandler,
                    dbgNorm,
                    ("# F$TLink: traphandler at $%08lX for pid=%d released\n",
                     rp->regs[REGS_A + 2],
                     cpid));
    }
    return err;
}

os9err OS9_F_DatMod(regs_type *rp, _pid_)
/* F$DatMod:
 * Input:   d0.l=size of data reuired (not including header or CRC)
 *          d1.w=desired attr/revision
 *          d2.w=desired access permission
 *          d3.w=desired type/language (optional)
 *          d4.w=memory color type (optional)
 *          (a0)=module name string ptr
 * Output:  d0.w=module type/language
 *          d1.w=module attr/revision
 *          (a0)=updated name string ptr
 *          (a1)=module data ptr ('execution' entry)
 *          (a2)=module header ptrp
 * Error:   Carry set
 *          d1.w = error
 */
{
    char   mpath[OS9PATHLEN], *p;
    ushort mid;
    uint32_t size, namsize, msz, xpos, npos;
    uint32_t *k;
    ushort hpar;
    short  access, tylan, attrev;

    size = rp->regs[REGS_D + 0];

    os9ptr PTR = rp->regs[REGS_A + 0];
    char *ptr = get_pointer(PTR);
    char *ret = p = nullterm(mpath, ptr, OS9PATHLEN);

    debugprintf(dbgModules,
                dbgNorm,
                ("# F$DatMod: for '%s', size=%d, mode=$%04X\n",
                 mpath,
                 size,
                 loword(rp->regs[REGS_D + 3])));

    /* Is there already a module with the same name ? */
    if ((mid = find_mod_id(mpath)) < MAXMODULES)
        return os9error(E_KWNMOD);
    mid = NextFreeModuleId(mpath);
    if (mid >= MAXMODULES)
        return os9error(E_DIRFUL); /* module directory is full */

    namsize = (uint32_t)strlen(mpath) + 1;
    if (namsize & 1)
        namsize++; /* align needed */
    msz = DatMod_Size(namsize, size);

    addrpair_typ pp = get_mem(msz);
    if (pp.host == NULL)
        return os9error(E_NORAM); /* not enough memory */

    mod_exec *module_host = pp.host;
    os9modules[mid].modulebase = pp; /* enter pointer in free table entry */
    os9modules[mid].isBuiltIn  = false;

    access = loword(rp->regs[REGS_D + 2]);
    tylan  = 0x0400; /*this is the data module type */
    attrev = loword(rp->regs[REGS_D + 1]);

    FillTemplate(module_host, access, tylan, attrev); /* fill module body */

    {
        int32_t *start = &module_host->_mexcpt;
        int32_t *end  = (int32_t *)((uint8_t *)module_host + msz);

        /* clear data area */
        for (int32_t *k = start; k < end; k++)
            *k = 0;
    }


    /* now calculated as offset */

    uint8_t *mod_host = (uint8_t *)mod_host;
    uint8_t *mod_host_mexcpt = (uint8_t *)&module_host->_mexcpt;

    xpos = (uint32_t)(mod_host_mexcpt - mod_host);
    npos = msz - sizeof(uint32_t) - namsize;

    module_host->_mexec     = os9_long(xpos); /* right behind the header */
    module_host->_mh._msize = os9_long(msz);  /* data area size */
    module_host->_mh._mname = os9_long(npos); /* name offset */
    strcpy(Mod_Name(module_host), mpath);     /* name */

    hpar = calc_parity((ushort *)module_host, 23); /* byte-order insensitive */
    module_host->_mh._mparity = hpar;              /* byte-order insensitive */

    mod_crc(module_host);
    os9modules[mid].linkcount = 1; /* module is created and linked */

    retword(rp->regs[REGS_D + 0]) = os9_word(module_host->_mh._mtylan);
    retword(rp->regs[REGS_D + 1]) = os9_word(module_host->_mh._mattrev);

    rp->regs[REGS_A + 0] = PTR + (uint32_t)(ret - ptr);
    rp->regs[REGS_A + 2] = pp.guest;
    rp->regs[REGS_A + 1] = pp.guest + os9_long(module_host->_mexec);

    return 0;
}

os9err OS9_F_Fork(regs_type *rp, ushort cpid)
/* F$Fork
 * Input:   d0.w=desired module type/revision
 *          d1.l=Additional memory size
 *          d2.l=Parameter size
 *          d3.w=number of I/O path to copy
 *          d4.w=priority
 *          (a0)=Module name pointer
 *          (a1)=Parameter pointer
 * Output:  d0.w=child process ID
 *          (a0)=updated past module name
 *
 * Note:    if global dummyfork is<>0, the dummy behaviour of os9exec 1.14 will
 * be emulated, that is, instead of launching a new process, the command is
 * written to stdout with all parameters, so that it can be executed later as a
 * MPW script
 */
{
    char         mpath[OS9PATHLEN], *p;
    ushort       newpid, newmid;
    long         n;
    os9err       err;
    ushort       numpaths = loword(rp->regs[REGS_D + 3]);
    ushort       grp, usr, prior;
    process_typ *cp = &procs[cpid];
    process_typ *np;

    /* get module name */
    {
        os9ptr p = rp->regs[REGS_A + 0];
        char *ptr = get_pointer(p);
        char *res = nullterm(mpath, ptr, OS9PATHLEN);

        rp->regs[REGS_A + 0] = p + (uint32_t)(res - ptr);
    }

    /* now fork */
    if (dummyfork) {
        /* --- dummy fork, no new process */
        fflush(stdout);
        uphe_printf("Program executed F$Fork: Command line equivalent is:\n");

        if (dummyfork == 2)
            printf("os9 -i -fo ");
        printf("%s ", mpath); /* show program name */

        /* --- scan and display parameters */
        p = get_pointer(rp->regs[REGS_A + 1]);
        n = rp->regs[REGS_D + 2];

        while (n-- > 0) {
            if (*p < ' ')
                break;
            if (*p == '/')
                putchar('|'); /* quote slashes, as they are MPW shell quotes */
            putchar(*p++);
        }

        puts(" ;\n");
        fflush(stdout);

        /* --- fake return values */
        retword(rp->regs[REGS_D + 0]) = cpid + 1; /* next process ID */
        return 0;
    }

    /* --- multitasking enabled, fork program as process */
    /* --- check if "OS9exec" module is about to be launched, */
    grp = os9_word(cp->pd._group); /* inherit grp.usr from parent's process */
    usr = os9_word(cp->pd._user);

    prior = loword(rp->regs[REGS_D + 4]);
    if (prior == 0)
        prior = os9_word(cp->pd._prior);

    /* exe dir only, link style first, then load style */
    err = new_process(cpid, &newpid, numpaths);
    if (err)
        return err;
    retword(rp->regs[REGS_D + 0]) =
        newpid;          // return forked process' ID, assign early enough
    np = &procs[newpid]; // is valid, even if error

    do {
        err = link_load(cpid, mpath, &newmid);
        if (err)
            break;
        err = prepFork(newpid,
                       mpath,
                       newmid,
                       rp->regs[REGS_A + 1],
                       rp->regs[REGS_D + 2],
                       rp->regs[REGS_D + 1],
                       numpaths,
                       grp,
                       usr,
                       prior);
        if (err)
            break;

        if (!np->isIntUtil) {
            if (cp->pd._cid != 0 && cp->pd._cid != newpid)
                np->pd._sid = cp->pd._cid;
        }

        if (!np->isIntUtil) {
            cp->pd._cid = os9_word(newpid); /* this is the child */
            if (!cp->isIntUtil)
                /* --- now actually cause process switch */
                currentpid = newpid; /* continue execution in new process */
            /* except for internal utilities, where it will be done just an
             * moment later */

            set_os9_state(newpid,
                          pActive,
                          "OS9_F_Fork"); /* make this process active */
        }

        retword(rp->regs[REGS_D + 0]) =
            newpid; // return forked process' ID, in case it has been changed
        arbitrate = true; /* really needed ! */
        return 0;
    } while (false);

    if (np->state != pDead) {
        /* -- save exit code */
        close_usrpaths(newpid);
        set_os9_state(newpid,
                      pUnused,
                      "OS9_F_Fork"); /* unused again because of error */
        // np->exiterr= err;
    }

    return err;

    /*
              err= link_load ( cpid,mpath,&newmid );
    if (!err) err= prepFork( newpid,mpath, newmid,
                             (byte*)rp->regs[REGS_A + 1],rp->regs[REGS_D + 2],rp->regs[REGS_D + 1],
                             numpaths, grp,usr, prior );

    np= &procs[ newpid ]; // is valid, even if error
    if (!err) {
      // --- now actually cause process switch
    //if  (!np->isIntUtil ||
    //      np->isNative) {
      if  (!np->isIntUtil) {
        if (cp->pd._cid!=0 &&
            cp->pd._cid!=newpid) np->pd._sid= cp->pd._cid;
      }

      if   (!np->isIntUtil) {
             cp->pd._cid= os9_word( newpid );                     // this is the
    child if (!cp->isIntUtil) currentpid=  newpid;                    //
    continue execution in new process
          // except for internal utilities, where it will be done just an moment
    later

        set_os9_state( newpid, pActive, "OS9_F_Fork" );    // make this process
    active }

      arbitrate= true; // really needed !
      return 0;
    }

    // -- save exit code
    close_usrpaths( newpid );
    set_os9_state ( newpid, pUnused, "OS9_F_Fork" ); // unused again because of
    error np->exiterr= err; return       err;
    */
}

os9err OS9_F_Chain(regs_type *rp, ushort cpid)
/* F$Chain
 * Input:   d0.w=desired module type/revision
 *          d1.l=Additional memory size
 *          d2.l=Parameter size
 *          d3.w=number of I/O path to copy
 *          d4.w=priority
 *          (a0)=Module name pointer
 *          (a1)=Parameter pointer
 * Output:  None: F$Chain does not return to the calling process
 *
 * Note:    if global dummyfork is<>0, the dummy behaviour of os9exec 1.14 will
 * be emulated, that is, instead of launching a new process, the command is
 * written to stdout with all parameters, so that it can be executed later as a
 * MPW script
 */
{
    process_typ *cp = &procs[cpid];
    char         mpath[OS9PATHLEN];
    ushort       newmid;
    os9err       err;
    ushort       numpaths, k, grp, usr, prior, sp, sib;
    addrpair_typ paramptr;
    uint32_t     paramsiz;

    /* no error so far */
    err = 0;

    /* get module name */
    os9ptr PTR = rp->regs[REGS_A + 0];
    char *src = get_pointer(PTR);
    char *dst = nullterm(mpath, src, OS9PATHLEN);

    rp->regs[REGS_A + 0] = PTR + (uint32_t)(dst - src);
    numpaths = loword(rp->regs[REGS_D + 3]);

    paramsiz = rp->regs[REGS_D + 2];
    paramptr = get_mem(paramsiz);

    if (paramptr.host == NULL)
        err = os9error(E_NORAM);
    if (!err) {
        /* save a copy of the parameter area */
        MoveBlk(paramptr.host, get_pointer(rp->regs[REGS_A + 1]), paramsiz);

        /* user paths higher than numpaths must be closed */
        for (k = numpaths; k < MAXUSRPATHS; k++) {
            sp = cp->usrpaths[k];
            if (sp > 0 && sp < MAXSYSPATHS)
                usrpath_close(cpid, k);
        }

        cp->pd._sigvec = 0; /* switch it off for the new process */

        /* this stuff is no longer used now */
        unlink_traphandlers(cpid);
        unlink_module(cp->mid); /* if any */
        free_mem(cpid);
        /* IMPORTANT: use this only after all other resources are freed     */
        /* (because they might have memory allocated, such as traphandlers) */

        /* now chain; exe dir only, load style */
    }

    grp =
        os9_word(cp->pd._group); /* inherit grp.usr from the replaced process */
    usr = os9_word(cp->pd._user);
    sib = os9_word(cp->pd._sid); /* save it */

    prior = loword(rp->regs[REGS_D + 4]);
    if (prior == 0)
        prior = os9_word(cp->pd._prior);

    if (!err)
        err = link_load(cpid, mpath, &newmid);
    if (!err)
        err = prepFork(cpid,
                       mpath,
                       newmid,
                       paramptr.guest,
                       rp->regs[REGS_D + 2],
                       rp->regs[REGS_D + 1],
                       numpaths,
                       grp,
                       usr,
                       prior);
    cp->pd._sid = os9_word(sib); /* restore it */

    /* return the parameter memory block, it is now allocated at the process */
    if (paramptr.host != NULL)
        release_mem(paramptr);

    if (err) {
        /* -- save exit code */
        cp->exiterr = err;
        /* -- kill the process */
        kill_process(cpid);
        debugprintf(
            dbgProcess,
            dbgNorm,
            ("# pid=%d exited thru F$Chain, exit-code=%d\n", cpid, err));
    }

    return 0; /* that's it */
}

os9err OS9_F_Wait(regs_type *rp, ushort cpid)
/* F$Wait
 * Input:   none
 * Output:  d0.w=terminating child process' ID
 *          d1.w=exit code of terminating child
 *
 * Notes: - Acts as dummy counterpart to F$Fork if dummyfork is non-zero.
 */
{
    ushort k;
    ushort activeChild;
    // process_typ* pp= &procs[ cpid ];
    process_typ *cp;
    ushort       sv_pid;
    // ushort*      chP;

    sig_mask(cpid, 0); /* disable signal mask */

    if (dummyfork) {
        /* wait for dummy fork */
        retword(rp->regs[REGS_D + 0]) =
            cpid + 1;          /* previously "forked" child's process ID */
        retword(rp->regs[REGS_D + 1]) = 0; /* exit status */
        return 0;
    }

    /* check if there are children at all */
    activeChild = MAXPROCESSES;
    for (k = 2; k < MAXPROCESSES; k++) { // start at process 2
        cp = &procs[k];
        if (cp->state == pDead &&
            os9_word(cp->pd._pid) == cpid) { // this is a child
            // child is dead, report that
            retword(rp->regs[REGS_D + 0]) = k;           // ID of dead child
            retword(rp->regs[REGS_D + 1]) = cp->exiterr; // exit error of child
            AssignNewChild(cpid, k);

            /*
         // restore former child id
                    chP= &pp->pd._cid;
            while (*chP!=0) {
              if  (*chP==k) { *chP= cp->pd._sid; break; }

                    pp= &procs[ *chP ];
              chP= &pp->pd._sid; // go to the next element
            }

          //pp->pd._cid= cp->pd._sid; // restore former child id
            */

            debugprintf(dbgProcess,
                        dbgNorm,
                        ("# F$Wait: child pid=%d has died before parent=%d "
                         "called F$Wait\n",
                         k,
                         cpid));
            set_os9_state(k, pUnused, "OS9_F_Wait");
            return 0; // process already died, no need to wait
        }
    }

    for (k = 2; k < MAXPROCESSES; k++) { // start at process 2
        cp = &procs[k];
        if (cp->state != pUnused && cp->state != pDead &&
            os9_word(cp->pd._pid) == cpid) { // this is a child
            // there is a non-dead child
            debugprintf(dbgProcess,
                        dbgNorm,
                        ("# F$Wait: child pid=%d is active\n", k));
            activeChild = k; // remember its id
            break;
        }
    }

    cp = &procs[cpid];
    if (activeChild < MAXPROCESSES) { /* there is (at least) one child, but it's
                                         not yet dead */
        retword(rp->regs[REGS_D + 0]) = 0;        /* no child, should never be used !! */
        retword(rp->regs[REGS_D + 1]) = 0;        /* no error */

        debugprintf(
            dbgProcess,
            dbgNorm,
            ("# F$Wait: no dead children, go waiting, activate child pid=%d\n",
             activeChild));
        set_os9_state(cpid,
                      pWaiting,
                      "OS9_F_Wait"); /* put myself to wait state */

        // in case of internal utility we have to wait until child program has
        // finished
        if (cp->isIntUtil) {
            while (true) {
                sv_pid    = currentpid;
                arbitrate = true;
                os9exec_loop(0, true);
                currentpid = sv_pid; // make sure it is not changed

                cp = &procs[currentpid];
                if (cp->state == pActive) {
                    retword(rp->regs[REGS_D + 0]) = cp->os9regs.regs[REGS_D + 0];
                    retword(rp->regs[REGS_D + 1]) = cp->os9regs.regs[REGS_D + 1]; // exit error of child
                    return 0; // it's done already !!
                }

                for (k = 2; k < MAXPROCESSES; k++) { /* start at process 2  */
                    cp = &procs[k];
                    if (cp->state == pDead &&
                        os9_word(cp->pd._pid) == cpid) { // this is a child
                        // child is dead, report that
                        retword(rp->regs[REGS_D + 0]) = k; // ID of dead child
                        retword(rp->regs[REGS_D + 1]) =
                            procs[k].exiterr; // exit error of child
                        debugprintf(dbgProcess,
                                    dbgNorm,
                                    ("# F$Wait: child pid=%d has died before "
                                     "parent=%d called F$Wait\n",
                                     k,
                                     cpid));
                        set_os9_state(k, pUnused, "OS9_F_Wait");
                        return 0; // process already died, no need to wait
                    }
                }
            } // loop
        }

        currentpid = activeChild; /* activate that child */
        arbitrate  = true;
        return 0; /* continue execution with another process */
    }

    /* there are no children */
    debugprintf(dbgProcess,
                dbgNorm,
                ("# F$Wait: pid=%d has no children to wait for\n", cpid));
    return os9error(E_NOCHLD);
}

os9err OS9_F_Sleep(regs_type *rp, ushort cpid)
/* F$Sleep
 * Input:   none
 * Output:  d0.w=terminating child process' ID
 *          d1.w=exit code of terminating child
 *
 * Notes: - Acts as dummy counterpart to F$Fork if dummyfork is non-zero.
 */
{
    process_typ *cp        = &procs[cpid];
    int          sleeptime = rp->regs[REGS_D + 0];
    int          sleep_x   = sleeptime & 0x7fffffff;
    int          ticks;

    sig_mask(cpid, 0); // disable signal mask
    if (cp->way_to_icpt)
        return 0; // don't sleep if signaled

    arbitrate = true; // allow next process to run
    if (sleep_x == 1)
        return 0; // do not really sleep

    CheckInputBuffers(); // make shure that special chars like
                         // CtrlC/CtrlE/XOn/XOff will be handled

    set_os9_state(cpid, pSleeping, "OS9_F_Sleep"); // status is sleeping now

    if (sleep_x == 0) {
        // --- put process to indefinite sleep
        debugprintf(dbgProcess,
                    dbgNorm,
                    ("# F$Sleep: put pid=%d to indefinite sleep!\n", cpid));
        cp->wakeUpTick = MAX_SLEEP;
    }
    else {
        // --- timed sleep
        if (sleeptime < 0)
            ticks = sleep_x * TICKS_PER_SEC / 256;
        else
            ticks = sleep_x;

        debugprintf(dbgPartial,
                    dbgNorm,
                    ("# F$Sleep: pid=%d sleep for %d ticks\n", cpid, ticks));
        cp->wakeUpTick = GetSystemTick() + ticks;
    }

    return 0;
}

/* F$Alarm
 * Input:   d0.l=Alarm ID (or zero)
 *          d1.w=Alarm function code
 *          d2.l=Signal code
 *          d3.l=time interval or time
 *          d4.l=date (when using absolute time)
 * Output:  d0.l=Alarm ID
 */
os9err OS9_F_Alarm(regs_type *rp, ushort cpid)
{
    os9err err;
    uint32_t aId   = rp->regs[REGS_D + 0];
    int16_t  aFunc = loword(rp->regs[REGS_D + 1]);
    ushort sig   = rp->regs[REGS_D + 2];
    uint32_t aTime = rp->regs[REGS_D + 3];
    uint32_t aDate = rp->regs[REGS_D + 4];

    err = Alarm(cpid, &aId, aFunc, sig, aTime, aDate);
    debugprintf(
        dbgProcess,
        dbgNorm,
        ("# OS9_F_Alarm: id=%08X aFunc=%d sig=%08X tim=%08X dat=%08X err=%d\n",
         aId,
         aFunc,
         sig,
         aTime,
         aDate,
         err));

    if (!err)
        rp->regs[REGS_D + 0] = aId; /* get back <aId> */
    return err;
}

os9err OS9_F_Sigmask(regs_type *rp, ushort cpid)
/* F$SigMask */
{
    int level = rp->regs[REGS_D + 1];
    sig_mask(cpid, level);
    return 0;
}

/* F$CRC
 * Input:   d0.l=Data byte count
 *          d1.l=CRC accumulator
 *          (a0)=Pointer to data (if 0, data of one single nullbyte is assumed)
 * Output:  d1.l=updated CRC accumulator
 */
os9err OS9_F_CRC(regs_type *rp, _pid_)
{
    if (rp->regs[REGS_A + 0] == 0) {
        /* add single 0 byte to CRC */
        void *buff = "\0";

        /* update with one additional 0 byte */
        rp->regs[REGS_D + 1] = calc_crc(buff, 1, rp->regs[REGS_D + 1]);
    }
    else {
        /* update CRC over given area */
        void *buff = get_pointer(rp->regs[REGS_A + 0]);
        rp->regs[REGS_D + 1] = calc_crc(buff, rp->regs[REGS_D + 0], rp->regs[REGS_D + 1]);
    }

    return 0;
}

/* F$SetCRC
 * Input:   (a0)=Pointer to module image
 * Output:  module image with updated CRC
 */
os9err OS9_F_SetCRC(regs_type *rp, _pid_)
{
    mod_exec *m = get_pointer(rp->regs[REGS_A + 0]);
    ulong     modsize;
    ushort    hpar;

    if (os9_word(m->_mh._msync) != MODSYNC)
        return os9error(E_BMID); /* no good module */
    modsize = os9_long(m->_mh._msize);

    hpar = calc_parity((ushort *)m, 23); /* byte-order insensitive */
    debugprintf(dbgModules,
                dbgNorm,
                ("# F$SetCRC: Module @ $%08lX (size=%ld): new parity=$%04X\n",
                 (ulong)m,
                 modsize,
                 hpar));
    m->_mh._mparity = hpar; /* byte-order insensitive */
    mod_crc(m);

    return 0;
}

os9err OS9_F_PrsNam(regs_type *rp, _pid_)
/* F$PrsNam
 * Input:   (a0)=pointer to path string to be parsed
 * Output:  d0.b=path element delimiter (=[a0])
 *          d1.w=length of pathlist element
 *              (a0)=pathlist pointer updated past optional "/"
 *          (a1)=pointer to path element delimiter (next / or <0x20)
 * Error:   E$BNam
 *
 * Note: also allows "{" and "}" in filenames to take advantage of MPW shell
 * variable substitution.
 */
{
    ushort n;

    os9ptr P = rp->regs[REGS_A + 0];
    char  *p = get_pointer(P);

    debugprintf(dbgFiles, dbgDeep, ("# F$PrsNam: input string='%s'\n", p));

    if (*p == '/') {
        /* assign updated ptr to path element */
        ++p;
        ++P;
        rp->regs[REGS_A + 0] = P;
    }

    n = 0;                       /* pathlist size=0 */
    while (isalnum(*p) || *p == '.' || *p == '_' || *p == '$' || *p == '{' ||
           *p == '}') {
        P++;
        p++;
        n++;
    }
    if (n == 0)
        return os9error(E_BNAM); /* null name is bad name */
    debugprintf(dbgFiles,
                dbgDeep,
                ("# F$PrsNam: a0='%s', a1='%s', terminator='%c'\n",
                 get_pointer(rp->regs[REGS_A + 0]),
                 p,
                 *p));

    /* pointer to terminator */
    rp->regs[REGS_A + 1] = P;

    /* terminator */
    retbyte(rp->regs[REGS_D + 0]) = (unsigned char)*p;

    /* size of path element */
    retword(rp->regs[REGS_D + 1]) = n;

    return 0;
}

os9err OS9_F_CmpNam(regs_type *rp, _pid_)
/* F$CmpNam
 * Input:   d1.w-length of pattern string
 *              (a0)=pointer to pattern string
 *          (a1)=pointer to target string
 * Output:  none (if strings match)
 * Error:   E$Differ if strings don't match
 *
 * Note: Only does simple, non-recursive pattern match
 */
{
    char   *pat, *targ, *spat, *starg, *patend;
    Boolean match;

    /* get pointers */
    pat    = (char *)get_pointer(rp->regs[REGS_A + 0]);
    targ   = (char *)get_pointer(rp->regs[REGS_A + 1]);
    patend = pat + loword(rp->regs[REGS_D + 1]); /* attention, high word can be <> 0 */
    spat   = NULL;

    /* start comparison */
    match = true;
    while (true) {
        if (*targ != 0 && pat < patend && *pat == '?') {
            pat++;
            targ++;
            /* skip EXACTLY one char, it's ok */
        }
        else if (pat < patend && *pat == '*') {
            while (*pat == '*') {
                pat++; /* pattern ends with *: whatever comes in target, this is
                          a match */
                if (pat >= patend) {
                    return 0;
                };
            }

            /* now we must find *pat in *targ */
            while (*targ != 0) {
                if (tolower(*pat) == tolower(*targ)) {
                    spat  = pat;
                    starg = targ;
                    break; /* continue normally, match so far */
                }
                targ++;
            }
            if (*targ == 0)
                match = false; /* no match found any more */
        }
        else {
            if (*targ == 0)
                break; /* end of string reached */

            if (pat < patend && tolower(*pat) == tolower(*targ)) {
                pat++;
                targ++;
            }
            else {
                if (spat == NULL) {
                    match = false;
                    break; /* no match */
                }

                /* we could possibly try to restart */
                pat  = spat--;  /* back to '*' */
                targ = starg++; /* begin behind last match */
            }
        }
    }

    /* only if match and both strings at end it is a real match */
    if (match && pat >= patend && *targ == 0)
        return 0;
    return os9error(E_DIFFER);
}

os9err OS9_F_PErr(regs_type *rp, _pid_)
/* F$PErr
 * Input:   d0.w=Error message path number (0=none)
 *          d1.w=Error number
 * Output:  none
 *
 * Restriction: Error message path is not used
 */
{
    os9err err;
    char  *nam, *desc;
    char   msgbuffer[255];

    err = loword(rp->regs[REGS_D + 1]);
    get_error_strings(err, &nam, &desc);

    sprintf(msgbuffer,
            "Error #%03d:%03d (%s) %s\n",
            err >> 8,
            err & 0xFF,
            nam,
            desc);
    upe_printf(msgbuffer);
    return 0;
}

os9err OS9_F_SysDbg(_rp_, _pid_)
/* F$SysDbg
 * Input : none
 * Output: none
 */
{
    if (quitFlag)
        stop_os9exec(); /* --- and never come back */
    debugwait();
    return 0;
}

os9err OS9_F_Panic(_rp_, ushort cpid)
/* F$Panic
 * Input : none
 * Output: none
 */
{
    uphe_printf("PANIC: F$Panic called by pid=%d\n", cpid);
    debugwait();
    return 0;
}

os9err OS9_F_SSvc(_rp_, _pid_)
/* F$SSvc
 * Input:   (a1)=pointer to service request initislization table
 *          (a3)=user defined
 * Output:  none
 * Restrictions: system-state system call
 */
{
    // ulong sqtab= rp->regs[REGS_A +  1 ]; /* %%% no nothing at the moment: 0x61 hardwired
    // */ ulong suser= rp->regs[REGS_A +  3 ];
    return 0;
}

os9err OS9_F_Permit(_rp_, _pid_)
/* F$Permit:
 * Input:   ?
 * Output:  none
 * Error:   none
 *
 * Restrictions: This is a dummy.
 */
{
    debugprintf(dbgPartial, dbgNorm, ("# F$Permit, dummy only\n"));
    arbitrate = true;
    return 0;
}

os9err OS9_F_SPrior(regs_type *rp, _pid_)
/* F$SPrior:
 * Input:   d0.w=process ID
 *          d1.w=new priority
 * Output:  none
 */
{
    return setprior(loword(rp->regs[REGS_D + 0]), loword(rp->regs[REGS_D + 1]));
}

/* --------------------------------------------------------- */
os9err OS9_F_Dummy(_rp_, ushort cpid)
/* F$Dummy:
 * Returns no error, but a debug warning
 */
{
    process_typ *cp = &procs[cpid];
    debugprintf(dbgAnomaly,
                dbgNorm,
                ("# dummy implementation of %s called by pid=%d\n",
                 get_syscall_name(cp->lastsyscall),
                 cpid));
    return 0;
}

os9err OS9_F_SDummy(_rp_, ushort cpid)
/* F$SDummy:
 * Silent dummy, does not warn except when dbgPartial is on
 */
{
    process_typ *cp = &procs[cpid];
    debugprintf(dbgPartial,
                dbgNorm,
                ("# (silent) dummy implementation of %s called by pid=%d\n",
                 get_syscall_name(cp->lastsyscall),
                 cpid));
    return 0;
}

os9err OS9_F_UnImp(_rp_, ushort cpid)
/* F$UnImp
 * Unimplemented system call, returns E_UNKSVC and a debug message
 */
{
    process_typ *cp = &procs[cpid];
    debugprintf(dbgAnomaly,
                dbgNorm,
                ("# unimplemented %s called by pid=%d\n",
                 get_syscall_name(cp->lastsyscall),
                 cpid));
    return E_UNKSVC;
}

os9err OS9_F_SUnImp(_rp_, ushort cpid)
/* F$SUnImp
 * Silent Unimplemented system call, returns E_UNKSVC
 */
{
    process_typ *cp = &procs[cpid];
    debugprintf(dbgPartial,
                dbgNorm,
                ("# (silent) unimplemented %s called by pid=%d\n",
                 get_syscall_name(cp->lastsyscall),
                 cpid));
    return E_UNKSVC;
}

/* eof */
