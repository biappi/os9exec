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



/* os9 service request */
/* defines for register usage */
#define d_b(x) (0x01 << (x+x))
#define d_w(x) (0x02 << (x+x))
#define d_l(x) (0x03 << (x+x))
#define a_l(x) (0x03 << (x+x+16))
#define a_p(x) (0x02 << (x+x+16)) /* string name pointer (bfo) */
#define SFUNC_STATCALL  0x80000000
#define SEVENT_CALL     0x40000000
#define REGMASK_REGBITS 0x3FFFFFFF

typedef os9err os9func_typ(regs_type *rp, ushort cpid);

typedef struct {
            os9func_typ *func;
            char *name;
            ulong inregs;
            ulong outregs;
        } funcdispatch_entry;

#define NUMFCALLS   0x61
extern const funcdispatch_entry fcalltable[NUMFCALLS];
#define ICALLS_Offs 0x80
#define NUMICALLS   0x13
extern const funcdispatch_entry icalltable[NUMICALLS];

extern  ulong  rw__idleticks; /* set by read/write of console */
extern  ulong  slp_idleticks; /* set by arbitration sleep */
extern  ulong glob_callticks;

#define STIM_NONE           0
#define STIM_FCALLS    0x0001
#define STIM_ICALLS    0x0002
#define STIM_OS9       0x0004
#define STIM_TICKAVAIL 0x0008
#define STIM_PERCENT   0x0010
#define STARTCALL      0xffff

ulong GetSystemTick(void);

const funcdispatch_entry *getfuncentry(ushort func);

void os9_to_xxx             ( ushort pid, const char* name );
void xxx_to_arb( ushort func, ushort pid );
void arb_to_os9( Boolean last_arbitrate  );


os9err exec_syscall(ushort func, ushort pid, regs_type *rp);

void   init_L2(void);
void   init_syscalltimers(void);
ulong  show_timing(ushort mode);
os9err int_systime(ushort pid, int argc, char **argv);

/* eof */