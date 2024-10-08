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
/*         for MacOS, Windows and Linux       */
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
 *    Revision 1.105  2007/04/17 23:04:05  bfo
 *    - FD_ID (hash) fName always visible
 *    - renamed to "dirtable_entry"
 *
 *    Revision 1.104  2007/03/24 12:52:41  bfo
 *    - MAXDIR is now 65536
 *    - MAXDIRHIT = 60 and <hittable>, for "ihit" statistics
 *    - <readflag> for RW (fileaccess)
 *    - <svD_n> and <svD_dEnt> for fast directory access
 *    - <next> element for linked hash list
 *    - Improved TFS start conditions for directory searching
 *
 *    Revision 1.103  2007/03/10 13:52:30  bfo
 *    MPW adaptions
 *
 *    Revision 1.102  2007/03/10 13:37:40  bfo
 *    Changes really comitted
 *
 *    Revision 1.100  2007/02/24 14:13:44  bfo
 *    - New <direntry> struct with <dirid> and fName>
 *    - pWaitRead recognized for internal commands
 *
 *    Revision 1.99  2007/02/22 23:03:15  bfo
 *    - <call_Intercept> added
 *    - Alphabetic sorting
 *    - Parameters reordered
 *
 *    Revision 1.98  2007/02/14 20:59:01  bfo
 *    Search DLLs at PLUGINS and one level higher
 *
 *    Revision 1.97  2007/02/11 14:45:31  bfo
 *    <geCnt> added (MacOS9 reactiveness/idle load balance)
 *
 *    Revision 1.96  2007/02/04 20:05:33  bfo
 *    "DLL_Suffix", "NumberOfNativeProgs" and "ConnectDLL" moved to "intcommand"
 *
 *    Revision 1.95  2007/01/28 21:56:54  bfo
 *    - NATIVE_SUPPORT added
 *    - native interface definitions added
 *    - <with_dbgDLLs> added
 *    - 'init_plugins' and the full plugin support added
 *    - Callback routines for 'cclib' implemented here now
 *
 *    Revision 1.94  2007/01/07 13:40:59  bfo
 *    New <pmem> structure added
 *    Callback definitions for plugin DLLs added
 *    native/plugin additions
 *    prp_IProg/run_IProg DLL connection added
 *    call "Init_IProg"
 *
 *    Revision 1.93  2007/01/04 20:50:58  bfo
 *    <dllList> introduced
 *    DLL connection for plugins added (Plugin Loader)
 *    BootLoader beautified
 *    titles() rearranged
 *
 *    Revision 1.92  2007/01/02 11:12:43  bfo
 *    <includeList>/<excludeList> for PtoC added
 *    "systime" is part of the <excludeList> by default
 *    <ptocActive> default value false w/o PtoC support
 *    some formatting adaptions
 *
 *    Revision 1.91  2006/12/16 22:18:25  bfo
 *    cp->oerr assigned correctly for internal commands
 *
 *    Revision 1.90  2006/12/02 12:07:43  bfo
 *    global <lastsyscall> eliminated
 *
 *    Revision 1.89  2006/12/01 19:51:28  bfo
 *    <mnt_devCopy> added
 *
 *    Revision 1.88  2006/11/18 09:48:49  bfo
 *    <mnt_sctSize> and <mnt_cluSize> added
 *
 *    Revision 1.87  2006/11/12 13:22:30  bfo
 *    <catch_ctrlC> flag and support added /
 *    Boot search order description  added /
 *    "load_OS9Boot" call (sector 0 searching strategy) added
 *
 *    Revision 1.86  2006/11/05 00:09:20  bfo
 *    some legacy debug logger stuff eliminated
 *
 *    Revision 1.85  2006/11/04 01:14:31  bfo
 *    Catch BusErrors and divisions by 0 as well
 *
 *    Revision 1.84  2006/10/25 21:41:20  bfo
 *    div 0 will be masked with TRAP 5 instead of error 105
 *
 *    Revision 1.83  2006/10/25 20:36:44  bfo
 *    error dump ONLY with dbgAnomaly (which is on by default)
 *
 *    Revision 1.82  2006/10/25 19:35:46  bfo
 *    Use <dbgAnomaly> flag and call debug_procdump()
 *
 *    Revision 1.81  2006/10/22 17:26:19  bfo
 *    intUtil <cwti> handling improved
 *
 *    Revision 1.80  2006/10/15 13:15:54  bfo
 *    Up to date
 *
 *    Revision 1.79  2006/10/15 13:14:28  bfo
 *    slightly rearranged for debugging purposes
 *
 *    Revision 1.78  2006/10/13 10:26:03  MG
 *    Version changed to V3.34
 *    "debug_procdump" (bus error reporting) called (by Martin Gregorie)
 *
 *    Revision 1.77  2006/10/01 15:21:06  bfo
 *    <ptocMask> eliminated; some masklevel adaptions for PtoC handling
 *
 *    Revision 1.76  2006/09/08 21:56:06  bfo
 *    <dbgPath> added / <cwti_svd> added
 *
 *    Revision 1.75  2006/09/03 20:45:41  bfo
 *    "ftp put" hanger fixed: test cp->masklevel <= 0
 *
 *    Revision 1.74  2006/09/01 15:13:24  bfo
 *    Version changed to V3.33
 *
 *    Revision 1.73  2006/08/26 23:55:08  bfo
 *    A lot of old commented out stuff eliminated
 *
 *    Revision 1.72  2006/08/04 18:36:19  bfo
 *    Comment changes / Improved arbitration
 *
 *    Revision 1.71  2006/07/29 09:00:41  bfo
 *    arbitration for internal utilities corrected
 *
 *    Revision 1.70  2006/07/23 14:18:13  bfo
 *    allow intUtil to arbitrate with other programs now
 *
 *    Revision 1.69  2006/07/21 07:17:11  bfo
 *    Enhancements for intUtils at bus error (dumpregs, ...)
 *
 *    Revision 1.68  2006/07/14 11:49:37  bfo
 *    Arbitration for intUtil prepared and improved
 *
 *    Revision 1.67  2006/07/10 10:10:57  bfo
 *    os9exec_loop( unsigned short xErr, Boolean fromIntUtil ) added
 *    os9exec_loop can be called by intUtils now
 *    SEGV handler is common for Windows/Unix now
 *    Some old code removed (which was commented out already)
 *
 *    Revision 1.66  2006/07/03 13:37:32  bfo
 *    SEGV handler also re-entrant for Linux (use sigsetjmp/siglongjmp)
 *
 *    Revision 1.65  2006/07/02 13:46:15  bfo
 *    Name adaptions
 *
 *    Revision 1.64  2006/06/29 22:46:33  bfo
 *    void llm_os9_copyback( regs_type *rp ) introduced (for register dump)
 *
 *    Revision 1.63  2006/06/29 10:28:13  bfo
 *    SEGV handler for Windows is re-entrant now
 *
 *    Revision 1.62  2006/06/28 18:14:13  bfo
 *    SEGV handler introduced for Windows
 *
 *    Revision 1.61  2006/06/28 17:38:53  bfo
 *    shandler/setjmp commented things eliminated
 *
 *    Revision 1.60  2006/06/26 22:13:49  bfo
 *    SEGV handler introduced, which allows now to catch bus errors !
 *
 *    Revision 1.59  2006/06/18 14:38:55  bfo
 *    "Windows-PC" can't be used (.mgr_loop) => switch back to "Windows - PC"
 *    Display the Platform now at the title sequence
 *
 *    Revision 1.58  2006/06/17 11:09:07  bfo
 *    Global var <withTitle> introduced / tab source formatting
 *
 *    Revision 1.57  2006/06/16 22:33:37  bfo
 *    Early termination of 1st internal utilities
 *
 *    Revision 1.56  2006/06/11 22:04:33  bfo
 *    set_os9_state with 3rd param <callingProc>
 *
 *    Revision 1.55  2006/06/10 10:24:54  bfo
 *    Some isIntUtil debugging made invisible
 *
 *    Revision 1.54  2006/06/07 16:10:52  bfo
 *    "PowerMac/IntelMac XCode" added
 *
 *    Revision 1.53  2006/06/02 19:00:25  bfo
 *    g_ipAddr ulong => char*
 *
 *    Revision 1.52  2006/06/01 21:02:50  bfo
 *    g_ipAddr added / printf things commented out
 *
 *    Revision 1.51  2006/06/01 18:05:57  bfo
 *    differences in signedness (for gcc4.0) corrected
 *
 *    Revision 1.50  2006/05/19 17:58:19  bfo
 *    Changed to V3.32
 *
 *    Revision 1.49  2006/05/16 13:06:45  bfo
 *    Version changed to V3.31, def. extension for linux
 *
 *    Revision 1.48  2006/02/19 16:04:45  bfo
 *    ptoc vars and pthread support things added (some debug things are still
 * in)
 *
 *    Revision 1.47  2005/07/06 21:08:56  bfo
 *    defined UNIX
 *
 *    Revision 1.46  2005/07/02 14:22:50  bfo
 *    Adapted for Mach-O / Title output adapted (reduced)  for real situation
 *
 *    Revision 1.45  2005/06/30 11:51:48  bfo
 *    Mach-O support
 *
 *    Revision 1.44  2005/05/13 17:26:46  bfo
 *    Use <imgMode>
 *
 *    Revision 1.43  2005/04/17 15:15:03  bfo
 *    Change to V3.27
 *
 *    Revision 1.42  2005/04/15 11:51:45  bfo
 *    Reduced size of RBF images is supported now, 2005 adaption
 *
 *    Revision 1.41  2005/01/22 16:23:20  bfo
 *    Changed to Version V3.26
 *
 *    Revision 1.40  2004/12/04 00:04:05  bfo
 *    MacOSX MACH adaptions
 *
 *    Revision 1.39  2004/11/20 11:44:08  bfo
 *    Changed to version V3.25 (titles adapted)
 *
 *    Revision 1.38  2004/10/22 22:51:12  bfo
 *    Most of the "pragma unused" eliminated
 *
 *    Revision 1.37  2004/09/15 19:56:19  bfo
 *    changed to V3.24
 *
 *    Revision 1.36  2004/01/04 19:53:17  bfo
 *    Changed to new version
 *
 *    Revision 1.35  2003/05/17 10:29:19  bfo
 *    Include the "non-debug" flag (bit7 of path) for OS9_I_WritLn
 *
 *    Revision 1.34  2003/04/25 19:34:30  bfo
 *    MAXDIR/MAX_PATH now visible for Macintosh version
 *
 *    Revision 1.33  2003/04/20 23:06:58  bfo
 *    <my_inetaddr> big endian/little endiran swap
 *
 *    Revision 1.32  2003/01/10 21:37:00  bfo
 *    Changed to new version
 *
 *    Revision 1.31  2003/01/02 14:34:37  bfo
 *    Some type castinmg things fixed
 *
 *    Revision 1.30  2003/01/02 12:20:31  bfo
 *    RTE registers correctly saved and restored
 *
 *    Revision 1.29  2002/11/24 18:49:14  bfo
 *    Changed to new release V3.21
 *
 *    Revision 1.28  2002/11/06 20:12:26  bfo
 *    lastsignal->pd._signal/icptroutine->pd._sigvec (directly defined at pd
 * struct)
 *
 *    Revision 1.27  2002/10/27 23:41:51  bfo
 *    memory and module structure definitions adapted
 *
 *    Revision 1.26  2002/10/15 21:37:29  bfo
 *    Changed to V3.20
 *
 *    Revision 1.25  2002/10/15 18:23:43  bfo
 *    memtable included
 *
 *    Revision 1.24  2002/10/02 18:45:34  bfo
 *    OS9exec version changed to V3.19 / <mdirField> variable included
 *
 *    Revision 1.23  2002/09/14 23:11:39  bfo
 *    Adapted for V3.18
 *
 *    Revision 1.22  2002/09/11 17:32:38  bfo
 *    Up to date for V3.17
 *
 *    Revision 1.21  2002/09/04 14:09:45  bfo
 *    built-in "init" will be used, if no external module available
 *
 *    Revision 1.20  2002/08/13 21:55:46  bfo
 *    grp,usr and prior at the real prdsc now
 *
 *    Revision 1.19  2002/08/09 22:39:21  bfo
 *    New procedure set_os9_state introduced and adapted everywhere
 *
 *    Revision 1.18  2002/08/08 21:54:13  bfo
 *    F$SetSys extended with D_PrcDBT support
 *
 *    Revision 1.17  2002/08/06 21:03:14  bfo
 *    Version changed to V3.16
 *
 *    Revision 1.16  2002/08/06 07:59:45  luz
 *    Fixed "Retry/Abort" pop-up dialog
 *
 *    Revision 1.15  2002/07/30 21:30:15  bfo
 *    Updated for OS9EXEC V3.15
 *
 *    Revision 1.14  2002/07/30 16:46:14  bfo
 *    E-Mail adress beat.forster@ggaweb.ch is updated everywhere
 *
 *    Revision 1.13  2002/07/24 22:33:14  bfo
 *    Timer synchronisation enhanced
 *
 *    Revision 1.12  2002/07/23 19:38:34  bfo
 *    introduce <syCorr> variable
 *
 *    Revision 1.11  2002/07/21 14:35:55  bfo
 *    Synchronise the currentTick with seconds of time/date
 *
 *    Revision 1.10  2002/07/06 15:54:53  bfo
 *    mnt_ramSize included
 *
 *    Revision 1.9  2002/07/01 21:29:23  bfo
 *    Slightly adapted for Carbon -> It is running now correctly in Carbon
 * window. But still running in Mac OS9 environment. Don't know why.
 *
 *    Revision 1.8  2002/06/25 20:44:33  luz
 *    Added /lp printer support under windows. Not tested or even compiled for
 * Mac
 *
 */

