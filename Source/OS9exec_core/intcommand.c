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
 *    Revision 1.52  2007/03/31 12:09:40  bfo
 *    "ihit" extended for LINKED_HASH support
 *
 *    Revision 1.51  2007/03/24 12:34:50  bfo
 *    - "ihit" introduced (hash hit rate/statistics)
 *     - internal utilities with unlimited number of command line params
 *
 *    Revision 1.50  2007/03/10 12:22:05  bfo
 *    "Change_DbgPath" implemented here now (visible from outside)
 *
 *    Revision 1.49  2007/02/22 23:12:04  bfo
 *     <call_Intercept> and parameter adaptions
 *
 *    Revision 1.48  2007/02/14 20:59:55  bfo
 *    Search DLLs at PLUGINS and one level higher
 *
 *    Revision 1.47  2007/02/04 20:08:29  bfo
 *    - DLL connect/disconnect implemented here now
 *    - "Plugin_Possible"/"Native_Possible" enhanced
 *
 *    Revision 1.46  2007/01/28 22:14:00  bfo
 *    - Native program handling, <modBaseP> param added
 *    - int command 'native' added
 *
 *    Revision 1.45  2007/01/07 13:48:17  bfo
 *    "Init_IProg" added
 *    Callback for "StrToFload"/"StrToDouble" placed locally
 *    Internal command "plugin" ( -d/-e ) added
 *    DLL jump implemented here (in a first working version)
 *
 *    Revision 1.44  2007/01/04 20:27:36  bfo
 *    <pa> is not used => eliminate
 *
 *    Revision 1.43  2007/01/02 11:19:35  bfo
 *    "ChangeElement" visible from outside
 *     usage functions more consistent
 *    int_on/int_off with much more functionality
 *    "isintcommand" considers <includeList>/<excludeList>
 *
 *    Revision 1.42  2006/12/02 17:52:09  bfo
 *    F$Chain supported for int commands
 *
 *    Revision 1.41  2006/11/18 09:59:13  bfo
 *    comment line removed
 *
 *    Revision 1.40  2006/11/04 23:37:29  bfo
 *    <procName> => <intProcName> / comment for -j=<pid> added
 *
 *    Revision 1.39  2006/11/04 01:16:53  bfo
 *    "icrash" added.
 *
 *    Revision 1.38  2006/11/01 11:38:45  bfo
 *    The <os9regs> reference will be provided to internal commands additionally
 *
 *    Revision 1.37  2006/10/01 15:17:47  bfo
 *    "ipmask" / "inopmask" internal commands eliminated;
 *    <isPtoC> replaced by <cp->isPtoC>
 *
 *    Revision 1.36  2006/09/08 21:59:16  bfo
 *    enhanced "-o <fileName>" (recommended by Martin Gregorie)
 *
 *    Revision 1.35  2006/09/03 20:43:02  bfo
 *    Old PtoC bridge stuff eliminated
 *
 *    Revision 1.34  2006/08/29 22:13:38  bfo
 *    universal "ptoc_calls" support / no large pipes for ptoc
 *
 *    Revision 1.33  2006/08/27 00:00:47  bfo
 *    "ptoc_calls" replaces all the direct PtoC calls now /
 *    debug logging is counting now correctly
 *
 *    Revision 1.32  2006/08/20 16:31:47  bfo
 *    5 more internal commands added
 *
 *    Revision 1.31  2006/08/06 22:43:33  bfo
 *    more direct ptoc intcommand calling
 *
 *    Revision 1.30  2006/08/04 18:46:10  bfo
 *    ptoc_prep => ptoc_call (unified)
 *
 *    Revision 1.29  2006/07/29 08:52:43  bfo
 *    <pBlocked> eliminated / "int_ranpapp" added as int util
 *
 *    Revision 1.28  2006/07/23 14:35:30  bfo
 *    ipmask/inopmask commands added
 *
 *    Revision 1.27  2006/07/21 07:35:50  bfo
 *    Reorganisation of "ihelp" display / os9_to_xxx arg adaption
 *    OS9STOP env support (by Martin gregorie)
 *    "stop/shutdown" : both names allowed now (by Martin Gregorie)
 *
 *    Revision 1.26  2006/07/10 09:56:50  bfo
 *    ushort parentid added for "callcommand"
 *
 *    Revision 1.25  2006/07/06 22:59:13  MG
 *    function 'int_stop' added (allowed for super user only)
 *     (by Marin Gregorie)
 *
 *    Revision 1.24  2006/06/11 22:11:26  bfo
 *    int_pento added to internal commands
 *
 *    Revision 1.23  2006/06/10 10:21:56  bfo
 *    PTOC_FULL included
 *
 *    Revision 1.22  2006/06/07 16:12:41  bfo
 *    PCall things switched off
 *
 *    Revision 1.21  2006/05/16 13:12:06  bfo
 *    Use "Pipe_NReady"
 *
 *    Revision 1.20  2006/02/19 16:28:56  bfo
 *    'Is_PtoC' visible from ouside / PtoC + thread support added
 *
 *    Revision 1.19  2005/07/06 21:03:26  bfo
 *    system call supported for the unix world
 *
 *    Revision 1.18  2005/06/30 11:47:20  bfo
 *    Mach-O support
 *
 *    Revision 1.17  2005/04/15 11:43:04  bfo
 *    Reduced size of RBF images is supported now
 *
 *    Revision 1.16  2004/12/04 00:00:52  bfo
 *    MacOSX MACH adaptions
 *
 *    Revision 1.15  2004/11/27 12:08:24  bfo
 *    _XXX_ introduced
 *
 *    Revision 1.14  2004/11/20 11:44:07  bfo
 *    Changed to version V3.25 (titles adapted)
 *
 *    Revision 1.13  2004/10/22 22:51:12  bfo
 *    Most of the "pragma unused" eliminated
 *
 *    Revision 1.12  2003/05/21 20:33:12  bfo
 *    Allocate 512k blocks / Additional parameter <mem_fulldisp> for "show_mem"
 *
 *    Revision 1.11  2003/05/17 10:45:03  bfo
 *    'show_mem' with <mem_unused> parameter / Full "imem" support ( -u )
 *
 *    Revision 1.10  2002/10/27 23:54:30  bfo
 *    "iunused" implemented
 *
 *    Revision 1.9  2002/10/09 20:43:48  bfo
 *    uphe_printf => upe_printf
 *
 *
 */

#include <ctype.h>

/* OS9exec/nt 2.0 internal commands */
/* ================================ */

/* global includes */
#include "os9exec_incl.h"

#include <dlfcn.h> // MacOSX and Linux DLL functionality

#ifdef THREAD_SUPPORT
#include <types.h>
#endif

#ifdef PTOC_SUPPORT
#include "native_interface.h"
#endif

/* global vars */
ushort icmpid;  /* current internal command's PID */
char  *icmname; /* current internal command's name = argv[0] */
/* -------------------------- */

#define Mx 10

// ---------------------------------------------------------------------

static os9err int_stop(ushort pid, _argc_, _argv_)
/* "stop/shutdown": exit from the OS9exec emulator */
{
    char *envstop;

    envstop = getenv("OS9STOP");
    if (is_super(pid) || envstop) {
        quitFlag = true; /* only the super user can stop os9exec */
        upo_printf("\n");
        upho_printf("OS-9 emulation stopped\n");
        fflush(stdout);
        exit(0);
    }

    return (E_PERMIT); /* not super user: reject the stop command */
}

