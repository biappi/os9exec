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
/*        forsterb@dial.eunet.ch              */
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
 *    Revision 1.8  2002/06/25 20:44:33  luz
 *    Added /lp printer support under windows. Not tested or even compiled for Mac
 *
 */

/* Revision History
 * ================
 * 000  : 93-06-01:  Created
 * 001  : 93-06-26:  First working version (with icc6811, a6801 and xlink) (-->PhilM, Simon Kaiser)
 * 002  : 93-06-27:  Some minor improvements in file handling and error reporting. Made I$ReadLn not
 *                   return the CR char (this seems to be what is expected, at least by xlib).
 * 003  : 93-07-10:  added spinning cursor, made IRQ proof (no masking any more), made GetStt/SS_Opt
 *                   work, made I$ReadLn/stdin work correctly, added F$Link, F$UnLink, I$Write
 *                   and I$Read. Added GetStt/SS_DevNm for stdpaths.
 * 004  : 93-07-15:  Added F$SRtMem. Obviously, making SS_Opt work in GetStt caused some icc output
 *                   routines behave differently and return blocks, which was never the case before
 *                   and caused crashes because E$UnkSvc was returned. Now it seems to work.
 * 005  : 93-07-20:  Added stderr. Added correct MPW exit codes. Added atexit-cleanup mechanism.
 *                   Added ResolvePath to path processing to resolve aliases within OS-9 paths.
 * 006  : 93-08-10:  Added CACHE_FLUSH code to make it run on the 040
 *                   Added F$ID, F$SetSys, F$GPrDsc
 * 007  : 93-08-11:  Added constants for process ID etc. (MYPROCID...)
 * 008  : 93-08-21:  Implemented F$TLink and Trap handler mechanisms. Improved parsepath to handle
 *                   /dd and parent dirs correctly. Implemented GetStt:SS_Size and GetStt:SS_Pos.
 * 009  : 93-08-23:  F$Time, F$Julian made work, F$CRC added (Foese)
 * 010  : 94-04-09:  Made os9exec return error code to main program instead of exiting.
 *
 * 1.11 : 94-09-25:  Added environment variables (passing all MPW environment variables with
 *                   names starting with '@'. Started real VERSION/REVISION numbering
 * 1.12 : 94-10-11:  Added F_SRqCMem, SS_FD and improved seek to make Ultra C work ok.
 * 1.13 : 94-10-22:  Added real file mod/crea dates in SS_FD.
 * 1.14 : 94-10-23:  Added HoldMemory()/UnholdMemort() calls to os9malloc()/os9free() for VM case.
 *                
 * 2.00 : 96-08-27:  Started cooperative multitprocess version. First Demoes issued at OS9 Conference '96
 * CONTINUED in separate History file!
 *
 * Note: VERSION/REVISION is in 'vers' ID=2 resource in os9exec_nt.r 
 */



/* includes */
/* ======== */

#include "os9exec_incl.h"

				
/* global variables */
/* ================ */

/* the "module directory" */
module_typ 	os9modules[MAXMODULES];
mod_exec*   init_module;
ulong       totalMem;

/* the system paths */
syspath_typ syspaths[MAXSYSPATHS];
ulong       syspth  [MAXSYSPATHS];

/* the SCSI devices */
scsi_typ    scsi    [MAXSCSI];

/* definitions for TTY */
ttydev_typ  ttydev  [MAXTTYDEV];		

/* the processes */
process_typ procs   [MAXPROCESSES];

/* the signal queue */
sig_typ     sig_queue;

/* the alarms */
alarm_typ   alarms     [MAXALARMS];
alarm_typ*  alarm_queue[MAXALARMS];

/* the dir table */
#ifdef windows32
  char*     dirtable[MAXDIRS];
#endif

/* I/O device table */
/* not really used, avid "devs" from crashing */
byte		devs[0x0900];		

/* the OS-9 statistics table */
st_typ		statistics[MAX_OS9PROGS];


/* the file file mgrs routine table */
fmgr_typ    fmgr_none,
            fmgr_cons,
            fmgr_nil,
            fmgr_scf,
            fmgr_file,
            fmgr_dir,
            fmgr_pipe,
            fmgr_pty,
            fmgr_rbf,
            fmgr_net,
            fmgr_printer;

/* the events */
event_typ   events  [MAXEVENTS];
ulong		newEventId= 0xffffffff; /* initial value */


/* the currently executing process, MAXPROCESSES if none */
ushort   currentpid;     /* currently executing process */
short    arbitrate;      /* set if arbitrate() should switch away from one running process to next */
ushort   interactivepid; /* process that will get keyboard abort signals */
dir_type mdir;	         /* current module dir */


char     startPath[OS9PATHLEN];   /* start path */
char     strtUPath[OS9PATHLEN];   /* next higher than start path */

#ifdef macintosh
  /* the MPW-level default directory */
  short  startVolID;	          /* startup dir's volume id    */
  long   startDirID;	          /* startup dir's directory id */
  char    callPath[OS9PATHLEN];
  
  short   applVolID;	          /* startup dir's volume id    */
  long    applDirID;	          /* startup dir's directory id */
  char    applName[OS9PATHLEN];
  
#else
  /* the default module load directory OS9MDIR */
  char    mdirPath[MAX_PATH];	  /* current mdir path */
#endif

/* the windows console definitions */
#ifdef windows32
  HANDLE hStdin;
#endif

#ifdef win_linux
  ttydev_typ   main_mco;
  short	       gConsoleNLExpand  = true;
  int   	   gConsoleID        =    0;
  syspath_typ* g_spP             = NULL;
  int   	   gLastwritten_pid  =    0;
  int		   gConsoleQuickInput= true;
  char         gTitle[OS9NAMELEN];
#endif

#ifdef MPW
  int  mpwsignal; /* the MPW signal flag */
#endif

/* flag which allows empty read functionality for multiple terminal support */
Boolean devIsReady   = true; /* true if char has been read (multiple terminal) */

/* this avoids recursion problems */
Boolean in_recursion = false;

/* global settings */
int     dbgOut        = -1;
int      without_pid  =  0;
int     justthis_pid  =  0;
Boolean quitFlag	  = false;
Boolean userOpt		  = false;
Boolean logtiming     = true; /* syscall loging, used by int cmd "systime" */
Boolean logtiming_disp= false;
Boolean async_area    = false;
Boolean async_pending = false;
short   defSCSIAdaptNo= -1; // unknown
short   defSCSIBusNo  =  0; // first
l2_typ  l2;
ulong   my_inetaddr   = os9_long(0x7F000001); /* loopback by default: 127.0.0.1 */


/* tickCount at start of the program */
ulong  startTick= 0; /* Initialize to 0, because subtracted in "GetSystemTick" */
ulong   lastTick= 0;

#ifdef linux
  ulong sec0;
#endif

/* error traceback */
ushort errpid;			/* PID of process that generated that error */
ushort lastsyscall;		/* last system call */
char*  lastpathparsed;	/* last pathstring parsed (NULL if none) */

/* the software's version and revision */
ushort appl_version;
ushort appl_revision;
ushort exec_version;
ushort exec_revision;
char*  hw_site;
char*  hw_name;
char*  sw_name;
char*  platform;

/* screen size */
ulong  screenW= -1; /* undefined */
ulong  screenH= -1;



/* global options */
/* -------------- */

/* fork mode */
int dummyfork= 0;				/* if set, forks are done as in os9exec 1.14 (only command printout to stdout) */

