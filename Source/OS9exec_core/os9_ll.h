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
 *
 */

#ifndef _os9_ll_h
#define _os9_ll_h

/* this is required to allow the use of original */
/* "module.h" with its ulong definition on Linux */
#undef __USE_MISC
typedef unsigned long int ulong;
typedef unsigned int      uint;

#ifdef __cplusplus
typedef int OSErr;
#endif

#define USE_UAEMU

/* XCode does not know about macintosh */
#if !defined macintosh && defined __MACH__
#define macintosh
#define powerc
#define UNIX
#define USE_UAEMU

// Support Intel Macs
#ifndef __BIG_ENDIAN__
#define __INTEL__
#endif
#endif

/* include file os9_uae.h */
#include "sysconfig.h"
#include "sysdeps.h"

#ifdef _in_uae_c
#define extern
#endif

#include <sys/types.h> /* according to Martin Gregorie's proposal */
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#define CARRY 0x0001
#define SUPERVISOR 0x2000

#ifdef __INTEL__
/* access register parts */
#define loword(reg) (*(((ushort *)&(reg)) + 0))
#define hiword(reg) (*(((ushort *)&(reg)) + 1))
#define lobyte(reg) (*(((byte *)&(reg)) + 0))

/* return word/byte values */
#ifdef PARTIALRETURNREGS
#define retword(reg) (*(((ushort *)&(reg)) + 0))
#define retbyte(reg)  *((  (byte*)&(reg))+0))
#else
#define retword(reg) (reg)
#define retbyte(reg) (reg)
#endif

/* access bytes/words/longs in OS9-byteorder */
/* (Intel needs byte reversal) */
#define os9_byte(b) (b)
#define os9_word(w) ((((w) << 8) & 0xFF00) | (((w) >> 8) & 0x00FF))
#define os9_long(l)                                                            \
    ((((l) << 24) & 0xFF000000) | (((l) >> 24) & 0x000000FF) |                 \
     (((l) << 8) & 0x00FF0000) | (((l) >> 8) & 0x0000FF00))
/* no longer possible with backslash notation because of DOS file format of
 * source !!! */
#else
/* access register parts */
#define loword(reg) (*(((ushort *)&(reg)) + 1))
#define hiword(reg) (*(((ushort *)&(reg)) + 0))
#define lobyte(reg) (*(((byte *)&(reg)) + 3))
/* return word/byte values */
#ifdef PARTIALRETURNREGS
#define retword(reg) (*(((ushort *)&(reg)) + 1))
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
typedef double fp_typ;

/* buffer for interupt stack */
#define TRAPFRAMEBUFLEN 128 /* should be fairly enough for all stack frames */
extern ulong trapframebuf[TRAPFRAMEBUFLEN];

/* definitions for user traphandlers */
#define NUMTRAPHANDLERS 15
#define FIRSTTRAP 33 /* TRAP 1 */
#define FIRSTUSERTRAP 1

/* definition of the header of a OS-9 traphandler module */
typedef struct {
    mod_exec progmod;
    long     _mtrapinit;
    long     _mtrapterm;
} mod_trap;

/* traphandler description */
typedef struct traphandler_typ_s {
    /* don't move these 2, they are used by low-level code */
    ulong trapentry;
    ulong trapmem;

    /* other entries are used by high-level code only */
    ulong     trapmemsz;
    mod_trap *trapmodule;
    ushort    mid;
    ushort    dummy;
} traphandler_typ;

#define TRAPENTRYSIZE sizeof(traphandler_typ)

/* os9regs.flags: */
#define FLAGS_FPU 0x0001 /* set if FPU is used */
#define FLAGS_IRQ 0x0002 /* allow IRQs for this process */
#define FLAGS_UM 0x0004  /* Mac context runs in user mode */

#ifndef flagtype
typedef char flagtype;
#endif

#include "regs_type.h"

/* high level interface */
/* -------------------- */

extern void handle_os9exec_exception(int nr, uaecptr oldpc);

ulong llm_os9_go(regs_type *rp);
void  llm_os9_copyback(regs_type *rp);

Boolean llm_fpu_present(void);
Boolean llm_vm_enabled(void);
Boolean llm_has_cache(void);
Boolean llm_runs_in_usermode(void);

void Flush68kCodeRange(void *address, ulong size);
void LockMemRange(void *address, ulong size);
void UnlockMemRange(void *address, ulong size);

OSErr lowlevel_prepare(void);
void  lowlevel_release(void);

#undef extern

#endif

/* eof */