/* Revision History
 * ================
 * 000  : 93-06-01:  Created
 * 001  : 93-06-26:  First working version (with icc6811, a6801 and xlink)
 * (-->PhilM, Simon Kaiser) 002  : 93-06-27:  Some minor improvements in file
 * handling and error reporting. Made I$ReadLn not return the CR char (this
 * seems to be what is expected, at least by xlib). 003  : 93-07-10:  added
 * spinning cursor, made IRQ proof (no masking any more), made GetStt/SS_Opt
 *                   work, made I$ReadLn/stdin work correctly, added F$Link,
 * F$UnLink, I$Write and I$Read. Added GetStt/SS_DevNm for stdpaths. 004  :
 * 93-07-15:  Added F$SRtMem. Obviously, making SS_Opt work in GetStt caused
 * some icc output routines behave differently and return blocks, which was
 * never the case before and caused crashes because E$UnkSvc was returned. Now
 * it seems to work. 005  : 93-07-20:  Added stderr. Added correct MPW exit
 * codes. Added atexit-cleanup mechanism. Added ResolvePath to path processing
 * to resolve aliases within OS-9 paths. 006  : 93-08-10:  Added CACHE_FLUSH
 * code to make it run on the 040 Added F$ID, F$SetSys, F$GPrDsc 007  :
 * 93-08-11:  Added constants for process ID etc. (MYPROCID...) 008  : 93-08-21:
 * Implemented F$TLink and Trap handler mechanisms. Improved parsepath to handle
 *                   /dd and parent dirs correctly. Implemented GetStt:SS_Size
 * and GetStt:SS_Pos. 009  : 93-08-23:  F$Time, F$Julian made work, F$CRC added
 * (Foese) 010  : 94-04-09:  Made os9exec return error code to main program
 * instead of exiting.
 *
 * 1.11 : 94-09-25:  Added environment variables (passing all MPW environment
 * variables with names starting with '@'. Started real VERSION/REVISION
 * numbering 1.12 : 94-10-11:  Added F_SRqCMem, SS_FD and improved seek to make
 * Ultra C work ok. 1.13 : 94-10-22:  Added real file mod/crea dates in SS_FD.
 * 1.14 : 94-10-23:  Added HoldMemory()/UnholdMemort() calls to
 * os9malloc()/os9free() for VM case.
 *
 * 2.00 : 96-08-27:  Started cooperative multitprocess version. First Demoes
 * issued at OS9 Conference '96 CONTINUED in separate History file!
 *
 * Note: VERSION/REVISION is in 'vers' ID=2 resource in os9exec_nt.r
 */

/* includes */
/* ======== */

#include <signal.h>
#include "os9exec_incl.h"
#include "os9exec_nt.h"

#ifdef PTOC_SUPPORT
#include "native_interface.h"
#endif

/* global variables */
/* ================ */

/* the "module directory" */
module_typ os9modules[MAXMODULES];
addrpair_typ init_module;
uint32_t   totalMem;
mdir_entry mdirField[MAXMODULES];

/* the system paths */
syspath_typ syspaths[MAXSYSPATHS];
ulong       syspth[MAXSYSPATHS];

/* the SCSI devices */
scsi_typ scsi[MAXSCSI];

/* definitions for TTY */
ttydev_typ ttydev[MAXTTYDEV];

/* the processes */
process_typ procs[MAXPROCESSES];
ulong       prDBT[MAXPROCESSES]; /* os9_long(process_typ*) */

/* the signal queue */
sig_typ sig_queue;

/* the alarms */
alarm_typ  alarms[MAXALARMS];
alarm_typ *alarm_queue[MAXALARMS];

/* the dir table */
dirtable_entry dirtable[MAXDIRS];
int            hittable[MAXDIRHIT];

/* the include/exclude/dll list for internal commands */
#if defined NATIVE_SUPPORT || defined PTOC_SUPPORT
addrpair_typ includeList[MAXLIST]; /* char * */
addrpair_typ excludeList[MAXLIST]; /* char * */
plug_typ     pluginList[MAXLIST];
callback_typ g_cb;
#endif

/* I/O device table */
/* not really used, avoid "devs" from crashing */
byte devs[0x0900];

/* the OS-9 statistics table */
st_typ statistics[MAX_OS9PROGS];

/* the file file mgrs routine table */
fmgr_typ fmgr_none, fmgr_cons, fmgr_nil, fmgr_scf, fmgr_file, fmgr_dir,
    fmgr_pipe, fmgr_pty, fmgr_rbf, fmgr_net, fmgr_printer;

/* the events */
event_typ events[MAXEVENTS];
uint32_t  newEventId = 0xffffffff; /* initial value */

short arbitrate;       // set if arbitrate() should switch away from one running
                       // process to next
ushort interactivepid; // process that will get keyboard abort signals

char     startPath[OS9PATHLEN]; /* start path */
char     strtUPath[OS9PATHLEN]; /* next higher than start path */
dir_type mdir;                  /* current module dir */

/* the default module load directory OS9MDIR */
char mdirPath[MAX_PATH]; /* current mdir path */

/* the windows console definitions */

ttydev_typ   main_mco;
short        gConsoleNLExpand   = true;
int          gConsoleID         = 0;
syspath_typ *g_spP              = NULL;
int          gLastwritten_pid   = 0;
int          gConsoleQuickInput = true;
char         gTitle[OS9NAMELEN];

// the currently executing process, MAXPROCESSES if none
ushort currentpid = MAXPROCESSES; // id of current process

/* flag which allows empty read functionality for multiple terminal support */
Boolean devIsReady = true; /* true if char has been read (multiple terminal) */

/* this avoids recursion problems */
Boolean in_recursion = false;

/* global settings */
int     dbgOut       = -1;
ushort  dbgPath      = 0;
int     without_pid  = 0;
int     justthis_pid = 0;
Boolean quitFlag     = false;
Boolean userOpt      = false;
Boolean catch_ctrlC  = true;

Boolean nativeActive = false;
Boolean pluginActive = false;

Boolean ptocThread = false;
Boolean fullArb    = false;
int     withTitle  = true;

Boolean logtiming      = true; /* syscall loging, used by int cmd "systime" */
Boolean logtiming_disp = false;
Boolean async_area     = false;
Boolean async_pending  = false;
short   defSCSIAdaptNo = -1; // unknown
short   defSCSIBusNo   = 0;  // first
l2_typ  l2;
ulong   my_inetaddr = 0x7F000001; /* loopback by default: 127.0.0.1 */

