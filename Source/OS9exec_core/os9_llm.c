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
/* (c) 1993-2004 by Lukas Zeller, CH-Zuerich  */
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
 *    Revision 1.4  2003/05/17 11:27:30  bfo
 *    (CVS header included)
 *
 *
 */

#include "os9exec_incl.h"

#include <OSUtils.h>
#include <Gestalt.h>
#include <Traps.h>

/* universal interface to 68k low level magic */
/* ========================================== */

/* statics */
static Boolean needsflush;
static Boolean hasfpu;
static Boolean vmenabled;
static Boolean fullcache;
static Boolean usesusermode;


#if GENERATINGCFM
/* PPC caller */
/* ---------- */

/* resource ID of low-level-magic code resource */
#define LLMRESID 200
#define LLMRESTY 'O9LM'

/* static low level vars */
static Handle llmhandle=NULL;

/* low level interface definition */
enum {
     uppOS9goProcInfo = kCStackBased 
         | RESULT_SIZE(SIZE_CODE(sizeof(ulong))) 
         | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(ulong)))             /* selector */
         | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(regs_type*))),       /* regs ptr */
     uppOS9debugProcInfo = kCStackBased 
         | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(ulong)))             /* selector */
         | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(regs_type*)))        /* regs ptr */
         | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(ulong))),            /* debugger message */
     uppCacheRFProcInfo = kCStackBased 
         | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(ulong)))             /* selector */
         | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(void*)))             /* area pointer */
         | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(ulong))),            /* area size */
     uppUMProbeProcInfo = kCStackBased 
         | RESULT_SIZE(SIZE_CODE(sizeof(ulong))) 
         | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(ulong)))             /* selector */
};
#define SEL_OS9GO           0
#define SEL_CACHERF         1
#define SEL_OS9DEBUG        2
#define SEL_UMPROBE         3

#else
/* 68k caller */
/* ---------- */

#pragma push
#pragma align=mac68k
#pragma d0_pointers on
#pragma mpwc on
/* returns either: - vector=0, func=OS9-function (OS-9 system call trough TRAP0)
 *                 - vector=TRAP1..15 vector, func=TCALL-function, stack ready for calling
 *                   trap handler routine. OS9 regs must be set up (A6, PC).
 *                 - vector=$FAFA, func=error trap vector offset, stack ready for F$STrap
 *                   service routine. OS9 regs (except pc,a6 and eventually A7) prepared
 *                   for service routine call, too.
 */
extern ulong OS9_GO(ulong, regs_type *rp);
extern void OS9_DEBUG(ulong, regs_type *rp,ConstStr255Param debuggerMsg);
extern void OS9_CACHE_RF(ulong, void *addr, ulong size);
extern ulong OS9_UMPROBE(void);
#pragma pop
#endif


/* routines */

#if !GENERATINGPOWERPC
/* everyone uses it, but no 68k library provides it (but MW C Tutorial 2 68k does!) */
static Boolean TrapAvailable ( short tNum)
{
    if (NGetTrapAddress(tNum,OSTrap) != GetToolTrapAddress(_Unimplemented)) return true;
    return NGetTrapAddress(tNum,ToolTrap) != GetToolTrapAddress(_Unimplemented);
// MW   return ( NGetTrapAddress(tNum,tType) != GetTrapAddress(_Unimplemented) );
}
#endif


/* enter OS9 context */
ulong llm_os9_go(regs_type *rp)
{
    #if GENERATINGCFM
    return CallUniversalProc((UniversalProcPtr)(*llmhandle), uppOS9goProcInfo, SEL_OS9GO, rp);
    #else
    return OS9_GO(0,rp);
    #endif
} /* llm_os9_go */


/* Flush code range in 68k (real one or emulator, especially DR emulator needs it!!) */
static void llm_cache_rf(void *addr, ulong size)
{
    #if GENERATINGCFM
    CallUniversalProc((UniversalProcPtr)(*llmhandle), uppCacheRFProcInfo, SEL_CACHERF, addr, size);
    #else
    OS9_CACHE_RF(0,addr,size);
    #endif
} /* llm_cache_rf */


/* enter debugger with (most of) OS9 context in registers */
void llm_os9_debug(regs_type *rp, ConstStr255Param debuggerMsg)
{
    #if GENERATINGCFM
    if (llmhandle==NULL) {
        upho_printf("llm_os9_debug (PPC): low-level not prepared, can't go to debugger\n");
    }
    else {
        CallUniversalProc((UniversalProcPtr)(*llmhandle), uppOS9debugProcInfo, SEL_OS9DEBUG, rp, debuggerMsg);
    }
    #else
    OS9_DEBUG(0,rp,debuggerMsg);
    #endif
} /* llm_os9_debug */


/* returns if FPU present */
Boolean llm_has_cache(void)
{
    return needsflush;
} /* llm_fpu_present */


/* returns if FPU present */
Boolean llm_fpu_present(void)
{
    return hasfpu;
} /* llm_fpu_present */


/* returns if virtual memory is enabled */
Boolean llm_vm_enabled(void)
{
    return vmenabled;
} /* llm_fpu_present */


