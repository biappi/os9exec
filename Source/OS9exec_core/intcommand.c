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
/* (c) 1993-2002 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        beat.forster@ggaweb.ch              */
/**********************************************/



/* OS9exec/nt 2.0 internal commands */
/* ================================ */

/* global includes */
#include "os9exec_incl.h"

/* global vars */
ushort icmpid; /* current internal command's PID */
char *icmname; /* current internal command's name = argv[0] */
/* -------------------------- */


#define Mx 10



#ifdef macintosh
  static os9err int_debugger(ushort pid, int argc, char **argv)
  /* internal debugger command */
  {
      #pragma unused(pid, argc, argv)
    
      uphe_printf("debugger internal command calls Mac OS debugger\n");
      DebugStr("\pDebugger entered from OS9exec/NT's debugger command");
      return 0;
  } /* int_debugger */
#endif



static os9err int_debughalt(ushort pid, int argc, char **argv)
/* OS9exec debug halt */
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    ushort k;
    Boolean opt;
    Boolean fullScreen= false;
    char    *p;
    ushort  *usp;
    ushort  level;
    
    /* get arguments and options */
    opt=false;
    for (k=1; k<argc; k++) {
        p=argv[k];
        if (*p=='-') {
            switch (tolower(*++p)) {
                case '?' :
                case 'h' :  upe_printf("Usage: %s [options]\n",argv[0]);
                            upe_printf("Function: enter internal debug mode\n");
                            upe_printf("Options:\n" );
                            upe_printf("    -d[n] msk    set debugging info mask [of level n, x=all, default=0]\n");
                            upe_printf("    -s    msk    set debugging stop mask (default=0)\n");
                            upe_printf("    -dh          show debug/stop mask help\n");
                            upe_printf("    -m           only show current debug/stop masks\n");
                            upe_printf("    -n=[<name>]  set name to trigger/disable trigger (causes debug stop)\n");
                            upe_printf("    -q           quit emulator\n");
                            upe_printf("    -o=<syspath> redirect debug output to <syspath>\n");
                            upe_printf("    -w=<pid>     do not generate debug output for <pid>\n");
    						upe_printf("    -x=<width>   define MGR screen width\n" );
    						upe_printf("    -y=<height>  define MGR screen height\n" );
    						upe_printf("    -z           define MGR fullscreen mode\n" );
                            return 0;
                            
                case 'm' :  break;
                case 's' :  usp= &debughalt; goto getmask;
                case 'd' :  level=0; /* default to level 0 */
                            if (*(p+1)=='h') { debug_help( 0,0,NULL ); return 0; }
                            if (*(p+1)=='x') { level=3; usp=&debug[0]; goto getmask; }
                            else if (isdigit(*(p+1))) {
                                level=*(p+1)-0x30;
                                if (level>DEBUGLEVELS) level=0; /* default to 0 if level invalid */
                            }
                            usp=&debug[level]; goto getmask;
                            getmask:
                            
                            if (*(p+1)=='=') p+=2;
                            else {  k++; /* next arg */
                                if (k>=argc) { printf("# Error: missing mask(hex) for '%c' option!\n",*p); return 1; }
                                p= argv[k];
                            }
                            
                            if (sscanf( p,"%hx", usp )!=1) {
                                printf("# Error in hex mask '%s'\n",p);
                                return 1;
                            }
                            if (level>2) debug[2]=debug[1]=debug[0]; /* set all */
                            debug_prep(); /* adjust debug masks */
                            break;
                                
                case 'n' :  k++; /* next arg */
                                if (k>=argc) {triggername[0]=0; break; }
                                strncpy(triggername,argv[k],TRIGNAMELEN);
                                break;

                case 'q' :  quitFlag= true;
                            upo_printf ("\n");
                            upho_printf( "OS9 emulation stopped\n"); 
                            fflush(stdout);
                            exit(0);
            
                case 'w' :  if (*(p+1)=='=') p+=2;
                            else {  k++; /* next arg */
                                if (k>=argc)    { without_pid= os9_word(procs[pid].pd._pid); break; }
                                p= argv[k];
                            }
                            if (sscanf( p,"%d",  &without_pid )<1)  without_pid= 0;
                            break;
                            
                case 'j' :  if (*(p+1)=='=') p+=2;
                            else {  k++; /* next arg */
                                if (k>=argc)   { justthis_pid= os9_word(procs[pid].pd._pid); break; }
                                p= argv[k];
                            }
                            if (sscanf( p,"%d", &justthis_pid )<1) justthis_pid= 0;
                            break;
            
                case 'o' :  if (*(p+1)=='=') p+=2;
                            else {  k++; /* next arg */
                                if (k>=argc) { dbgOut= 0; break; }
                                p= argv[k];
                            }
                            
                            if (sscanf( p,"%d", &dbgOut )<1) dbgOut= 0;
                            break;
				
				case 'z' :  fullScreen= true; break; /* full screen mode */

                case 'x' :  if (*(p+1)=='=') p+=2;
                            else {  k++; /* next arg */
                                if (k>=argc) break;
                                p= argv[k];
                            }
                            
                            if (sscanf( p,"%d", &screenW )<1) screenW= 0;
                            break;
                
                case 'y' :  if (*(p+1)=='=') p+=2;
                            else {  k++; /* next arg */
                                if (k>=argc) break;
                                p= argv[k];
                            }
                            
                            if (sscanf( p,"%d", &screenH )<1) screenH= 0;
                            break; 

                default  :  printf("# Error: unknown option '%c'!\n",*p); return 1;
            }
            opt=true;
        }
    }

	if (fullScreen) {
		screenW= 0; /* full screen mode */
		screenH= 0;
	}

    if (opt) {
        uphe_printf("Debug Masks = $%04X,$%04X,$%04X, Stopmask = $%04X, Trigger='%s'\n",debug[dbgNorm],debug[dbgDetail],debug[dbgDeep],debughalt,triggername);
    }
    else {
        debugwait();
    }
    return 0;
} /* int_debughalt */