static void idbg_usage(char *name)
{
    upe_printf("Usage: %s [options]\n", name);
    upe_printf("Function: enter internal debug mode\n");
    upe_printf("Options:\n");
    upe_printf("    -d[n] msk    set debugging info mask [of level n, x=all, "
               "default=0]\n");
    upe_printf("    -s    msk    set debugging stop mask (default=0)\n");
    upe_printf("    -dh          show debug/stop mask help\n");
    upe_printf("    -m           only show current debug/stop masks\n");
    upe_printf("    -n=[<name>]  set name to trigger/disable trigger (causes "
               "debug stop)\n");
    upe_printf("    -q           quit emulator\n");
    upe_printf("    -o=<syspath> redirect debug output to <syspath>\n");
    upe_printf("    -j=<pid>     generate debug output only   for <pid>\n");
    upe_printf("    -w=<pid>     do not generate debug output for <pid>\n");
    upe_printf("    -x=<width>   define MGR screen width\n");
    upe_printf("    -y=<height>  define MGR screen height\n");
    upe_printf("    -z           define MGR fullscreen mode\n");
} // idbg_usage

void Change_DbgPath(int argc, char **argv, char **pp, ushort *kp)
{
    os9err    err;
    ptype_typ type;
    ulong     size;
    char     *v;

    // switch off a potentialy open dbgPath
    if (dbgPath > 0) {
        err     = syspath_close(0, dbgPath);
        dbgPath = 0;
    }

    v = (*pp) + 1;
    if (*v == '=' || *v == ' ')
        (*pp) += 2;
    else {
        (*kp)++; // next arg
        if ((*kp) >= argc) {
            dbgOut = 0;
            return;
        }

        *pp = argv[(*kp)];
    }

    // try as path number first
    if (sscanf(*pp, "%d", &dbgOut) >= 1)
        return;
    dbgOut = 0;

    // open log file, create it if not yet existing ...
    if (**pp != NUL) {
        type = IO_Type(0, *pp, 0x03);
        err  = syspath_open(0, &dbgPath, type, *pp, 0x03);
        if (err == E_PNNF)
            err = syspath_open(0, &dbgPath, type, *pp, poCreateMask | 0x03);
        if (!err) {
            err = syspath_gs_size(0, dbgPath, &size);
            err = syspath_seek(0, dbgPath, size);
        }

        if (err)
            dbgPath = 0;
        dbgOut = dbgPath;
    }
} // Change_DbgPath

static os9err int_debughalt(ushort pid, int argc, char **argv)
/* OS9exec debug halt */
{
    // os9err    err;
    ushort  k;
    Boolean opt;
    Boolean fullScreen = false;
    char   *p;
    ushort *usp;
    ushort  level;
    // ptype_typ type;
    // ulong     size;

    /* get arguments and options */
    opt = false;
    for (k = 1; k < argc; k++) {
        p = argv[k];
        if (*p == '-') {
            switch (tolower(*++p)) {
            case '?':
            case 'h':
                idbg_usage(argv[0]);
                return 0;

            case 'm':
                break;
            case 's':
                usp = &debughalt;
                goto getmask;
            case 'd':
                level = 0; /* default to level 0 */
                if (*(p + 1) == 'h') {
                    debug_help(0, 0, NULL);
                    return 0;
                }
                if (*(p + 1) == 'x') {
                    level = 3;
                    usp   = &debug[0];
                    goto getmask;
                }
                else if (isdigit(*(p + 1))) {
                    level = *(p + 1) - 0x30;
                    if (level > DEBUGLEVELS)
                        level = 0; /* default to 0 if level invalid */
                }
                usp = &debug[level];
                goto getmask;
            getmask:

                if (*(p + 1) == '=')
                    p += 2;
                else {
                    k++; /* next arg */
                    if (k >= argc) {
                        printf("# Error: missing mask(hex) for '%c' option!\n",
                               *p);
                        return 1;
                    }
                    p = argv[k];
                }

                if (sscanf(p, "%hx", usp) != 1) {
                    printf("# Error in hex mask '%s'\n", p);
                    return 1;
                }
                if (level > 2)
                    debug[2] = debug[1] = debug[0]; /* set all */
                debug_prep();                       /* adjust debug masks */
                break;

            case 'n':
                k++; /* next arg */
                if (k >= argc) {
                    triggername[0] = 0;
                    break;
                }
                strncpy(triggername, argv[k], TRIGNAMELEN);
                break;

            case 'q':
                return int_stop(pid, argc, argv);

            case 'w':
                if (*(p + 1) == '=')
                    p += 2;
                else {
                    k++; /* next arg */
                    if (k >= argc) {
                        without_pid = os9_word(procs[pid].pd._pid);
                        break;
                    }
                    p = argv[k];
                }
                if (sscanf(p, "%d", &without_pid) < 1)
                    without_pid = 0;
                break;

            case 'j':
                if (*(p + 1) == '=')
                    p += 2;
                else {
                    k++; /* next arg */
                    if (k >= argc) {
                        justthis_pid = os9_word(procs[pid].pd._pid);
                        break;
                    }
                    p = argv[k];
                }
                if (sscanf(p, "%d", &justthis_pid) < 1)
                    justthis_pid = 0;
                break;

            case 'o':
                Change_DbgPath(argc, argv, &p, &k);
                break;

                /*
                case 'o' :  // switch off a potentialy open dbgPath
                            if (dbgPath>0) {
                              err= syspath_close( 0, dbgPath );
                              dbgPath=  0;
                            }

                            if (*(p+1)=='=') p+=2;
                            else {  k++; // next arg
                                if (k>=argc)  { dbgOut= 0; break; }
                                p= argv[k];
                            }

                            // try as path number first
                            if (sscanf( p,"%d", &dbgOut )>=1) break;
                            dbgOut= 0;

                            // open log file, create it if not yet existing ...
                            if (*p!=NUL) {                         type=
                IO_Type( 0,  p, 0x03 ); err= syspath_open( 0, &dbgPath, type, p,
                0x03 ); if  (err==E_PNNF) err= syspath_open( 0, &dbgPath, type,
                p, poCreateMask|0x03 ); if (!err) { err= syspath_gs_size( 0,
                dbgPath, &size ); err= syspath_seek   ( 0, dbgPath,  size ); }


                              if (err) dbgPath= 0;
                              dbgOut=  dbgPath;
                            }
                            break;
                                */

            case 'z':
                fullScreen = true;
                break; /* full screen mode */

            case 'x':
                if (*(p + 1) == '=')
                    p += 2;
                else {
                    k++; /* next arg */
                    if (k >= argc)
                        break;
                    p = argv[k];
                }

                if (sscanf(p, "%ld", &screenW) < 1)
                    screenW = 0;
                break;

            case 'y':
                if (*(p + 1) == '=')
                    p += 2;
                else {
                    k++; /* next arg */
                    if (k >= argc)
                        break;
                    p = argv[k];
                }

                if (sscanf(p, "%ld", &screenH) < 1)
                    screenH = 0;
                break;

            default:
                idbg_usage(argv[0]);
                printf("# Error: unknown option '%c'\n", *p);
                return 1;
            }
            opt = true;
        }
    }

    if (fullScreen) {
        screenW = 0; /* full screen mode */
        screenH = 0;
    }

    if (opt) {
        uphe_printf(
            "Debug Masks = $%04X,$%04X,$%04X, Stopmask = $%04X, Trigger='%s'\n",
            debug[dbgNorm],
            debug[dbgDetail],
            debug[dbgDeep],
            debughalt,
            triggername);
    }
    else {
        debugwait();
    }
    return 0;
}