/* returns if 68k Mac Context runs in usermode */
/* (seems to be the case with newer G3/MacOS8.5 combinations) */
Boolean llm_runs_in_usermode(void)
{
    return usesusermode;
} /* llm_runs_in_usermode */


/* flush given range so it can be executed as 68k code (by emulator or real 68k) */
void Flush68kCodeRange(void *address, ulong size)
{
    if (needsflush) {
        #if GENERATINGPOWERPC
        /* no separate caches on PPC */
        //  MakeDataExecutable(address, size);
        /* switch to 68k required to flush DR emulator's cache */
        llm_cache_rf(address,size);
        #else
        /* instruction cache */
        if (fullcache) {
            FlushCodeCacheRange(address, size); /* make sure llm code is safely installed */
            /* data cache */
            //FlushDataCache(); /* if we really need it */
        }
        else FlushCodeCache();
        #endif
    }   
} /* Flush68kCodeRange */


/* Ensure that specified range is held paged in memory and never paged out by VM */
void LockMemRange(void *address, ulong size)
{
    if (vmenabled) {
        LockMemory(address,size);
    }
} /* LockMemRange */


/* Release specified range from VM paging lock */
void UnlockMemRange(void *address, ulong size)
{
    if (vmenabled) {
        UnlockMemory(address,size);
    }
} /* UnlockMemRange */


/* prepare access to low-level code */
OSErr lowlevel_prepare(void)
{
    ulong gestaltresponse;
    
    /* --- get environment flags */
    /* processor type */
    if (Gestalt(gestaltProcessorType, &gestaltresponse)==0) { /* get 680xx processor type */
        needsflush=(gestaltresponse>=gestalt68020); /* 68020 and above have an instruction chache */
    }
    else needsflush=false; /* no gestalt, no cache */
    /* FPU */
    if (Gestalt(gestaltFPUType, &gestaltresponse)==0) { /* get 68xxx FPU type */
        hasfpu=(gestaltresponse!=gestaltNoFPU); /* has some kind of FPU */
    }
    else hasfpu=false; /* no gestalt, no FPU */

    /* cache services */
    #ifndef TRAPAVAIL_UNAVAILABLE
    fullcache=   (TrapAvailable(_HWPriv)); /* check if we have all the Cache control calls */
    #else
    fullcache=(GetOSTrapAddress(_HWPriv) != NULL); /* %%% New method */
    #endif
    
    /* virtual memory */
    if (Gestalt(gestaltVMAttr, &gestaltresponse)==0) { /* get VM attribute */
        vmenabled=(gestaltresponse & (1<<gestaltVMPresent)); /* set if VM is on */
    }
    else vmenabled=false; /* no gestalt, no virtual memory */
    /* --- install code resource if needed */

    #if GENERATINGCFM
    /* PPC native caller, install code resource */
    llmhandle = Get1Resource(LLMRESTY, LLMRESID);
    if (llmhandle==NULL) return ResError();
    HLock(llmhandle);
    /* --- make the low-level code safe */
    {
        ulong size = GetHandleSize(llmhandle);
        Flush68kCodeRange(*llmhandle, size); /* make sure llm code is safely installed */
        debugprintf(dbgStartup,dbgDeep,("# lowlevel_prepare: (PPC) 'O9LM' loaded at $%08lX\n",*llmhandle));
    }
    /* test user mode */
    usesusermode=CallUniversalProc((UniversalProcPtr)(*llmhandle), uppUMProbeProcInfo, SEL_UMPROBE);
    #else
    /* caller is 68k, directly linked os9_llm */
    debugprintf(dbgStartup,dbgDeep,("# lowlevel_prepare: (68k) low level code is static linked\n"));
    /* test user mode */
    usesusermode=OS9_UMPROBE();
    
    /* %%% for now, 68k-only version cannot handle user-mode-only operation */
    if (usesusermode) {
        debugprintf(dbgStartup,dbgDeep,("# lowlevel_prepare: (68k) 68k-low-level code cannot yet handle Mac program running in user mode\n"));
        uphe_printf("FATAL ERROR: 'os968k' cannot run on this PowerMac / on this operating system\n");      
        uphe_printf("              Please use the PowerPC-native 'os9ppc'\n"); 
        uphe_printf("                                 or the FAT 'os9' tool instead\n");        
        uphe_printf("\n");
        
        #ifdef  TERMINAL_CONSOLE
          gConsoleID= NO_CONSOLE; /* final condition for no console */
          g_spP     = NULL;
          RemoveConsole();
        #endif
        
        return -1;
    }
    #endif
    
    /* now get info about usage of user mode */
    debugprintf(dbgStartup,dbgDeep,("# lowlevel_prepare: vmenabled=%d, needsflush=%d, fullcache=%d, hasfpu=%d, usesusermode=%d\n",vmenabled,needsflush,fullcache,hasfpu,usesusermode));
    return 0;
} /* lowlevel_prepare */


/* uninstall low-level code */
void lowlevel_release(void)
{
    /* release code resource if PPC */
    #if GENERATINGCFM
    if (llmhandle!=NULL) {
        ReleaseResource(llmhandle); /* forget the block */
    }
    llmhandle=NULL; /* not there any more */
    #endif  
} /* lowlevel_release */

/* eof */