/* jump back environment for SEGV exceptions */
jmp_buf main_env;

/* tickCount at start of the program */
uint32_t startTick = 0;
uint32_t lastTick  = 0;
uint32_t syCorr    = 0;

uint32_t sec0;

/* error traceback */
ushort errpid; /* PID of process that generated that error */
// ushort lastsyscall;   /* last system call */
char *lastpathparsed; /* last pathstring parsed (NULL if none) */

/* the software's version and revision */
// ushort appl_version;
// ushort appl_revision;
ushort exec_version;
ushort exec_revision;

char *hw_site;
char *hw_name;
char *sw_name;
char *platform;

/* screen size */
uint32_t screenW  = -1; /* undefined */
uint32_t screenH  = -1;
char *g_ipAddr = NULL;

/* global options */
/* -------------- */

/* fork mode */
int dummyfork = 0; /* if set, forks are done as in os9exec 1.14 (only command
                      printout to stdout) */

/* Fetch FTP name translation */
int fetchnames; /* if set, OS9-filenames beginning with '.' will be translated
                   to begin with '/' on the mac */

/* internal commands */
Boolean with_intcmds = true;  // by default, internal commands will be used
Boolean with_dbgDLLs = false; // by default, '*_dbg' DLLs are not preferred

/* cursor spinning interval in 1/60 seconds */
uint32_t spininterval = DEFAULTSPININTERVAL;

/* mount device name and write protection   */
char   *mnt_name      = "";
int     mnt_ramSize   = 0;
int     mnt_sctSize   = 0;
int     mnt_cluSize   = 1;
char   *mnt_devCopy   = "";
int     mnt_scsiID    = NO_SCSI;
short   mnt_scsiAdapt = -1; // use default adaptor
short   mnt_scsiBus   = -1; // use default bus
short   mnt_scsiLUN   = -1; // use default LUN
Boolean mnt_wProtect  = false;
Boolean mnt_imgMode   = false;
/* ---------------------------------------- */

#ifdef THREAD_SUPPORT
pthread_mutex_t sysCallMutex;
#endif

/* Routines */
/* ======== */

/* prepare parameter area from argv/argc envp
 * Note: If routine exits w/o error, it has malloc()-ed a block at pap
 *       containing the parameter structure which must be freed afterwards.
 */
static os9err prepParams(mod_exec     *theModule,
                         char        **argv,
                         int           argc,
                         char        **envp,
                         uint32_t     *psiz,
                         addrpair_typ *param_ptr)
{
    uint32_t  paramsiz, argsiz, envsiz;
    int    os9envc;
    byte  *p, *pp, *hp;
    ulong *alp, *elp;
    char  *modnam;
    size_t h, k;

    debugprintf(dbgStartup,
                dbgDeep,
                ("# prepParams: argv=$%08lX, argc=%d, envp=$%08lX\n",
                 (ulong)argv,
                 argc,
                 (ulong)envp));
    /* -- initial preparations */
    modnam = Mod_Name(theModule);
    /* -- calculate parameter size */
    paramsiz = 2 /* 000D param delimiter */ + 1 /* env term */
               + 2 /* FC01 code */ + 4 /* argv[0] pointer */ +
               1 /* argv[0] term */
               + 2 /* 000D word */ + 4 /* argv[] list beginning */ +
               4 /* envp[] separator */
               + 4 /* envp[] terminator */;
    /* -- calculate argument string size */
    for (argsiz = 0, k = 0; k < argc; k++)
        argsiz += strlen(argv[k]) + 1; /* string length+space */
    if (argc != 0)
        argsiz--; /* last argument is not followed by a space. */
    if (argsiz & 1)
        paramsiz++; /* align needed if odd argsize */
    /* -- calculate environment string size */
    for (envsiz = 0, k = 0, os9envc = 0; envp[k] != NULL && *envp[k] != 0;
         k++) {
        if (*envp[k] == '@') {
            /* -- OS-9 environment variables must start with a '@' */
            h = strlen(envp[k]); /* env variable name */
            debugprintf(
                dbgStartup,
                dbgDeep,
                ("# prepParams: counting envp[%d]='%s', len=%d, reserved=%d\n",
                 k,
                 envp[k],
                 h,
                 h + strlen(envp[k] + h + 1) + 1));
            envsiz += h + strlen(envp[k] + h + 1) +
                      1; /* env variable contents plus one for the '=' */
            os9envc++;   /* count OS-9 environment variables */
        }
    }
    if (os9envc != 0)
        envsiz--; /* last env is not followed by a space. */
    if ((envsiz & 1) == 0)
        paramsiz++; /* another align needed if even envsize */
    debugprintf(dbgStartup,
                dbgDeep,
                ("# prepParams: os9envc=%d,envsiz=%d, argc=%d,argsiz=%d\n",
                 os9envc,
                 envsiz,
                 argc,
                 argsiz));
    if (!(strlen(modnam) & 1))
        paramsiz++; /* align if module name has even # of chars (=odd with NUL)
                     */
    paramsiz += argsiz + argc * 4 + envsiz + os9envc * 4 +
                strlen(modnam); /* add module name plus pointer spaces */
                                /* -- allocate buffer for it */

    *param_ptr = get_mem(paramsiz);
    pp         = param_ptr->host;
    if (pp == NULL)
        return os9error(E_NORAM);

    /* -- prepare parameters shell-like */
    debugprintf(dbgStartup,
                dbgDeep,
                ("# prepParams: Calculated parameter size=$%lX\n", paramsiz));
    p = pp + paramsiz; /* end of param area */
    p -= 4;
    *((ulong *)p) = 0; /* envp[] terminator */
    p -= os9envc * 4;  /* reserve room for envp[] pointers */
    elp = (ulong *)p;  /* save ptr to first envp pointer */
    p -= 4;
    *((ulong *)p) = 0; /* argv[]/envp[] separator */
    p -= argc * 4;     /* reserve room for argv[] pointers */
    alp = (ulong *)p;  /* save ptr to first argv pointer */
    p -= 4;
    *((ulong *)p) = 0; /* argv[] list beginning */
    p -= 2;
    *((ushort *)p) = os9_word(0x000D);
    k              = strlen(modnam);
    if (!(k & 1))
        *(--p) =
            0; /* align if module name has even # of chars (=odd with NUL) */
    p -= k + 1;
    strcpy((char *)p, modnam); /* copy module name as argv[0] */
    p -= 4;
    *((ulong *)p) = os9_long(p + 4 - pp); /* set argv[0] offset */
    p -= 2;
    *((ushort *)p) = os9_word(0xFC01); /* special sync code */
    /* --- environment variable strings */
    if ((envsiz & 1) == 0)
        *(--p) = 0; /* align needed if even envsize */
    p -= 1;
    *p = 0;      /* environment variables terminator */
    p -= envsiz; /* reserve space for environment strings */
    hp = p;
    debugprintf(
        dbgStartup,
        dbgDeep,
        ("# prepParams: Starting to write envs at $%08lX, memstart=$%08lX\n",
         (ulong)hp,
         (ulong)pp));
    k = 0;
    while (os9envc) {
        if (*envp[k] == '@') {
            /* --- it is an OS-9 environment variable */
            *(elp++) = os9_long((ulong)hp - (ulong)pp); /* set offset */
            strcpy((char *)hp, envp[k] + 1); /* copy the environment variable
                                                name, but without the '@' */
            h = strlen(envp[k]) - 1; /* size of variable name without '@' */
            debugprintf(dbgStartup,
                        dbgDeep,
                        ("# prepParams: envp[%d] name='%s', len=%d",
                         k,
                         envp[k] + 1,
                         h));
            hp += h;     /* advance pointer */
            *hp++ = '='; /* insert '=' between name and value */
            strcpy((char *)hp,
                   envp[k] + h + 2);     /* copy contents of the variable */
            h = strlen(envp[k] + h + 2); /* size of contents */
            debugprintf(dbgStartup,
                        dbgDeep,
                        (", contents='%s', len=%d\n", hp, h));
            hp += h; /* advance pointer */
            os9envc--;
            if (os9envc == 0)
                break;
            *(hp++) = ' '; /* add space, if more vars follow */
        }
        k++;
    }
    /* --- argument strings */
    if (argsiz & 1)
        *(--p) = 0; /* align needed if odd argsize */
    p -= 2;
    *((ushort *)p) = os9_word(0x000D); /* command line parameter terminator */
    p -= argsiz;                       /* reserve space for argument strings */
    debugprintf(
        dbgStartup,
        dbgDeep,
        ("# prepParams: Starting to write args at $%08lX, memstart=$%08lX\n",
         (ulong)p,
         (ulong)pp));
    k = 0;
    while (argc) {
        *(alp++) = os9_long((ulong)p - (ulong)pp); /* set offset */
        strcpy((char *)p, argv[k]);                /* copy the argument */
        p += strlen(argv[k]);                      /* advance pointer */
        if (k + 1 >= argc)
            break;
        *(p++) = ' '; /* add space */
        k++;
    }
    *psiz = paramsiz; /* return parameter size */
    return 0;
}

/* prepare first OS9 process to launch from MPW command line
 * Note: - allocates process descriptor and sets global currentpid
 *       - may only be called once by the MPW tool, as it assumes
 *         that no process is running yet.
 */
static os9err prepLaunch(char  *toolname,
                         char **argv,
                         int    argc,
                         char **envp,
                         uint32_t memplus,
                         ushort prior)
{
    uint32_t psiz;
    addrpair_typ pap;
    os9err err;
    ushort newpid;
    ushort mid;
    ushort numpaths;

    numpaths = 1; /* use only 1 path at the beginning with TERMINAL_CONSOLE */

    /* father of first process is now 0 (bfo) */
    err = new_process(0, &newpid, numpaths);
    if (err)
        return err;
    err = link_load(newpid, toolname, &mid);
    if (err)
        return err;

    err = prepParams(os9mod(mid).host, argv, argc, envp, &psiz, &pap);
    if (err)
        return err;

    /* group/user 0.0 are set for the first process */
    err = prepFork(newpid,
                   toolname,
                   mid,
                   pap.guest,
                   psiz,
                   memplus,
                   numpaths,
                   0,
                   0,
                   prior);
    release_mem(pap); /* return arg buffer anyway, alloc as pointer */

    if (!err) {              /* make this process ready to execute */
        currentpid = newpid; /* make this process current */
        set_os9_state(currentpid, pActive, "prepLaunch"); /* now active */
    }

    return err;
}