/* show procs */
static os9err int_procs(_pid_, _argc_, _argv_)
{
    show_processes();
    return 0;
}

/* show modules */
static os9err int_mdir(_pid_, int argc, char **argv)
{
    char *cmp = NULL;
    if (argc > 1)
        cmp = argv[1];
    show_modules(cmp);
    return 0;
}

static void ipaths_usage(char *name)
{
    upe_printf("Syntax:   %s [<pid>]\n", name);
    upe_printf("Function: Print OS9exec system paths\n");
    upe_printf("Options:  None.\n");
}

/* OS9exec internal path list */
static os9err int_paths(_pid_, int argc, char **argv)
{
#define IPATHS_MAXARGS 1
    int   nargc = 0, h;
    char *p;
    int   k = MAXPROCESSES;

    for (h = 1; h < argc; h++) {
        p = argv[h];

        if (*p == '-') {
            p++;
            switch (tolower(*p)) {
            case '?':
            case 'h':
                ipaths_usage(argv[0]);
                return 0;
            default:
                ipaths_usage(argv[0]);
                upe_printf("Error: unknown option '%c'\n", *p);
                return 1;
            } // switch
        }
        else {
            if (nargc >= IPATHS_MAXARGS) {
                upe_printf("Error: Only 1 argument allowed\n");
                return 1;
            }

            k = atoi(p);
            nargc++;
        }
    }

    show_files(k);
    return 0;
}

static void imem_usage(char *name)
{
    upe_printf("Syntax:   %s [<opts>]\n", name);
    upe_printf("Function: Print OS9exec memory segments\n");
    upe_printf("Options:\n");
    upe_printf("    -i=<pid> show memory of <pid>\n");
    upe_printf("    -u       show unused memory\n");
} // imem_usage

static os9err int_mem(_pid_, int argc, char **argv)
/* "imem": OS9exec internal allocated memory */
{
#define IMEM_MAXARGS 0
    int     nargc = 0, h;
    char   *p;
    Boolean mem_unused   = false;
    Boolean mem_fulldisp = false;
    int     my_pid       = MAXPROCESSES;

    for (h = 1; h < argc; h++) {
        p = argv[h];

        if (*p == '-') {
            p++;
            switch (tolower(*p)) {
            case '?':
            case 'h':
                imem_usage(argv[0]);
                return 0;

            case 'i':
                if (*(p + 1) == '=')
                    p += 2;
                else {
                    h++; /* next arg */
                    if (h >= argc) {
                        my_pid = MAXPROCESSES;
                        break;
                    }
                    p = argv[h];
                }

                if (sscanf(p, "%d", &my_pid) < 1)
                    my_pid = MAXPROCESSES;
                break;

            case 'u':
                mem_unused = true;
                break;
            case 'f':
                mem_fulldisp = true;
                break;

            default:
                imem_usage(argv[0]);
                upe_printf("Error: unknown option '%c'\n", *p);
                return 1;
            } // switch
        }
        else {
            if (nargc >= IMEM_MAXARGS) {
                upe_printf("Error: no arguments allowed\n");
                return 1;
            }

            nargc++;
        }
    }

    show_mem(my_pid, mem_unused, mem_fulldisp);
    return 0;
}

static os9err int_unused(_pid_, _argc_, _argv_)
/* Show unused memory */
{
    show_unused();
    return 0;
}

static void idevs_usage(char *name)
{
    upe_printf("Syntax:   %s [<opts>]\n", name);
    upe_printf("Function: Print OS9exec device table\n");
    upe_printf("Options:\n");
    upe_printf("    -r    show RBF devices only\n");
    upe_printf("    -s    show statistic values\n");
}

static void devs_printf(syspath_typ *spP, char *driv, char *fmgr)
{
    char s[OS9NAMELEN];
    char d[OS9NAMELEN];
    strcpy(s, spP->name);
    strcpy(d, driv);

    upo_printf("%-10s %-8s %-7s %2d\n",
               StrBlk_Pt(s, Mx),
               StrBlk_Pt(d, 8),
               fmgr,
               spP->nr);
}

static os9err int_devs(_pid_, int argc, char **argv)
/* idevs": OS9exec internal devices */
{
#define IDEVS_MAXARGS 0
    int     nargc = 0, h;
    char   *p;
    Boolean rbf_devs  = false;
    Boolean statistic = false;

    int           ii;
    ttydev_typ   *tdev;
    pipechan_typ *pch;
    syspath_typ  *spP;

    for (h = 1; h < argc; h++) {
        p = argv[h];

        if (*p == '-') {
            p++;
            switch (tolower(*p)) {
            case '?':
            case 'h':
                idevs_usage(argv[0]);
                return 0;

            case 'r':
                rbf_devs = true;
                break;
            case 's':
                statistic = true;
                break;

            default:
                idevs_usage(argv[0]);
                upe_printf("Error: unknown option '%c'\n", *p);
                return 1;
            }
        }
        else {
            if (nargc >= IDEVS_MAXARGS) {
                upe_printf("Error: no arguments allowed\n");
                return 1;
            }
            nargc++;
        }
    }

    upo_printf("%s  %s\n", hw_name, sw_name);
    upo_printf("\n");

    if (statistic) {
        upo_printf(
            "Device     read: miss /     total  write: miss /     total\n");
        upo_printf(
            "---------- ----------------------  -----------------------\n");
    }
    else {
        upo_printf("Device     Driver   FileMgr nr sect wPrt Image\n");
        upo_printf("---------- -------- ------- -- ---- ---- "
                   "--------------------------------------\n");
    }

#ifdef RBF_SUPPORT
    Disp_RBF_Devs(statistic);
#endif

    if (!statistic && !rbf_devs) {

        spP = &syspaths[sysStdnil];
        devs_printf(spP, "null", "scf");
        spP = &syspaths[sysVMod];
        devs_printf(spP, "vmod_drv", "scf");

        for (ii = 0; ii < MAXTTYDEV; ii++) {
            tdev = &ttydev[ii];
            if (tdev->installed && tdev->spP != NULL) {
                devs_printf(tdev->spP, "tty", "scf");

                pch = tdev->spP->u.pipe.pchP;
                if (pch != NULL) {
                    spP = &syspaths[pch->sp_lock];
                    devs_printf(spP, "pty", "scf");
                }
            }
        }
    }

    return 0;
}

// ---------------------------------------------------------------------------------
#if defined NATIVE_SUPPORT || defined PTOC_SUPPORT
// Get the DLL suffix of my operating system
const char *DLL_Suffix()
{
    const char *suff;

#if defined MACOSX
    suff = ".dylib";
#else
    suff = ".so";
#endif

    return suff;
} // DLL_Suffix

// Get <aFunc> of <aFuncName>
static os9err DLL_Func(void *aDLL, const char *aFuncName, void **aFunc)
{
    *aFunc = dlsym(aDLL, aFuncName);

    if (*aFunc == NULL)
        return E_NES;
    return 0;
} // DLL_Func

// Get the number of internal programs for <p>
int NumberOfNativeProgs(plug_typ *p)
{
    char iName[OS9NAMELEN];
    char iOpt[OS9NAMELEN];

    int i = 0;
    while (p->next_NativeProg(&i, (char *)&iName, (char *)&iOpt)) {
    }
    return i;
} // NumberOfNativeProgs

