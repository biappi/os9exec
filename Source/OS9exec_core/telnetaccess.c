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
 *    Revision 1.13  2007/01/04 21:08:08  bfo
 *    Avoid unused var (cons)
 *
 *    Revision 1.12  2007/01/04 20:52:59  bfo
 *    Shorter window title for Windows version
 *
 *    Revision 1.11  2006/12/01 19:56:44  bfo
 *    "HandleEvent": no more separate MACH implementation
 *
 *    Revision 1.10  2006/06/11 22:07:13  bfo
 *    set_os9_state with 3rd param <callingProc>
 *
 *    Revision 1.9  2006/06/01 15:17:15  bfo
 *    Signedness adaptions (for gcc 4.0)
 *
 *    Revision 1.8  2006/02/19 15:28:50  bfo
 *    Header changed to 2006
 *
 *    Revision 1.7  2005/06/30 11:55:40  bfo
 *    Mach-O support
 *
 *    Revision 1.6  2004/11/20 11:44:08  bfo
 *    Changed to version V3.25 (titles adapted)
 *
 *    Revision 1.5  2003/05/17 10:14:41  bfo
 *    Disabled for USE_CARBON
 *
 *    Revision 1.4  2002/08/09 22:39:21  bfo
 *    New procedure set_os9_state introduced and adapted everywhere
 *
 *
 */

/* Telnet socket interface */
/* ======================= */

#include "os9exec_incl.h"

void InitTTYs()
/* initialize them all (bfo) */
{
    ttydev_typ *mco;
    int         k;

    for (k = 0; k < MAXTTYDEV; k++) {
        mco             = &ttydev[k];
        mco->installed  = false;
        mco->inBufUsed  = 0;
        mco->holdScreen = false;
        mco->pid        = 0;
    }
}

short InstallTTY(syspath_typ *spP, int consoleID)
/* initialize access to TTY */
{
    ttydev_typ *mco = &ttydev[consoleID - TTY_Base];

    if (!mco->installed) {
        mco->spP       = spP;
        mco->installed = true;
    }

    return 0;
}

void RemoveTTY(int consoleID)
/* de-initalize TTY */
{
    ttydev_typ *mco = &ttydev[consoleID - TTY_Base];
    mco->installed  = false;
}

/* write characters to TTY */
uint32_t WriteCharsToPTY(char *buffer, uint32_t n, int consoleID, Boolean do_lf)
{
#ifdef PIP_SUPPORT
    ushort       pid = currentpid;
    process_typ *cp  = &procs[pid];
    syspath_typ *spC = crossedPath(currentpid, g_spP);
    ttydev_typ  *mco = &ttydev[consoleID - TTY_Base];

    if (cp->state == pWaitRead) {
        set_os9_state(pid, cp->saved_state, "WriteCharsToPTY");
        n = cp->saved_cnt;
    }

    if (mco->holdScreen) {
        cp->saved_cnt   = n;
        cp->saved_state = cp->state;
        set_os9_state(pid, pWaitRead, "WriteCharsToPTY");
        return n;
    }

    spC->u.pipe.pchP->do_lf = do_lf;                   /* store it here also */
    PutCharsToTTY(currentpid, spC, &n, buffer, do_lf); /* put it into pipe !! */
    if (n > 0)
        lw_pid(mco); /* assign for later use */
    return n;

#else

    return 0;
#endif
}

void CheckInputBuffersTTY(void)
{
#ifdef PIP_SUPPORT
    int k;
    for (k = 0; k < MAXTTYDEV; k++)
        CheckInBufferTTY(&ttydev[k]);
#endif
}

/* how many characters are ready at the TTY ? */
Boolean DevReadyTTY(uint32_t *cnt, int consoleID)
{
    Boolean     ok;
    ttydev_typ *mco = &ttydev[consoleID - TTY_Base];

    *cnt     = 0; /* default */
    mco->pid = currentpid;
    CheckInputBuffersTTY();

    /* how many characters are currently ready in input buffer ? */
    ok = mco->inBufUsed;
    if (ok)
        *cnt = mco->inBufUsed;
    return ok;
}