/* show procs */
static os9err int_procs( ushort pid, int argc, char **argv )
{
    #ifndef linux
    #pragma unused(pid,argc,argv)
    #endif
    
    show_processes(); return 0;
} /* int_procs */



/* show modules */
static os9err int_mdir( ushort pid, int argc, char **argv )
{
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    char*         cmp= NULL;
    if (argc>1)   cmp= argv[1];
    show_modules( cmp ); return 0;
} /* int_mdir */



static void ipaths_usage( char* name, ushort pid )
{
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    upe_printf( "Syntax:   %s [<pid>]\n", name );
    upe_printf( "Function: Print OS9exec system paths\n" );
    upe_printf( "Options:  None.\n" );
} /* ipaths_usage */



/* OS9exec internal path list */
static os9err int_paths( ushort pid, int argc, char **argv )
{
    #ifndef linux
    #pragma unused(pid)
    #endif

	#define IPATHS_MAXARGS 1
    int     nargc=0, h;
    char*   p;
    int     k= MAXPROCESSES;

    for (h=1; h<argc; h++) {
             p= argv[h];    
        if (*p=='-') { 
            p++;
            switch (tolower(*p)) {
                case '?' :  ipaths_usage( argv[0],pid ); return 0;
                default  :  uphe_printf("Error: unknown option '%c'!\n",*p); 
                            ipaths_usage( argv[0],pid ); return 1;
            }   
        }
        else {
            if (nargc>=IPATHS_MAXARGS) { 
                uphe_printf("Error: Only 1 argument allowed\n"); return 1;
            }

			k= atoi(p);
			nargc++;
        }
    } /* for */
    
    show_files( k ); return 0;
} /* int_paths */



static os9err int_mfree( ushort pid, int argc, char **argv)
/* "imem": OS9exec internal allocated memory */
{
    #ifndef linux
    #pragma unused(pid,argc,argv)
    #endif

    show_mem(MAXPROCESSES); return 0;
} /* int_mfree */




static void idevs_usage( char* name, ushort pid )
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    upe_printf( "Syntax:   %s [<opts>]\n", name );
    upe_printf( "Function: Print OS9exec device table\n" );
    upe_printf( "Options:\n" );
    upe_printf( "    -r    show RBF devices only\n" );
    upe_printf( "    -s    show statistic values\n" );
} /* idevs_usage */




static void devs_printf( syspath_typ* spP, char* driv, char* fmgr )
{
    char    s[OS9NAMELEN];
    strcpy( s,spP->name );
    
    upo_printf( "%-10s %-10s %-7s %2d  %3s %s\n", 
                 StrBlk_Pt( s,Mx ), driv, fmgr, spP->nr, "","" );
} /* devs_printf */



