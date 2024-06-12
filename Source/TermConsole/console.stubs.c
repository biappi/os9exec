/*      termconsole.c
**
**      A console based on a CTB terminal window.
**      by Joseph J. Strout (jstrout@ucsd.edu)
**      based on console.stubs.c by Metrowerks
**
**      Adapted by LuZ/Bfo for the OS9exec project
*/

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
 *    Revision 1.11  2006/12/01 19:54:52  bfo
 *    <consoleSleep> param for "HandleOneEvent" (reduce MacClassic load)
 *
 *    Revision 1.10  2006/05/26 14:31:02  bfo
 *    Renamed to "ResourceConstants.h"
 *
 *    Revision 1.9  2005/06/30 10:51:45  bfo
 *    ifndef USE_CARBON => ifdef USE_CLASSIC; type casting
 *
 *    Revision 1.8  2004/10/22 22:51:26  bfo
 *    Most of the "pragma unused" eliminated
 *
 *    Revision 1.7  2003/05/17 10:50:26  bfo
 *    Some parts for carbon disabled
 *
 *    Revision 1.6  2002/10/27 23:08:08  bfo
 *    get_mem/release_mem no longer with param "mac_asHandle"
 *
 *
 */

#ifndef __CONSOLE__
#include <console.h>
#endif

#include "os9exec_incl.h"
#include "serialaccess.h"

#define STARTVAL -200

/* definitions for Multi-Strout support (bfo)   */
/* there are currently 50 terminal consoles and */
/* 50 (!) drawing windows available */
ttydev_typ mac_console[MAX_CONSOLE];

static void InitConsoles()
/* there are many different consoles supported now */
/* initialize them all */
{
    ttydev_typ *mco;
    int         k;

    for (k = 0; k < MAX_CONSOLE; k++) {
        mco                 = &mac_console[k];
        mco->installed      = false;
        mco->consoleTermPtr = nil;
        mco->inBufUsed      = 0;
        mco->holdScreen     = false;
        mco->spP            = NULL;
    }
} /* InitConsoles */

// module global variables...
int   toolBoxDone        = false;
int   gInBackground      = false;
int   gConsoleQuickInput = false;
short gConsoleEcho       = true; /* as it was standard in the original */
int   gConsoleSleep      = 1;    /* default was 5 */
int   gCnt               = 0;

short gConsoleNLExpand = true;
char  gTitle[OS9NAMELEN]; /* title for   /vmod output */

int          gLastwritten_pid = 0; /* last written character's process */
int          gConsoleID       = 0; /* global console ID */
syspath_typ *g_spP            = NULL;
ulong        gNetActive       = 0; /* speed-up for bg tasks if net activity */
ulong        gNetLast         = 0; /* speed-up for bg tasks if net activity */
Boolean      gDocDone         = false;

#include "ResourceConstants.h"

#define kDITop 0x0050
#define kDILeft 0x0070
#define kOSEvent app4Evt /* event used by MultiFinder */
#define kSuspendResumeMessage                                                  \
    1                 /* high byte of suspend/resume event message             \
                       */
#define kResumeMask 1 /* bit of message field for resume vs. suspend */
#define kMouseMovedMessage 0xFA /* high byte of mouse-moved event message */
#define kNoEvents 0             /* no events mask */
#define keyRETURN '\r'          /* ASCII code given when Return is pressed */

// forward declarations of functions within this module

static void DoSetupToolbox(void);
void        AdjustMenus();
int         CanCopy();
int         DoCopy();
int         CanPaste();
int         DoPaste();
int         DoEditConfig();
void        DoMenuCommand(long menuResult);
void        HandleKey(char key);
void        EventLoop();

short   InstallConsole(short fd);
void    RemoveConsole(void);
long    WriteCharsToConsole(char *buffer, long n);
long    ReadCharsFromConsole(char *buffer, long n);
Boolean DevReady(long *count);
ulong   SysPathWindow(syspath_typ *spP);
ulong   CurrentWindow(void);
void    UpdatePrms(ulong  wPtr,
                   ulong  wStore,
                   ulong  wSize,
                   ushort windex,
                   ushort wTot);
void    CheckInputBuffers(void);
Boolean KeyToBuffer(ttydev_typ *mco, char key);

//  Set up the tool box, if not done already...
static void DoSetupToolbox(void) { toolBoxDone = true; }

/*  Enable and disable menus based on the current state.
    The user can only select enabled menu items. We set up all the menu items
    before calling MenuSelect or MenuKey, since these are the only times that
    a menu item can be selected. Note that MenuSelect is also the only time
    the user will see menu items. This approach to deciding what enable/
    disable state a menu item has the advantage of concentrating all
    the decision-making in one routine, as opposed to being spread throughout
    the application. Other application designs may take a different approach
    that is just as valid. */

/* Define an event loop, so the user doesn't have to
 */
