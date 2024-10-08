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
 *    Revision 1.30  2007/03/24 12:30:24  bfo
 *    <isRBF> dependency introduced
 *
 *    Revision 1.29  2007/01/28 21:27:21  bfo
 *    -k: use '*_dbg' plugin dlls / closing the last path correctly
 *
 *    Revision 1.28  2007/01/07 14:02:58  bfo
 *    Using MACOSX instead of __MACH__
 *
 *    Revision 1.27  2007/01/02 11:31:31  bfo
 *    2007 text adaption
 *
 *    Revision 1.26  2006/12/01 20:04:34  bfo
 *    Enhanced (and shorter) "systime" display
 *
 *    Revision 1.25  2006/11/12 21:08:18  bfo
 *    "-v" description added
 *
 *    Revision 1.24  2006/11/12 13:32:59  bfo
 *    "-?" text updated / <userOpt> adaption for MacOSX/Linux
 *
 *    Revision 1.23  2006/10/25 20:36:27  bfo
 *    <dbgAnomaly> flag will be switched on by default
 *
 *    Revision 1.22  2006/08/29 22:04:13  bfo
 *    an error of tcgetattr() will not terminate the program
 *    (this error happens e.g. with CW in debugger mode)
 *
 *    Revision 1.21  2006/08/20 18:34:59  MG
 *    restore_term(void) / setup_term(void) added
 *    (Changes done by Martin Gregorie)
 *
 *    Revision 1.20  2006/07/29 08:49:54  bfo
 *    "adaptor" => "adapter"
 *
 *    Revision 1.19  2006/06/17 14:19:44  bfo
 *    <CR> adaption
 *
 *    Revision 1.18  2006/06/17 11:09:47  bfo
 *    Global var <withTitle> introduced
 *
 *    Revision 1.17  2006/06/07 16:08:15  bfo
 *    XCode / IntelMacs supported now
 *
 *    Revision 1.16  2006/06/02 18:59:09  bfo
 *    Option -g activated
 *
 *    Revision 1.15  2006/06/01 21:04:39  bfo
 *    g_ipAddr things added
 *
 *    Revision 1.14  2006/02/19 15:47:30  bfo
 *    Header changed to 2006
 *
 *    Revision 1.13  2005/07/06 21:07:57  bfo
 *    defined UNIX
 *
 *    Revision 1.12  2005/07/02 14:23:42  bfo
 *    Adapted for Mach-O
 *
 *    Revision 1.11  2005/06/30 11:54:36  bfo
 *    Mach-O support
 *
 *    Revision 1.10  2005/01/22 16:19:37  bfo
 *    Windows path search adaption
 *
 *    Revision 1.9  2004/12/04 00:10:56  bfo
 *    MacOSX MACH adaptions
 *
 *    Revision 1.8  2004/11/20 11:44:08  bfo
 *    Changed to version V3.25 (titles adapted)
 *
 *    Revision 1.7  2003/05/17 10:32:15  bfo
 *    UserOpt activated for Carbon (gs_ready problem, same as Linux version)
 *
 *    Revision 1.6  2002/10/09 20:14:03  bfo
 *    "show_timing" call with additional parameter <ticksLim>
 *
 *
 */

#include <ctype.h>

/* includes */
/* ======== */

#include "os9exec_incl.h"

#include <termios.h>
#include <inttypes.h>

/* statics */
/* ======= */

int   commentoutput;  /* commenting stdout */
int   disablefilters; /* filtering stdout */
uint32_t memplus;        /* additional memory for first process */
uint32_t iniprior;       /* priority for first process */

struct termios savedmodes; /* saved terminal attributes     */

/* locally defined procedures */
Boolean F_Avail(const char *pathname);
void    restore_term(void);
Boolean setup_term(void);
/* -------------------------- */

/* default standard output filter */
/* ============================== */

/* This routine is called by OS9exec/NT to output to stdout/stderr whenever
 * no dedicated filter procedure could be found for the executing process.
 * This standard writeline() implementation preceedes each non-empty output
 * line with a MPW comment character "#", unless RAWOUTPUT is defined.
 * Outputting informational and progress messages in comment format is standard
 * for MPW utilities.
 */
