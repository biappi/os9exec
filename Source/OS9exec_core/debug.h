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


/* debugging aids */
/* debug mask definitions */

#define dbgAnomaly      0x0001   /* anomalies such as unimplemented/dummy system calls (default) */
#define dbgSysCall      0x0002   /* call and return status of system calls */
#define dbgFMgrType     0x0004   /* call of traphandler calls */
#define dbgProcess      0x0008   /* Process creation/killing, signals */
#define dbgTaskSwitch   0x0010   /* task switching/arbitration */
#define dbgModules      0x0020   /* module link/load/unlink & CRC */
#define dbgMemory       0x0040   /* memory allocation/deallocation */
#define dbgUtils        0x0080   /* internal utilities */
#define dbgTrapHandler  0x0100   /* traphandlers and exception handlers */
#define dbgFiles        0x0200   /* file operations */
#define dbgErrors       0x0400   /* error generation/translation */
#define dbgStartup      0x0800   /* os9exec/nt startup and initial argument preparation */
#define dbgPartial      0x1000   /* warnings for inaccurate/partial emulated functionality */
#define dbgTerminal     0x2000   /* interactive I/O, output filtering */

#define dbgWarnings     0x8000   /* warnings */

#define dbgAllInfo      0xFFFE   /* all except anomalies */
#define dbgAll          0xFFFF   /* all debug info */


/* debug levels/options */
#define DEBUGLEVELS 3
#define dbgNorm              0       /* normal debug info */
#define dbgDetail            1       /* detailed debug info */
#define dbgDeep              2       /* deep detail debug info */
#define dbgLevelMask        0x000F   /* mask for debug level index */

#define dbgNohalt           0x8000   /* suppress halt here, even if enabled */


/* debugging masks */
extern ushort debug[];                          /* debug level array */
extern ushort debughalt;                        /* debug halt bitmask */
#define TRIGNAMELEN 30
extern char triggername[TRIGNAMELEN];           /* debug trigger name */


/* routines */

#define RCHK_DRU 0x01
#define RCHK_ARU 0x02
#define RCHK_MEM 0x04
#define RCHK_MOD 0x08

Boolean out_of_mods(ulong addr);
Boolean out_of_mem(ushort pid,ulong addr);


#ifndef NODEBUG
/* real debug routines */
Boolean debugcheck ( ushort mask, ushort level );
Boolean debugprintf( ushort mask, ushort level, char *format, ... );
#define debugprintf(a,b,c) if (debugcheck(a,b)) _debugprintf c;

ushort  debug_halt( ushort haltmask );
void    regcheck  ( ushort pid, char *nam,ulong reg,ushort mode );
void    trigcheck ( char  *msg, char *name );
#else
/* avoid code generation (and argument preparation) at all */
#define debugcheck(a,b) (false)
#define debugprintf(a,b,c) 
#define debug_halt(a) (false)
#define regcheck(r,s,p,m)
#define trigcheck(m,s)
#endif

void  _debugprintf( char *format, ... );
void   debug_prep ( void );
os9err debug_help ( ushort pid, int argc, char **argv );
void   dumpregs   ( ushort pid );
ushort debugwait  ( void );

void showonereg(ulong value, Boolean isa, ushort regnum, ushort lenspec);
void show_maskedregs(regs_type *rp, ulong regmask);

char* get_stat_name(ushort stat);
char* get_ev_name  (ushort ev);
void  get_error_strings(os9err err, char **nameP, char **descP);
char* get_syscall_name(ushort syscall); 

/* eof */