/* Fetch FTP name translation */
int fetchnames; /* if set, OS9-filenames beginning with '.' will be translated to begin with '/' on the mac */

/* internal commands */
Boolean with_intcmds  = true;   /* by default, internal commands will be used */

/* cursor spinning interval in 1/60 seconds */
ulong   spininterval= DEFAULTSPININTERVAL;

/* mount device name and write protection   */
char*   mnt_name      = "";
int     mnt_scsi      = NO_SCSI;
short   mnt_scsiAdapt = -1; // use default adaptor
short   mnt_scsiBus   = -1; // use default bus
short   mnt_scsiLUN   = -1; // use default LUN
Boolean mnt_wProtect  = false;
/* ---------------------------------------- */





/* Routines */
/* ======== */


/* prepare parameter area from argv/argc envp
 * Note: If routine exits w/o error, it has malloc()-ed a block at pap
 *       containing the parameter structure which must be freed afterwards.
 */
static os9err prepParams(mod_exec *theModule, char **argv,int argc, char**envp, ulong* psiz, byte **pap)
{
   ulong paramsiz, argsiz, envsiz;
   int os9envc,h;
   byte *p,*pp, *hp;
   ulong *alp,*elp;
   char *modnam;
   int k;
   
   debugprintf(dbgStartup,dbgDeep,("# prepParams: argv=$%08lX, argc=%d, envp=$%08lX\n",(ulong) argv, argc, (ulong) envp));
   /* -- initial preparations */
   modnam= Mod_Name( theModule );
   /* -- calculate parameter size */
   paramsiz = 2 /* 000D param delimiter */ + 1 /* env term */
            + 2 /* FC01 code */ + 4 /* argv[0] pointer */ + 1 /* argv[0] term */
            + 2 /* 000D word */ + 4 /* argv[] list beginning */ + 4 /* envp[] separator */
            + 4 /* envp[] terminator */;
   /* -- calculate argument string size */
   for (argsiz=0,k=0;k<argc;k++) argsiz+=strlen(argv[k])+1; /* string length+space */
   if (argc!=0) argsiz--; /* last argument is not followed by a space. */
   if (argsiz & 1) paramsiz++; /* align needed if odd argsize */
   /* -- calculate environment string size */
   for (envsiz=0,k=0,os9envc=0;envp[k]!=NULL && *envp[k]!=0;k++) {
      if (*envp[k]=='@') {
         /* -- OS-9 environment variables must start with a '@' */			
         h=strlen(envp[k]); /* env variable name */
   		debugprintf(dbgStartup,dbgDeep,("# prepParams: counting envp[%d]='%s', len=%d, reserved=%d\n",k,envp[k],h,h+strlen(envp[k]+h+1)+1));
         envsiz+=h+strlen(envp[k]+h+1)+1; /* env variable contents plus one for the '=' */
         os9envc++; /* count OS-9 environment variables */
      }
   }
   if (os9envc!=0) envsiz--; /* last env is not followed by a space. */
   if ((envsiz & 1)==0) paramsiz++; /* another align needed if even envsize */
   debugprintf(dbgStartup,dbgDeep,("# prepParams: os9envc=%d,envsiz=%d, argc=%d,argsiz=%d\n",os9envc,envsiz,argc,argsiz));
   if (!(strlen(modnam) & 1)) paramsiz++; /* align if module name has even # of chars (=odd with NUL) */
   paramsiz+=argsiz+argc*4+envsiz+os9envc*4+strlen(modnam); /* add module name plus pointer spaces */
   /* -- allocate buffer for it */

   #ifdef MACMEM
   pp=NewPtr(paramsiz);
   #else
   pp=malloc((size_t)paramsiz);
   #endif
   if (pp==NULL) return os9error(E_NORAM);
   
   /* -- prepare parameters shell-like */
   debugprintf(dbgStartup,dbgDeep,("# prepParams: Calculated parameter size=$%lX\n",paramsiz));
   p=pp+paramsiz; /* end of param area */
   p-=4; *((ulong *)p)=0; /* envp[] terminator */
   p-=os9envc*4; /* reserve room for envp[] pointers */
   elp=(ulong *)p; /* save ptr to first envp pointer */
   p-=4; *((ulong *)p)=0; /* argv[]/envp[] separator */
   p-=argc*4; /* reserve room for argv[] pointers */
   alp=(ulong *)p; /* save ptr to first argv pointer */
   p-=4; *((ulong *)p)=0; /* argv[] list beginning */
   p-=2; *((ushort *)p)=os9_word(0x000D);
   k=strlen(modnam);
   if (!(k & 1)) *(--p)=0; /* align if module name has even # of chars (=odd with NUL) */
   p-=k+1;
   strcpy(p,modnam); /* copy module name as argv[0] */
   p-=4;
   *((ulong *)p)=os9_long(p+4-pp); /* set argv[0] offset */
   p-=2; *((ushort *)p)=os9_word(0xFC01); /* special sync code */
   /* --- environment variable strings */
   if ((envsiz & 1)==0) *(--p)=0; /* align needed if even envsize */
   p-=1; *p=0; /* environment variables terminator */
   p-=envsiz; /* reserve space for environment strings */
   hp=p;
   debugprintf(dbgStartup,dbgDeep,("# prepParams: Starting to write envs at $%08lX, memstart=$%08lX\n",(ulong) hp,(ulong) pp));
   k=0;
   while (os9envc) {
      if (*envp[k]=='@') {
         /* --- it is an OS-9 environment variable */
         *(elp++)=os9_long((ulong)hp-(ulong)pp); /* set offset */
         strcpy(hp,envp[k]+1); /* copy the environment variable name, but without the '@' */
         h=strlen(envp[k])-1; /* size of variable name without '@' */
   		debugprintf(dbgStartup,dbgDeep,("# prepParams: envp[%d] name='%s', len=%d",k,envp[k]+1,h));
         hp+=h; /* advance pointer */
         *hp++='='; /* insert '=' between name and value */
         strcpy(hp,envp[k]+h+2); /* copy contents of the variable */
         h=strlen(envp[k]+h+2); /* size of contents */
   		debugprintf(dbgStartup,dbgDeep,(", contents='%s', len=%d\n",hp,h));
         hp+=h; /* advance pointer */
         os9envc--;
         if (os9envc==0) break;
         *(hp++)=' '; /* add space, if more vars follow */
      }
      k++;
   }
   /* --- argument strings */
   if (argsiz & 1) *(--p)=0; /* align needed if odd argsize */
   p-=2; *((ushort *)p)=os9_word(0x000D); /* command line parameter terminator */
   p-=argsiz; /* reserve space for argument strings */
   debugprintf(dbgStartup,dbgDeep,("# prepParams: Starting to write args at $%08lX, memstart=$%08lX\n",(ulong) p,(ulong) pp));
   k=0;
   while (argc) {
      *(alp++)=os9_long((ulong)p-(ulong)pp); /* set offset */
      strcpy(p,argv[k]); /* copy the argument */
      p+=strlen(argv[k]); /* advance pointer */
      if (k+1>=argc) break;
      *(p++)=' '; /* add space */
      k++;
   }
   *psiz=paramsiz; /* return parameter size */
   *pap=pp; /* return pointer to prepared parameter area */
   return 0; 
} /* prepParams */