void writeline(char *linebuf, FILE *stream)
{
    /* --- output line first */
    if (*linebuf != 0) {
        if (commentoutput) {
            fputs("# ", stream);
        }
        fputs(linebuf, stream);
    }

    putc('\n', stream);
    fflush(stream);
}

Boolean F_Avail(const char *pathname)
{
    struct stat info;
    Boolean     fOK, isd, ok;

    fOK = stat(pathname, &info) == 0; /* file available ? */
    if (!fOK) {
        debugprintf(dbgStartup,
                    dbgNorm,
                    ("# F_Avail '%s' (not found)\n", pathname));
        return false;
    }

    isd = IsTrDir(info.st_mode);   /* it is a directory ? */
    ok  = isd || info.st_size > 0; /* size is > 0 */
    debugprintf(dbgStartup,
                dbgNorm,
                ("# F_Avail '%s' %s (%s) size=%d %08X\n",
                 pathname,
                 ok ? "true" : "false",
                 isd ? "dir" : "file",
                 info.st_size,
                 info.st_mode));
    return ok;
}

char *egetenv(const char *name)
{
    static char tmp[OS9PATHLEN];
    static char ocm[OS9PATHLEN]; /* Can't use <tmp> recursively */

    Boolean u_disk, u_cmds, u_mdir;
    char   *q;
    char   *sv;
    char   *rslt;
    Boolean cm = false;
    Boolean isRBF;

    Boolean isWin = false;

    /* getenv for use under MPW and for PC/Linux version */
    rslt = getenv(name);

    u_disk = (ustrcmp(name, "OS9DISK") == 0);
    u_cmds = (ustrcmp(name, "OS9CMDS") == 0);
    u_mdir = (ustrcmp(name, "OS9MDIR") == 0);

    if (u_disk || u_cmds || u_mdir) {
        if (rslt == NULL) {
            if (u_disk) {
                strcpy(ocm, PATHDELIM_STR);
                strcat(ocm, "dd");
                rslt = ocm;
            }

            if (u_cmds) {
                strcpy(ocm, egetenv("OS9DISK"));
                strcat(ocm, PATHDELIM_STR);
                strcat(ocm, "CMDS");
                rslt = ocm;
                cm   = true;
            }

            if (u_mdir) {
                strcpy(ocm, PATHDELIM_STR);
                strcat(ocm, "OS9MDIR");
                rslt = ocm;
            }
        }
        sv = rslt;

        isRBF = isWin && cm && IO_Type(1, startPath, 0) == fRBF;
        // if (!F_Avail(rslt) || isWin) some problems, mix is best of both
        // worlds ??
        if (!F_Avail(rslt) || isRBF || (isWin && !cm)) {
            // upe_printf( "ty=%d fRBf=%d\n", IO_Type( 1, startPath, 0 ), fRBF
            // ); // get device type: Mac/PC or RBF

            if (cm)
                rslt = "/dd/CMDS"; /* make it suitable for RBF devices */
            else {
                debugprintf(dbgStartup,
                            dbgNorm,
                            ("# startPath: '%s'\n", startPath));
                strcpy(tmp, startPath);
                if (*rslt != PATHDELIM)
                    strcat(tmp, PATHDELIM_STR);

                strcat(tmp, rslt);
                rslt = tmp;
                debugprintf(dbgStartup, dbgNorm, ("# startPath: '%s'\n", rslt));
            }
        }

        if (!cm && !F_Avail(rslt)) {
            rslt = sv;
            strcpy(tmp, startPath);
            q = tmp + strlen(tmp) - 1;
            while (*q != PATHDELIM)
                q--;
            *q = NUL; /* cut the string at delimiter */

            if (*rslt != PATHDELIM)
                strcat(tmp, PATHDELIM_STR);
            strcat(tmp, rslt);
            rslt = tmp;
        }
    }

    debugprintf(dbgStartup, dbgNorm, ("# egetenv %s: '%s'\n", name, rslt));
    return rslt;
}

/* advance cursor and force time slicing */
void eAdvanceCursor(void)
{
    // sleep(1);
}

/* spin cursor (event processing) */
void eSpinCursor(short incr)
{
#if defined MACOSX
    // sleep(1);

#else
    // sleep(1);

#endif
}

/* os9 utility program */
/* =================== */