static os9err int_devs( ushort pid, int argc, char **argv )
/* idevs": OS9exec internal devices */
{
    #ifndef linux
    #pragma unused( pid,argc,argv )
    #endif

    #define IDEVS_MAXARGS 0
    int     nargc=0, h;
    char*   p;
    Boolean rbf_devs = false;
    Boolean statistic= false;
    
    int           ii;
    ttydev_typ*   tdev;
    pipechan_typ* pch;
    syspath_typ*  spP;

    for (h=1; h<argc; h++) {
        p= argv[h];    
        if (*p=='-') { 
            p++;
            switch (tolower(*p)) {
                case '?' :  idevs_usage( argv[0],pid ); return 0;
                case 'r' :  rbf_devs = true; break;
                case 's' :  statistic= true; break;
                default  :  uphe_printf("Error: unknown option '%c'!\n",*p); 
                            idevs_usage( argv[0],pid ); return 1;
            }   
        }
        else {
            if (nargc>=IDEVS_MAXARGS) { 
                uphe_printf("Error: no arguments allowed\n"); return 1;
            }
            nargc++;
        }
    } /* for */
           
            
    upo_printf( "%s  %s\n", hw_name,sw_name );
    upo_printf( "\n" );
    
    
    if (statistic) {
        upo_printf( "Device     read: miss /     total  write: miss /     total\n" );
        upo_printf( "---------- ----------------------  -----------------------\n" );
    }
    else {
        upo_printf( "Device     Driver     FileMgr nr sect wProt Image\n" );
        upo_printf( "---------- ---------- ------- -- ---- ----- -----------------------------------\n" );
    }
    
    #ifdef RBF_SUPPORT 
      Disp_RBF_Devs( statistic );
    #endif
    
    if (!statistic && !rbf_devs) {
        #ifdef windows32
              tdev= &main_mco; 
          if (tdev->installed && tdev->spP!=NULL)
              devs_printf( tdev->spP, "console","scf" );
        #endif

                     spP= &syspaths[sysStdnil];
        devs_printf( spP, "null",    "scf" );
                     spP= &syspaths[sysVMod];
        devs_printf( spP, "vmod_drv","scf" );
    
        for (ii=0; ii<MAXTTYDEV; ii++) {
                tdev= &ttydev[ii]; 
            if (tdev->installed && tdev->spP!=NULL) {
                devs_printf      ( tdev->spP, "tty","scf" );
            
                    pch= tdev->spP->u.pipe.pchP;
                if (pch!=NULL)   { spP= &syspaths[pch->sp_lock];
                    devs_printf( spP, "pty","scf" );
                }
            }
        } /* for */
    } /* if */
    
    return 0;
} /* int_devs */




static os9err int_quit( ushort pid, int argc, char **argv )
{
    #ifndef linux
    #pragma unused(pid,argc,argv)
    #endif

    quitFlag= true; return 0;
} /* int_quit */



static os9err int_ignored( ushort pid, int argc, char **argv )
{
    #ifndef linux
    #pragma unused(pid,argc,argv)
    #endif

    /* do nothing */
    return 0;
} /* int_ignored */




/* Command table */
/* ------------- */

typedef os9err (*intcmdfunc)(ushort pid, int argc, char **argv);

/* MWC: removed const %%% */
typedef struct {
                char*      name;
                intcmdfunc routine;
                char*      helptext;
               } cmdtable_typ;



cmdtable_typ commandtable[] =
{
    { "rename",        int_rename,    "renames a file or directory (100% compatible)" },
    { "move",          int_move,      "moves files and directories" },
    { "ls",            int_dir,       "shows dir in [extended] format" },

    #ifdef RBF_SUPPORT
    { "mount",         int_mount,     "mount   (RBF) device" },
    { "unmount",       int_unmount,   "unmount (RBF) device" },
	#endif
	
    #ifdef windows32
    { "cmd",           int_wincmd,    "calls Windows Command Line / DOS command" },
    #endif
    
    { "systime",       int_systime,   "emulation timing management/display" },
    { "iprocs",        int_procs,     "shows OS9exec's processes" },
    { "imdir",         int_mdir,      "shows OS9exec's loaded OS-9 modules" },
    { "ipaths",        int_paths,     "shows OS9exec's path list" },
    { "imem",          int_mfree,     "shows OS9exec's memory block list" },
    { "idevs",         int_devs,      "shows OS9exec's devices" },
    { "ihelp/icmds",   int_help,      "shows this help text" },
    { "dhelp",         debug_help,    "shows debug flag information" },
    { "debughalt/idbg",int_debughalt, "sets debug options/enters OS9exec's debug menu" },

    #ifdef macintosh
    { "debugger",      int_debugger,  "directly calls Mac OS debugger" },
    #endif

    { "iquit",         int_quit,      "sets flag to quit directly" },
    { "dch/diskcache", int_ignored,   "simply ignored, because not supported by OS9exec" },

    { NULL, NULL, NULL } /* terminator */
};