static os9err DisconnectDLL(plug_typ *p)
{
    if (!p || !p->fDLL)
        return E_PNNF;

    dlclose(p->fDLL);

    return 0;
} // DisconnectDLL

// Connect to all available DLLs
os9err ConnectDLL(plug_typ *p)
{
    os9err err;
    // Boolean first= true;
    int i;

    typedef long (*VersionProc)(void);
    VersionProc fModVersion;

    // Get the full path where to search for plugins
    char fullName[OS9PATHLEN];

    i = 1;
    while (true) {
        if (i <= 2)
            strcpy(fullName, startPath);
        else
            strcpy(fullName, strtUPath);

        if (i == 1 || i == 3) {
            strcat(fullName, PATHDELIM_STR); // search at "PLUGINS" first
            strcat(fullName, "PLUGINS");
        }

        strcat(fullName, PATHDELIM_STR);
        strcat(fullName, p->name.host);
        // upe_printf( "connectDLL='%s'\n", fullName );
        //  now we have the complete path name

#if defined MACOSX
#define mode RTLD_NOW + RTLD_GLOBAL
#else
#define mode RTLD_LAZY
#endif

        p->fDLL = dlopen(fullName, mode);

        if (p->fDLL)
            break; // found

        if (i >= 4)
            return E_PNNF;
        i++;

        // if (!first) return E_PNNF; // not found on both paths
        // first= false;
    } // loop

    /* These are the rquired plugin functions: */
    err = DLL_Func(p->fDLL, "Module_Version", (void **)&fModVersion);
    if (!err)
        err =
            DLL_Func(p->fDLL, "Next_NativeProg", (void **)&p->next_NativeProg);
    if (!err)
        err = DLL_Func(p->fDLL, "Is_NativeProg", (void **)&p->is_NativeProg);
    if (!err)
        err = DLL_Func(p->fDLL,
                       "Start_NativeProg",
                       (void **)&p->start_NativeProg);

    if (!err) {
        p->pVersion       = fModVersion();
        p->call_Intercept = NULL;

        if (p->pVersion >= 0x03360000)
            err = DLL_Func(p->fDLL,
                           "Call_Intercept",
                           (void **)&p->call_Intercept);
    }

    if (err) {
        DisconnectDLL(p);
        return err;
    }

    p->numNativeProgs = NumberOfNativeProgs(p);
    return 0;
} // ConnectDLL

// ---------------------------------------------------------------------------------

// Get an element, dependent on <addIt>
static addrpair_typ *MyElem(int i, bool addIt)
{
    if (addIt)
        return &includeList[i];
    else
        return &excludeList[i];
}

// Check, if include list ( <asInclude> = true  ) or
//           exclude list ( <asInclude> = false ) is empty
static bool Native_Empty(bool asInclude)
{
#if defined NATIVE_SUPPORT || defined PTOC_SUPPORT
    addrpair_typ *q     = MyElem(0, asInclude);
    bool          found = (q->host != NULL);
#endif

    return !found;
}

static Boolean Native_Enabled(plug_typ *p)
{
    char   *name      = p->name.host;
    Boolean isBuiltIn = name == NUL; // built-in has no name
    Boolean bp        = isBuiltIn || pluginActive;
    return (bp || p->pEnabled) && !p->pDisabled;
} // Native_Enabled
#endif

// Returns true, if at least one native program can be used
Boolean Native_Possible(Boolean hardCheck)
{
    Boolean possible = hardCheck; // at least used once

#if defined NATIVE_SUPPORT || defined PTOC_SUPPORT
    Boolean   enabled;
    plug_typ *p;
    int       i;

    for (i = 0; i < MAXLIST; i++) {
        p = &pluginList[i];
        if (p->name.host == NULL)
            break;

        possible = hardCheck || (nativeActive && Native_Enabled(p));

        enabled = possible;
        if (enabled)
            return true;
    }
#endif

    return false;
} // Native_Possible

// Returns true, if at least one plugin is connected
Boolean Plugin_Possible(Boolean hardCheck)
{
    Boolean possible = hardCheck; // at least used once

#if defined NATIVE_SUPPORT || defined PTOC_SUPPORT
    Boolean   isBuiltIn, enabled;
    Boolean   natAny = nativeActive || !Native_Empty(true);
    plug_typ *p;
    int       i;

    for (i = 0; i < MAXLIST; i++) {
        p = &pluginList[i];
        if (p->name.host == NULL)
            break;
        char *name = p->name.host;
        isBuiltIn  = name[0] == NUL; // built-in has no name
        possible  = hardCheck || (natAny && Native_Enabled(p));

        enabled = possible && !isBuiltIn;
        if (enabled)
            return true;
    }
#endif

    return false;
} // Plugin_Possible

#if defined NATIVE_SUPPORT || defined PTOC_SUPPORT
static void native_usage(const char *name, const char *swMode)
{
    upe_printf("Syntax:   %s [<opts>] [<native utility>]\n", name);
    upe_printf("Function: Switch %s native utilities\n", swMode);
    upe_printf("Options:\n");

    if (strcmp(swMode, "on/off") == 0) {
        upe_printf("    -e [<native utility>] enable\n");
        upe_printf("    -d [<native utility>] disable\n");
        upe_printf("    -n [<native utility>] no longer specific\n");
        upe_printf("\n");
    }

    upe_printf("    -i   display include list\n");
    upe_printf("    -x      \"    exclude   \" \n");
} // native_usage

static void display_nativeList(Boolean asInclude)
{
    int i, n = 0;

    if (asInclude)
        upe_printf("Include list:\n");
    else
        upe_printf("Exclude list:\n");

    for (i = 0; i < MAXLIST; i++) {
        addrpair_typ *q = MyElem(i, asInclude);
        if (q->host == NULL)
            break;

        if (n > 0 && (n % 5) == 0)
            upe_printf("\n");
        n++;

        upe_printf(" %-15s", q->host);
    }

    if (Native_Empty(asInclude))
        upe_printf("<none>");
    upe_printf("\n");
} // display_nativeList