/* routines to display special wish text */
static void show_wish(void)
{
    upho_printf(
        "I do not allow using this program to apply any kind of violence\n");
    upho_printf("to anybody.\n");
    upho_printf("As there is no absolute criterium for violent applications of "
                "software,\n");
    upho_printf("I trust your subjective interpretation - if it's honest for "
                "you, it's ok\n");
    upho_printf("for me.\n");
    upho_printf("Disregarding this statement means not respecting my intention "
                "I had when\n");
    upho_printf("writing this software and therefore would void your right to "
                "use this\n");
    upho_printf("program.\n");
    upho_printf("Lukas Zeller, September 1996\n");
    upo_printf("\n");
}

/* display os9 usage */
static void os9_usage(char *name)
{
    /* print help */
    upho_printf("Usage:    %s [options] [<os9program> [<os9parameters>,...]]\n",
                name);
    upho_printf("Function: Executes OS-9 program (which may start other os9 "
                "processes\n");
    upho_printf("          itself).\n");
    upho_printf("          <os9program> must be present within the tool's "
                "'OS9C' resources,\n");
    upho_printf(
        "          in the {OS9MDIR} directory or in the {OS9CMDS} directory\n");

    upho_printf("Options:\n");
    upho_printf("   -c          comment output to MPW window (preceede lines "
                "with #)\n");
    upho_printf("   -i          disable internal commands\n");
    upho_printf("   -k          use '*_dbg' plugin dlls\n");
    upho_printf("   -ih         show internal command help\n");
    upho_printf(
        "   -o          disable output filtering (error message conversion)\n");
    upho_printf("   -oh         show available output filters\n");
    upho_printf("   -t          enable timing measurements\n");
    upho_printf("   -m  n[k|M]  Give 1st OS-9 process extra static storage "
                "(kilo/mega)\n");
    upho_printf("   -mm n[k|M]  Give all OS-9 process extra static storage "
                "(kilo/mega)\n");
    upho_printf("   -p prio     Run  1st OS-9 process with prio (default=%d, "
                "NOIRQ>=%d)\n",
                MYPRIORITY,
                IRQBLOCKPRIOR);
    upho_printf("   -d[n] msk   set  debug info mask [of level n, default=0] "
                "(default=1)\n");
    upho_printf("   -s msk      set  debug stop mask (default=0)\n");
    upho_printf("   -dh         show debug/stop mask help\n");
    upho_printf("   -n [name]   set name to trigger (causes debug stop)\n");
    upho_printf("   -w n        set waitnextevent() interval in 1/60 sec "
                "(default=%d)\n",
                DEFAULTSPININTERVAL);
    upho_printf("   -fe         use Fetch FTP name translation: '.' at "
                "filename beginning\n");
    upho_printf("               will be converted to '/' on Mac HFS)\n");
    upho_printf("   -f[o]       use os9exec 1.14 dummy F$Fork emulation (no "
                "fork, but\n");
    upho_printf(
        "               only writing command line to stdout; -fo causes\n");
    upho_printf("               cmd line generation for execution with 'os9' "
                "MPW tool)\n");
    upho_printf("   -x width    define MGR screen width\n");
    upho_printf("   -y height   define MGR screen width\n");
    upho_printf("   -z          define MGR fullscreen mode\n");
    upho_printf("   -g ip_addr  open   MGR screen at IP address\n");
    upho_printf("   -u          user defined option\n");
    upho_printf("   -v          ctrl-C will stop OS9exec immediately\n");
    upho_printf("   -h[h]       show this help [and conditions for using the "
                "software]\n");
    upho_printf("\n");

    upho_printf("%s\n", OS9exec_Name());
    upho_printf(
        "1993-2007 by luz/bfo ( luz@synthesis.ch / bfo@synthesis.ch )\n");
    upo_printf("\n");
}

static void GetStartTick()
{
    //  struct tm tim; /* Important Note: internal use of <tm> as done in OS-9
    //  */ int    old_sec;
    //
    //  GetTim( &tim );
    //  old_sec= tim.tm_sec;
    //
    //  do { /* this loop can take up to 1 second of time !! */
    //      GetTim( &tim );
    //  } while     (tim.tm_sec==old_sec);
    //
    startTick = 0; /* Initialize to 0, because subtracted in "GetSystemTick" */
    startTick = GetSystemTick();
    /* now the tick counter is synchronised to the second changing, as in OS-9
     */
}