/* prepare first OS9 process to launch from MPW command line
 * Note: - allocates process descriptor and sets global currentpid
 *       - may only be called once by the MPW tool, as it assumes
 *         that no process is running yet.
 */
static os9err prepLaunch(char *toolname, char **argv, int argc, char **envp, ulong memplus, ushort prior)
{
	ulong   psiz;
	byte*   pap;
	os9err  err;
	ushort  newpid;
	ushort  mid;
	ushort  numpaths;
	Boolean intCmd;
	
	#ifdef MPW
	  numpaths=3; /* use the 3 standard paths for MPW */
	#else
	  numpaths=1; /* use only 1 path at the beginning with TERMINAL_CONSOLE */
	#endif        
	
	/* father of first process is now 0 (bfo) */
	err= new_process( 0, &newpid,numpaths      ); if (err) return err;
	err= link_load  (     newpid,toolname,&mid ); if (err) return err;
	
	err= prepParams( os9mod(mid), argv,argc, envp, &psiz, &pap ); if (err) return err;
	
										  /* group/user 0.0 are set for the first process */
	err= prepFork( newpid,toolname, mid,pap,psiz,memplus,numpaths, 0,0, prior, &intCmd );
	release_mem( pap,false ); /* return arg buffer anyway, alloc as pointer */
	
    if (!err) { /* make this process ready to execute */
        if (!intCmd) {
   		    currentpid= newpid; /* make this process current */
            procs[currentpid].state= pActive; /* now active */
        }
	}
	
    return err;
} /* prepLaunch */


/* the os9exec kernel */
/* ================== */

/*typedef void (*SignalHandler)(int);*/

#ifdef MPW
  #pragma push
  #pragma mpwc
  #pragma d0_pointers on

  /* MPW signal handler */
  static void mpwSignalHandler(int signum)
  {
	#pragma unused(signum)
	mpwsignal=signum;
	/* signal(signum,SIG_IGN); %%% *//* de-activate */
  } /* mpwSignalHandler */
#endif


/* clean up exit-handler */
static void cleanup(void)
{
	fflush(stdout);
	
	if (debugcheck(dbgAllInfo,dbgNorm)) {
		/* --- avoid endless messages only for cleanup */
			debug[dbgNorm  ]= debug[dbgNorm  ] && 0xfdff; /* without error display */
	}
	
	if (debugcheck(dbgAllInfo,dbgDetail)) {
		/* --- avoid endless messages only for cleanup */
		if (debugcheck(dbgAllInfo,dbgNorm)) {
			uphe_printf("cleanup: debug reset to debugAnomaly/dbgNorm (use [x] to keep!)\n");
			debug_halt( dbgAllInfo );
			debug[dbgDeep  ]= 0;
			debug[dbgDetail]= 0;
			debug[dbgNorm  ]= 0;
		}
	}

	debugprintf(dbgStartup,dbgNorm,("# cleanup: killing processes\n"));
	kill_processes(); /* kill all "running" processes */

	debugprintf(dbgStartup,dbgNorm,("# cleanup: closing syspaths\n"));
	close_syspaths(); /* make sure all paths are closed */

	debugprintf(dbgStartup,dbgNorm,("# cleanup: freeing modules\n"));
    free_modules(); /* unlink the OS9 module resources */
	fflush(stdout);

	#ifdef macintosh
	  debugprintf(dbgStartup,dbgNorm,("# cleanup: restoring MPW current directory\n"));
	  HSetVol(NULL,startVolID,startDirID); /* restore the original directory */
	#endif
	
	debugprintf(dbgStartup,dbgNorm,("# cleanup: releasing low-level magic\n"));
	lowlevel_release(); /* release low-level stuff */
} /* cleanup */

#ifdef MPW
  #pragma pop
#endif


void getversions()
/* obtain tool and package (os9exec) version */
{
	#ifdef macintosh
	Handle versH;
	#endif
	
	appl_version = 0; 
	appl_revision= 0;
	exec_version = 0; 
	exec_revision= 0;
	
	#ifdef macintosh
	/* obtain tool's version */
	if ((versH=GetResource('vers',1))!=NULL) {
		appl_version = *((byte*)*versH+0);
		appl_revision= *((byte*)*versH+1);
		ReleaseResource(versH);
	}
	
	/* obtain os9exec version */
	if ((versH=GetResource('vers',2))!=NULL) {
		exec_version = *((byte*)*versH+0);
		exec_revision= *((byte*)*versH+1);
		ReleaseResource(versH);
	}
	
	#else
	/* %%% simply hardwired for now */
	appl_version =    1; 
	appl_revision= 0x01;	
	exec_version =    3;
	exec_revision= 0x14;
	#endif
} /* getversions */


void get_hw()
/* must be done before linking "init" module */
/* and this will be done earlier now */
{
	#ifdef macintosh
      hw_site= "Mac";
     
      #ifdef powerc
        hw_name = "Apple PowerMac";  platform= "PPC";
      #else
        hw_name = "Apple Macintosh"; platform= "68k";
      #endif
      
    #elif defined(windows32)
      hw_site = "PC";
	  hw_name = "Windows - PC"; platform= "Windows";
	  
    #elif defined linux
      hw_site = "PC";
	  hw_name = "Linux - PC";   platform= "Linux";
       
    #else
    /* unknown */
      hw_site = "?";
	  hw_name = "?";            platform= "?";
    #endif
} /* get_hw */



#ifdef linux
void StartDir( char* pathname )
{
	char    acc   [OS9PATHLEN];
    char    result[OS9PATHLEN];
	struct  stat info;
	
	strcpy( pathname,"" ); /* start with an empty string */
	strcpy( acc,    "." ); /* and take current dir for start */

	while (true) {
		stat_  ( acc, &info );
		strcat ( acc,"/.." );
		DirName( acc,  info.st_ino, (char*)&result );
		if (strcmp( result,".")==0) {
			strcpy( result,"" ); break;
		}

		strcat( result,  pathname );
		strcpy( pathname,PATHDELIM_STR );
		strcat( pathname,result );
	} /* while */
}  
#endif


static os9err GetStartPath( char* pathname )
{
	#ifdef macintosh     
	  os9err err;
	  Str255 rel, tmp;
	  FSSpec spec; /* the HFS object's FSSpec */
	
	  strcpy( pathname,"" ); /* initialize */
	  strcpy( rel,     "" );

	  while (true) {
		  strcat( rel,":" ); /* next higher path */ 
		      err= getFSSpec( 0,rel, _start, &spec );
		  if (err) return err;
		
		  p2cstr    ( spec.name );
		  strcpy( tmp,spec.name );
		  strcat( tmp,":" );
		  strcat( tmp,pathname );
		  strcpy    ( pathname,tmp );
		  if (spec.parID==1) return 0;
	  } /* while */

	#elif defined(windows32)
	  /* determine path to current directory */
	  if (!GetCurrentDirectory(MAX_PATH,pathname))
		  strcpy( pathname,"" ); // no default path for some reason

	//	/* make sure it ends with slash */
	//	if (pathname[strlen(pathname)-1]!=PATHDELIM ) strcat(pathname,PATHDELIM_STR );

	#elif defined linux
	  StartDir( pathname );
	#else
	  strcpy  ( pathname,"" );
	#endif
	return 0;
} /* GetStartPath */




static void PathUp( char* p )
{
	char*    q= p+strlen(p)-2;
	while   (q>p) {
		if (*q==PATHDELIM) {
			#ifdef macintosh
			  q++;
			#endif
			   
		    *q= NUL; 
		    break; 
		}
		
		q--;
	}
} /* PathUp */