void display_pluginList(Boolean dispTitle, Boolean atStartup)
{
    const char *bb = " [built-in]";
    const char *tt;
    char        nm[OS9PATHLEN];
    char        form[40];

    size_t    maxN        = 0;
    Boolean   withBuiltIn = false;
    Boolean   isBuiltIn   = false;
    Boolean   done        = false;
    Boolean   enabled;
    plug_typ *p;
    size_t    i, len;
    long      ver;
    ushort    rev;

#ifdef PTOC_SUPPORT
    withBuiltIn = true;
#endif

    if (withBuiltIn)
        maxN = strlen(bb) - 2; // minimum length

    // get the max length
    for (i = 0; i < MAXLIST; i++) {
        p = &pluginList[i];
        if (p->name.host == NULL)
            break;

        len = strlen(p->name.host);
        if (maxN < len)
            maxN = len;
    }
    maxN += 2; // apo addition

    if (!atStartup)
        upe_printf("Plugin list:\n");

    if (dispTitle) {
        for (i = 0; i < MAXLIST; i++) {
            p = &pluginList[i];
            if (p->name.host == NULL)
                break;
            char *name = p->name.host;
            isBuiltIn  = name[0] == NUL;

            if (isBuiltIn)
                strcpy(nm, bb);
            else {
                strcpy(nm, "'"); // Apo
                strcat(nm, name);
                strcat(nm, "'");
            }

            // convert into version and revision
            ver = p->pVersion / (256 * 256);
            rev = (ushort)(ver % 256);
            ver = ver / 256;

            tt = "";
            if (p->numNativeProgs > 1)
                tt = " (n=%d)"; // attention !!

            enabled = Native_Enabled(p);
            if (!enabled)
                tt = " (disabled)";

            sprintf(form, "%s %s%zus %s%s\n", "%s", "%", -maxN, "V%d.%02x", tt);

            tt = "";
            if (atStartup) {
                if (i == 0)
                    tt = "# Connecting plugin module:";
                else
                    tt = "#                          ";
            }

            upe_printf(form, tt, nm, ver, rev, p->numNativeProgs);
            done = true;
        }
    }

    if (!atStartup) {
        if (!done)
            upe_printf("<none>");
        upe_printf("\n");
    }

    /*
    int       i;
    plug_typ* p;
    Boolean   done= false;

    upe_printf( "Plugin list:\n" );

    for (i=0; i<MAXLIST; i++) {
          p= &pluginList[ i ];
      if (p->name     ==NULL) break;

      upe_printf( " %-30s %08X\n", p->name, p->pVersion );
      done= true;
    }

    if (!done) upe_printf( "<none>" );
               upe_printf( "\n" );
    */
} // display_pluginList

static void ShiftDown(int i, Boolean addIt)
// Close the gap of a removed element
{
    addrpair_typ *q = MyElem(i, addIt);

    for (int j = i + 1; j < MAXLIST; j++) {
        addrpair_typ *r = MyElem(j, addIt);
        *q = *r;
        if (r->host == NULL)
            break;
        q = r;
    }
} // ShiftDown

static void RemoveNativeElement(const char *s, Boolean addIt, Boolean *already)
{
    int i;

    *already = false;
    for (i = 0; i < MAXLIST; i++) {
        addrpair_typ *q;

        // Extract, if already in the other list
        q = MyElem(i, !addIt);
        if (q->host != NULL && ustrcmp(q->host, s) == 0) {
            release_mem(*q);
            q->host  = NULL;
            q->guest = 0;
            ShiftDown(i, !addIt);
        }

        // --------------------------------
        q = MyElem(i, addIt);
        if (q->host != NULL && ustrcmp(q->host, s) == 0) {
            if (*already) {
                // Extract if more than once
                release_mem(*q);
                q->host  = NULL;
                q->guest = 0;
                ShiftDown(i, addIt);
            }

            *already = true; // it's there already
        }
    }
}

void ChangeNativeElement(const char *s, Boolean addIt)
{
    int       i;
    plug_typ *p;
    Boolean   already;
    Boolean   found = false;
    void     *modBase;

    RemoveNativeElement(s, addIt, &already);
    if (already)
        return;

    for (i = 0; i < MAXLIST; i++) {
        p = &pluginList[i];
        if (p->name.host == NULL)
            break; // it must be at least once available as native prog
        if (p->is_NativeProg(s, &modBase)) {
            found = true;
            break;
        }
    }
    if (!found)
        return;

    // Insert new element
    for (i = 0; i < MAXLIST - 1; i++) { // last element will not be used
        addrpair_typ *q = MyElem(i, addIt);
        if (q->host == NULL) {
            *q = get_mem(strlen(s));
            strcpy(q->host, s);
            return;
        }
    }
} // ChangeNativeElement

static void ChangePluginElement(const char *s, Boolean enable, Boolean removeIt)
{
    os9err    err;
    int       i;
    plug_typ *p;
    char      suffName[OS9PATHLEN];
    strcpy(suffName, s);
    strcat(suffName, DLL_Suffix());

    for (i = 0; i < MAXLIST; i++) {
        p = &pluginList[i];
        if (p->name.host == NULL)
            break;
        char *name = p->name.host;
        if (name[0] == NUL)
            continue;

        if (ustrcmp(name, s) == 0 || ustrcmp(name, suffName) == 0) {
            if (removeIt) {
                p->pEnabled  = false;
                p->pDisabled = false;
            }
            else {
                err = 0;
                if (enable && !p->pEnabled)
                    err = ConnectDLL(p);
                if (!enable && !p->pDisabled)
                    err = DisconnectDLL(p);
                if (err)
                    break;

                p->pEnabled  = enable;
                p->pDisabled = !enable;
            }

            break;
        }
    }
} // ChangePluginElement

static os9err int_xx(int argc, char **argv, Boolean isOn)
/* Common part for "ion" and "ioff" command */
{
    int         h;
    char       *p;
    const char *v;
    Boolean     dispIn   = false;
    Boolean     dispEx   = false;
    Boolean     removeIt = false;
    Boolean     already;

    if (isOn)
        v = "on";
    else
        v = "off";

    for (h = 1; h < argc; h++) {
        p = argv[h];

        if (*p == '-') {
            p++;
            switch (tolower(*p)) {
            case '?':
            case 'h':
                native_usage(argv[0], v);
                return 0;

                // case 'd' : if (isOn) dispIn= true;
                //            else      dispEx= true;

            case 'i':
                dispIn = true;
                break;
            case 'x':
                dispEx = true;
                break;
            case 'n':
                removeIt = true;
                break;

            default:
                native_usage(argv[0], v);
                upe_printf("Error: unknown option '%c'\n", *p);
                return 1;
            } // switch
        }
        else {
            if (removeIt) {
                RemoveNativeElement(p, true, &already);
                RemoveNativeElement(p, false, &already);
            }
            else {
                if (!dispIn && !dispEx)
                    ChangeNativeElement(p, isOn);
            }
        }
    }

    if (dispIn || dispEx) {
        if (dispIn)
            display_nativeList(true);
        if (dispEx)
            display_nativeList(false);
        return 0;
    }

    if (argc == 1) { // no additional arguments
        nativeActive = isOn;
    }

    return 0;
} // int_xx

static os9err int_on(_pid_, int argc, char **argv)
{
    return int_xx(argc, argv, true);
}
static os9err int_off(_pid_, int argc, char **argv)
{
    return int_xx(argc, argv, false);
}

// ---------------------------------------------------------------------------------
static os9err int_native(_pid_, int argc, char **argv)
// Switch on/off plugin DLLs
{
    int     h;
    char   *p;
    Boolean dispIn   = false;
    Boolean dispEx   = false;
    Boolean removeIt = false;
    Boolean isOn     = false;
    Boolean isOff    = false;
    Boolean specific = false;
    Boolean already;

    for (h = 1; h < argc; h++) {
        p = argv[h];

        if (*p == '-') {
            p++;
            removeIt = false;

            switch (tolower(*p)) {
            case '?':
            case 'h':
                native_usage(argv[0], "on/off");
                return 0;

            case 'd':
                isOff = true;
                if (specific)
                    isOn = false;
                break;
            case 'e':
                isOn = true;
                if (specific)
                    isOff = false;
                break;
            case 'n':
                removeIt = true;
                break;
            case 'i':
                dispIn = true;
                break;
            case 'x':
                dispEx = true;
                break;

            default:
                native_usage(argv[0], "on/off");
                upe_printf("Error: unknown option '%c'\n", *p);
                return 1;
            } // switch
        }
        else {
            if (removeIt) {
                RemoveNativeElement(p, true, &already);
                RemoveNativeElement(p, false, &already);
            }
            else if (!dispIn && !dispEx && isOn != isOff) {
                ChangeNativeElement(p, isOn);
                specific = true;
            }
        }
    }

    if (dispIn) {
        display_nativeList(true);
        return 0;
    }
    if (dispEx) {
        display_nativeList(false);
        return 0;
    }

    if (!specific && isOn != isOff) { // no additional arguments
        nativeActive = isOn;
    }

    return 0;
} // int_native

