// 
// Ê ÊOS9exec,   OS-9 emulator for Mac OS, Windows and Linux 
// Ê ÊCopyright (C) 2002 Ê[ Lukas Zeller / Beat Forster ]
//	  Available under http://www.synthesis.ch/os9exec
// 
// Ê ÊThis program is free software; you can redistribute it and/or 
// Ê Êmodify it under the terms of the GNU General Public License as 
// Ê Êpublished by the Free Software Foundation; either version 2 of 
// Ê Êthe License, or (at your option) any later version. 
// 
// Ê ÊThis program is distributed in the hope that it will be useful, 
// Ê Êbut WITHOUT ANY WARRANTY; without even the implied warranty of 
// Ê ÊMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// Ê ÊSee the GNU General Public License for more details. 
// 
// Ê ÊYou should have received a copy of the GNU General Public License 
// Ê Êalong with this program; if not, write to the Free Software 
// Ê ÊFoundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
//

/*******************************************/
/*             OS9exec/nt                  */
/*    OS-9 emulator for Apple Macintosh    */
/*   (c) 1996 by ZEP electroncics          */
/*            Lukas Zeller                 */
/*             luz@zep.ch                  */
/*******************************************/

#include "os9exec_incl.h"

// #include <OSUtils.h>
// #include <Gestalt.h>
// #include <Traps.h>

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
#endif


/* routines */

static Boolean TrapAvailable ( short tNum)
{
    return false;
}



/* enter OS9 context */
ulong llm_os9_go(regs_type *rp)
{
    return 0;
} /* llm_os9_go */


/* Flush code range in 68k (real one or emulator, especially DR emulator needs it!!) */
static void llm_cache_rf(void *addr, ulong size)
{
} /* llm_cache_rf */


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
} /* Flush68kCodeRange */


/* Ensure that specified range is held paged in memory and never paged out by VM */
void LockMemRange(void *address, ulong size)
{
} /* LockMemRange */


/* Release specified range from VM paging lock */
void UnlockMemRange(void *address, ulong size)
{
} /* UnlockMemRange */


/* prepare access to low-level code */
OSErr lowlevel_prepare(void)
{
    return(0);
} /* lowlevel_prepare */


/* uninstall low-level code */
void lowlevel_release(void)
{
} /* lowlevel_release */

/* eof */