/* read characters from TTY */
uint32_t ReadCharsFromPTY(char *buffer, uint32_t n, int consoleID)
{
    uint32_t    cnt; /* this is the data base */
    ttydev_typ *mco = &ttydev[consoleID - TTY_Base];

    /* if not yet ready handle other events */
    if (mco->holdScreen || (!DevReadyTTY(&cnt, consoleID))) {
        devIsReady = false;
        *buffer    = NUL;
        return 1; /* if 0, it would not return to caller */
    }

    devIsReady = true;
    if (mco->inBufUsed) { // got some chars to return...
        cnt =
            (mco->inBufUsed < n ? mco->inBufUsed : n); // return this many chars
        MoveBlk(buffer, mco->inBuf, cnt);
        if (cnt < mco->inBufUsed) {
            // didn't return all chars; shift buffer contents down
            MoveBlk(mco->inBuf, mco->inBuf + cnt, mco->inBufUsed - cnt);
            mco->inBufUsed -= cnt; /* copy <inBuf>, and not <buffer> !!! */
        }
        else
            mco->inBufUsed = 0;
        return cnt;
    }

    return 0;
}

/* ---------------------------------------------------------------- */

void WindowTitle(char *title, Boolean vmod)
{
    const char *p;

#if defined TERMINAL_CONSOLE
    char cons[40];
#endif

    p = " (UAE)";

    sprintf(title, "%s%s", OS9exec_Name(), p);

#ifdef TERMINAL_CONSOLE
    if (vmod)
        sprintf(title, "%s  /vmod - display for  \"%s\"", title, gTitle);
    else {
        Console_Name(gConsoleID, (char *)&cons);
        sprintf(title, "%s  /%s - terminal window", title, cons);
    }
#endif
}

void HandleEvent(void) {}

/* CheckInputBuffers */
void CheckInputBuffers(void)
{

    CheckInputBuffersTTY(); /*   "        "       "  ttys         */
    HandleEvent();          /* and check also the Mac events      */
}

/* true, if next character(s) can be read */
/* used by "ReadCharsFromConsole" and "GetStat.SS_Ready" */
Boolean DevReadyTerminal(uint32_t *count, ttydev_typ *mco)
{
    Boolean ok = mco->inBufUsed;
    if (ok)
        *count = mco->inBufUsed;
    return ok;
}

Boolean DevReady(uint32_t *count)
{
    HandleEvent();

    *count = 1;
    return true; /* %%% not yet as it should be */

#ifdef MACOSX
    *count = 1;
    return true; /* %%% not yet as it should be */
#endif

    return DevReadyTerminal(count, &main_mco);
}

#ifdef TERMINAL_CONSOLE
long ReadCharsFromTerminal(char *buffer, long n, ttydev_typ *mco)
{
    long cnt; /* this is the data base */

    if (mco->holdScreen) {
        devIsReady = false;
        *buffer    = NUL;
        return 1;
    }

    devIsReady = true;    // the default value for devIsReady
    if (mco->inBufUsed) { // got some chars to return...
        cnt =
            (mco->inBufUsed < n ? mco->inBufUsed : n); // return this many chars
        MoveBlk(buffer, mco->inBuf, cnt);

        if (cnt < mco->inBufUsed) {
            // didn't return all chars; shift buffer contents down
            MoveBlk(mco->inBuf, mco->inBuf + cnt, mco->inBufUsed - cnt);
            mco->inBufUsed -= cnt; /* copy <inBuf>, and not <buffer> !!! */
        }
        else
            mco->inBufUsed = 0;
        return cnt;
    }

    // no chars in the buffer... enter an event loop till we get some (or a
    // RETURN)
    while (true) {
        HandleEvent();
        if (gConsoleQuickInput) {
            // terminate read immediately (non-blocking)
            if (mco->inBufUsed)
                return ReadCharsFromTerminal(buffer, n, mco);

            devIsReady = false; // break the loop
            *buffer    = NUL;
            return 1; /* if 0, it will not return to caller */
        }
        else { // terminate read only when the RETURN key is pressed
            for (cnt = 0; cnt < mco->inBufUsed; cnt++) {
                if (mco->inBuf[cnt] == keyRETURN)
                    return ReadCharsFromTerminal(buffer, n, mco);
            }
        }
    }
}
#endif

/* eof */