// ---------------------------------------------------------------------------------
static void plugin_usage(char *name)
{
    upe_printf("Syntax:   %s [<opts>]\n", name);
    upe_printf("Function: Switch on/off plugin DLLs\n");
    upe_printf("Options:\n");
    upe_printf("    -e [<plugin DLL>] enable\n");
    upe_printf("    -d [<plugin DLL>] disable\n");
    upe_printf("    -n [<plugin DLL>] no longer specific\n");
    upe_printf("\n");
    upe_printf("    -l                display list\n");
} // plugin_usage

static os9err int_plugin(_pid_, int argc, char **argv)
// Switch on/off plugin DLLs
{
    int     h;
    char   *p;
    Boolean disp     = false;
    Boolean removeIt = false;
    Boolean isOn     = false;
    Boolean isOff    = false;
    Boolean specific = false;

    for (h = 1; h < argc; h++) {
        p = argv[h];

        if (*p == '-') {
            p++;
            switch (tolower(*p)) {
            case '?':
            case 'h':
                plugin_usage(argv[0]);
                return 0;

            case 'd':
                isOff = true;
                if (specific)
                    isOn = false;
                break;
            case 'e':
                isOn = true;
                if (specific)
                    isOff = false;
                break;
            case 'n':
                removeIt = true;
                break;
            case 'l':
                disp = true;
                break;

            default:
                plugin_usage(argv[0]);
                upe_printf("Error: unknown option '%c'\n", *p);
                return 1;
            } // switch
        }
        else {
            if (removeIt) {
                ChangePluginElement(p, true, true);
            }
            else if (!disp && isOn != isOff) {
                ChangePluginElement(p, isOn, false);
                specific = true;
            }
        }
    }

    if (disp) {
        display_pluginList(true, false);
        return 0;
    }

    if (!specific && isOn != isOff) { // no additional arguments
        pluginActive = isOn && Plugin_Possible(true);
    }

    return 0;
} // int_plugin

// ---------------------------------------------------------------------------------
static os9err int_thread(_pid_, _argc_, _argv_)
{
    ptocThread = true;
    return 0;
}
static os9err int_nothread(_pid_, _argc_, _argv_)
{
    ptocThread = false;
    return 0;
}
static os9err int_arb(_pid_, _argc_, _argv_)
{
    fullArb = true;
    return 0;
}
static os9err int_noarb(_pid_, _argc_, _argv_)
{
    fullArb = false;
    return 0;
}

// ---------------------------------------------------------------------------------
static os9err native_calls(ushort pid, _argc_, char **argv)
{
    os9err       err;
    char        *name = argv[0];
    process_typ *cp   = &procs[pid];
    // plug_typ*      p;
    int            i;
    Boolean        isBuiltIn, enabled;
    void          *modBase;
    nativeinfo_typ ni;

#ifdef THREAD_SUPPORT
    // mutex lock for systemcalls
    if (ptocThread)
        pthread_mutex_lock(&sysCallMutex);
    currentpid = pid;
#endif

    for (i = 0; i < MAXLIST; i++) {
        cp->plugElem = &pluginList[i];
        if (cp->plugElem->name.host == NULL)
            break;
        char *name = cp->plugElem->name.host;
        isBuiltIn  = name[0] == NUL; // built-in has no name

        enabled = Native_Enabled(cp->plugElem);
        if (enabled && cp->plugElem->is_NativeProg(name, &modBase)) {
            cp->isPlugin = !isBuiltIn;
            break;
        }
    }

#ifdef THREAD_SUPPORT
    // mutex unlock for systemcalls
    if (ptocThread)
        pthread_mutex_unlock(&sysCallMutex);
#endif

    err = E_MNF; // default, if not found
    if (cp->plugElem->name.host) {
        ni.pid      = &currentpid;
        ni.modBase  = os9modules[cp->mid].modulebase;
        ni.os9_args = (void *)cp->my_args;
        ni.cbP      = &g_cb;

        err = cp->plugElem->start_NativeProg(name, &ni);
    }

    cp->isPlugin = false;
    cp->plugElem = NULL;
    return err;
} // native_calls
#endif

static os9err int_hit(_pid_, _argc_, _argv_)
{
    const int NBlk = 4;
    const int MaxL = MAXDIRHIT / NBlk;

    int  i, j, hi, n = 0, iLast = 0, diff = 0;
    char s[10];
    char v[10];

    for (i = 1; i < MAXDIRHIT; i++) {
        hi = hittable[i];
        diff += hi;
        n += hi * i; // weighted
        if (hi > 0)
            iLast = i;
    }

    if (diff == 0)
        strcpy(s, "");
    else
        sprintf(s, " %5.3f", (float)n / (float)diff);

    upo_printf("File name hash field hit rate:%s\n", s);

    for (i = 0; i < MaxL; i++) {
        for (j = 0; j < NBlk; j++) {
            n  = i + j * MaxL;
            hi = hittable[n];

            if (n == 0) {
                strcpy(s, "unused");
            }
            else {
                if (hi == 0)
                    strcpy(s, "      ");
                else
                    sprintf(s, "%5.1f%%", (float)hi / (float)diff * 100);
            }

            if (hi == 0)
                sprintf(v, "-");
            else
                sprintf(v, "%d", hi);

            if (n <= iLast)
                upo_printf("%3d:%6s %s%s", n, v, s, j < NBlk - 1 ? "  " : "");

            if (j == NBlk - 1 && (iLast >= MaxL || iLast >= i))
                upo_printf("\n");
        }
    }

    return 0;
} // int_hit

static os9err int_crash(_pid_, _argc_, _argv_)
{
    ulong *a;

    a = (ulong *)0xCE00BEF0; /* non existing address */
    a = (ulong *)*a;
    upe_printf("a=%08X\n", a); /* must do something with <a>, because high */
    /* optimizing system would remove the <a> assignment !! */
    return 0;
}

static os9err int_quit(_pid_, _argc_, _argv_)
{
    quitFlag = true;
    return 0;
}

static os9err int_ignored(_pid_, _argc_, _argv_) { return 0; /* do nothing */ }

/* Command table */
/* ------------- */

typedef os9err (*intcmdfunc)(ushort pid, int argc, char **argv);

typedef struct {
    char      *iName;
    intcmdfunc iRoutine;
    char      *iHelpText;
} cmdtable_typ;