/* show available internal commands */
os9err int_help(ushort pid, int argc, char **argv)
{
    #ifndef linux
    #pragma unused(pid,argc,argv)
    #endif

    int k;
        
    upho_printf("%s internal commands:\n", OS9exec_Name() );
    upho_printf("(case sensitive, so use uppercase to use external versions)\n");
    upho_printf("\n");
    for (k=0; commandtable[k].name!=NULL; k++) {
        upho_printf("  %-14s: %s\n",commandtable[k].name,commandtable[k].helptext);
    }

    if (pid==0) upo_printf("\n");
    return 0;
} /* int_help */



/* Routines */
/* -------- */


/* checks if command is internal
 * -1 if not,
 * command index otherwise
 */
int isintcommand(char *name)
{
    cmdtable_typ* cp;
    int           index= 0;
    char          *p, *q;
    char          part[OS9NAMELEN];
    
    if (!with_intcmds) return -1;
    
    while (true) {
           cp= &commandtable[index];
        p= cp->name; if (p==NULL) break;

        do {    q= strstr( p,PSEP_STR ); /* more than one part ? */
            if (q!=NULL) {
                strncpy( part,p, q-p );
                part[q-p]= NUL; /* nul termination */
                p= part;
            }

            if (strcmp( p,name )==0) return index;
            p= q+1;
        } while (q!=NULL);
        
        index++;
    } /* loop */
    
    return -1;
} /* isintcommand */


/* print error message in OS-9 format */
os9err _errmsg(os9err err, char* format, ...)
{
    char obuf[300];
    va_list vp;
    va_start(vp,format);
    upe_printf("%s: ",icmname);
    vsprintf(obuf,format,vp);
    va_end(vp);
    
    upe_printf(obuf);
    return err;
} /* _errmsg */


/* prepare arguments for internal commands
 * will return a allocated block of memory with argv[] and args
 * Note: only simple arg checking is used (argv[] is built from space
 *       delimiter information.
 */
os9err prepArgs(char *arglist, ushort *argcP, char*** argP)
{
    ushort argc,k;
    char *p, *cp;
    char **pp;
    #define MAXARGS 20
    char *localargv[MAXARGS];
    int scanarg;
    
    argc=0; /* none found yet */
    p=arglist;
    scanarg=false;
    debugprintf(dbgUtils,dbgDeep,("# prepArgs: arglist @ $%08lX\n",(ulong)arglist));
    while (*p >= 0x20) {
        if (argc>MAXARGS) return os9error(E_NORAM);
        if (!scanarg) {
            if (*p!=0x20) {
                localargv[argc]=p; /* remember beginning of next arg */
                argc++;
                scanarg=true; /* now scanning argument */
            }
        }
        else {
            if (*p==0x20) scanarg=false; /* end of arg found */
        }
        p++;
    }
    /* p now points behind last arg char */
    localargv[argc]=p; /* save as end pointer */
    
    #ifdef MACMEM
    pp=(char **)NewPtr(        (p-arglist+(argc+1)*sizeof(char**)+1)); 
    #else
    pp=(char **)malloc((size_t)(p-arglist+(argc+1)*sizeof(char**)+1));
    #endif
    if (pp==NULL) return os9error(E_NORAM);
    
    p=(char *)(pp + argc +1);
    debugprintf(dbgUtils,dbgDeep,("# prepArgs: argc=%d, argv[] @ $%lX, args @ $%lX\n",argc,(ulong)pp, (ulong)p));
    /* now copy argv[] and args, leave argv[0] free */
    for (k=0; k<argc; k++) {
        pp[k+1]=p; /* pointer to arg, leave argv[0] free */
        for (cp=localargv[k]; cp<localargv[k+1]; cp++) *p++=*cp;
        if (*(p-1)==0x20) *(p-1)=0; else *p++=0; /* replace space by terminator or append it at end */
    }
    debugprintf(dbgUtils,dbgDeep,("# prepArgs: prepared argc=%d, params @ $%08lX\n",
                                     argc,(ulong)pp));
    /* return args */
    *argP=&pp[0];
    *argcP=argc+1; /* include argv[0] */
    return 0;
} /* prepArgs */



static void large_pipe_connect( ushort pid, syspath_typ* spC )
{
    pipe_typ* pipe= &spC->u.pipe;
    
    #ifdef PIP_SUPPORT
      ulong        n;
      pipechan_typ *p,*q;
      
      if (pipe->i_svd_pchP==NULL) {         /* no pipe is in saved status */
          pipe->i_svd_pchP= pipe->pchP;                        /* save it */
            
          getPipe( pid, spC, ICMDPBUFF );             /* get the new pipe */
          pipe->pchP->sp_lock= pipe->i_svd_pchP->sp_lock; /* inherit lock */
            
          p= pipe->i_svd_pchP;        /* make copies into local variables */
          q= pipe->pchP;
            
          /* is there still something in the buffer ? */
          while (true) {
              n=  p->pwp-p->prp;
              if (p->pwp<p->prp) n+= p->size; /* wrapper */
              if (n<=0) break;
                
              *(q->pwp++)= *(p->prp++);   /* put into the new buffer */
              if  (p->prp >= p->buf+p->size) p->prp= p->buf; /* wrap */
          } /* while */
      } /* if */
    
    #else
      #ifndef linux
      #pragma unused(pid)
      #endif
    #endif
} /* large_pipe_connect */