/* the os9exec kernel */
/* ================== */

/*typedef void (*SignalHandler)(int);*/

/* clean up exit-handler */
static void cleanup(void)
{
    fflush(stdout);

    if (debugcheck(dbgAllInfo, dbgNorm)) {
        /* --- avoid endless messages only for cleanup */
        debug[dbgNorm] = debug[dbgNorm] & 0xfdff; /* without error display */
    }

    if (debugcheck(dbgAllInfo, dbgDetail)) {
        /* --- avoid endless messages only for cleanup */
        if (debugcheck(dbgAllInfo, dbgNorm)) {
            uphe_printf("cleanup: debug reset to debugAnomaly/dbgNorm (use [x] "
                        "to keep!)\n");
            debug_halt(dbgAllInfo);
            debug[dbgDeep]   = 0;
            debug[dbgDetail] = 0;
            debug[dbgNorm]   = 0;
        }
    }

    debugprintf(dbgStartup, dbgNorm, ("# cleanup: killing processes\n"));
    kill_processes(); /* kill all "running" processes */

    debugprintf(dbgStartup, dbgNorm, ("# cleanup: closing syspaths\n"));
    close_syspaths(); /* make sure all paths are closed */

    debugprintf(dbgStartup, dbgNorm, ("# cleanup: freeing modules\n"));
    free_modules(); /* unlink the OS9 module resources */
    fflush(stdout);

    debugprintf(dbgStartup,
                dbgNorm,
                ("# cleanup: releasing low-level magic\n"));
    lowlevel_release(); /* release low-level stuff */
}

/* must be done before linking "init" module */
/* and this will be done earlier now */
void get_hw()
{
    hw_site  = "PC";
    hw_name  = "Linux - PC";
    platform = "x86";
}

// Get the start directory
void StartDir(char *pathname)
{
    char        acc[OS9PATHLEN];
    char        sv[OS9PATHLEN];
    char        result[OS9PATHLEN];
    struct stat info;

    strcpy(pathname, ""); // start with an empty string
    strcpy(acc, ".");     // and take current dir for start
    strcpy(result, "");

    while (true) {
        stat_(acc, &info);
        strcat(acc, "/..");
        DirName(acc, info.st_ino, (char *)&result, true);
        if (strcmp(result, ".") == 0) {
            strcpy(result, "");
            break;
        }

        strcpy(sv, pathname);
        strcpy(pathname, PATHDELIM_STR);
        strcat(pathname, result);
        strcat(pathname, sv);
    }
} // StartDir

static os9err GetStartPath(char *pathname)
{
    StartDir(pathname);

    return 0;
}

// Search one path up ( .. )
static void PathUp(char *p)
{
    char *q = p + strlen(p) - 2;
    while (q > p) {
        if (*q == PATHDELIM) {

            *q = NUL;
            break;
        }

        q--;
    }
}

static void
GetCurPaths(char *envname, ushort mode, dir_type *drP, Boolean recursive)
{
    os9err  err;
    char    tmp[OS9PATHLEN];
    Boolean doRep = false;

    char *p = egetenv(envname); /* get path for default module loading dir */
    if (p == NULL)
        return;

    strcpy(tmp, p);
    p = tmp; /* make a local copy */
    MakeOS9Path(p);

    drP->type = IO_Type(1, p, mode); // get device type: Mac/PC or RBF
    if (drP->type == fRBF) {
        err = change_dir(1, drP->type, p, mode); // set the types at procid 0
        return;
    }

    /*
        drP->type= IO_Type( 1,           p,mode ); // get device type: Mac/PC or
    RBF if (drP->type==fRBF || drP->type==fDir) { err=      change_dir(
    1,drP->type, p,mode ); // set the types at procid 0 if (drP->type==fRBF)
    return; }
        */

    if (tmp != p)
        strcpy(tmp, p);

    p = tmp;

    if (doRep) {
        while (*p != NUL) { /* replace slashes -> PATHDELIM */
            if (*p == PSEP)
                *p = PATHDELIM;
            p++;
        }

        p = tmp;
    }

    /* do this before the recursive loop */
    strncpy(drP->path, p, OS9PATHLEN);
}

#if defined NATIVE_SUPPORT || defined PTOC_SUPPORT
// Test, if upshifted string <str> begins with <cmp>
static Boolean SameBegin(const char *str, const char *cmp)
{
    size_t sLen = strlen(str);
    size_t cLen = strlen(cmp);
    if (sLen < cLen)
        return false; // too short ?

    return ustrncmp(str, cmp, cLen) == 0;
} // SameBegin

// Test, if upshifted string <str> ends with <cmp>
static Boolean SameEnd(const char *str, const char *cmp)
{
    size_t sLen = strlen(str);
    size_t cLen = strlen(cmp);
    if (sLen < cLen)
        return false; // too short ?

    str += sLen - cLen; // compare the ends only
    return ustrncmp(str, cmp, cLen) == 0;
} // SameEnd

// Test, if upshifted string <str> ends with "_dbg"<suff>
static Boolean DbgSuff(const char *str, const char *suff)
{
    char dbEnd[OS9PATHLEN];
    strcpy(dbEnd, "_dbg");
    strcat(dbEnd, suff);

    return SameEnd(str, dbEnd);
} // SameEnd

Boolean OtherThere(const char *str, const char *suff)
{
    char      nm[OS9PATHLEN];
    size_t       len = strlen(str) - strlen(suff);
    int       i;
    plug_typ *p;

    Boolean cond = DbgSuff(str, suff);
    if (cond)
        len -= strlen("_dbg");

    strncpy(nm, str, len);
    nm[len] = NUL;
    if (!cond)
        strcat(nm, "_dbg");
    strcat(nm, suff);

    // upe_printf( "nm='%s' %d %d\n", nm, len, cond );

    for (i = 0; i < MAXLIST; i++) {
        p = &pluginList[i];
        if (p->name.host == NULL)
            break;

        if (ustrcmp(p->name.host, nm) == 0)
            return true;
    }

    return false;
} // OtherThere

// Search for all DLLs at the "PLUGINS" subdirectory
static os9err SearchDLLs(ushort      pid,
                         int        *i,
                         Boolean     withDLLs,
                         const char *dirName,
                         Boolean     atPluginDir,
                         Boolean     avoidDup)
{
    os9err          err, cErr;
    ushort          path;
    uint32_t        size;
    process_typ    *cp = &procs[pid];
    char            pathName[OS9PATHLEN];
    os9direntry_typ dEnt;
    addrpair_typ   *m;
    Boolean         cond;

    const char *suff = DLL_Suffix();
    if (strcmp(suff, "") == 0)
        return 0; // no error, if not supported

    strcpy(pathName, dirName);
    MakeOS9Path(pathName);

    if (atPluginDir)
        strcat(pathName, "/PLUGINS"); // sub directory where "dd" has been found

    err = usrpath_open(pid, &path, fDir, pathName, 0x81);
    debugprintf(dbgFiles, dbgNorm, ("# DLL path='%s' err=%d\n", pathName, err));
    if (err)
        return err;
    // upe_printf( "ePATH='%s'\n", pathName );

    while (true) {
        size = sizeof(dEnt);
        err  = usrpath_read(pid, path, &size, &dEnt, false);
        if (err)
            break;
        LastCh_Bit7(dEnt.name, false);
        // upe_printf( "eName='%s'\n", dEnt.name );
        cond = DbgSuff(dEnt.name, suff);
        if (!withDLLs)
            cond = !cond;

        if (cond && !SameBegin(dEnt.name, "._") && // begins with "._" ?
            SameEnd(dEnt.name, suff)) {
            if (avoidDup && OtherThere(dEnt.name, suff))
                continue;

            // upe_printf( "hello='%s'\n", dEnt.name );
            m  = &pluginList[*i].name;
            *m = get_mem((uint32_t)strlen(dEnt.name) + 1);
            strcpy(m->host, dEnt.name);
            (*i)++;
        }
    }
    if (err == E_EOF)
        err = 0; // just end of file

    cErr = usrpath_close(pid, path);
    if (!err)
        err = cErr;

    return err;
} // SearchDLLs

// Initialize include/exclude list
static void Init_NativeProgs(plug_typ *p)
{
    char iName[OS9NAMELEN];
    char iOpt[OS9NAMELEN];

    int i = 0;
    while (p->next_NativeProg(&i, (char *)&iName, (char *)&iOpt)) {
        if (strcmp(iOpt, "+") == 0 || strcmp(iOpt, "I+") == 0)
            ChangeNativeElement(iName, true);
        if (strcmp(iOpt, "-") == 0 || strcmp(iOpt, "I-") == 0)
            ChangeNativeElement(iName, false);
    }
} // Init_NativeProgs

static void DispList(int last)
{
    int       i;
    plug_typ *p;

    upe_printf("last=%d\n", last);
    for (i = 0; i <= last; i++) {
        p = &pluginList[i];
        if (p->name.host == NULL)
            continue;

        upe_printf("%d '%s' (n=%d)\n", i, p->name, p->numNativeProgs);
    }
} // DispList

static void SwapElements(int i, int j)
{
    plug_typ tmp;

    if (j <= i)
        return;

    MoveBlk(&tmp, &pluginList[j], sizeof(plug_typ));

    while (j > i) {
        MoveBlk(&pluginList[j], &pluginList[j - 1], sizeof(plug_typ));
        j--;
    }

    MoveBlk(&pluginList[j], &tmp, sizeof(plug_typ));
} // SwapElements