cmdtable_typ commandtable[] = {
    {"ihelp/icmds", int_help, "shows this help text"},
    {"dhelp", debug_help, "shows debug flag information"},
    {"stop/shutdown", int_stop, "exit from OS9exec"},
    {"rename", int_rename, "renames a file or directory (100% compatible)"},
    {"move", int_move, "moves files and directories"},
    {"ls", int_dir, "shows dir in [extended] format"},

#ifdef RBF_SUPPORT
    {"mount", int_mount, "mount   (RBF) device"},
    {"unmount", int_unmount, "unmount (RBF) device"},
#endif

    {"systime", int_systime, "emulation timing management/display"},
    {"iprocs", int_procs, "shows OS9exec's processes"},
    {"imdir", int_mdir, "shows OS9exec's loaded OS-9 modules"},
    {"ipaths", int_paths, "shows OS9exec's path list"},
    {"imem", int_mem, "shows OS9exec's memory block list"},
    {"ihit", int_hit, "shows OS9exec's directory hash hit rate"},


    {"idevs", int_devs, "shows OS9exec's devices"},
    {"idbg/debughalt",
     int_debughalt,
     "sets debug options/enters OS9exec's debug menu"},
    {"icrash", int_crash, "accesses an invalid address: 0xCE00BEFO"},
    {"iquit", int_quit, "sets flag to quit directly"},
    {"dch/diskcache",
     int_ignored,
     "simply ignored, because not supported by OS9exec"},

#if defined NATIVE_SUPPORT || defined PTOC_SUPPORT
    {"ion", int_on, "Switch on     native utilities (default)"},
    {"ioff", int_off, "Switch off    native utilities"},
    {"native", int_native, "Switch on/off native utilities"},
    {"plugin", int_plugin, "Switch on/off plugin DLLs"},
    {"ithread", int_thread, "Threading     native utilities"},
    {"inothread", int_nothread, "Direct call   native utilities (default)"},
    {"iarb", int_arb, "Full          arbitration"},
    {"inoarb", int_noarb, "Std           arbitration      (default)"},
    {"native_calls", native_calls, "Native calls"},
#endif

    {NULL, NULL, NULL}};

/* show available internal commands */
os9err int_help(ushort pid, _argc_, _argv_)
{
    int         k;
    const char *nm;

    upho_printf("%s internal commands:\n", OS9exec_Name());
    upho_printf(
        "(case sensitive, so use uppercase to use external versions)\n");
    upho_printf("\n");
    for (k = 0;; k++) {
        nm = commandtable[k].iName;
        if (nm == NULL)
            break;
        if (!nativeActive && strcmp(nm, "") == 0)
            break;

        upho_printf("  %-14s: %s\n", nm, commandtable[k].iHelpText);
    }

    if (pid == 0)
        upo_printf("\n");
    return 0;
}

/* Routines */
/* -------- */

static int IntCmdIndex(const char *name)
{
    cmdtable_typ *cp;
    char         *p;
    char         *q;
    char          part[OS9NAMELEN];
    int           index = 0;

    while (true) {
        cp = &commandtable[index];
        p  = cp->iName;
        if (p == NULL)
            return -1; // not found

        // search for eventual sub strings
        do {
            q = strstr(p, PSEP_STR); /* more than one part ? */
            if (q != NULL) {
                strncpy(part, p, q - p);
                part[q - p] = NUL; /* nul termination */
                p           = part;
            }

            if (strcmp(p, name) == 0)
                return index; // found

            p = q + 1;
        } while (q != NULL);

        index++;
    }
} // IntCmdIndex

// checks if command is internal
// -1 if not,
// command index otherwise
//
int isintcommand(const char *name, Boolean *isNative, void **modBaseP)
{
#if defined NATIVE_SUPPORT || defined PTOC_SUPPORT
    int         i;
    plug_typ   *p;
    Boolean     ok = nativeActive;
    Boolean     isBuiltIn, enabled;
    const char *cut = name + strlen(name) - 1;
#endif

    int index = 0;
    *isNative = false;
    *modBaseP = NULL;

#if defined NATIVE_SUPPORT || defined PTOC_SUPPORT
    // get the pure file name from eventual abs path name
    while (cut > name) {
        if (*cut == '/') {
            cut++;
            break;
        }
        cut--;
    }

    for (i = 0; i < MAXLIST; i++) {
        addrpair_typ *q = MyElem(i, !nativeActive);
        if (q->host == NULL)
            break;

        // Consider include/exclude list
        if (ustrcmp(q->host, cut) == 0) {
            ok = !ok;
            break;
        }
    }

    if (ok) {
        ok = false; // start again testing

        for (i = 0; i < MAXLIST; i++) {
            p = &pluginList[i];
            if (p->name.host == NULL)
                break;
            char *name = p->name.host;
            isBuiltIn  = name[0] == NUL; // built-in has no name

            enabled = Native_Enabled(p);
            if (enabled && p->is_NativeProg(name, modBaseP)) {
                ok = true;
                break;
            }
        }
    }

    if (!ok)
        *modBaseP = NULL; // just to be sure
    *isNative = ok;
#endif

    if (*isNative)
        name = "native_calls"; // one entry point for all
    index = IntCmdIndex(name);
    return index;
} // isintcommand

/* print error message in OS-9 format */
os9err _errmsg(os9err err, char *format, ...)
{
    char    obuf[300];
    va_list vp;
    va_start(vp, format);
    upe_printf("%s: ", icmname);
    vsprintf(obuf, format, vp);
    va_end(vp);

    upe_printf(obuf);
    return err;
}

os9err prepArgs(char *arglist, ushort *argcP, addrpair_typ *arguments_ptr)
/* prepare arguments for internal commands
 * will return a allocated block of memory with argv[] and args
 * Note: only simple arg checking is used (argv[] is built from space
 *       delimiter information.
 */
{
    ushort       argc, k;
    char        *p, *cp;
    char       **pp;
    int          scanarg;
    addrpair_typ localargv_ptr;
    char       **localargv;
    argc    = 0; /* none found yet */
    p       = arglist;
    scanarg = false;

    while (*p >= 0x20) {
        if (!scanarg) {
            if (*p != 0x20) {
                argc++;
                scanarg = true; /* now scanning argument */
            }
        }
        else {
            if (*p == 0x20)
                scanarg = false; /* end of arg found */
        }

        p++;
    }

    localargv_ptr = get_mem((argc + 1) * sizeof(char **));
    localargv     = localargv_ptr.host;

    debugprintf(
        dbgUtils,
        dbgNorm,
        ("# prepArgs: arglist @ $%08lX argc=%d\n", (ulong)arglist, argc));

    argc    = 0; /* start again */
    p       = arglist;
    scanarg = false;

    while (*p >= 0x20) {
        if (!scanarg) {
            if (*p != 0x20) {
                localargv[argc] = p; /* remember beginning of next arg */
                argc++;
                scanarg = true; /* now scanning argument */
            }
        }
        else {
            if (*p == 0x20)
                scanarg = false; /* end of arg found */
        }
        p++;
    }

    /* p now points behind last arg char */
    localargv[argc] = p; /* save as end pointer */

    *arguments_ptr = get_mem(p - arglist + (argc + 1) * sizeof(char **) + 1);
    pp             = arguments_ptr->host;
    if (pp == NULL)
        return os9error(E_NORAM);

    p = (char *)(pp + argc + 1);
    debugprintf(dbgUtils,
                dbgDeep,
                ("# prepArgs: argc=%d, argv[] @ $%lX, args @ $%lX\n",
                 argc,
                 (ulong)pp,
                 (ulong)p));

    /* now copy argv[] and args, leave argv[ 0 ] free */
    for (k = 0; k < argc; k++) {
        pp[k + 1] = p; /* pointer to arg, leave argv[0] free */
        for (cp = localargv[k]; cp < localargv[k + 1]; cp++)
            *p++ = *cp;
        if (*(p - 1) == 0x20)
            *(p - 1) = 0;
        else
            *p++ = 0; /* replace space by terminator or append it at end */
    }

    debugprintf(
        dbgUtils,
        dbgDeep,
        ("# prepArgs: prepared argc=%d, params @ $%08lX\n", argc, (ulong)pp));

    release_mem(localargv_ptr);

    /* return args */
    *argcP = argc + 1; /* include argv[ 0 ] */
    return 0;
}