/* executes internal command */
os9err callcommand(char *name, ushort pid, int argc, char **argv)
{
    os9err       err;
    int          index;
    ushort       sp;
    syspath_typ* spP;
    syspath_typ* spC;

    icmpid = pid; /* save as global for ported utilities and _errmsg */
    icmname= name; /* dito */
    
        index= isintcommand(name);
    if (index<0) return os9error(E_PNNF);

    /* there is a problem: during internal commands, multitasking can't
       be active. If intcommands are used via telnet, the tty/pty connection
       will not work. The only solution is to make the tty/pty buffer large
       enough that the whole output can be written into this buffer.
       After intcommand is finished, buffer will be emptied normally. If
       the buffer is empty for the first time, this buffer will be released
       again. The following sequence creates the larger buffer.
       the same problems occurs with normal pipes also */
    sp = procs[pid].usrpaths[usrStdout];
    spC= NULL; /* not ye in use */
    spP= get_syspathd( pid,sp ); 

    if (spP->type==fTTY ) spC= get_syspathd( pid,spP->u.pipe.pchP->sp_lock );
    if (spP->type==fPipe) spC= spP; /* no crossover */
    
    if (spC!=NULL) large_pipe_connect( pid,spC );


    if (logtiming) { xxx_to_arb( F_Fork, pid );
                     arb_to_os9( false ); }
    debugprintf(dbgUtils,dbgNorm,("# call internal '%s' (before) pid=%d\n", name,pid ));
    
    err= (commandtable[index].routine)( pid,argc,argv );
        
    debugprintf(dbgUtils,dbgNorm,("# call internal '%s' (after)  pid=%d\n", name,pid ));
    if (logtiming)   os9_to_xxx( pid, name );

    return err;
} /* callcommand */


/* call of external Win/DOS commands for OS9exec/nt */
os9err call_hostcmd(char *cmdline, ushort pid, int moreargs, char **argv)
{
    #ifdef windows32
      PROCESS_INFORMATION procinfo;
      STARTUPINFO      startupinfo;
      DWORD exitcode;
      char cl[MAX_PATH];
      int k;
      char* p;
      process_typ* cp= &procs[pid];
    
      /* create command line */
      strcpy( cl,"cmd /c " );
      strcat( cl, cmdline  );
      
      k=0;
      while (moreargs-- > 0) {
          if (cl[0]!=0) strcat(cl," "); // space if not first
          strcat(cl,argv[k++]);
      }
      
      debugprintf(dbgUtils,dbgNorm,("# call_hostcmd '%s'\n", cl ));
      
      p=  cp->d.path;  // pointer to current directory name 
      if (cp->d.type!=fDir) p= startPath;
      
      /* Now create new process */
      GetStartupInfo( &startupinfo );
      if (CreateProcess(
            NULL,           // pointer to name of executable module
            cl,             // pointer to command line string
            NULL,           // pointer to process security attributes 
            NULL,           // pointer to thread security attributes 
            true,           // handle inheritance flag
            0,              // creation flags 
            NULL,           // pointer to new environment block (%%% implement that later?)
            p,              // pointer to current directory name 
            &startupinfo,   // pointer to STARTUPINFO (using that of myself)
            &procinfo)) {   // pointer to PROCESS_INFORMATION  
          /* process created ok */
          WaitForSingleObject(procinfo.hProcess, INFINITE);

          // Get exit code
          exitcode= 0;
          GetExitCodeProcess(procinfo.hProcess, &exitcode);

          // Close process and thread handles. 
          CloseHandle(procinfo.hProcess);
          CloseHandle(procinfo.hThread);
          return host2os9err( exitcode,E_IFORKP );
      }
      
      return host2os9err( GetLastError(),E_IFORKP );
    
    #else
      #ifndef linux
      #pragma unused(cmdline,pid,moreargs,argv)
      #endif

      // Call MPW command???
      // Launch program???
      uphe_printf("Calling external commands not yet implemented!\n");  
      return 0;
    #endif  
} /* call_hostcmd */

/* eof */