static int PLast(void)
{
    plug_typ *p;

    int last = MAXLIST - 1;
    while (last >= 0) {
        p = &pluginList[last];
        if (p->name.host != NULL)
            break;
        last--;
    }

    return last;
} // PLast;

static void SortedPluginList(void)
{
    // close potential gap first
    plug_typ *p;
    plug_typ *q;
    int       last, i, j;

    // Get the ones with names first
    last = PLast();
    for (i = 0; i <= last; i++) {
        p = &pluginList[i];
        if (p->name.host == NULL) {
            for (j = i + 1; j <= last; j++) {
                q = &pluginList[j];
                if (q->name.host != NULL) {
                    MoveBlk(p, q, sizeof(plug_typ));
                    q->name.host  = NULL;
                    q->name.guest = 0;
                    break;
                }
            }
        }
    }

    // bubble sort
    last = PLast();
    for (i = 0; i <= last; i++) {
        for (j = i + 1; j <= last; j++) {
            p = &pluginList[i];
            q = &pluginList[j];
            if (strcmp(p->name.host, q->name.host) > 0)
                SwapElements(i, j);
        }
    }

    i = 0;
    while (i <= last) {
        p = &pluginList[i];
        if (p->numNativeProgs > 1)
            break;
        i++;
    }

    // Get the ones with just 1 program first
    j = i + 1;
    while (j <= last) {
        p = &pluginList[j];
        if (p->numNativeProgs == 1) {
            // upe_printf( "swap %d <=> %d\n", i, j );
            SwapElements(i, j);

            i++;
            while (i <= last) {
                p = &pluginList[i];
                if (p->numNativeProgs > 1)
                    break;
                i++;
            }

            j = i;
        }

        j++;
    }
} // SortedPluginList

// Main routine for connecting plugin DLLs
static void PluginLoader(ushort cpid)
{
    os9err    err = 0;
    plug_typ *p;
    int       i;

    debugprintf(dbgStartup, dbgNorm, ("# Plugin Loader\n"));
    debugprintf(dbgStartup, dbgNorm, ("# startPath='%s'\n", startPath));
    debugprintf(dbgStartup, dbgNorm, ("# strtUPath='%s'\n", strtUPath));

    /*                      i= 0;
    err= SearchDLLs( cpid, &i,  with_dbgDLLs, true,  false ); if (err) return;
    err= SearchDLLs( cpid, &i,  with_dbgDLLs, false, false ); if (err) return;
    err= SearchDLLs( cpid, &i, !with_dbgDLLs, true,  true  ); if (err) return;
    err= SearchDLLs( cpid, &i, !with_dbgDLLs, false, true  ); if (err) return;
    */

    i = 0;
    SearchDLLs(cpid,
               &i,
               with_dbgDLLs,
               startPath,
               true,
               false); // don't break on error
    SearchDLLs(cpid, &i, with_dbgDLLs, strtUPath, true, false);
    SearchDLLs(cpid, &i, with_dbgDLLs, startPath, false, false);
    SearchDLLs(cpid, &i, with_dbgDLLs, strtUPath, false, false);

    SearchDLLs(cpid, &i, !with_dbgDLLs, startPath, true, true);
    SearchDLLs(cpid, &i, !with_dbgDLLs, strtUPath, true, true);
    SearchDLLs(cpid, &i, !with_dbgDLLs, startPath, false, true);
    SearchDLLs(cpid, &i, !with_dbgDLLs, strtUPath, false, true);

    for (i = 0; i < MAXLIST; i++) {
        p = &pluginList[i];
        if (p->name.host == NULL)
            break;

        err = ConnectDLL(p);
        if (!err && p->numNativeProgs > 0)
            continue; // at least one native prog must be there

        release_mem(p->name);
        p->name.host  = NULL;
        p->name.guest = 0;
    }

    SortedPluginList();

// for built-in only
#ifdef PTOC_SUPPORT
    i = 0;
    while (i < MAXLIST) {
        p = &pluginList[i]; // connect the built-in module as the last
        if (p->name == NULL) {
            p->name             = "";
            p->pVersion         = lVersion();
            p->pEnabled         = true;
            p->pDisabled        = false;
            p->next_NativeProg  = Next_NativeProg; // do this before counting
            p->numNativeProgs   = NumberOfNativeProgs(p);
            p->is_NativeProg    = Is_NativeProg;
            p->start_NativeProg = Start_NativeProg;
            p->call_Intercept   = Call_Intercept;
            break;
        }

        i++;
    }
#endif

    for (i = 0; i < MAXLIST; i++) {
        p = &pluginList[i];
        if (p->name.host == NULL)
            break;
        Init_NativeProgs(p);
    }

    pluginActive =
        Plugin_Possible(true); // built-in will not be taken in account
    display_pluginList(withTitle, true);

    debugprintf(dbgStartup, dbgNorm, ("# Plugin Loader done\n"));
} // PluginLoader
#endif

static void BootLoader(ushort cpid)
/* Search order:
 *
 * 1)  "OS9exec" built-in version
 * 2)  "init"    at "OS9MDIR"
 * 3)  "init"    at startup device
 * 4)  <OS9Boot> at startup device (according to sector 0 position)
 * 5)  "OS9Boot" at "OS9MDIR"
 * 6)  "OS9Boot" at startup device
 * 7)  "init"    built-in version
 * 8)  "socket"  built-in version
 *
 */
{
    char        *BLTxt = "# Boot Loader: '%s' err=%d\n";
    os9err       err;
    char        *p;
    ushort       mid;
    process_typ *cp = &procs[cpid];

    p   = "OS9exec";
    err = link_module(cpid, p, &mid); // do not abort on error
    debugprintf(dbgStartup, dbgNorm, (BLTxt, p, err));

    p   = "init";
    err = load_module(cpid, p, &mid, false); // do not abort on error
    debugprintf(dbgStartup, dbgNorm, (BLTxt, p, err));

    if (cp->d.type == fRBF) {
        err = load_OS9Boot(cpid);
    }

    if (err) {
        p   = "OS9Boot";
        err = load_module(cpid, p, &mid, false);
        debugprintf(dbgStartup, dbgNorm, (BLTxt, p, err));
    }

    if (err) {
        p   = "init";
        err = link_module(cpid, p, &mid);
        debugprintf(dbgStartup, dbgNorm, (BLTxt, p, err));
    }

    p   = "socket";
    err = link_module(cpid, p, &mid);
    debugprintf(dbgStartup, dbgNorm, (BLTxt, p, err));
}

static void CheckStartup(int cpid, char *toolname, int *argc, char **argv)
/* Check if "startup" file is available */
{
    os9err    err;
    ptype_typ type;
    ushort    path; /* be careful: not an "int" */
    char     *p = argv[0];

    if (toolname == NULL)
        return;
    debugprintf(dbgStartup,
                dbgNorm,
                ("# CheckStartup: '%s' '%s'\n", toolname, p));

    /* just in case a "startup" file is there: execute it */
    if (*argc == 1 && (ustrcmp(toolname, "shell") == 0) &&
        (ustrcmp(p, "startup") == 0)) {
        type = IO_Type(cpid, p, 0x01);
        err  = usrpath_open(cpid, &path, type, p, 0x01);
        if (err) {
            *argc = 0;
            return;
        }
        err      = usrpath_close(cpid, path);
        quitFlag = true; /* in case of startup sequence, quit automatically */
    }
}

static void CtrlC_Handler(int sig)
{
    Boolean fnd = false;

    fnd = (main_mco.spP->lastwritten_pid != 0);
    KeyToBuffer(&main_mco, CtrlC);

    // printf("Ctrl-C Addr: %d %d fnd=%s\n", sig, main_mco.spP->lastwritten_pid,
    // fnd ? "TRUE":"FALSE" ); fflush(0);
} // CtrlC_Handler

static void titles(void)
{
    /* - Apperance (CTB-terminal, Win32 or Linux Console or MPW tool) */
    upho_printf("Platform: '%s' (%s)\n", hw_name, platform);

#ifdef TERMINAL_CONSOLE
    upho_printf("- Linux XTerm Version\n");

#else
    upho_printf("- Macintosh programmer's Workshop (MPW) Tool Version \n");
#endif /* terminal */

    /* - what kind of 68k */
    upho_printf("- Using 68k-Emulator from UAE - Un*x Amiga Emulator (c) 1995 "
                "B. Schmidt\n");

    fflush(stderr);
    fflush(stdout);
}