static void large_pipe_connect(ushort pid, syspath_typ *spC)
{
    pipe_typ *pipe = &spC->u.pipe;

#ifdef PIP_SUPPORT
    ulong         n;
    pipechan_typ *p, *q;

    if (pipe->i_svd_pchP == NULL) {    /* no pipe is in saved status */
        pipe->i_svd_pchP = pipe->pchP; /* save it */

        getPipe(pid, spC, ICMDPBUFF);                    /* get the new pipe */
        pipe->pchP->sp_lock = pipe->i_svd_pchP->sp_lock; /* inherit lock */

        p = pipe->i_svd_pchP; /* make copies into local variables */
        q = pipe->pchP;

        /* is there still something in the buffer ? */
        while (true) {
            // n=  p->pwp-p->prp;
            // if (p->pwp<p->prp) n+= p->size; /* wrapper */

            n = Pipe_NReady(p);
            if (n == 0)
                break;

            *(q->pwp++) = *(p->prp++); /* put into the new buffer */
            if (p->prp >= p->buf + p->size)
                p->prp = p->buf; /* wrap */
        }
    }

#else
#endif
}

#ifdef THREAD_SUPPORT
typedef struct {
    ushort pid;
    int    index;
    int    argc;
    char **argv;
} ThreadVars;

// The POSIX thread function, must be passed the Thread Object address as
// parameter
// extern "C" void * IntCmdThread( void* threadVars );
void *IntCmdThread(ThreadVars *t)
{
    os9err       err;
    process_typ *cp = &procs[t->pid];
    process_typ *pp;
    cp->tid = pthread_self();

    err = (commandtable[t->index].iRoutine)(t->pid, t->argc, t->argv);

    // mutex lock for systemcalls
    pthread_mutex_lock(&sysCallMutex);
    currentpid = t->pid;

    pp          = &procs[os9_word(cp->pd._pid)];
    pp->pd._cid = cp->pd._sid; /* restore former child id */
                               // upe_printf( "end of %d: %d\n", t->pid, err );
    cp->exiterr = err;
    sig_mask(t->pid, 0); /* activate queued intercepts */
    kill_process(t->pid);
    free(t);
    cp->tid = NULL;

    // mutex unlock for systemcalls
    pthread_mutex_unlock(&sysCallMutex);

    // Exit thread now
    pthread_exit((void *)0);
    return NULL;
} // IntCmdThread

static void
PrepareParams(ushort pid, int index, int argc, char **argv, ThreadVars **t)
{
    int   blk  = sizeof(ThreadVars) + argc * sizeof(void *);
    int   size = blk;
    char *p;

    int i;
    for (i = 0; i < argc; i++) {
        size += strlen(argv[i]) + 1;
        if ((size % 2) == 1)
            size++;
    }

    *t          = malloc(size);
    (*t)->pid   = pid;
    (*t)->index = index;
    (*t)->argc  = argc;
    (*t)->argv  = (*t) + 1;

    p = (char *)*t + blk;

    for (i = 0; i < argc; i++) {
        (*t)->argv[i] = p;
        strcpy(p, argv[i]);
        p += strlen(p) + 1;
        if (((ulong)p % 2) == 1)
            p++;
    }
} // PrepareParams
#endif

/* executes internal command */
os9err callcommand(char    *name,
                   ushort   pid,
                   ushort   parentid,
                   int      argc,
                   char   **argv,
                   Boolean *asThread)
{
    os9err       err;
    int          index;
    ushort       sp;
    syspath_typ *spP;
    syspath_typ *spC;
    process_typ *cp = &procs[pid];
    // process_typ* pa= &procs[ parentid ];
    void *modBase = NULL;

#ifdef THREAD_SUPPORT
    ulong       rslt;
    pthread_t   threadID;
    ThreadVars *t;
#endif

    icmpid  = pid;  /* save as global for ported utilities and _errmsg */
    icmname = name; /* dito */

    index     = isintcommand(name, &cp->isNative, &modBase);
    *asThread = ptocThread && cp->isNative;

    if (index < 0)
        return os9error(E_PNNF);

    // save it
    if (pid != parentid) // avoid F$Chain blocking
        set_os9_state(parentid,
                      pWaiting,
                      "IntCmd (in)"); // make it waiting, for PtoC arbitration

    /* There is a problem: during internal commands, multitasking can't
     * be active. If intcommands are used via telnet, the tty/pty connection
     * will not work. The only solution is to make the tty/pty buffer large
     * enough that the whole output can be written into this buffer.
     * After intcommand is finished, buffer will be emptied normally. If
     * the buffer is empty for the first time, this buffer will be released
     * again. The following sequence creates the larger buffer.
     * the same problems occurs with normal pipes also.
     *
     * There is 2nd solution for this problem: If THREAD_SUPPORT is active,
     * the internal command(s) runs as a thread(s) in parallel to the rest, so
     * the tty/pty will run normally. To avoid parallel access to kernel
     * routines, a mutex will be set up, to have only one process there
     * at the same time.
     *
     * The 3rd solution is to arbitrate at system calls of internal
     * utilities. This will be done for all PtoC utilities.
     * The large buffer will be connected for this case as well.
     */
    strcpy(cp->intProcName, name);

    if (!*asThread) {
        sp  = cp->usrpaths[usrStdout];
        spC = NULL; /* not yet in use */
        spP = get_syspathd(pid, sp);

        if (spP->type == fTTY)
            spC = get_syspathd(pid, spP->u.pipe.pchP->sp_lock);
        if (spP->type == fPipe)
            spC = spP; /* no crossover */

        if (spC != NULL && !cp->isNative)
            large_pipe_connect(pid, spC);
    }

    if (logtiming) {
        xxx_to_arb(F_Fork, pid);
        arb_to_os9(false);
    }
    debugprintf(dbgUtils,
                dbgNorm,
                ("# call internal '%s' (before) pid=%d\n", name, pid));

    if (cp->isNative)
        debug_return(&cp->os9regs, pid, false);

#ifdef THREAD_SUPPORT
    if (*asThread) {
        PrepareParams(pid, index, argc, argv, &t);
        rslt = pthread_create(&threadID, NULL, IntCmdThread, t);
        err  = 0;
    }
#endif

    if (!*asThread) {
        currentpid = pid;
        err        = (commandtable[index].iRoutine)(pid, argc, argv);
    }

    debugprintf(dbgUtils,
                dbgNorm,
                ("# call internal '%s' (after)  pid=%d\n", name, pid));
    if (logtiming && !cp->isNative)
        os9_to_xxx(pid);

    if (pid != parentid) // F$Chain adaption
        set_os9_state(parentid,
                      pActive,
                      "IntCmd (out)"); // make it active again
    return err;
}

/* call of external Win/DOS commands for OS9exec/nt */
os9err call_hostcmd(char *cmdline, ushort pid, int moreargs, char **argv)
{
#if defined MACOSX
    os9err err;

#pragma unused(pid, moreargs, argv)
    err = system(cmdline);
    return host2os9err(err, E_IFORKP);

#else

    // Call MPW command???
    // Launch program???
    upe_printf("Calling external commands not yet implemented!\n");
    return 0;
#endif
}

/* eof */
