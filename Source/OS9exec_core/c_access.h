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
 *    Revision 1.3  2007/01/29 22:32:49  bfo
 *    ulong support for MPW
 *
 *    Revision 1.2  2007/01/28 21:47:57  bfo
 *    - __GLOBDEF added / Boolean => int /
 *    - 'nativeinfo_typ' added
 *    - getversion/lVersion implemented here now
 *    - 'ustrcmp' implemented here now
 *
 *    Revision 1.1  2007/01/07 13:25:33  bfo
 *    *** NEW MODULE ADDED ***
 *
 *
 */

// ----- C access from Plugin side and OS9exec internal use -----
#include "target_options.h"

#ifndef __GLOBDEF
#define __GLOBDEF
typedef unsigned char  byte;
typedef unsigned short os9err;

#if defined __INTEL__ || defined __MACH__
typedef char                     Boolean;
#define true 1
#define false 0
#endif

#if defined __MACH__ || defined __GNUC__ || defined MPW
typedef unsigned long                               ulong;
#endif

#if !defined __MACH__ || defined __GNUC__
typedef unsigned short           ushort;
#endif

#define VHPCNT
#endif //__GLOBDEF

#ifndef VHPCNT
typedef unsigned short ushort;
#endif

// --------------------------------------------------------------
#if defined __cplusplus
extern "C" {
#endif

#define No_Module (void *)1 // predefined value for 'no module'

// 68k registers
typedef struct {
    ulong d[8];
    ulong a[8];
} Regs_68k;

// The callback function for system calls
typedef os9err (*Trap0_Call_Typ)(ushort code, Regs_68k *regs);

typedef int (*StrToShort_Typ)(short *i, const char *str);
typedef int (*StrToFloat_Typ)(float *f, const char *str);
typedef int (*StrToDouble_Typ)(double *d, const char *str);

// The callback structure
typedef struct {
    Trap0_Call_Typ trap0; // OS-9 system calls

    StrToShort_Typ  strToShort;
    StrToFloat_Typ  strToFloat;
    StrToDouble_Typ strToDouble;
} callback_typ;

// the callback structure reference
extern callback_typ *cbP;

typedef struct {
    ushort       *pid;
    void         *modBase;
    void         *os9_args;
    callback_typ *cbP;
} nativeinfo_typ;

// Get the OS9exec version
void getversion(ushort *ver, ushort *rev);
long lVersion(void);

// ---- some utility functions are implemented here -----------------------
// Memory block copy
void MoveBlk(void *dst, void *src, ulong size);

// case insensitive version of strcmp
int ustrcmp(const char *s1, const char *s2);

// ---- missing "sprintf" operations for "cclib", temporary placed here ----
void IntToStr(char *s, int i);
void IntToStrN(char *s, int i, int n);

void UIntToStr(char *s, unsigned int h);
void UIntToStrN(char *s, unsigned int h, int n);

void BoolToStr(char *s, int bo);
void BoolToStrN(char *s, int bo, int n);

void RealToStr(char *s, double d, int res);
void RealToStrN(char *s, double d, int n, int res);

#if defined __cplusplus
} // end extern "C"
#endif

/* eof */