static Boolean TCALL_or_Exception(process_typ *cp, regs_type *crp, ushort cpid)
/* Exception or trap handler call */
{
#define ZERO_DIVISION_TRAP 5
    ushort           vect;
    traphandler_typ *tp;
    short            parentid;

    if (cp->vector == 0xFAFA) { /* error exception */
        vect = cp->func >> 2;   /* vector number (=offset div 4 !) */
        if (debugcheck(dbgAnomaly, dbgNorm)) {
            if (vect != ZERO_DIVISION_TRAP) {
                // uphe_printf("main loop: Exception occurred [pid=%d] ! Vector
                // offset=$%04X (num=%d)\n", cpid,cp->func,vect);
                //  if (!cp->isIntUtil) dumpregs(cpid);

                // debug_procdump(cp, cpid);
            }
        }

        if ((vect >= FIRSTEXCEPTION) &&
            (vect < FIRSTEXCEPTION + NUMEXCEPTIONS)) {
            if (cp->ErrorTraps[vect - 2].handleraddr != 0) {
                /* there is an installed handler */

                /* set handler routine address */
                crp->pc = cp->ErrorTraps[vect - FIRSTEXCEPTION].handleraddr;

                /* set A6 base */
                crp->regs[REGS_A + 6] = cp->memstart + 0x8000;

                if (cp->ErrorTraps[vect - FIRSTEXCEPTION].handlerstack != 0) {
                    crp->regs[REGS_A + 7] =
                        cp->ErrorTraps[vect - FIRSTEXCEPTION].handlerstack;
                }
                if (debugcheck(dbgTrapHandler, dbgDetail)) {
                    upe_printf(
                        "Calling handler at $%08lX, stack ptr=$%08lX\n",
                        cp->ErrorTraps[vect - FIRSTEXCEPTION].handleraddr,
                        cp->ErrorTraps[vect - FIRSTEXCEPTION].handlerstack);
                    debug_halt(dbgTrapHandler);
                }

                /* next call to os9_go will execute the handler routine */
                return false; /* avoid any calls to arbitrate etc. */
            }
            else
                cp->func = 0; /* no handler installed, fatal */
        }
        else
            cp->func = 0; /* fatal exception, aborts process */

        if (cp->func == 0) {
            /* process does not handle exception, kill it */
            if (cp->isIntUtil) { // in case of built-in command, the parent must
                                 // be activated again
                parentid = os9_word(cp->pd._pid);
                debugprintf(
                    dbgTrapHandler,
                    dbgNorm,
                    ("# main loop: [pid=%d] intUtil parent=%d reactivated\n",
                     cpid,
                     parentid));
                set_os9_state(parentid,
                              pActive,
                              "IntCmd (excp)"); // make it active again
            }

            debugprintf(dbgTrapHandler,
                        dbgNorm,
                        ("# main loop: [pid=%d] ready to kill\n", cpid));
            cp->exiterr = vect - FIRSTEXCEPTION + E_BUSERR; /* set exit code */
            //            if (!debugcheck(dbgAnomaly,dbgNorm)) /* dump proc if
            //            not debugging */
            debug_procdump(cp, cpid);
            kill_process(cpid); /* kill the process, change currentpid */
            /* show exception */
            debugprintf(dbgTrapHandler,
                        dbgNorm,
                        ("# main loop: Process pid=%d aborted: Exception "
                         "vector=$%02X, err=#%03d\n",
                         cpid,
                         vect,
                         cp->exiterr));
            debug_halt(dbgTrapHandler);
            /* will continue with another process */
        }
    }
    else { /* --- Trap handler called (but could not be handled by low level
              code!) */
        debugprintf(dbgTrapHandler,
                    dbgNorm,
                    ("# main loop: Pid=%d called uninitialized TRAP Vector "
                     "$%04X (User Trap #%d), function code=$%04X\n",
                     cpid,
                     cp->vector,
                     cp->vector - FIRSTTRAP + 1,
                     cp->func));

        tp = &cp->TrapHandlers[cp->vector - FIRSTTRAP];
        if (tp->trapmodule.host == NULL) {
            /* --- called uninitialized TRAP, check for TrapInit entry */

            /* get module pointer */
            addrpair_typ module = get_module_ptr(cp->mid);
            mod_exec *module_host = module.host;

            if (module_host->_mexcpt != 0) {
                /* there is an exception entry point, call it */
                /* set new PC into exception handling routine */
                crp->pc = module.guest + os9_long(module_host->_mexcpt);

                debugprintf(dbgTrapHandler,
                            dbgNorm,
                            ("# main loop: Calling exception entry point of "
                             "pid=%s's main module\n",
                             cpid));
            }
            else { /* no exception entry point: Abort process */
                cp->exiterr = os9error(E_ITRAP); /* set exit code */
                kill_process(cpid); /* kill the process, change currentpid */
                /* show exception */
                debugprintf(dbgAnomaly + dbgTrapHandler,
                            dbgNorm,
                            ("# main loop: Process pid=%d aborted due to "
                             "unintialized User Trap #%d, err=#%03d\n",
                             cpid,
                             cp->vector - FIRSTTRAP + 1,
                             cp->exiterr));
                /* will continue with another process */
            }
        }
        else {
            /* --- trap handler is installed for this vector, call it */

            /* continue in trap handler */
            crp->pc   = tp->trapentry;

            /* pass trap handler data pointer with offset */
            crp->regs[REGS_A + 6] = tp->trapmem.guest + 0x8000;

            debugprintf(dbgTrapHandler,
                        dbgDeep,
                        ("# main loop: calling Trap function now"));

            /* execution continues in trap handler and will return to program
             * within OS-9 context */

            /* avoid any calls to arbitrate etc. */
            return false;
        }
    }

    return true;
}

static void debug_retsystask(regs_type *crp, ushort cpid)
{
    process_typ              *cp   = &procs[cpid];
    const funcdispatch_entry *fdeP = getfuncentry(cp->lastsyscall);
    char                     *errnam;
    char                     *errdesc;

    uphe_printf("<<< Pid=%02d: OS9 %s returns (from sysTask!): ",
                cpid,
                fdeP->name);

    if (cp->oerr) {
        get_error_strings(cp->oerr, &errnam, &errdesc);
        upe_printf("#%03d:%03d - %s\n", cp->oerr >> 8, cp->oerr & 0xFF, errnam);
    }
    else {
        show_maskedregs(crp, fdeP->outregs);
        upe_printf("\n");
    }
}

// ---- Callback routines for "cclib" ----------------------------------
// missing "atoi"/"atof" operations for "cclib", placed here
int StrToShort(short *i, const char *str);
int StrToFloat(float *f, const char *str);
int StrToDouble(double *d, const char *str);

static Boolean IsZeroI(const char *s)
{
    if (*s == NUL)
        return false;
    while (*s != NUL) {
        if (*s < '0' && *s > '9' && *s != '-' && *s != '+')
            return false;
        s++;
    }

    return true;
} // IsZeroI

static Boolean IsZeroR(const char *s)
{
    if (*s == NUL)
        return false;
    while (*s != NUL) {
        if (*s < '0' && *s > '9' && *s != '.' && *s != 'E' && *s != '-' &&
            *s != '+')
            return false;
        s++;
    }

    return true;
} // IsZeroR

int StrToShort(short *i, const char *str)
{
    *i = atoi(str);
    return *i == 0 && !IsZeroI(str);
} // StrToShort

int StrToFloat(float *f, const char *str)
{
    *f = atof(str);
    return *f == 0 && !IsZeroR(str);
} // StrToFlat

int StrToDouble(double *d, const char *str)
{
    *d = atof(str);
    return *d == 0 && !IsZeroR(str);
} // StrToDouble

// ---------------------------------------------------------------------
static void init_plugins(void)
{
#if defined NATIVE_SUPPORT || defined PTOC_SUPPORT
    for (int i = 0; i < MAXLIST; i++) {
        includeList[i].host  = NULL;
        includeList[i].guest = 0;

        excludeList[i].host  = NULL;
        excludeList[i].guest = 0;

        pluginList[i].name.host      = NULL;
        pluginList[i].name.guest     = 0;
        pluginList[i].numNativeProgs = 0;
        pluginList[i].pEnabled       = false;
        pluginList[i].pDisabled      = false;
    }

    nativeActive = true; // Native programs are possible

    g_cb.trap0       = trap0_call; // once would be sufficient
    g_cb.strToShort  = StrToShort;
    g_cb.strToFloat  = StrToFloat;
    g_cb.strToDouble = StrToDouble;
#endif
} // init_plugins

void os9exec_globinit(void)
{
    /* --- global environment initialisation --- */
    init_all_mem();   // initialize memory handling
    init_fmgrs();     // initialize all the file manager objects
    init_processes(); // no processes yet
    init_modules();   // clear module handle list
    init_events();    // init OS-9 events
    init_alarms();    // init OS-9 alarms
    init_syspaths();  // prepare system paths, ready for usrpath print now
    init_L2();        // prepare the /L2 stuff
    init_plugins();   // prepare plugin DLL stuff
}