/* Set the the terminal for unbuffered, no-echo operation.
   If errors are found they will be reported but the function
   exits normally so its caller can handle the cleanup. */
Boolean setup_term()
{
    int reply = 0;

    struct termios modes;

    reply = tcgetattr(0, &modes); /* retrieve terminal attrs */
    if (reply == 0) {
        /*
            store the original modes so they can be restored
        */
        savedmodes = modes;

        /*
            change the attributes to set
            character at a time input and
            turn off echo.
        */

        modes.c_cc[VMIN]  = 0; /* non-blocking mode  */
        modes.c_cc[VTIME] = 1; /* return after 100 mSec */
        modes.c_lflag &= ~ICANON;
        modes.c_lflag &= ~(ECHO | ECHOE | ECHOK /*| ECHOKE*/);

        /*
            set up the terminal for OS-9
        */
        reply = tcsetattr(0, TCSAFLUSH, &modes);
        if (reply) {
            upo_printf("Error %s: %s\n",
                       "setting up os9exec's terminal attributes",
                       strerror(errno));
        }
    }
    else {
        upo_printf("Error reading initial terminal settings: %s\n",
                   strerror(errno));
    }

    return (reply == 0);
} // setup_term

/* Restore the original terminal operation.
   It is invoked as part of the exit() function, so
   thje error report isn't followed by an exit() */
void restore_term()
{
    int reply;

    struct termios modes;

    modes = savedmodes;
    reply = tcsetattr(0, TCSAFLUSH, &modes);
    if (reply)
        upo_printf("Error restoring normal terminal operation: %s\n",
                   strerror(errno));
} // restore_term

