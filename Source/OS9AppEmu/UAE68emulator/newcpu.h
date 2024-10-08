 /*
  * UAE - The Un*x Amiga Emulator
  *
  * MC68000 emulation
  *
  * Copyright 1995 Bernd Schmidt
  */

#include "m68k.h"

#ifndef SET_CFLG

#define SET_CFLG(x) (CFLG = (x))
#define SET_NFLG(x) (NFLG = (x))
#define SET_VFLG(x) (VFLG = (x))
#define SET_ZFLG(x) (ZFLG = (x))
#define SET_XFLG(x) (XFLG = (x))

#define GET_CFLG CFLG
#define GET_NFLG NFLG
#define GET_VFLG VFLG
#define GET_ZFLG ZFLG
#define GET_XFLG XFLG

#define CLEAR_CZNV do { \
 SET_CFLG (0); \
 SET_ZFLG (0); \
 SET_NFLG (0); \
 SET_VFLG (0); \
while (0)

#define COPY_CARRY (SET_XFLG (GET_CFLG))
#endif

extern int areg_byteinc[];
extern int imm8_table[];

extern int movem_index1[256];
extern int movem_index2[256];
extern int movem_next[256];

extern int fpp_movem_index1[256];
extern int fpp_movem_index2[256];
extern int fpp_movem_next[256];

extern int broken_in;

extern int os9_running;
extern unsigned long m68_os9go_result;


typedef unsigned long cpuop_func (uae_u32) REGPARAM;

struct cputbl {
    cpuop_func *handler;
    int specific;
    uae_u16 opcode;
};

extern unsigned long op_illg (uae_u32) REGPARAM;

#ifndef _os9_ll_h
typedef char flagtype;
#endif


#include "regs_type.h"

extern regs_type regs, lastint_regs;

#define m68k_dreg(r,num) ((r).regs[(num)])
#define m68k_areg(r,num) (((r).regs + 8)[(num)])

#define get_ibyte(o) get_byte(regs.pc_p + (o) + 1)
#define get_iword(o) get_word(regs.pc_p + (o))
#define get_ilong(o) get_long(regs.pc_p + (o))

#ifdef HAVE_GET_WORD_UNSWAPPED
#define GET_OPCODE (do_get_mem_word_unswapped (regs.pc_p))
#else
#define GET_OPCODE (get_iword (0))
#endif

static __inline__ void fill_prefetch_0 (void) { }
static __inline__ void fill_prefetch_2 (void) { }

static __inline__ uae_u32 get_ibyte_prefetch (uae_s32 o)
{
	return get_byte(regs.pc_p + o + 1);
}
static __inline__ uae_u32 get_iword_prefetch (uae_s32 o)
{
	return get_word(regs.pc_p + o);
}
static __inline__ uae_u32 get_ilong_prefetch (uae_s32 o)
{
	return get_long(regs.pc_p + o);
}

#define m68k_incpc(o) (regs.pc_p += (o))

/* These are only used by the 68020/68881 code, and therefore don't
 * need to handle prefetch.  */
static __inline__ uae_u32 next_ibyte (void)
{
    uae_u32 r = get_ibyte (0);
    m68k_incpc (2);
    return r;
}

static __inline__ uae_u32 next_iword (void)
{
    uae_u32 r = get_iword (0);
    m68k_incpc (2);
    return r;
}

static __inline__ uae_u32 next_ilong (void)
{
    uae_u32 r = get_ilong (0);
    m68k_incpc (4);
    return r;
}

static __inline__ void m68k_setpc (uaecptr newpc)
{
    regs.pc_p = regs.pc_oldp = newpc;
    regs.pc = newpc;
}

static __inline__ uaecptr m68k_getpc (void)
{
    return regs.pc + (regs.pc_p - regs.pc_oldp);
}

static __inline__ uaecptr m68k_getpc_p (uaecptr p)
{
    return regs.pc + (p - regs.pc_oldp);
}

#define m68k_setpc_fast m68k_setpc
#define m68k_setpc_bcc  m68k_setpc
#define m68k_setpc_rte  m68k_setpc

static __inline__ void m68k_setstopped (int stop)
{
    regs.stopped = stop;
    // if (stop)
	// 	regs.spcflags |= SPCFLAG_STOP; // Luz %%% No custom chip...
}

extern uae_u32 get_disp_ea_020 (uae_u32 base, uae_u32 dp);
extern uae_u32 get_disp_ea_000 (uae_u32 base, uae_u32 dp);

extern uae_s32 ShowEA (int reg, amodes mode, wordsizes size, char *buf);

extern void MakeSR (void);
extern void MakeFromSR (void);
extern void Exception (int, uaecptr);
extern void dump_counts (void);
extern void m68k_move2c (int, uae_u32 *);
extern void m68k_movec2 (int, uae_u32 *);
extern void m68k_divl (uae_u32, uae_u32, uae_u16, uaecptr);
extern void m68k_mull (uae_u32, uae_u32, uae_u16);
extern void init_m68k (void);
extern unsigned long m68k_os9go (void); // special emulator call for os9exec
extern void m68k_go (int);
extern void m68k_dumpstate (uaecptr *, int);
extern void m68k_disasm (uaecptr, uaecptr *, int);
extern void m68k_reset (void);

extern void mmu_op (uae_u32, uae_u16);

extern void fpp_opp (uae_u32, uae_u16);
extern void fdbcc_opp (uae_u32, uae_u16);
extern void fscc_opp (uae_u32, uae_u16);
extern void ftrapcc_opp (uae_u32,uaecptr);
extern void fbcc_opp (uae_u32, uaecptr, uae_u32);
extern void fsave_opp (uae_u32);
extern void frestore_opp (uae_u32);

/* Opcode of faulting instruction */
extern uae_u16 last_op_for_exception_3;
/* PC at fault time */
extern uaecptr last_addr_for_exception_3;
/* Address that generated the exception */
extern uaecptr last_fault_for_exception_3;

#define CPU_OP_NAME(a) op ## a

/* 68020 + 68881 */
extern struct cputbl op_smalltbl_0[];
/* 68020 */
extern struct cputbl op_smalltbl_1[];
/* 68010 */
extern struct cputbl op_smalltbl_2[];
/* 68000 */
extern struct cputbl op_smalltbl_3[];
/* 68000 slow but compatible.  */
extern struct cputbl op_smalltbl_4[];

extern cpuop_func *cpufunctbl[65536] ASM_SYM_FOR_FUNC ("cpufunctbl");


