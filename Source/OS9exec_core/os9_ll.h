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
/* (c) 1993-2006 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        bfo@synthesis.ch                    */
/**********************************************/



#ifndef _os9_ll_h
#define _os9_ll_h


/* this is required to allow the use of original */
/* "module.h" with its ulong definition on Linux */
#ifdef linux
  #undef  __USE_MISC
  typedef unsigned long int ulong;
  typedef unsigned int uint;
//typedef int OSErr;

  #define USE_UAEMU
#endif

/* XCode does not know about macintosh */
#if !defined macintosh && defined __MACH__
  #define    macintosh
  #define    powerc
  #define    UNIX
  #define    USE_UAEMU
  
  // Support Intel Macs
  #ifndef __BIG_ENDIAN__
    #define __INTEL__
  #endif
#endif

#ifdef USE_UAEMU
  /* include file os9_uae.h */
  #include "sysconfig.h"
  #include "sysdeps.h"

  #ifdef _in_uae_c
    #define extern
  #endif
#endif

//#else
  #ifdef linux
    /* include file for empty interface */
    #include <sys/types.h>   /* according to Martin Gregorie's proposal */
    #include <stdio.h>
    #include <stdarg.h>
    #include <time.h>
  #endif
//#endif


#define CARRY      0x0001
#define SUPERVISOR 0x2000


#if !defined(USE_UAEMU) && !defined(linux)
  /* struct alignment must be 68k for these structs! */
  #pragma push
  #ifdef macintosh
    #ifndef powerc
    #pragma align=mac68k
    #endif
  #endif
#endif


#ifdef __INTEL__
    /* access register parts */
    #define loword(reg) (*(((ushort*)&(reg))+0))
    #define hiword(reg) (*(((ushort*)&(reg))+1))
    #define lobyte(reg) (*((  (byte*)&(reg))+0))

    /* return word/byte values */
    #ifdef PARTIALRETURNREGS
        #define retword(reg) (*(((ushort*)&(reg))+0))
        #define retbyte(reg)  *((  (byte*)&(reg))+0))
    #else
        #define retword(reg) (reg)
        #define retbyte(reg) (reg)
    #endif

    /* access bytes/words/longs in OS9-byteorder */
    /* (Intel needs byte reversal) */
    #define os9_byte(b)    (b)
    #define os9_word(w) ((((w)<<8) &0xFF00)    |(((w)>>8) &0x00FF))
    #define os9_long(l) ((((l)<<24)&0xFF000000)|(((l)>>24)&0x000000FF)|(((l)<<8)&0x00FF0000)|(((l)>>8)&0x0000FF00))
    /* no longer possible with backslash notation because of DOS file format of source !!! */
#else
    /* access register parts */
    #define loword(reg) (*(((ushort*)&(reg))+1))
    #define hiword(reg) (*(((ushort*)&(reg))+0))
    #define lobyte(reg) (*((  (byte*)&(reg))+3))
    /* return word/byte values */
    #ifdef PARTIALRETURNREGS
        #define retword(reg) (*(((ushort*)&(reg))+1))
        #define retbyte(reg)  *((  (byte*)&(reg))+3))
    #else
        #define retword(reg) (reg)
        #define retbyte(reg) (reg)
    #endif

    /* access bytes/words/longs in OS9-byteorder */
    /* (Mac and OS9 have the same byte order) */
    #define os9_byte(b) (b)
    #define os9_word(w) (w)
    #define os9_long(l) (l)
#endif


/* floating point register */
#ifdef USE_UAEMU
typedef  double        fp_typ;     
#else
typedef  unsigned char fp_typ[12];
#endif


#ifdef USE_UAEMU
/* buffer for interupt stack */
#define TRAPFRAMEBUFLEN 128 /* should be fairly enough for all stack frames */
extern ulong trapframebuf[TRAPFRAMEBUFLEN];
#endif


/* definitions for user traphandlers */
#define NUMTRAPHANDLERS 15
#define FIRSTTRAP       33 /* TRAP 1 */
#define FIRSTUSERTRAP    1


/* definition of the header of a OS-9 traphandler module */
typedef  struct {
            mod_exec progmod;
            long _mtrapinit;
            long _mtrapterm;
         } mod_trap;


/* traphandler description */
typedef  struct {
            /* don't move these 2, they are used by low-level code */
            ulong trapentry;
            ulong trapmem;
            
            /* other entries are used by high-level code only */
            ulong trapmemsz;
            mod_trap *trapmodule;
            ushort mid;
            ushort dummy;
         } traphandler_typ;

#define TRAPENTRYSIZE sizeof(traphandler_typ)

/* os9regs.flags: */
#define FLAGS_FPU 0x0001   /* set if FPU is used */
#define FLAGS_IRQ 0x0002   /* allow IRQs for this process */
#define FLAGS_UM  0x0004   /* Mac context runs in user mode */


#ifdef USE_UAEMU
  #ifndef flagtype
    typedef char flagtype;
  #endif
#endif

/* register context:
   !!!!!!!! must be STRUCTURALLY EQUAL to regstruct in newcpu.h */
#ifdef USE_UAEMU
  typedef struct {
#else
  typedef volatile struct {
#endif

    /* common in UAE and os9exec */
    ulong d[8]; /* data registers */
    ulong a[8]; /* address registers */

    #ifdef USE_UAEMU
      /* UAE only */
      uae_u32 usp,isp,msp;
    #endif
    
    /* common */
    unsigned short sr;   /* status register */
    unsigned short flags;/* divs OS9exec specific flags */

    #ifdef USE_UAEMU
      /* UAE only */
      flagtype t1;
      flagtype t0;
      flagtype s;
      flagtype m;
      flagtype x;
      flagtype stopped;
      int intmask;
    #endif

    /* common */
    ulong pc;               /* program counter */

    #ifdef USE_UAEMU
    /* UAE only */
    uae_u8 *pc_p;
    uae_u8 *pc_oldp;
    uae_u32 vbr,sfc,dfc;
    #endif
    
    /* common */                
    fp_typ fp[8];        /* FPU data registers */
    ulong fpcr;
    ulong fpsr;
    ulong fpiar;            /* FPU control registers */ 

    #ifdef USE_UAEMU
      /* UAE only */             
      uae_u32 prefetch;
    #endif

    /* --- link to traphandler entry of TRAP #1 */
    traphandler_typ* ttP;
    /* --- base address of process' static storage */
    void *membase;          /* used to fool StackSniffer via modified ApplLimit */
}   regs_type;



/* high level interface */
/* -------------------- */

#ifdef USE_UAEMU
extern void handle_os9exec_exception(int nr, uaecptr oldpc);
#endif


ulong llm_os9_go(regs_type *rp);

#if defined macintosh && !defined __MACH__
  void llm_os9_debug( regs_type *rp, ConstStr255Param debuggerMsg );
#endif

Boolean llm_fpu_present     (void);
Boolean llm_vm_enabled      (void);
Boolean llm_has_cache       (void);
Boolean llm_runs_in_usermode(void);

void  Flush68kCodeRange(void *address, ulong size);
void  LockMemRange     (void *address, ulong size);
void  UnlockMemRange   (void *address, ulong size);

OSErr lowlevel_prepare (void);
void  lowlevel_release (void);


#ifdef USE_UAEMU
#undef extern
#endif

#if !defined(USE_UAEMU) && !defined(linux)
#pragma pop
#endif


#endif

/* eof */