// This is the main loop, where all the OS9exec processes will be handled and
// arbitrated. In one direction the 68k emulation (MacOS9 built-in or UAE) will
// be called, in the other direction the systemcall interface will be used. An
// exception handler one level higher can recall this procedure again.
void os9exec_loop(unsigned short xErr, Boolean fromIntUtil)
{
    ushort       cpid;
    process_typ *cp;
    Boolean      cwti, cwti_svd;
    // Boolean      ncwti;
    regs_type *crp;
    save_type *svd;

    ulong my_tick;
    ulong lastspin = GetSystemTick();

    ushort       spid;           // will be assigned before use
    ushort       svd_intpid = 0; // by default, we are not called from intutil
    ulong        resL;           // llm_os9_go result
    alarm_typ   *aa;
    uint32_t     aaNew; // new alarm number for cyclic alarm
    Boolean      last_arbitrate;
    process_typ *sigp;

    // If call is coming from within intUtil, one loop thru all (active)
    // processes will be done. Exit criteria is the cp->isIntUtil call, which
    // should be reached after
    if (fromIntUtil) {
        async_area = false; // don't allow this now
        arbitrate  = true;

        svd_intpid = currentpid;  // allow only this one as int util
        do_arbitrate(svd_intpid); // now search another active process
    }

    do {
        // --- prepare for entry into OS9 world, process is "currentpid"
        cpid = currentpid; // make a copy to allow currentpid to be changed by
                           // F$xxx calls
        cp       = &procs[cpid]; // pointer to procs   descriptor
        crp      = &cp->os9regs; // pointer to process' registers
        svd      = &cp->savread; // pointer to process' saved registers
        cwti     = cp->way_to_icpt;
        cwti_svd = cwti;

        if (xErr == 0) {
            /*
            if (cp->isIntUtil &&
                cp->state==pSleeping) cwti= false;

                                     ncwti= cwti && !cp->isIntUtil;
            if                      (ncwti) cp->masklevel= 1;
            debug_return( cpid, crp, ncwti );
            */

            if (cp->isIntUtil)
                cwti = false;
            if (cwti)
                cp->masklevel = 1;
            debug_return(crp, cpid, cwti);

            if (cp->isIntUtil &&
                (cp->state == pActive || cp->state == pWaiting ||
                 cp->state == pWaitRead))
                break;

            // if (cp->state==pActive &&
            //     cp->isIntUtil) break; // for an int utility everything is
            //     done already
        } // if                     // only in case of bus error, error handling
          // is required

        // --- make sure, that good old MacOS gets its time, too
        my_tick = GetSystemTick();
        if (my_tick - lastspin > spininterval) {
            lastspin = my_tick;
            CheckInputBuffers();
            eSpinCursor(32); // rotate forward once
        }

        // --- now execute
        if (cp->state == pSysTask) {
            arbitrate =
                true; // allow arbitration by default after sysTask execution

            // --- execute system task function
            cp->oerr = (cp->systask)(cpid, cp->systaskdataP, crp);
            if (cp->oerr)
                set_os9_state(
                    cpid,
                    pActive,
                    ""); // on error, continue with task execution anyway
            if (cp->state == pActive) {
                // process gets active again, report errors to OS9 programm
                if (Dbg_SysCall(crp, cpid))
                    debug_retsystask(crp, cpid);

                if (cp->oerr) {
                    retword(crp->regs[REGS_D + 1]) = cp->oerr;
                    crp->sr |= CARRY;
                }
                else
                    crp->sr &= ~CARRY;
            }
        } // system task

        else if (cp->state == pActive || cp->state == pWaitRead) {
            if ((cp->state == pActive || cp->way_to_icpt)) {
                // --- go execute OS9 code until next trap or exception
                // %%% for low-level calling interface debug: Debugger();
                if (xErr) {
                    cp->vector = 0xFAFA;
                    cp->func   = 4 * (xErr - 100);

                    xErr = 0; // do that only once
                }
                else {
// Threads can only run in parallel during 68k EMU access
// open it here
#ifdef THREAD_SUPPORT
                    if (ptocThread)
                        pthread_mutex_unlock(&sysCallMutex);
#endif

                    resL = llm_os9_go(crp);

#ifdef THREAD_SUPPORT
                    if (ptocThread)
                        pthread_mutex_lock(&sysCallMutex);
                    currentpid = cpid;
#endif
                    // and close it again

                    // --- and now exec syscall
                    cp->vector = hiword(resL);
                    cp->func   = loword(resL);
                }

                cp->way_to_icpt = false; // now it is done

                // --- safeguarding
                if (debugcheck(dbgWarnings, dbgDeep)) {
                    if (cp->memstart + 0x8000 != crp->regs[REGS_A + 6]) {
                        uphe_printf(">>> Warning [pid=%d]: A6 has changed from "
                                    "$%08lX to $%08lX !\n",
                                    cpid,
                                    cp->memstart + 0x8000,
                                    crp->regs[REGS_A + 6]);
                        debug_halt(dbgWarnings);
                    }

                    if ((crp->regs[REGS_A + 7] > cp->memtop) ||
                        (crp->regs[REGS_A + 7] < cp->memstart)) {
                        uphe_printf(">>> Warning [pid=%d]: A7=$%08lX is out of "
                                    "data area ($%08lX - $%08lX) !\n",
                                    cpid,
                                    crp->regs[REGS_A + 7],
                                    cp->memstart,
                                    cp->memtop);
                        debug_halt(dbgWarnings);
                    }
                }

                // register will be saved, in case they are reused in pWaitRead
                // state
                svd->r      = cp->os9regs;
                svd->vector = cp->vector;
                svd->func   = cp->func;
            }

            // in case of a unsuccessful read just repeat the call with saved
            // registers correct exception handling
            if (cp->state == pWaitRead && !cp->isIntUtil) {
                // registers of the last command will be restored
                cp->os9regs = svd->r;
                cp->vector  = svd->vector;
                cp->func    = svd->func;
            }

            // --- now handle trap
            if (cp->vector != 0) {
                if (!TCALL_or_Exception(cp, crp, cpid))
                    continue;
            }
            else {
                // TRAP0 = OS-9 system call called
                arbitrate = false; // disallow arbitration by default
                debug_comein(crp, cpid);
                exec_syscall(cp->func, cpid, crp, false);

                // analyze result
                if (debugcheck(dbgSysCall, dbgDeep)) {
                    if (cp->state != pActive)
                        uphe_printf(
                            "  * OS9 %s sets Pid=%d into new state=%s\n",
                            get_syscall_name(cp->func),
                            cpid,
                            PStateStr(cp));
                }

                // --- Alarm handling
                aa = alarm_queue[0];
                if (aa != NULL) {
                    if (GetSystemTick() >= aa->due) {
                        aaNew = 0; // must be zero !
                        if (aa->cyclic)
                            A_Make(aa->pid,
                                   &aaNew,
                                   aa->signal,
                                   aa->ticks,
                                   true);
                        send_signal(aa->pid, aa->signal); // renew it
                        A_Remove(aa); // and remove the old one
                    }
                }

                // --------------------------------------------
                // asynchronous signals are allowed here
                async_area = true;
                if (async_pending && cp->masklevel <= 0) {
                    sig_mask(cpid, 0); /* pending signals */
                    // debugprintf(dbgSysCall,dbgNorm,("# SIGNAL HANDLED
                    // isInt=%d pid=%d sig9=%d\n",
                    //             cp->isIntUtil, cpid, procs[ 9 ].icpt_signal
                    //             ));
                }

                // execute syscall, except in case of way to intercept, where
                // icpt routine must be done first Can have been triggered by
                // sig_mask call above

                async_area = false;
                // asynchronous signals are no longer allowed
                // --------------------------------------------

                // if the system is on the way to intercept, d1 and carry
                // must be stored (must not override values of "send_signal")
                cwti = cp->way_to_icpt;
                if (cwti) {
                    spid = cp->icpt_pid;
                    sigp = &procs[spid];

                    if (sigp->isIntUtil) {
                        printf("intutil intercept\n");
                    }

                    if (cp->icpt_pid ==
                        currentpid) { // in case of the own process
                        if (cp->icpt_signal != S_Wake && cp->pd._sigvec != 0)
                            crp = &cp->rteregs;
                    }
                    else {
                        cp->way_to_icpt = false; // now the way to icpt changes
                        currentpid = cp->icpt_pid; // continue as this process
                    }
                } // if cwti

                if (cp->state == pActive || cp->state == pWaitRead ||
                    cp->oerr) {
                    // report errors to OS9 programm
                    if (!cp->oerr)
                        crp->sr &= ~CARRY;
                    else {
                        if ((cp->oerr & 0xFF00) == E_OKFLAG)
                            cp->oerr &= 0x00FF; // ensure that silent errors are
                                                // restored to normal error code
                        retword(crp->regs[REGS_D + 1]) = cp->oerr;
                        crp->sr |= CARRY;
                    }
                }

                if (cwti && cp->icpt_signal != S_Wake && sigp->state != pDead) {
                    sigp->masklevel = 1; // not interrupteable during intercept
                    sigp->os9regs.regs[REGS_D + 1] =
                        cp->icpt_signal; // get signal code at icpt routine
                    sigp->rtevector = sigp->vector;
                    sigp->rtefunc   = sigp->func;

                    if (sigp == cp && sigp->state == pWaitRead &&
                        sigp->pd._sigvec != 0) {

                        sigp->rtestate = sigp->state;
                        memcpy((void *)&sigp->rteregs,
                               (void *)&svd->r,
                               sizeof(regs_type));
                        sigp->rtevector = svd->vector;
                        sigp->rtefunc   = svd->func;
                        set_os9_state(spid, pActive, "");
                    }

                    if (sigp->state ==
                            pWaiting &&          // a signalled waiting process
                        sigp->pd._sigvec != 0) { // will be active afterwards
                        set_os9_state(spid, pActive, "");
                        sigp->rtestate = pActive;
                    }
                } // if (cwti && ...)
            }
        } // active OS9 process that has called a TRAP

        else {
            if (cp->state != pUnused && cp->state != pSleeping &&
                cp->state != pWaiting && // allow this also
                (cp->state != pDead || cp->pd._signal == 0)) {
                // signal handling already done correctly
                // %%% for now, fatal: current process is not pActive
                uphe_printf("main loop: INTERNAL ERROR, active process=%d has "
                            "invalid state=%s\n",
                            cpid,
                            PStateStr(cp));
                debug_halt(dbgAll);
            }
        }

        if (debugcheck(dbgTaskSwitch, dbgDetail) && cp->state == pWaitRead) {
            uphe_printf("arbitrate: current pid=%d: pWaitRead (before) \n",
                        cpid);
        }

        // do not arbitrate if way to intercept
        // if (!arb_cnt--) arb_cnt= ARB_RATE-1; // arbitrate= true;
        last_arbitrate = arbitrate;

        if (cp->state == pWaitRead)
            memcpy((void *)&cp->os9regs,
                   (void *)&svd->r,
                   sizeof(regs_type)); // save all regs

        // if (currentpid==justthis_pid) {
        //   debugprintf(dbgTaskSwitch,dbgNorm,("# LOOOPI cwti=%d masklevel=%d
        //   arbitrate=%d\n", cwti, cp->masklevel, arbitrate ));
        // }

        // if (fullArb || fromIntUtil) arbitrate= true;
        if (fullArb)
            arbitrate = true;
        if (!cwti && !cwti_svd && cp->masklevel <= 0) {
            do_arbitrate(svd_intpid);
            // if (cp->isIntUtil)
            //   debugprintf(dbgSysCall,dbgNorm,("# int after arb: pid=%d
            //   state=%d isInt=%d\n",
            //                                      currentpid, cp->state,
            //                                      cp->isIntUtil ));
        }

        if (logtiming)
            arb_to_os9(last_arbitrate);

        cp = &procs[currentpid];

        if (!cwti) {
            cwti = cp->way_to_icpt;
            if (cwti) {
                // if (Dbg_SysCall( cpid,crp ))
                //   upe_printf( "%d %d %d %x\n", cp->icpt_pid,currentpid,
                //                                cp->icpt_signal, os9_long(
                //                                (ulong)cp->pd._sigvec ) );

                if (cp->icpt_pid != currentpid) {
                    cp->way_to_icpt = false;        // reset if another process
                    currentpid      = cp->icpt_pid; // continue as this process
                }

                if (cp->icpt_signal != S_Wake) {
                    cp->os9regs.regs[REGS_D + 1] =
                        cp->icpt_signal; // get signal code at icpt routine
                }

                cp = &procs[currentpid];
            }
        } // if not cwti

        if (debugcheck(dbgTaskSwitch, dbgDetail) && cp->state == pWaitRead) {
            uphe_printf("arbitrate: current pid=%d: pWaitRead (after) \n",
                        cpid);
        }

        // --- show process changes
        if (debugcheck(dbgTaskSwitch, dbgNorm)) {
            if (currentpid != cpid && cp->state != pWaitRead) {
                uphe_printf("main loop: Task switch from pid=%d to pid=%d\n",
                            cpid,
                            currentpid);
                debug_halt(dbgTaskSwitch);
            }
        }
    } while (currentpid < MAXPROCESSES); /* while active processes */
} // os9exec_loop