// main program
void os9_main(int argc, char **argv, char **envp)
{
    int     k, kX;
    char   *p;
    uint32_t *ulp;
    ushort *usp;
    char    modifier;
    char   *toolname;
    ushort  level;
    ushort  err;
    Boolean no_app     = true;
    Boolean fullScreen = false;

    // set default options
    // - debug options
    debug[dbgNorm]   = 1; // normal debug flags
    debug[dbgDetail] = 0;
    debug[dbgDeep]   = 0;
    debughalt        = 0;

    // - other options
    commentoutput  = 0;
    disablefilters = 0;
    memplus        = 0;
    dummyfork      = 0;
    fetchnames     = 0;
    iniprior       = MYPRIORITY;

    get_hw(); // make this very very early in program
    getversion(&exec_version, &exec_revision);

    sw_name = "OS-9/68k Emu UAE";

    sec0 = 0;

    // set up global init stuff, so user path output will work
    os9exec_globinit();
    GetStartTick();

    // start the logging system
    init_syscalltimers();

    // get arguments and options
    for (k = 1; k < argc; k++) {
        p = argv[k];

        if (*p == '-') {
            switch (tolower(*++p)) {
            case '?':
            case 'h':
                if (*(p + 1) == 'h') {
                    show_wish();
                    exit(0);
                }
                os9_usage(argv[0]);
                exit(0);

            case 'f':
                if (*(p + 1) == 'e') {
                    fetchnames = true;
                    break;
                }
                if (*(p + 1) == 'o')
                    dummyfork = 2;
                else
                    dummyfork = 1;
                break;

            case 'i':
                if (*(p + 1) == 'h') {
#ifdef INT_CMD
                    int_help(0, 0, NULL);
#endif
                    exit(0);
                }

                with_intcmds = false;
                break;
            case 'k':
                with_dbgDLLs = true;
                break;
            case 'c':
                commentoutput = true;
                break;

            case 'o':
                if (*(p + 1) == 'h') {
                    printfilters();
                    exit(0);
                }
                disablefilters = true;
                break;

            case 't':
                logtiming_disp = true;
                break;

            case 'n':
                k++; /* next arg */
                if (k >= argc) {
                    triggername[0] = 0;
                    break;
                }
                strncpy(triggername, argv[k], TRIGNAMELEN);
                break;

            case 's':
                usp = &debughalt;
                goto getmask;

            case 'd':
                level = 0; /* default to level 0 */
                if (*(p + 1) == 'h') {
                    debug_help(0, 0, NULL);
                    exit(0);
                }
                if (isdigit(*(p + 1))) {
                    level = *(p + 1) - 0x30;
                    if (level > DEBUGLEVELS)
                        level = 0; /* default to 0 if level invalid */
                }
                usp = &debug[level];
                goto getmask;

            getmask:
                k++; /* next arg */
                if (k >= argc) {
                    printf("# Error: missing mask(hex) for '%c' option!\n", *p);
                    exit(1);
                }
                p = argv[k];
                if (sscanf(p, "%hx", usp) != 1) {
                    printf("# Error in hex mask '%s'\n", p);
                    exit(1);
                }
                break;

            case 'u':
                userOpt = true;
                break; // set user option
            case 'v':
                catch_ctrlC = false;
                break; // don not install a ctrl C handler
            case 'z':
                fullScreen = true;
                break; // full screen mode

            case 'g':
                if (g_ipAddr == NULL) {
                    k++; /* next arg */
                    if (k >= argc) {
                        printf("# Error: missing argument for '%s' option!\n",
                               p);
                        exit(1);
                    }

                    p        = argv[k];
                    g_ipAddr = malloc(strlen(p) + 1);
                    strcpy(g_ipAddr, p);
                }
                break;

            case 'x':
                ulp = &screenW;
                goto getlnum;
            case 'y':
                ulp = &screenH;
                goto getlnum;
            case 'w':
                ulp = &spininterval;
                goto getlnum;
            case 'p':
                ulp = &iniprior;
                goto getlnum;
            case 'm':
                if (*(p + 1) == 'm') {
                    ulp = &memplusall;
                    goto getlnum;
                }
                ulp = &memplus;
                goto getlnum;

            getlnum:
                k++; /* next arg */
                if (k >= argc) {
                    printf("# Error: missing argument for '%s' option!\n", p);
                    exit(1);
                }

                p        = argv[k];
                modifier = 0;
                if (*p == '$') {
                    if (sscanf(++p, "%" SCNx32 "%c", ulp, &modifier) < 1) {
                        printf("# Error in hex number '$%s'\n", p);
                        exit(1);
                    }
                }
                else {
                    if (sscanf(p, "%" SCNd32 "%c", ulp, &modifier) < 1) {
                        printf("# Error in decimal number '%s'\n", p);
                        exit(1);
                    }
                }

                switch (tolower(modifier)) {
                case 'm':
                    *ulp *= 1024; /* fall into Kbytes */
                case 'k':
                    *ulp *= 1024;
                case 0:
                    break;
                default:
                    printf("# Error in modifier: '%c', must be 'k', 'M' or "
                           "none\n",
                           modifier);
                    exit(1);
                } // switch
                break;

            default:
                printf("# Error: unknown option '%c'!\n", *p);
                os9_usage(argv[0]);
                exit(1);
            } // switch
        }
        else {
            no_app = false;
            kX     = k;
            break;
        }
    }

    // not enough arguments
    if (no_app) {
#ifdef KEINEAPP
        printf("# Not enough arguments. Type \"%s -h\"to get help\n", argv[0]);
        printf("# Usage:    %s [options] <os9program> [<os9parameters>,...]\n",
               argv[0]);
        exit(1);
#endif

        kX       = argc - 1;
        toolname = ""; // will be replaced by "shell" later
    }
    else {
        toolname = argv[kX];
    }

    if (fullScreen) {
        screenW = 0; // full screen mode
        screenH = 0;
    }

    if (userOpt)
        catch_ctrlC = false;

    // Set the terminal modes and hook mode restoration to the exit function.
    // If setting then fails, the error will already have been reported
    // for suitably detailed fault analysis but the function exits so
    // os9_main() can handle the cleanup: at present this is just to exit.
    if (setup_term())
        atexit(restore_term);

    // now here starts the os9 command line: go execute
    debug_prep(); // make sure debug info is adjusted

    // don't print before setup
    err = os9exec_nt(toolname,
                     argc - kX - 1,
                     argv + kX + 1,
                     envp,
                     memplus,
                     iniprior);
    if (logtiming_disp)
        show_timing(STIM_NONE, 1, false);

    // always one new line
    upo_printf("\n");

    // end message, if <withTitle> = calling shell/sh
    if (withTitle)
        upho_printf("OS-9 emulation ends here.\n");

    fflush(stdout);
    exit(err);
} // os9_main
/* eof */