int HandleOneEvent(EventRecord *pEvent, int consoleSleep)
{
    //  RgnHandle      cursorRgn;
    Boolean     gotEvent;
    EventRecord event, *eventPtr;
    WindowPtr   window = NULL;
    int         k;
    short       part  = 0;
    Boolean     front = false;
    ulong       ctick = GetSystemTick();
    static int  hov   = STARTVAL;
    // static Boolean first= true;

    if (gNetActive > ctick) {
        if (gNetLast > 0 && gNetLast + TICKS_PER_SEC > ctick && hov < 0) {
            hov++;
            return gCnt; /* every nth only */
        }                // if

        hov      = STARTVAL;
        gNetLast = ctick;
    } // if

    // if we're not given an event, then wait for one
    if (!pEvent) {
        // LATER: check for WaitNextEvent trap; if unavailable, do something
        // else!
        // if (first)
        //     first= false; /* can be used as brkpnt place for ugly starter */

        gotEvent = WaitNextEvent(everyEvent, &event, consoleSleep, 0);
        eventPtr = &event;

        if (gotEvent)
            gCnt = 0;
        else
            gCnt++;
    }
    else {
        gotEvent = 1;
        eventPtr = pEvent;
    } // if

    //
    //  These are the dispatchers for the TermWindow stuff.
    //
    if (gDocDone) { /* if already initialized */
        for (k = 0; k < MAX_CONSOLE; k++) {
        } /* for */
    }     /* if */

    return gCnt;
} /* HandleOneEvent */

void HandleEvent(void) { HandleOneEvent(nil, gConsoleSleep); } /* HandleEvent */

static void HandleNthEvent()
{
    static int hvv = STARTVAL;

    if (hvv < 0)
        hvv++;
    else {
        hvv = STARTVAL;
        HandleEvent();
        arbitrate = true;
    }
} /* HandleNthEvent */

void EventLoop()
{
    while (true)
        HandleEvent();
} /* EventLoop */

/*
 *  The following four functions provide the UI for the console package.
 *  Users wishing to replace SIOUX with their own console package need
 *  only provide the four functions below in a library.
 */

/*
 *  extern short InstallConsole(short fd);
 *
 *  Installs the Console package, this function will be called right
 *  before any read or write to one of the standard streams.
 *
 *  short fd:       The stream which we are reading/writing to/from.
 *  returns short:  0 no error occurred, anything else error.
 */

/*
 *  extern void RemoveConsole(void);
 *
 *  Removes the console package.  It is called after all other streams
 *  are closed and exit functions (installed by either atexit or _atexit)
 *  have been called.  Since there is no way to recover from an error,
 *  this function doesn't need to return any.
 */

/*
 *  extern long WriteCharsToConsole(char *buffer, long n);
 *
 *  Writes a stream of output to the Console window.  This function is
 *  called by write.
 *
 *  char *buffer:   Pointer to the buffer to be written.
 *  long n:         The length of the buffer to be written.
 *  returns short:  Actual number of characters written to the stream,
 *                  -1 if an error occurred.
 */

Boolean DevReady(long *count)
/* true, if next character(s) can be read */
/* used by "ReadCharsFromConsole" and "GetStat.SS_Ready" */
{
    Boolean     ok;
    ttydev_typ *mco;

    HandleNthEvent();
    if (gConsoleID < 0)
        ok = false;

    else if (gConsoleID >= TTY_Base)
        ok = DevReadyTTY(count, gConsoleID);

    else {
        mco = &mac_console[gConsoleID];
        ok  = DevReadyTerminal(count, mco);
    }

    arbitrate = true;
    return ok;
} /* DevReady */

ulong SysPathWindow(syspath_typ *spP)
/* Get window pointer of a given syspath */
/* used for the "/vmod" system */
{
    ttydev_typ *mco;

    if (spP->type != fCons)
        return (ulong)NULL;

    mco = &mac_console[spP->term_id];
    return mco->consoleTermPtr;
} /* SysPathWindow */

ulong CurrentWindow(void)
/* Get window pointer of a given syspath */
/* used for the "/vmod" system */
{
    ttydev_typ *mco = &mac_console[gConsoleID];
    return mco->consoleTermPtr;
} /* CurrentWindow */

/*
 *  extern long ReadCharsFromConsole(char *buffer, long n);
 *
 *  Reads from the Console into a buffer.  This function is called by
 *  read.
 *
 *  char *buffer:   Pointer to the buffer which will recieve the input.
 *  long n:         The maximum amount of characters to be read (size of
 *                  buffer).
 *  returns short:  Actual number of characters read from the stream,
 *                  -1 if an error occurred.
 */

/*
 *  extern char *__ttyname(long fildes);
 *
 *  Return the name of the current terminal (only valid terminals are
 *  the standard stream (ie stdin, stdout, stderr).
 *
 *  long fildes:    The stream to query.
 *
 *  returns char*:  A pointer to static global data which contains a C string
 *                  or NULL if the stream is not valid.
 */

/* eof */