void segv_handler(int sig)
{
    // int          sv= sig;
    process_typ *cp  = &procs[currentpid]; // pointer to procs   descriptor
    regs_type   *crp = &cp->os9regs;       // pointer to process' registers

    cp->exiterr = E_BUSERR;

    switch (sig) {
    case SIGFPE:
        cp->exiterr = E_ZERDIV;
        break;
    } // switch

    // printf("*** Bus Error *** %d\n", sig );
    // fflush(0);

    in_m68k_go = 0;        // remove the blocker in UAE
    llm_os9_copyback(crp); // copy registers back for BusError reporting

    siglongjmp(main_env, cp->exiterr); // go back with bus error
} // segv_handler

typedef void (*loop_proc)(unsigned short xErr, Boolean fromIntUtil);
static void setup_exception(loop_proc lo)
{
    unsigned short err = 0;
    unsigned short xErr;

    // The UNIX exception handler, using sigsetjmp/siglongjmp
    struct sigaction sa;

    err = sigsetjmp(main_env, 1);

    sa.sa_handler = &segv_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGSEGV, &sa, NULL); // catch SEG faults
    sigaction(SIGBUS, &sa, NULL);  // ... and others as well
    sigaction(SIGFPE, &sa, NULL);

    do {
        if (err && debugcheck(dbgTrapHandler, dbgNorm)) {
            uphe_printf("Exception occurred [pid=%d] err=%d\n",
                        currentpid,
                        err);
        }

        xErr = err;
        err  = 0; // preserve this in case of no exception

        lo(xErr, false); // go in with the last exception error

    } while (err);
} // setup_exception

/* Entry into OS9 emulation
 * If toolname==NULL, the os9 program is loaded from 'OS9C' Id=0
 */
ushort os9exec_nt(const char *toolname,
                  int         argc,
                  char      **argv,
                  char      **envp,
                  uint32_t    memplus,
                  ushort      prior)
{
    ushort       cpid;
    process_typ *cp;

    dir_type *drP;
    char     *errnam;
    char     *errdesc;
    char     *argv_startup[2];
    char     *my_toolname =
        (char *)toolname; /* do not change <toolname> directly */
    int ii;

    char *p;

    struct sigaction ia;

    // ulong    res;	    // /* llm_os9_go result */
    ushort err = 0; /* MPW Error */
    OSErr  oserr;   /* Mac error */

// Prepare the mutex
#ifdef THREAD_SUPPORT
    pthread_mutex_init(&sysCallMutex, NULL);
#endif

    /* ---- win32 specific global init ------------------------------- */

    /* ---- create the kernel process -------------------------------- */
    new_process(0, &cpid, 0); /* get the kernel process */
    cp = &procs[cpid];
    set_os9_state(cpid, pSleeping, ""); /* to be compliant to real OS-9 */
    cp->pd._prior = os9_word(0xFFFF);   /* "  "      "     "    "    "  */
    cp->mid       = 0; /* take "OS9exec" as the kernel module */

    /* ---- assign startVolID/dirID ---------------------------------- */

    if (catch_ctrlC) {

        ia.sa_handler = &CtrlC_Handler;
        sigemptyset(&ia.sa_mask);
        ia.sa_flags = 0;

        sigaction(SIGINT, &ia, NULL);
    }

    // some global init moved from here to os9exec_globinit() to allow them
    // being called in os9main.c already

    interactivepid = 0; // send aborts to first process by default
    sig_queue.cnt  = 0; // no signal pending at the beginning

    /* no table entries at the beginning */
    for (ii = 0; ii < MAXDIRS; ii++) {
        dirtable[ii].ident = NULL;

#ifdef LINKED_HASH
        dirtable[ii].next = NULL;
#endif
    }

    for (ii = 0; ii < MAXDIRHIT; ii++)
        hittable[ii] = 0;
    hittable[0] = MAXDIRS;

    debug_prep();
    debugprintf(dbgStartup,
                dbgNorm,
                ("# os9exec_nt: entered routine, no op yet\n"));

    /* include 'startup' as default parameter */
    /* will be eliminated again later, if no 'startup' file is available */
    if (argc == 0 && ustrcmp(my_toolname, "") == 0) {
        argc            = 1;
        my_toolname     = "shell";
        argv_startup[0] = "startup";
        argv_startup[1] = NULL;
        argv            = (char **)&argv_startup;
    }

    withTitle =
        ustrcmp(my_toolname, "shell") == 0 || ustrcmp(my_toolname, "sh") == 0;

    /* Sign-on message */
    if (withTitle) {
        upo_printf("\n");
        upho_printf("%s:     OS-9  User Runtime Emulator\n", OS9exec_Name());
        upho_printf("Copyright (C) 2007 Lukas Zeller / Beat Forster\n");
        upho_printf("This program is distributed under the terms of\n");
        upho_printf("       the GNU General Public License         \n");
        upho_printf("\n");
        titles();
    }

    /* prepare low-level magic, to allow using llm_xxx() routines */
    oserr = lowlevel_prepare();
    if (oserr) {
        uphe_printf("FATAL ERROR: can't initialize low-level, OSErr=%d\n",
                    oserr);
        my_toolname = ""; /* no tool to be used */
        err         = 3;  /* MPW system/missing resource error */
        goto mainabort;
    }
    debugprintf(dbgStartup, dbgNorm, ("# main startup: low level prepared\n"));

    /* show environment */
    debugprintf(dbgStartup,
                dbgNorm,
                ("# main startup: FPU=%d, Virtual Mem=%d, Cache=%d, Runs in "
                 "User mode=%d\n",
                 llm_fpu_present(),
                 llm_vm_enabled(),
                 llm_has_cache(),
                 llm_runs_in_usermode()));

    GetStartPath(startPath);
    strcpy(strtUPath, startPath);
    PathUp(strtUPath);
    // upo_printf( "'%s' '%s'\n", startPath, strtUPath );

/* get default SCSI setting */
#ifdef RBF_SUPPORT
#endif

    drP       = &cp->d;
    drP->type = fDir; /* set to default */
    drP->dev  = 0;
    drP->lsn  = 0;

    GetCurPaths("OS9DISK", 0x80, &cp->d, true);
    memcpy(&cp->x, &cp->d, sizeof(dir_type)); /* at least this should be ok */
    GetCurPaths("OS9CMDS", 0x84, &cp->x, true);

    drP       = &mdir;
    drP->type = fDir; /* set to default */
    drP->dev  = 0;
    drP->lsn  = 0;

    GetCurPaths("OS9MDIR", 0x80, &mdir, true);

    /* establish the virtual mdir (dir used to load modules from by default) */
    p = egetenv("OS9MDIR"); /* get path for default module loading dir */
    strcpy(mdirPath, p);

    debugprintf(dbgStartup, dbgNorm, ("# main startup: mdir='%s'\n", mdirPath));

    /* install asynchronous handlers */
    atexit(&cleanup);

#if defined NATIVE_SUPPORT || defined PTOC_SUPPORT
    PluginLoader(cpid);
#endif

    if (withTitle)
        upo_printf("\n");
    BootLoader(cpid);
    CheckStartup(cpid, my_toolname, &argc, argv);

    /* initialize spinning cursor */
    eSpinCursor(0); /* reset cursor spinning */

    /* init error tracking vars */
    totalMem       = max_mem(); /* get startup memory */
    lastpathparsed = NULL;
    // lastsyscall=0;

    /* set-up MPW signal handling */

    /* prepare initial process */
    // NOTE: An internal command can crash here, because segv_handler is not yet
    // up
    cp->oerr = prepLaunch(my_toolname, argv, argc, envp, memplus, prior);
    if (cp->oerr)
        goto mainabort; /* prepare for execution */

    debugprintf(dbgStartup,
                dbgNorm,
                ("# main startup: Main module loaded & prepared for launch\n"));

    setup_exception(os9exec_loop);

    debugprintf(dbgStartup, dbgDeep, ("# os9exec_nt: exited main loop\n"));
    /* --- exit status of MPW tool comes from exit status of first process */
    cp       = &procs[0];
    cp->oerr = procs[0].exiterr;
    err      = 0; /* assume no MPW error */

    if (cp->oerr) {
        if (cp->oerr != 1) {
            get_error_strings(cp->oerr, &errnam, &errdesc);
            err = 2; /* MPW: some error in processing */
            upho_printf(
                "OS-9 emulation exits with Error #%03d:%03d (%s):\n#   %s\n",
                cp->oerr >> 8,
                cp->oerr & 0xFF,
                errnam,
                errdesc);
        }
        else
            err = cp->oerr;
    }
    goto cleanup;

mainabort:
    get_error_strings(cp->oerr, &errnam, &errdesc);
    upho_printf("Emulation could not start due to OS-9 error #%03d:%03d (%s): "
                "'%s'\n#   %s\n",
                cp->oerr >> 8,
                cp->oerr & 0xFF,
                errnam,
                my_toolname,
                errdesc);
    err = 3; /* MPW system/missing resource error */

cleanup:
    /* cleanup is also done automatically when exit() is called */
    debugprintf(dbgStartup, dbgNorm, ("# os9exec_nt: cleaning up now\n"));
    cleanup();
    debugprintf(dbgStartup,
                dbgNorm,
                ("# os9exec_nt: returning to caller with err=%d\n", err));
    return err;
}

/* Entry for old os9exec V1.14 style tools */
ushort os9exec(int argc, char **argv, char **envp)
{
    /* execute default module (OS9C ID=0) with no extra memory */
    return os9exec_nt(NULL, argc, argv, envp, 0, MYPRIORITY);
}

/* eof */