static void GetCurPaths( char* envname, ushort mode, dir_type *drP, Boolean recursive )
{
	os9err err;
	char   tmp[OS9PATHLEN];

	#ifdef macintosh
	  char svPath[OS9PATHLEN];
	#endif
	
	char*  p= egetenv( envname ); /* get path for default module loading dir */
	if    (p==NULL) return;
	
	strcpy( tmp,p ); p= tmp; /* make a local copy */
		MakeOS9Path( p ); 
    	drP->type= IO_Type( 1,           p,mode ); /* get device type: Mac/PC or RBF */
    if (drP->type==fRBF) {
		err= change_dir   ( 1,drP->type, p,mode ); /* set the types at procid 0 */
		return;
	}
	
	#ifdef macintosh
	  p++;
	#elif defined(windows32)
	  if (p[0]==PSEP && 
		  p[2]==PSEP) { /* adapt for windows notation */
		  p[0]= p[1];
		  p[1]= ':';
	  }	  
	#endif
		      
	strcpy( tmp, p );
	p=      tmp;

	while  (*p!=NUL) { /* replace slashes -> PATHDELIM */
		if (*p==PSEP) *p= PATHDELIM;
		p++;
	} /* while */
	p= tmp;
	
	/* do this before the recursive loop */
	strncpy( drP->path,p, OS9PATHLEN );
			
	#ifdef macintosh     
	  get_dirid( &drP->volID, &drP->dirID, tmp );
	  
	  if (recursive && drP->volID==0 &&
	                   drP->dirID==0) {
		  strcpy(    svPath,startPath );
		  strcpy( startPath, callPath );
		  PathUp( startPath );
		
		  GetCurPaths( envname, mode, drP, false );
		
		  strcpy( startPath,svPath );
	  }

	#endif
} /* GetCurPaths */



static void BootLoader( int cpid )
/* try to load "init" first, then "OS9Boot" */
{
	os9err   err;
	char*    p;
	ushort   mid;

	p= "OS9exec";
             err= link_module( cpid, p, &mid ); /* do not abort on error */
		debugprintf(dbgStartup,dbgNorm,( "# Bootloader: '%s' err=%d\n", p,err ));

	p= "init";
             err= link_module( cpid, p, &mid );
    if (err) err= load_module( cpid, p, &mid, false,false );
   		debugprintf(dbgStartup,dbgNorm,( "# Bootloader: '%s' err=%d\n", p,err )); 
   	
   	if (err) {
		p= "OS9Boot";
	         err= load_module( cpid, p, &mid, false,false );
		debugprintf(dbgStartup,dbgNorm,( "# Bootloader: '%s' err=%d\n", p,err ));
	} /* if */
	
	p= "socket";
             err= link_module( cpid, p, &mid );
   		debugprintf(dbgStartup,dbgNorm,( "# Bootloader: '%s' err=%d\n", p,err )); 
} /* Bootloader */



static void CheckStartup( int cpid, char* toolname, int *argc, char **argv )
/* Check if "startup" file is available */
{
	os9err    err;
	ptype_typ type;
	ushort    path; /* be careful: not an "int" */
	char*     p= argv[0];
	
	if (toolname==NULL) return;  
	debugprintf(dbgStartup,dbgNorm,( "# CheckStartup: '%s' '%s'\n", toolname,p ));
		
	/* just in case a "startup" file is there: execute it */	
	if (*argc==1 && (ustrcmp(toolname,"shell"  )==0)
	             && (ustrcmp(p,       "startup")==0)) {
		type= IO_Type      ( cpid,            p, 0x01 );	
		err = usrpath_open ( cpid,&path,type, p, 0x01 ); 
		if (err) { *argc= 0; return; } /* no startup file, ignore it */
		err = usrpath_close( cpid, path );
		quitFlag= true; /* in case of startup sequence, quit automatically */
	}
} /* CheckStartup */


#ifdef windows32
/* catch the Ctrl C */
BOOL CtrlC_Handler( DWORD ctrlType )
{
	Boolean fnd= false;
	
	if (ctrlType==CTRL_C_EVENT) {
		fnd=         (main_mco.spP->lastwritten_pid!=0);
		KeyToBuffer( &main_mco, CtrlC );
		if (fnd) return true; /* do not abort program if signalled process found */
	}
	
	return false; /* abort for other events */
} /* CtrlC_Handler */
#endif



static void titles( void )
{
   /* - Apperance (CTB-terminal, Win32 or Linux Console or MPW tool) */
   #ifdef TERMINAL_CONSOLE
     #ifdef macintosh
       #ifndef SERIAL_INTERFACE
       upho_printf("- CTB Console Version, 'Termconsole'      (c) 1996 by Joseph J. Strout\n");
       #endif
       upho_printf("- Mac Serial Interface Version            (c) 2000 by B. Forster\n");
     
     #elif defined(windows32)
       upho_printf("- Windows Console Version\n");
     #elif defined linux
       upho_printf("- Linux XTerm Version\n");
     #else
       /* unknown */
       upho_printf("- Unknown System\n");
     #endif
     
   #else
     upho_printf("- Macintosh programmer's Workshop (MPW) Tool Version \n");
   #endif /* terminal */
   
    /* - what kind of 68k */
   #ifdef USE_UAEMU
       upho_printf("- Using 68k-Emulator from UAE - Un*x Amiga Emulator (c) 1995 B. Schmidt\n");
   #else
     #if defined linux
       upho_printf("- Using currently no 68k-Emulator\n");
	 #elif defined powerc
       upho_printf("- Using built-in Macintosh 68k-Emulator from PPC code\n");
	 #else
       upho_printf("- Running as 68k application on 68k Mac\n");
	 #endif
   #endif
   
   upo_printf( "\n" );   
   fflush(stderr);
   fflush(stdout);
} /* titles */
				


static Boolean TCALL_or_Exception( process_typ* cp, regs_type* crp, ushort cpid )
/* Exception or trap handler call */
{				
    ushort           vect;
    traphandler_typ* tp;
	mod_exec*        mp; /* module pointer */
   
	if 		 (cp->vector==0xFAFA) { /* error exception */
		vect= cp->func >> 2; /* vector number (=offset div 4 !) */
		if (debugcheck(dbgTrapHandler,dbgNorm)) { 
			uphe_printf("main loop: Exception occurred [pid=%d] ! Vector offset=$%04X (num=%d)\n",
					     cpid,cp->func,vect); 
			dumpregs(cpid);
		} 
					
		if ((vect>=FIRSTEXCEPTION) && (vect<FIRSTEXCEPTION+NUMEXCEPTIONS)) {
			if (cp->ErrorTraps[vect-2].handleraddr!=0) {
				/* there is an installed handler */
				crp->pc  = cp->ErrorTraps[vect-FIRSTEXCEPTION].handleraddr; /* set handler routine address */
				crp->a[6]= cp->memstart+0x8000; /* set A6 base */
				
				if (cp->ErrorTraps[vect-FIRSTEXCEPTION].handlerstack!=0) {
					crp->a[7]=cp->ErrorTraps[vect-FIRSTEXCEPTION].handlerstack;
				}
				if (debugcheck(dbgTrapHandler,dbgDetail)) { 
					upe_printf("Calling handler at $%08lX, stack ptr=$%08lX\n",
								cp->ErrorTraps[vect-FIRSTEXCEPTION].handleraddr,
								cp->ErrorTraps[vect-FIRSTEXCEPTION].handlerstack); 
					debug_halt( dbgTrapHandler );
				}
				
				/* next call to os9_go will execute the handler routine */
				return false; /* avoid any calls to arbitrate etc. */
			}
			else cp->func=0; /* no handler installed, fatal */
		}
		else     cp->func=0; /* fatal exception, aborts process */
					
		if (cp->func==0) {
			/* process does not handle exception, kill it */
			cp->exiterr=vect-FIRSTEXCEPTION+E_BUSERR; /* set exit code */
			kill_process(cpid); /* kill the process, change currentpid */
			/* show exception */
			debugprintf(dbgAnomaly,dbgNorm,("# main loop: Process pid=%d aborted: Exception vector=$%02X, err=#%03d\n",
						                     cpid,vect,cp->exiterr));
			debug_halt( dbgAnomaly );
			/* will continue with another process */
		}
	}
	else { /* --- Trap handler called (but could not be handled by low level code!) */
		debugprintf(dbgTrapHandler,dbgNorm,("# main loop: Pid=%d called uninitialized TRAP Vector $%04X (User Trap #%d), function code=$%04X\n",
					                         cpid,cp->vector,
					                              cp->vector-FIRSTTRAP+1,
					                              cp->func));
					                              
			tp= &cp->TrapHandlers[cp->vector-FIRSTTRAP];
		if (tp->trapmodule==NULL) {
			/* --- called uninitialized TRAP, check for TrapInit entry */
				mp= get_module_ptr(cp->mid); /* get module pointer */
			if (mp->_mexcpt!=0) {
				/* there is an exception entry point, call it */
				crp->pc=(ulong)mp+os9_long(mp->_mexcpt); /* set new PC into exception handling routine */
				debugprintf(dbgTrapHandler,dbgNorm,("# main loop: Calling exception entry point of pid=%s's main module\n",cpid));
			}
			else { /* no exception entry point: Abort process */
				cp->exiterr= os9error(E_ITRAP); /* set exit code */
				kill_process(cpid); /* kill the process, change currentpid */
				/* show exception */
				debugprintf(dbgAnomaly+dbgTrapHandler,dbgNorm,("# main loop: Process pid=%d aborted due to unintialized User Trap #%d, err=#%03d\n",
							                                    cpid,cp->vector-FIRSTTRAP+1,
							                                         cp->exiterr));
				/* will continue with another process */
			}
		}
		else {  
			/* --- trap handler is installed for this vector, call it */
			crp->pc=tp->trapentry; /* continue in trap handler */
			crp->a[6]=tp->trapmem+0x8000; /* pass trap handler data pointer with offset */
			debugprintf(dbgTrapHandler,dbgDeep,("# main loop: calling Trap function now"));
			/* execution continues in trap handler and will return to program within OS-9 context */
			return false; /* avoid any calls to arbitrate etc. */
		}
	}
	
	return true;  
} /* TCALL_or_Exception */



static void debug_comein( process_typ* cp, regs_type* crp, ushort cpid )
{
	const funcdispatch_entry* fdeP= getfuncentry(cp->func);
	Boolean                   msk = cp->masklevel>0;
	
	if (cp->state==pWaitRead) return; /* avoid dbg display in pWaitRead mode */

	/* (enclosed in another if to avoid get_syscall_name invocation */
	/* in nodebug case!) */
	uphe_printf(">>>%cPid=%02d: OS9 %s : ",msk ? '*':' ',cpid,fdeP->name);
	
	show_maskedregs(crp,fdeP->inregs);
	debugprintf(dbgSysCall,dbgNorm,("\n"));
} /* debug_comein */



static void debug_retsystask( process_typ* cp, regs_type* crp, ushort cpid )
{
	const funcdispatch_entry* fdeP= getfuncentry(cp->lastsyscall);
	char  *errnam,*errdesc;
	
	uphe_printf("<<< Pid=%02d: OS9 %s returns (from sysTask!): ",cpid,fdeP->name);
	
	if (cp->oerr) {
		get_error_strings(cp->oerr, &errnam,&errdesc);
		debugprintf(dbgSysCall,dbgNorm,("#%03d:%03d - %s\n",
						  cp->oerr>>8,cp->oerr &0xFF,errnam));
	}
	else {
		show_maskedregs(crp,fdeP->outregs);
		debugprintf(dbgSysCall,dbgNorm,("\n"));
	}
} /* debug_retsystask */

	

static void debug_return( process_typ* cp, Boolean cwti )
{	
	const funcdispatch_entry* fdeP= getfuncentry(cp->func);
	char      *errnam,*errdesc;
	mod_exec* mod;
	char*     p;
	char      item[OS9NAMELEN];

	Boolean msk= cp->masklevel>0;
	Boolean hdl= cp->icptroutine!=0;
	Boolean strt;
		
	if (cwti) {
		uphe_printf("<<<%cPid=%02d: OS9 INTERCEPT%s, state=%s, ", 
						msk ? '*':' ',currentpid,
						hdl ? "" :" (no handler)", PStateStr(cp) );
							 
		show_maskedregs( &cp->os9regs, d_w(1)+a_l(6) );
		debugprintf(dbgSysCall,dbgNorm,("\n"));
	}
	else {
		if ((cp->state==pActive || cp->oerr) &&
			 cp->state!=pWaitRead) { /* avoid dbg display in pWaitRead mode */

			/* D1.w/carry error reporting will be done when process is active or syscall delivered error, */
			/* otherwise, d1.w will be updated when suspended process gets active again */
			    strt= (ustrcmp(fdeP->name,"START")==0);
			if (strt) {
				mod= (mod_exec *)cp->os9regs.a[3];
				p  =  Mod_Name( mod );
				strcpy( item,"\"" );
				strcat( item, p  );
				strcat( item,"\"" );
			}

			uphe_printf("<<<%cPid=%02d: OS9 %s %s",msk ? '*':' ',
			                currentpid,fdeP->name, strt ? item:"returns: " );

			if (cp->oerr) {
				get_error_strings(cp->oerr, &errnam,&errdesc);
			//  errnam= "XXX"; errdesc= "";
				debugprintf(dbgSysCall,dbgNorm,("#%03d:%03d - %s\n",
					        cp->oerr>>8,cp->oerr &0xFF,errnam));
			}
			else {
				show_maskedregs( &cp->os9regs,fdeP->outregs );
				debugprintf(dbgSysCall,dbgNorm,("\n"));
			}
		} 
	} /* if (cwti) */
} /* debug_return */




void os9exec_globinit(void)
{
    /* --- global environment initialisation --- */
    totalMem= 0;               /* initialize startup memory */
    init_fmgrs    ();          /* initialize all the file manager objects */
	init_processes();          /* no processes yet */
	init_modules  ();          /* clear module handle list */
	init_events   ();	       /* init OS-9 events */
	init_alarms   ();		   /* init OS-9 alarms */
	init_syspaths ();          /* prepare system paths, ready for usrpath print now */
	init_L2       ();		   /* prepare the /L2 stuff */
} /* os9exec_globinit */

   
   
/* Entry into OS9 emulation
 * If toolname==NULL, the os9 program is loaded from 'OS9C' Id=0
 */
ushort os9exec_nt( const char* toolname, int argc, char **argv, char **envp, 
                   ulong memplus, ushort prior)
{
	#define      ARB_RATE 10

	ushort       cpid;
	process_typ* cp;
	regs_type*   crp;
	save_type*   svd;
    process_typ* sigp;
	ulong        lastspin, my_tick;
	dir_type*    drP;
	char*        errnam;
	char*        errdesc;
    char*        argv_startup[2];
	Boolean      cwti, last_arbitrate;
	int          arb_cnt= 0;
	char*		 my_toolname= (char*)toolname; /* do not change <toolname> directly */
	alarm_typ*   aa;
	ulong		 aNew; /* new alarm number for cyclic alarm */
	
	#ifdef MACFILES
	  FSSpec     fs;
      CInfoPBRec cipb;
	#else
	  char   *p, title[255];
	#endif
		
	ulong      res;	    /* llm_os9_go result */
    ushort     err= 0;  /* MPW Error */
	OSErr    oserr;		/* Mac error */


	/* ---- create the kernel process ------------------------------- */
	new_process( 0,&cpid, 0 ); /* get the kernel process */
	cp=      &procs[cpid];
	cp->state = pSleeping;     /* to be compliant to real OS-9 */
	cp->_prior= 0xFFFF;        /* "  "      "     "    "    "  */
	cp->mid   = 0;             /* take "OS9exec" as the kernel module */

	
	/* ---- assign startVolID/dirID --------------------------------- */
	#ifdef macintosh
			  fs.vRefNum= 0; /* default path is my own path */
			  fs.parID  = 0;
	  strcpy( fs.name,"" );
	  c2pstr( fs.name );     /* it is pascal notation */

	  startVolID= fs.vRefNum;
	  startDirID= fs.parID; /* get default dir */
	#endif
	

	#if defined MACTERMINAL && defined powerc && !defined USE_CARBON
	  Install_AppleEvents(); /* do this as early as possible */
	#endif

	#ifdef windows32
      hStdin= GetStdHandle(STD_INPUT_HANDLE);
      SetConsoleCtrlHandler( CtrlC_Handler, true );
      WindowTitle    ( &title,false ); /* adapt title line of DOS window */
      SetConsoleTitle( &title );     	  
    #endif

  	
    // some global init moved from here to os9exec_globinit() to allow them being
    // called in os9main.c already	
	
	interactivepid=  0;        /* send aborts to first process by default */
	sig_queue.cnt =  0;        /* no signal pending at the beginning */
	
	#ifdef windows32
	  dirtable[0]= NULL;       /* no dir table at the beginning */
	#endif
	
	debug_prep();
	debugprintf(dbgStartup,dbgNorm,("# os9exec_nt: entered routine, no op yet\n")); 
    
	#ifdef macintosh
	  GetStartPath( callPath );

	  applVolID= 0;
	  applDirID= 0;
	  strcpy( applName,"" );

	  /* must be synchronized to OpenDocument Apple Event */
	  #ifdef MACTERMINAL
	    #ifndef USE_CARBON
  		  memcpy( &gFS,  &fs, sizeof(FSSpec) );
	      while (!gDocDone) HandleEvent();
		  memcpy(  &fs, &gFS, sizeof(FSSpec) );
		#endif
		
		/* If file has correct type and creator,
		/* it can be executed directly */
		err= getCipb( &cipb, &fs );
		if (argc==0 &&
		    cipb.hFileInfo.ioFlFndrInfo.fdCreator=='os9a' &&
		    cipb.hFileInfo.ioFlFndrInfo.fdType   =='PROG') {

	  		applVolID= fs.vRefNum;
	  		applDirID= fs.parID; /* get default dir */
	  		
    		memcpy( applName,fs.name, 32 );
    		p2cstr( applName );
		    my_toolname= "shell"; /* currently one shell is used */

    		argc= 1;
    		argv_startup[0]= applName;
    		argv_startup[1]= NULL;
    		argv= (char**)&argv_startup;
		}
 	  #endif
	#endif


	/* include 'startup' as default parameter */
	/* will be eliminated again later, if no 'startup' file is available */
    if (argc==0 && ustrcmp( my_toolname,"" )==0) {
    	argc= 1;
    	my_toolname    = "shell";
    	argv_startup[0]= "startup";
    	argv_startup[1]=  NULL;
    	argv= (char**)&argv_startup;
    } /* if */


    /* Sign-on message */
     upo_printf( "\n" );
    upho_printf( "%s:     OS-9  User Runtime Emulator\n", OS9exec_Name() );
    upho_printf( "Copyright (C) 2002 Lukas Zeller / Beat Forster\n" );
    upho_printf( "This program is distributed under the terms of\n" ); 
    upho_printf( "        the GNU General Public License\n" );
    upho_printf( "\n" );

   	/* prepare low-level magic, to allow using llm_xxx() routines */
	    oserr=lowlevel_prepare();
	if (oserr) {
		uphe_printf("FATAL ERROR: can't initialize low-level, OSErr=%d\n",oserr);
		my_toolname= ""; /* no tool to be used */
        err=3;           /* MPW system/missing resource error */
		goto mainabort;
	}
	debugprintf(dbgStartup,dbgNorm,("# main startup: low level prepared\n"));

    /* show environment */
	debugprintf(dbgStartup,dbgNorm,("# main startup: FPU=%d, Virtual Mem=%d, Cache=%d, Runs in User mode=%d\n",
	                                   llm_fpu_present(),llm_vm_enabled(),llm_has_cache(),llm_runs_in_usermode()));

	GetStartPath    ( startPath );
	strcpy( strtUPath,startPath );
	PathUp( strtUPath );
//	upo_printf( "'%s' '%s'\n", startPath, strtUPath ); 
	
	#ifdef macintosh	
	  /* save current default directory */
	  startVolID= fs.vRefNum;
	  startDirID= fs.parID; /* get default dir */

	  get_dirid( &startVolID,&startDirID, startPath );
	  debugprintf(dbgStartup,dbgNorm,("# main startup: (start) startVolID=%d, startDirID=%ld\n",
							             startVolID,startDirID));
	#endif

    /* get default SCSI setting */
    #ifdef RBF_SUPPORT
      #ifdef windows32
        if (defSCSIAdaptNo<0) {
    	    debugprintf(dbgStartup,dbgNorm,("# main startup: scanning for SCSI adaptors/buses/devices\n"));
    	    scsi_finddefaults();
        }
	    debugprintf(dbgStartup,dbgNorm,("# main startup: Default SCSI adaptor=%d, bus=%d\n",defSCSIAdaptNo,defSCSIBusNo));
      #endif
    #endif
    		
	drP      = &cp->d;
	drP->type= fDir; /* set to default */
	drP->dev = 0;
	drP->lsn = 0;

	#ifdef macintosh
	  drP->volID= 0;
	  drP->dirID= 0;
	#endif
	
	GetCurPaths( "OS9DISK", 0x80, &cp->d, true );
	memcpy              ( &cp->x, &cp->d, sizeof(dir_type) ); /* at least this should be ok */
	GetCurPaths( "OS9CMDS", 0x84, &cp->x, true );


	drP      = &mdir;
	drP->type= fDir; /* set to default */
	drP->dev = 0;
	drP->lsn = 0;

	#ifdef macintosh
	  drP->volID= 0;
	  drP->dirID= 0;
	#endif
	
	GetCurPaths( "OS9MDIR", 0x80, &mdir, true );
	
	#ifdef macintosh  
	  debugprintf(dbgStartup,dbgNorm,("# main startup: (mdir) mdir.volID=%d, mdir.dirID=%ld\n",
									     mdir.volID,mdir.dirID));
	#endif

	
	#if defined win_linux
	  /* establish the virtual mdir (dir used to load modules from by default) */
	      p= egetenv("OS9MDIR");    /* get path for default module loading dir */
	      strcpy( mdirPath,p );
	      
//	  if (p!=NULL) {
//		  if (*p!=PATHDELIM) {
//			   // relative
//			   strncpy( mdirPath,startPath,    MAX_PATH );
//			   strncat( mdirPath,PATHDELIM_STR,MAX_PATH );
//			   strncat( mdirPath,p,            MAX_PATH-strlen(mdirPath) );
//		  }
//		  else strncpy( mdirPath,p,            MAX_PATH );
//	  }
//	  else {
//		  /* use default */
//	      strncpy( mdirPath, startPath,     MAX_PATH );
//	   	  strncat( mdirPath, PATHDELIM_STR, MAX_PATH );
//	      strncat( mdirPath,"OS9MDIR",      MAX_PATH );
//
//	  	  if    (stat( mdirPath, &info )!=0) {
//	      	  strncpy( mdirPath, startPath, MAX_PATH );
//
//			  q= mdirPath+strlen(mdirPath)-1;
//			  while (*q!=PATHDELIM) q--;
//			  *(++q)= NUL; /* cut the string after delimiter */
//		
//			  strncat( mdirPath,"OS9MDIR",  MAX_PATH );
//		  }
//	  }
	  debugprintf(dbgStartup,dbgNorm,( "# main startup: mdir='%s'\n",mdirPath ));
	#endif

	
    /* install asynchronous handlers */
    atexit(&cleanup);

    /* obtain package (os9exec) version */
	BootLoader  ( cpid );
	CheckStartup( cpid, my_toolname, &argc, argv );

    /* initialize spinning cursor */ 
    lastspin= GetSystemTick();
    eSpinCursor(0); /* reset cursor spinning */	

	titles();

   /* init error tracking vars */
    totalMem= max_mem(); /* get startup memory */
	lastpathparsed=NULL;
	lastsyscall=0;
	
	/* set-up MPW signal handling */
    #ifdef MPW
  	  mpwsignal=0; /* no signal received yet */
	  signal(SIGINT,&mpwSignalHandler); /* install handler */
    #endif
    
	/* prepare initial process */
	    cp->oerr= prepLaunch( my_toolname, argv,argc,envp, memplus,prior );
	if (cp->oerr) goto mainabort; /* prepare for execution */
		
	debugprintf(dbgStartup,dbgNorm,("# main startup: Main module loaded & prepared for launch\n"));


	/* now start running emulated OS9 tasks */
    do {
   	    /* --- prepare for entry into OS9 world, process is "currentpid" */
    	cpid= currentpid;   /* make a copy to allow currentpid to be changed by F$xxx calls */
   		cp  = &procs[cpid]; /* pointer to procs   descriptor */
   		crp = &cp->os9regs; /* pointer to process' registers */
   		svd = &cp->savread; /* pointer to process' saved registers */
		cwti= false;
		
     	/* --- make sure MacOS gets its time, too */
     		my_tick= GetSystemTick();
		if (my_tick-lastspin > spininterval) {
			lastspin= my_tick;
			CheckInputBuffers();
			eSpinCursor(32); /* rotate forward once */
		} /* if */
				
		/* --- now execute */
		if (cp->state==pSysTask) {
			arbitrate=true; /* allow arbitration by default after sysTask execution */

			/* --- execute system task function */
				cp->oerr=   (cp->systask)(cpid,cp->systaskdataP,crp);
			if (cp->oerr!=0) cp->state= pActive; /* on error, continue with task execution anyway */
			if (cp->state==pActive) {
				/* process gets active again, report errors to OS9 programm */
				if (debugcheck(dbgSysCall,dbgNorm)) 
					debug_retsystask( cp,crp, cpid );
				
				if (!cp->oerr) crp->sr &= ~CARRY;
				else {
					retword(crp->d[1])= cp->oerr;
					crp->sr |= CARRY;
				}
			}			
		} /* system task */
		
 		else if (cp->state==pActive   ||
 		         cp->state==pWaitRead) {
 			if  (cp->state==pActive ||
 			     cp->way_to_icpt) {
				/* --- go execute OS9 code until next trap or exception */
				/* %%% for low-level calling interface debug: Debugger(); */
				res= llm_os9_go(crp);
		        cp->way_to_icpt= false; /* now it is done */

				/* --- and now exec syscall */
				cp->vector= hiword(res);
				cp->func  = loword(res);
				
				/* --- safeguarding */
				if (debugcheck(dbgWarnings,dbgDeep)) {
					if (cp->memstart+0x8000!=crp->a[6]) {
						uphe_printf(">>> Warning [pid=%d]: A6 has changed from $%08lX to $%08lX !\n",cpid,cp->memstart+0x8000,crp->a[6]);
						debug_halt(dbgWarnings);
					}
					if ((crp->a[7]>cp->memtop) || (crp->a[7]<cp->memstart)) {
						uphe_printf(">>> Warning [pid=%d]: A7=$%08lX is out of data area ($%08lX - $%08lX) !\n",cpid,crp->a[7],cp->memstart,cp->memtop);
						debug_halt(dbgWarnings);
					}  
				}
			  
				/* register will be saved, in case they are reused in pWaitRead state */
			    svd->r     = cp->os9regs;
			  	svd->vector= cp->vector;
			  	svd->func  = cp->func;
			} /* if */
				
			/* in case of a unsuccessful read just repeat the call with saved registers */
			/* correct exception handling */
 		    if (cp->state==pWaitRead) {
            	/* registers of the last command will be restored */
			    cp->os9regs= svd->r;
			  	cp->vector = svd->vector;
			  	cp->func   = svd->func;
			} /* if */	
			
			/* --- now handle trap */
			if (cp->vector!=0) {
				if (!TCALL_or_Exception( cp, crp, cpid )) continue;
			}
			else {
				/* TRAP0 = OS-9 system call called */
				arbitrate=false; /* disallow arbitration by default */

				if (debugcheck(dbgSysCall,dbgNorm)) 
					debug_comein( cp, crp,cpid );
				
    			
    			/* --- Alarm handling */
    				aa= alarm_queue[0];
    			if (aa!=NULL) {
    			    if (GetSystemTick()>=aa->due) {       aNew= 0; /* must be zero ! */
       					if (aa->cyclic) A_Make( aa->pid, &aNew, aa->signal, aa->ticks, true );
   						send_signal           ( aa->pid,        aa->signal ); /* renew it */
       					A_Remove( aa ); /* and remove the old one */
    				}
    			} /* if */
   
			 // ----------------------
				    async_area= true; 
				if (async_pending) sigmask( cpid,0 ); /* disable signal mask */
				/* asynchronous signals are allowed here */
				
				/* execute syscall */
				cp->lastsyscall=  lastsyscall= cp->func; /* remember for error tracking (globally and for process) */
				cp->oerr       = exec_syscall( cp->func,cpid,crp );
				
				/* analyze result */
				if (debugcheck(dbgSysCall,dbgDeep)) {
					if (cp->state!=pActive) uphe_printf("  * OS9 %s sets Pid=%d into new state=%s\n",
					                                       get_syscall_name(cp->func),cpid,PStateStr(cp));
				}
				    async_area= false; 
				/* asynchronous signals are no longer allowed */
			 // ----------------------

								
				/* if the system is on the way to intercept, d1 and carry 
			       must be stored (must not override values of "send_signal" */
			    	cwti= cp->way_to_icpt;
				if (cwti) {
					sigp= &procs[cp->icpt_pid];
					
					if     (cp->icpt_pid==currentpid) { /* in case of the own process */
						if (cp->icpt_signal!=S_Wake  &&
							cp->icptroutine!=0) crp= &cp->rteregs;
					}
					else {
						cp->way_to_icpt= false;   /* now the way to icpt changes */
						currentpid= cp->icpt_pid; /* continue as this process */
					}
		    	} /* if cwti */
						
				if (cp->state==pActive   || 
				    cp->state==pWaitRead || cp->oerr) {
					/* report errors to OS9 programm */
					if (!cp->oerr) crp->sr &= ~CARRY;
					else {
						if ((cp->oerr & 0xFF00)==E_OKFLAG) cp->oerr &= 0x00FF; /* ensure that silent errors are restored to normal error code */
						retword(crp->d[1])= cp->oerr;
						crp->sr |= CARRY;
					}
				} /* if */

				if (cwti && cp->icpt_signal!=S_Wake) {
						sigp->masklevel   = 1; /* not interrupteable during intercept */		
						sigp->os9regs.d[1]= cp->icpt_signal; /* get signal code at icpt routine */
						sigp->rtevector   = sigp->vector;
						sigp->rtefunc     = sigp->func;		

					if (sigp==cp &&
					    sigp->state==pWaitRead &&
						sigp->icptroutine!=0) {
						sigp->rtestate = sigp->state;
						sigp->rteregs  = svd->r;      /* ignore latest d1 and carry */
						sigp->rtevector= svd->vector;
						sigp->rtefunc  = svd->func;
				    	sigp->state    = pActive;
				    }

					if (sigp->state==pWaiting &&     /* a signalled waiting process */
						sigp->icptroutine!=0) {        /* will be active afterwards */
				    	sigp->state    = pActive;
						sigp->rtestate = pActive;
				    }
				} /* if (cwti) */
			}
		} /* active OS9 process that has called a TRAP */
				
		else {
			if (cp->state!=pUnused   &&
			    cp->state!=pSleeping &&
			   (cp->state!=pDead ||
			    cp->lastsignal==0)) {
			/* signal handling already done correctly */
			/* %%% for now, fatal: current process is not pActive */
				uphe_printf("main loop: INTERNAL ERROR, active process=%d has invalid state=%s\n",
		                  	 cpid,PStateStr(cp));
				debug_halt( dbgAll );
			}
		}
		
		
		#ifdef MPW
  		  /* handle MPW signals, if any */
		  if (mpwsignal) {
		  	  mpwsignal=0; /* we've seen it now */
			  clearerr(stdin); /* prevent input from causing troubles */
			  signal(SIGINT,&mpwSignalHandler); /* install handler again*/
			  
			  if (debugcheck(dbgAllInfo,dbgNorm)) {
				  /* give user opportunity to just enter debug menu */
				  uphe_printf("main loop: MPW Cmd-'.' signal received, [c] to continue, [x] to send S_Abort to pid=%d\n",interactivepid);
				  if (debugwait()) {
					  /* kill process only if special continue from debug_halt */
					  send_signal(interactivepid,S_Abort); /* send keyboard abort to process */
				  }
			  }
			  else {
				  /* w/o debug, always send abort signal */
				  send_signal(interactivepid,S_Abort); /* send keyboard abort to process */
			  }
		}
		#endif

		if (debugcheck(dbgTaskSwitch,dbgDetail) &&
		    cp->state==pWaitRead) {
		  uphe_printf("arbitrate: current pid=%d: pWaitRead (before) \n",cpid);
		}

		/* do not arbitrate if way to intercept */
		if (!arb_cnt--) { arb_cnt= ARB_RATE-1; /* arbitrate= true; */ }
		last_arbitrate= arbitrate;			
		if (!cwti) do_arbitrate();
		if (logtiming) arb_to_os9( last_arbitrate );
			
		cp= &procs[currentpid];

		if    (!cwti) {
			    cwti=  cp->way_to_icpt;
			if (cwti) {
				debugprintf(dbgSysCall,dbgNorm,( "%d %d %d %x\n", 
								 				  cp->icpt_pid,currentpid,
								 				  cp->icpt_signal,
								 				  cp->icptroutine ));

				if (cp->icpt_pid!=currentpid) {
					cp->way_to_icpt= false;    /* reset if another process */
					currentpid= cp->icpt_pid;  /* continue as this process */
				}
				
				if (cp->icpt_signal!=S_Wake) {
					cp->os9regs.d[1]= cp->icpt_signal; /* get signal code at icpt routine */
				}
				
				cp= &procs[currentpid];
		    }
		} /* if not cwti */

		if (debugcheck(dbgTaskSwitch,dbgDetail) &&
		    cp->state==pWaitRead) {
		  uphe_printf("arbitrate: current pid=%d: pWaitRead (after) \n",cpid);
		}

		/* --- show process changes */
		if (debugcheck(dbgTaskSwitch,dbgNorm)) {
			if (currentpid!=cpid && 
			    cp->state!=pWaitRead) {
				uphe_printf("main loop: Task switch from pid=%d to pid=%d\n",cpid,currentpid);
				debug_halt(dbgTaskSwitch);
			}
		}
		
		if (debugcheck(dbgSysCall,dbgNorm))
			debug_return( cp,cwti );
   } while(currentpid<MAXPROCESSES); /* while active processes */
   /* End of emulation, all processes exited */
	
	
   debugprintf(dbgStartup,dbgDeep,("# os9exec_nt: exited main loop\n"));
   /* --- exit status of MPW tool comes from exit status of first process */
   cp=      &procs[0];
   cp->oerr= procs[0].exiterr;
   err= 0; /* assume no MPW error */
   
   if 	 (cp->oerr) {
      if (cp->oerr!=1) { 
			get_error_strings(cp->oerr, &errnam,&errdesc);
         err=2; /* MPW: some error in processing */ 
         upho_printf("OS-9 emulation exits with Error #%03d:%03d (%s):\n#   %s\n",
                      cp->oerr>>8,cp->oerr &0xFF,errnam,errdesc);
      }
      else err= cp->oerr;  
   }
   goto cleanup;

mainabort:
	get_error_strings( cp->oerr, &errnam,&errdesc );
	upho_printf( "Emulation could not start due to OS-9 error #%03d:%03d (%s): '%s'\n#   %s\n",
                  cp->oerr>>8,cp->oerr &0xFF,errnam, my_toolname, errdesc );
	err=3; /* MPW system/missing resource error */
	
cleanup:
   /* cleanup is also done automatically when exit() is called */
	debugprintf(dbgStartup,dbgNorm,("# os9exec_nt: cleaning up now\n"));
	cleanup();
	debugprintf(dbgStartup,dbgNorm,("# os9exec_nt: returning to caller with err=%d\n",err));
   return err;
} /* os9exec_nt */


/* Entry for old os9exec V1.14 style tools */
ushort os9exec(int argc,char **argv,char **envp)
{
	/* execute default module (OS9C ID=0) with no extra memory */
	return os9exec_nt( NULL, argc,argv,envp, 0, MYPRIORITY);
} /* os9exec */


/* eof */

