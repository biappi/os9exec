 /*
  * UAE - The Un*x Amiga Emulator
  *
  * MC68000 emulation
  *
  * (c) 1995 Bernd Schmidt
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "config.h"
#include "options.h"
#include "memory.h"
#include "readcpu.h"
#include "newcpu.h"
#include "compiler.h"

#include "luzstuff.h"

#include <sys/types.h>
#include <inttypes.h>

#ifdef __GNUC__
  typedef short Boolean;
  #define true  1
  #define false 0
#endif

extern int withTitle;

/* Opcode of faulting instruction */
uae_u16 last_op_for_exception_3;
/* PC at fault time */
uaecptr last_addr_for_exception_3;
/* Address that generated the exception */
uaecptr last_fault_for_exception_3;

int areg_byteinc[] = { 1,1,1,1,1,1,1,2 };
int imm8_table[] = { 8,1,2,3,4,5,6,7 };

int movem_index1[256];
int movem_index2[256];
int movem_next[256];

int fpp_movem_index1[256];
int fpp_movem_index2[256];
int fpp_movem_next[256];

cpuop_func *cpufunctbl[65536];

// flag to exit out of emulator on OS9 related occasions (Traps etc.)
int os9_running=0;
unsigned long m68_os9go_result;

#define COUNT_INSTRS 0

#if COUNT_INSTRS
static unsigned long int instrcount[65536];
static uae_u16 opcodenums[65536];

static int compfn (const void *el1, const void *el2)
{
    return instrcount[*(const uae_u16 *)el1] < instrcount[*(const uae_u16 *)el2];
}

static char *icountfilename (void)
{
    char *name = getenv ("INSNCOUNT");
    if (name)
	return name;
    return COUNT_INSTRS == 2 ? "frequent.68k" : "insncount";
}

void dump_counts (void)
{
    FILE *f = fopen (icountfilename (), "w");
    unsigned long int total;
    int i;

    write_log ("Writing instruction count file...\n");
    for (i = 0; i < 65536; i++) {
	opcodenums[i] = i;
	total += instrcount[i];
    }
    qsort (opcodenums, 65536, sizeof(uae_u16), compfn);

    fprintf (f, "Total: %lu\n", total);
    for (i=0; i < 65536; i++) {
	unsigned long int cnt = instrcount[opcodenums[i]];
	struct instr *dp;
	struct mnemolookup *lookup;
	if (!cnt)
	    break;
	dp = table68k + opcodenums[i];
	for (lookup = lookuptab;lookup->mnemo != dp->mnemo; lookup++)
	    ;
	fprintf (f, "%04x: %lu %s\n", opcodenums[i], cnt, lookup->name);
    }
    fclose (f);
}
#else
void dump_counts (void)
{
}
#endif

int broken_in;

#define cft_map(f) f

static unsigned long op_illg_1 (uae_u32 opcode) REGPARAM;

static unsigned long REGPARAM2 op_illg_1 (uae_u32 opcode)
{
    op_illg (cft_map (opcode));
    return 4;
}

static void build_cpufunctbl (void)
{
    int i;
    unsigned long opcode;
    struct cputbl *tbl = (currprefs.cpu_level == 3 ? op_smalltbl_0
			  : currprefs.cpu_level == 2 ? op_smalltbl_1
			  : currprefs.cpu_level == 1 ? op_smalltbl_2
			  : currprefs.cpu_compatible ? op_smalltbl_4
			  : op_smalltbl_3);

    if (withTitle) write_log ("# Building CPU function table (%d %d %d).\n",
	       currprefs.cpu_level, currprefs.cpu_compatible, currprefs.address_space_24);

    for (opcode = 0; opcode < 65536; opcode++)
	cpufunctbl[cft_map (opcode)] = op_illg_1;
    for (i = 0; tbl[i].handler != NULL; i++) {
	if (! tbl[i].specific)
	    cpufunctbl[cft_map (tbl[i].opcode)] = tbl[i].handler;
    }
    for (opcode = 0; opcode < 65536; opcode++) {
	cpuop_func *f;

	if (table68k[opcode].mnemo == i_ILLG || table68k[opcode].clev > currprefs.cpu_level)
	    continue;

	if (table68k[opcode].handler != -1) {
	    f = cpufunctbl[cft_map (table68k[opcode].handler)];
	    if (f == op_illg_1)
		    abort();
	    cpufunctbl[cft_map (opcode)] = f;
	}
    }
    for (i = 0; tbl[i].handler != NULL; i++) {
	if (tbl[i].specific)
	    cpufunctbl[cft_map (tbl[i].opcode)] = tbl[i].handler;
    }
}

unsigned long cycles_mask, cycles_val;

static void update_68k_cycles (void)
{
    cycles_mask = 0;
    cycles_val = currprefs.m68k_speed;
    if (currprefs.m68k_speed < 1) {
	cycles_mask = 0xFFFFFFFF;
	cycles_val = 0;
    }
}

void check_prefs_changed_cpu (void)
{
}

void init_m68k (void)
{
    int i;

    update_68k_cycles ();

    for (i = 0 ; i < 256 ; i++) {
	int j;
	for (j = 0 ; j < 8 ; j++) {
		if (i & (1 << j)) break;
	}
	movem_index1[i] = j;
	movem_index2[i] = 7-j;
	movem_next[i] = i & (~(1 << j));
    }
    for (i = 0 ; i < 256 ; i++) {
	int j;
	for (j = 7 ; j >= 0 ; j--) {
		if (i & (1 << j)) break;
	}
	fpp_movem_index1[i] = j;
	fpp_movem_index2[i] = 7-j;
	fpp_movem_next[i] = i & (~(1 << j));
    }
#if COUNT_INSTRS
    {
	FILE *f = fopen (icountfilename (), "r");
	memset (instrcount, 0, sizeof instrcount);
	if (f) {
	    uae_u32 opcode, count, total;
	    char name[20];
	    write_log ("Reading instruction count file...\n");
	    fscanf (f, "Total: %lu\n", &total);
	    while (fscanf (f, "%lx: %lu %s\n", &opcode, &count, name) == 3) {
		instrcount[opcode] = count;
	    }
	    fclose(f);
	}
    }
#endif
    if (withTitle) {
      write_log ("# Building CPU table for configuration: 68");
      
      if (currprefs.address_space_24 && currprefs.cpu_level > 1)
        write_log ("EC");
    
      switch( currprefs.cpu_level )
      {
      case 1:
          write_log ("010");
          break;
      case 2:
          write_log ("020");
          break;
      case 3:
          write_log ("020/881");
          break;
      default:
          write_log ("000");
          break;
      }
      if (currprefs.cpu_compatible)
          write_log (" (compatible mode)");
      write_log ("\n");
    } 
    
    read_table68k ();
    do_merges ();

    if (withTitle) write_log ("# %8d CPU functions\n", nr_cpuop_funcs);

    build_cpufunctbl ();
}

regs_type regs, lastint_regs;
static regs_type regs_backup[16];
static int backup_pointer = 0;
uaecptr m68kpc_offset;
int lastint_no;

uae_u8 get_ibyte_1(uaecptr o) {
    return get_byte(regs.pc + (regs.pc_p - regs.pc_oldp) + (o) + 1);
}

uae_u16 get_iword_1(uaecptr o) {
    return get_word(regs.pc + (regs.pc_p - regs.pc_oldp) + (o));
}

uae_u32 get_ilong_1(uaecptr o) {
    get_long(regs.pc + (regs.pc_p - regs.pc_oldp) + (o));
}

uae_s32 ShowEA (int reg, amodes mode, wordsizes size, char *buf)
{
    uae_u16 dp;
    uae_s8 disp8;
    uae_s16 disp16;
    int r;
    uae_u32 dispreg;
    uaecptr addr;
    uae_s32 offset = 0;
    char buffer[80];

    switch (mode){
     case Dreg:
	sprintf (buffer,"D%d", reg);
	break;
     case Areg:
	sprintf (buffer,"A%d", reg);
	break;
     case Aind:
	sprintf (buffer,"(A%d)", reg);
	break;
     case Aipi:
	sprintf (buffer,"(A%d)+", reg);
	break;
     case Apdi:
	sprintf (buffer,"-(A%d)", reg);
	break;
     case Ad16:
	disp16 = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	addr = m68k_areg(regs,reg) + (uae_s16)disp16;
	sprintf (buffer,"(A%d,$%04x) == $%08lx", reg, disp16 & 0xffff,
					(long unsigned int)addr);
	break;
     case Ad8r:
	dp = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	disp8 = dp & 0xFF;
	r = (dp & 0x7000) >> 12;
	dispreg = dp & 0x8000 ? m68k_areg(regs,r) : m68k_dreg(regs,r);
	if (!(dp & 0x800)) dispreg = (uae_s32)(uae_s16)(dispreg);
	dispreg <<= (dp >> 9) & 3;

	if (dp & 0x100) {
	    uae_s32 outer = 0, disp = 0;
	    uae_s32 base = m68k_areg(regs,reg);
	    char name[10];
	    sprintf (name,"A%d, ",reg);
	    if (dp & 0x80) { base = 0; name[0] = 0; }
	    if (dp & 0x40) dispreg = 0;
	    if ((dp & 0x30) == 0x20) { disp = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
	    if ((dp & 0x30) == 0x30) { disp = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }
	    base += disp;

	    if ((dp & 0x3) == 0x2) { outer = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
	    if ((dp & 0x3) == 0x3) { outer = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }

	    if (!(dp & 4)) base += dispreg;
	    if (dp & 3) base = get_long (base);
	    if (dp & 4) base += dispreg;

	    addr = base + outer;
	    sprintf (buffer,"(%s%c%d.%c*%d+%d)+%d == $%08lx", name,
		    dp & 0x8000 ? 'A' : 'D', (int)r, dp & 0x800 ? 'L' : 'W',
		    1 << ((dp >> 9) & 3),
		    disp,outer,
		    (long unsigned int)addr);
	} else {
	  addr = m68k_areg(regs,reg) + (uae_s32)((uae_s8)disp8) + dispreg;
	  sprintf (buffer,"(A%d, %c%d.%c*%d, $%02x) == $%08lx", reg,
	       dp & 0x8000 ? 'A' : 'D', (int)r, dp & 0x800 ? 'L' : 'W',
	       1 << ((dp >> 9) & 3), disp8,
	       (long unsigned int)addr);
	}
	break;
     case PC16:
	addr = m68k_getpc () + m68kpc_offset;
	disp16 = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	addr += (uae_s16)disp16;
	sprintf (buffer,"(PC,$%04x) == $%08lx", disp16 & 0xffff,(long unsigned int)addr);
	break;
     case PC8r:
	addr = m68k_getpc () + m68kpc_offset;
	dp = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	disp8 = dp & 0xFF;
	r = (dp & 0x7000) >> 12;
	dispreg = dp & 0x8000 ? m68k_areg(regs,r) : m68k_dreg(regs,r);
	if (!(dp & 0x800)) dispreg = (uae_s32)(uae_s16)(dispreg);
	dispreg <<= (dp >> 9) & 3;

	if (dp & 0x100) {
	    uae_s32 outer = 0,disp = 0;
	    uae_s32 base = addr;
	    char name[10];
	    sprintf (name,"PC, ");
	    if (dp & 0x80) { base = 0; name[0] = 0; }
	    if (dp & 0x40) dispreg = 0;
	    if ((dp & 0x30) == 0x20) { disp = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
	    if ((dp & 0x30) == 0x30) { disp = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }
	    base += disp;

	    if ((dp & 0x3) == 0x2) { outer = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
	    if ((dp & 0x3) == 0x3) { outer = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }

	    if (!(dp & 4)) base += dispreg;
	    if (dp & 3) base = get_long (base);
	    if (dp & 4) base += dispreg;

	    addr = base + outer;
	    sprintf (buffer,"(%s%c%d.%c*%d+%d)+%d == $%08lx", name,
		    dp & 0x8000 ? 'A' : 'D', (int)r, dp & 0x800 ? 'L' : 'W',
		    1 << ((dp >> 9) & 3),
		    disp,outer,
		    (long unsigned int)addr);
	} else {
	  addr += (uae_s32)((uae_s8)disp8) + dispreg;
	  sprintf (buffer,"(PC, %c%d.%c*%d, $%02x) == $%08lx", dp & 0x8000 ? 'A' : 'D',
		(int)r, dp & 0x800 ? 'L' : 'W',  1 << ((dp >> 9) & 3),
		disp8, (long unsigned int)addr);
	}
	break;
     case absw:
	sprintf (buffer,"$%08lx", (long unsigned int)(uae_s32)(uae_s16)get_iword_1 (m68kpc_offset));
	m68kpc_offset += 2;
	break;
     case absl:
	sprintf (buffer,"$%08lx", (long unsigned int)get_ilong_1 (m68kpc_offset));
	m68kpc_offset += 4;
	break;
     case imm:
	switch (size){
	 case sz_byte:
	    sprintf (buffer,"#$%02x", (unsigned int)(get_iword_1 (m68kpc_offset) & 0xff));
	    m68kpc_offset += 2;
	    break;
	 case sz_word:
	    sprintf (buffer,"#$%04x", (unsigned int)(get_iword_1 (m68kpc_offset) & 0xffff));
	    m68kpc_offset += 2;
	    break;
	 case sz_long:
	    sprintf (buffer,"#$%08lx", (long unsigned int)(get_ilong_1 (m68kpc_offset)));
	    m68kpc_offset += 4;
	    break;
	 default:
	    break;
	}
	break;
     case imm0:
	offset = (uae_s32)(uae_s8)get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	sprintf (buffer,"#$%02x", (unsigned int)(offset & 0xff));
	break;
     case imm1:
	offset = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	sprintf (buffer,"#$%04x", (unsigned int)(offset & 0xffff));
	break;
     case imm2:
	offset = (uae_s32)get_ilong_1 (m68kpc_offset);
	m68kpc_offset += 4;
	sprintf (buffer,"#$%08lx", (long unsigned int)offset);
	break;
     case immi:
	offset = (uae_s32)(uae_s8)(reg & 0xff);
	sprintf (buffer,"#$%08lx", (long unsigned int)offset);
	break;
     default:
	break;
    }
    if (buf == 0)
	printf("%s", buffer);
    else
	strcat (buf, buffer);
    return offset;
}

/* The plan is that this will take over the job of exception 3 handling -
 * the CPU emulation functions will just do a longjmp to m68k_go whenever
 * they hit an odd address. */
static int verify_ea (int reg, amodes mode, wordsizes size, uae_u32 *val)
{
    uae_u16 dp;
    uae_s8 disp8;
    uae_s16 disp16;
    int r;
    uae_u32 dispreg;
    uaecptr addr;
    uae_s32 offset = 0;

    switch (mode){
     case Dreg:
	*val = m68k_dreg (regs, reg);
	return 1;
     case Areg:
	*val = m68k_areg (regs, reg);
	return 1;

     case Aind:
     case Aipi:
	addr = m68k_areg (regs, reg);
	break;
     case Apdi:
	addr = m68k_areg (regs, reg);
	break;
     case Ad16:
	disp16 = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	addr = m68k_areg(regs,reg) + (uae_s16)disp16;
	break;
     case Ad8r:
	addr = m68k_areg (regs, reg);
     d8r_common:
	dp = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	disp8 = dp & 0xFF;
	r = (dp & 0x7000) >> 12;
	dispreg = dp & 0x8000 ? m68k_areg(regs,r) : m68k_dreg(regs,r);
	if (!(dp & 0x800)) dispreg = (uae_s32)(uae_s16)(dispreg);
	dispreg <<= (dp >> 9) & 3;

	if (dp & 0x100) {
	    uae_s32 outer = 0, disp = 0;
	    uae_s32 base = addr;
	    if (dp & 0x80) base = 0;
	    if (dp & 0x40) dispreg = 0;
	    if ((dp & 0x30) == 0x20) { disp = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
	    if ((dp & 0x30) == 0x30) { disp = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }
	    base += disp;

	    if ((dp & 0x3) == 0x2) { outer = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
	    if ((dp & 0x3) == 0x3) { outer = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }

	    if (!(dp & 4)) base += dispreg;
	    if (dp & 3) base = get_long (base);
	    if (dp & 4) base += dispreg;

	    addr = base + outer;
	} else {
	  addr += (uae_s32)((uae_s8)disp8) + dispreg;
	}
	break;
     case PC16:
	addr = m68k_getpc () + m68kpc_offset;
	disp16 = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	addr += (uae_s16)disp16;
	break;
     case PC8r:
	addr = m68k_getpc () + m68kpc_offset;
	goto d8r_common;
     case absw:
	addr = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	break;
     case absl:
	addr = get_ilong_1 (m68kpc_offset);
	m68kpc_offset += 4;
	break;
     case imm:
	switch (size){
	 case sz_byte:
	    *val = get_iword_1 (m68kpc_offset) & 0xff;
	    m68kpc_offset += 2;
	    break;
	 case sz_word:
	    *val = get_iword_1 (m68kpc_offset) & 0xffff;
	    m68kpc_offset += 2;
	    break;
	 case sz_long:
	    *val = get_ilong_1 (m68kpc_offset);
	    m68kpc_offset += 4;
	    break;
	 default:
	    break;
	}
	return 1;
     case imm0:
	*val = (uae_s32)(uae_s8)get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	return 1;
     case imm1:
	*val = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	return 1;
     case imm2:
	*val = get_ilong_1 (m68kpc_offset);
	m68kpc_offset += 4;
	return 1;
     case immi:
	*val = (uae_s32)(uae_s8)(reg & 0xff);
	return 1;
     default:
	addr = 0;
	break;
    }
    if ((addr & 1) == 0)
	return 1;

    last_addr_for_exception_3 = m68k_getpc () + m68kpc_offset;
    last_fault_for_exception_3 = addr;
    return 0;
}

uae_u32 get_disp_ea_020 (uae_u32 base, uae_u32 dp)
{
    int reg = (dp >> 12) & 15;
    uae_s32 regd = regs.regs[reg];
    if ((dp & 0x800) == 0)
	regd = (uae_s32)(uae_s16)regd;
    regd <<= (dp >> 9) & 3;
    if (dp & 0x100) {
	uae_s32 outer = 0;
	if (dp & 0x80) base = 0;
	if (dp & 0x40) regd = 0;

	if ((dp & 0x30) == 0x20) base += (uae_s32)(uae_s16)next_iword();
	if ((dp & 0x30) == 0x30) base += next_ilong();

	if ((dp & 0x3) == 0x2) outer = (uae_s32)(uae_s16)next_iword();
	if ((dp & 0x3) == 0x3) outer = next_ilong();

	if ((dp & 0x4) == 0) base += regd;
	if (dp & 0x3) base = get_long (base);
	if (dp & 0x4) base += regd;

	return base + outer;
    } else {
	return base + (uae_s32)((uae_s8)dp) + regd;
    }
}

uae_u32 get_disp_ea_000 (uae_u32 base, uae_u32 dp)
{
    int reg = (dp >> 12) & 15;
    uae_s32 regd = regs.regs[reg];
#if 1
    if ((dp & 0x800) == 0)
	regd = (uae_s32)(uae_s16)regd;
    return base + (uae_s8)dp + regd;
#else
    /* Branch-free code... benchmark this again now that
     * things are no longer inline.  */
    uae_s32 regd16;
    uae_u32 mask;
    mask = ((dp & 0x800) >> 11) - 1;
    regd16 = (uae_s32)(uae_s16)regd;
    regd16 &= mask;
    mask = ~mask;
    base += (uae_s8)dp;
    regd &= mask;
    regd |= regd16;
    return base + regd;
#endif
}

void MakeSR (void)
{
#if 0
    assert((regs.t1 & 1) == regs.t1);
    assert((regs.t0 & 1) == regs.t0);
    assert((regs.s & 1) == regs.s);
    assert((regs.m & 1) == regs.m);
    assert((XFLG & 1) == XFLG);
    assert((NFLG & 1) == NFLG);
    assert((ZFLG & 1) == ZFLG);
    assert((VFLG & 1) == VFLG);
    assert((CFLG & 1) == CFLG);
#endif
    regs.sr = ((regs.t1 << 15) | (regs.t0 << 14)
	       | (regs.s << 13) | (regs.m << 12) | (regs.intmask << 8)
	       | (GET_XFLG << 4) | (GET_NFLG << 3) | (GET_ZFLG << 2) | (GET_VFLG << 1)
	       | GET_CFLG);
}

void MakeFromSR (void)
{
    int oldm = regs.m;
    int olds = regs.s;

    regs.t1 = (regs.sr >> 15) & 1;
    regs.t0 = (regs.sr >> 14) & 1;
    regs.s = (regs.sr >> 13) & 1;
    regs.m = (regs.sr >> 12) & 1;
    regs.intmask = (regs.sr >> 8) & 7;
    SET_XFLG ((regs.sr >> 4) & 1);
    SET_NFLG ((regs.sr >> 3) & 1);
    SET_ZFLG ((regs.sr >> 2) & 1);
    SET_VFLG ((regs.sr >> 1) & 1);
    SET_CFLG (regs.sr & 1);
    if (currprefs.cpu_level >= 2) {
		if (olds != regs.s) {
		    if (olds) {
				if (oldm)
				    regs.msp = m68k_areg(regs, 7);
				else
				    regs.isp = m68k_areg(regs, 7);
				m68k_areg(regs, 7) = regs.usp;
		    } else {
				regs.usp = m68k_areg(regs, 7);
				m68k_areg(regs, 7) = regs.m ? regs.msp : regs.isp;
		    }
		} else if (olds && oldm != regs.m) {
		    if (oldm) {
				regs.msp = m68k_areg(regs, 7);
				m68k_areg(regs, 7) = regs.isp;
		    } else {
				regs.isp = m68k_areg(regs, 7);
				m68k_areg(regs, 7) = regs.msp;
		    }
		}
    } else {
		if (olds != regs.s) {
		    if (olds) {
				regs.isp = m68k_areg(regs, 7);
				m68k_areg(regs, 7) = regs.usp;
		    } else {
				regs.usp = m68k_areg(regs, 7);
				m68k_areg(regs, 7) = regs.isp;
		    }
		}
    }

}

void handle_os9exec_exception(int nr, uaecptr oldpc); // WIL

void Exception(int nr, uaecptr oldpc)
{
    compiler_flush_jsr_stack();
    MakeSR();
    handle_os9exec_exception(nr,m68k_getpc());
}

static void Interrupt(int nr)
{
    assert(nr < 8 && nr >= 0);
    lastint_regs = regs;
    lastint_no = nr;
    Exception(nr+24, 0);

    regs.intmask = nr;
}

static int caar, cacr;

void m68k_move2c (int regno, uae_u32 *regp)
{
    if (currprefs.cpu_level == 1 && (regno & 0x7FF) > 1)
	op_illg (0x4E7B);
    else
	switch (regno) {
	 case 0: regs.sfc = *regp & 7; break;
	 case 1: regs.dfc = *regp & 7; break;
	 case 2: cacr = *regp & 0x3; break;	/* ignore C and CE */
	 case 0x800: regs.usp = *regp; break;
	 case 0x801: regs.vbr = *regp; break;
	 case 0x802: caar = *regp &0xfc; break;
	 case 0x803: regs.msp = *regp; if (regs.m == 1) m68k_areg(regs, 7) = regs.msp; break;
	 case 0x804: regs.isp = *regp; if (regs.m == 0) m68k_areg(regs, 7) = regs.isp; break;
	 default:
	    op_illg (0x4E7B);
	    break;
	}
}

void m68k_movec2 (int regno, uae_u32 *regp)
{
    if (currprefs.cpu_level == 1 && (regno & 0x7FF) > 1)
	op_illg (0x4E7A);
    else
	switch (regno) {
	 case 0: *regp = regs.sfc; break;
	 case 1: *regp = regs.dfc; break;
	 case 2: *regp = cacr; break;
	 case 0x800: *regp = regs.usp; break;
	 case 0x801: *regp = regs.vbr; break;
	 case 0x802: *regp = caar; break;
	 case 0x803: *regp = regs.m == 1 ? m68k_areg(regs, 7) : regs.msp; break;
	 case 0x804: *regp = regs.m == 0 ? m68k_areg(regs, 7) : regs.isp; break;
	 default:
	    op_illg (0x4E7A);
	    break;
	}
}

static __inline__ int
div_unsigned(uae_u32 src_hi, uae_u32 src_lo, uae_u32 div, uae_u32 *quot, uae_u32 *rem)
{
	uae_u32 q = 0, cbit = 0;
	int i;

	if (div <= src_hi) {
	    return 1;
	}
	for (i = 0 ; i < 32 ; i++) {
		cbit = src_hi & 0x80000000ul;
		src_hi <<= 1;
		if (src_lo & 0x80000000ul) src_hi++;
		src_lo <<= 1;
		q = q << 1;
		if (cbit || div <= src_hi) {
			q |= 1;
			src_hi -= div;
		}
	}
	*quot = q;
	*rem = src_hi;
	return 0;
}

void m68k_divl (uae_u32 opcode, uae_u32 src, uae_u16 extra, uaecptr oldpc)
{
#if defined(uae_s64)
    if (src == 0) {
	Exception (5, oldpc);
	return;
    }
    if (extra & 0x800) {
	/* signed variant */
	uae_s64 a = (uae_s64)(uae_s32)m68k_dreg(regs, (extra >> 12) & 7);
	uae_s64 quot, rem;

	if (extra & 0x400) {
	    a &= 0xffffffffu;
	    a |= (uae_s64)m68k_dreg(regs, extra & 7) << 32;
	}
	rem = a % (uae_s64)(uae_s32)src;
	quot = a / (uae_s64)(uae_s32)src;
	if ((quot & UVAL64(0xffffffff80000000)) != 0
	    && (quot & UVAL64(0xffffffff80000000)) != UVAL64(0xffffffff80000000))
	{
	    SET_VFLG (1);
	    SET_NFLG (1);
	    SET_CFLG (0);
	} else {
	    if (((uae_s32)rem < 0) != ((uae_s64)a < 0)) rem = -rem;
	    SET_VFLG (0);
	    SET_CFLG (0);
	    SET_ZFLG (((uae_s32)quot) == 0);
	    SET_NFLG (((uae_s32)quot) < 0);
	    m68k_dreg(regs, extra & 7) = rem;
	    m68k_dreg(regs, (extra >> 12) & 7) = quot;
	}
    } else {
	/* unsigned */
	uae_u64 a = (uae_u64)(uae_u32)m68k_dreg(regs, (extra >> 12) & 7);
	uae_u64 quot, rem;

	if (extra & 0x400) {
	    a &= 0xffffffffu;
	    a |= (uae_u64)m68k_dreg(regs, extra & 7) << 32;
	}
	rem = a % (uae_u64)src;
	quot = a / (uae_u64)src;
	if (quot > 0xffffffffu) {
	    SET_VFLG (1);
	    SET_NFLG (1);
	    SET_CFLG (0);
	} else {
	    SET_VFLG (0);
	    SET_CFLG (0);
        SET_ZFLG (((uae_s32)quot) == 0);
	    SET_NFLG (((uae_s32)quot) < 0);
	    m68k_dreg(regs, extra & 7) = rem;
	    m68k_dreg(regs, (extra >> 12) & 7) = quot;
	}
    }
#else
    if (src == 0) {
	Exception (5, oldpc);
	return;
    }
    if (extra & 0x800) {
	/* signed variant */
	uae_s32 lo = (uae_s32)m68k_dreg(regs, (extra >> 12) & 7);
	uae_s32 hi = lo < 0 ? -1 : 0;
	uae_s32 save_high;
	uae_u32 quot, rem;
	uae_u32 sign;

	if (extra & 0x400) {
	    hi = (uae_s32)m68k_dreg(regs, extra & 7);
	}
	save_high = hi;
	sign = (hi ^ src);
	if (hi < 0) {
	    hi = ~hi;
	    lo = -lo;
	    if (lo == 0) hi++;
	}
	if ((uae_s32)src < 0) src = -src;
	if (div_unsigned(hi, lo, src, &quot, &rem) ||
	    (sign & 0x80000000) ? quot > 0x80000000 : quot > 0x7fffffff) {
	    SET_VFLG (1);
	    SET_NFLG (1);
	    SET_CFLG (0);
	} else {
	    if (sign & 0x80000000) quot = -quot;
	    if (((uae_s32)rem < 0) != (save_high < 0)) rem = -rem;
	    SET_VFLG (0);
	    SET_CFLG (0);
	    SET_ZFLG (((uae_s32)quot) == 0);
	    SET_NFLG (((uae_s32)quot) < 0);
	    m68k_dreg(regs, extra & 7) = rem;
	    m68k_dreg(regs, (extra >> 12) & 7) = quot;
	}
    } else {
	/* unsigned */
	uae_u32 lo = (uae_u32)m68k_dreg(regs, (extra >> 12) & 7);
	uae_u32 hi = 0;
	uae_u32 quot, rem;

	if (extra & 0x400) {
	    hi = (uae_u32)m68k_dreg(regs, extra & 7);
	}
	if (div_unsigned(hi, lo, src, &quot, &rem)) {
	    SET_VFLG (1);
	    SET_NFLG (1);
	    SET_CFLG (0);
	} else {
        SET_VFLG (0);
	    SET_CFLG (0);
	    SET_ZFLG (((uae_s32)quot) == 0);
	    SET_NFLG (((uae_s32)quot) < 0);
	    m68k_dreg(regs, extra & 7) = rem;
	    m68k_dreg(regs, (extra >> 12) & 7) = quot;
	}
    }
#endif
}

static __inline__ void
mul_unsigned(uae_u32 src1, uae_u32 src2, uae_u32 *dst_hi, uae_u32 *dst_lo)
{
	uae_u32 r0 = (src1 & 0xffff) * (src2 & 0xffff);
	uae_u32 r1 = ((src1 >> 16) & 0xffff) * (src2 & 0xffff);
	uae_u32 r2 = (src1 & 0xffff) * ((src2 >> 16) & 0xffff);
	uae_u32 r3 = ((src1 >> 16) & 0xffff) * ((src2 >> 16) & 0xffff);
	uae_u32 lo;

	lo = r0 + ((r1 << 16) & 0xffff0000ul);
	if (lo < r0) r3++;
	r0 = lo;
	lo = r0 + ((r2 << 16) & 0xffff0000ul);
	if (lo < r0) r3++;
	r3 += ((r1 >> 16) & 0xffff) + ((r2 >> 16) & 0xffff);
	*dst_lo = lo;
	*dst_hi = r3;
}

void m68k_mull (uae_u32 opcode, uae_u32 src, uae_u16 extra)
{
#if defined(uae_s64)
    if (extra & 0x800) {
	/* signed variant */
	uae_s64 a = (uae_s64)(uae_s32)m68k_dreg(regs, (extra >> 12) & 7);

	a *= (uae_s64)(uae_s32)src;
	SET_VFLG (0);
	SET_CFLG (0);
	SET_ZFLG (a == 0);
	SET_NFLG (a < 0);
	if (extra & 0x400)
	    m68k_dreg(regs, extra & 7) = a >> 32;
	else if ((a & UVAL64(0xffffffff80000000)) != 0
		 && (a & UVAL64(0xffffffff80000000)) != UVAL64(0xffffffff80000000))
	{
	    SET_VFLG (1);
	}
	m68k_dreg(regs, (extra >> 12) & 7) = (uae_u32)a;
    } else {
	/* unsigned */
	uae_u64 a = (uae_u64)(uae_u32)m68k_dreg(regs, (extra >> 12) & 7);

	a *= (uae_u64)src;
	SET_VFLG (0);
	SET_CFLG (0);
	SET_ZFLG (a == 0);
	SET_NFLG (((uae_s64)a) < 0);
	if (extra & 0x400)
	    m68k_dreg(regs, extra & 7) = a >> 32;
	else if ((a & UVAL64(0xffffffff00000000)) != 0) {
	    SET_VFLG (1);
	}
	m68k_dreg(regs, (extra >> 12) & 7) = (uae_u32)a;
    }
#else
    if (extra & 0x800) {
	/* signed variant */
	uae_s32 src1,src2;
	uae_u32 dst_lo,dst_hi;
	uae_u32 sign;

	src1 = (uae_s32)src;
	src2 = (uae_s32)m68k_dreg(regs, (extra >> 12) & 7);
	sign = (src1 ^ src2);
	if (src1 < 0) src1 = -src1;
	if (src2 < 0) src2 = -src2;
	mul_unsigned((uae_u32)src1,(uae_u32)src2,&dst_hi,&dst_lo);
	if (sign & 0x80000000) {
		dst_hi = ~dst_hi;
		dst_lo = -dst_lo;
		if (dst_lo == 0) dst_hi++;
	}
	SET_VFLG (0);
	SET_CFLG (0);
	SET_ZFLG (dst_hi == 0 && dst_lo == 0);
	SET_NFLG (((uae_s32)dst_hi) < 0);
	if (extra & 0x400)
	    m68k_dreg(regs, extra & 7) = dst_hi;
	else if ((dst_hi != 0 || (dst_lo & 0x80000000) != 0)
		 && ((dst_hi & 0xffffffff) != 0xffffffff
		     || (dst_lo & 0x80000000) != 0x80000000))
	{
	    SET_VFLG (1);
	}
	m68k_dreg(regs, (extra >> 12) & 7) = dst_lo;
    } else {
	/* unsigned */
	uae_u32 dst_lo,dst_hi;

	mul_unsigned(src,(uae_u32)m68k_dreg(regs, (extra >> 12) & 7),&dst_hi,&dst_lo);

	SET_VFLG (0);
	SET_CFLG (0);
	SET_ZFLG (dst_hi == 0 && dst_lo == 0);
	SET_NFLG (((uae_s32)dst_hi) < 0);
	if (extra & 0x400)
	    m68k_dreg(regs, extra & 7) = dst_hi;
	else if (dst_hi != 0) {
	    SET_VFLG (1);
	}
	m68k_dreg(regs, (extra >> 12) & 7) = dst_lo;
    }
#endif
}
static char* ccnames[] =
{ "T ","F ","HI","LS","CC","CS","NE","EQ",
  "VC","VS","PL","MI","GE","LT","GT","LE" };

void m68k_reset (void)
{
    if(savestate_wanted) {
        m68k_setpc (regs.pc);
	/* MakeFromSR() must not swap stack pointer */
	regs.s = (regs.sr >> 13) & 1;
	MakeFromSR();
	/* set stack pointer */
	if (regs.s) m68k_areg(regs, 7) = regs.isp; else m68k_areg(regs, 7) = regs.usp;
	return;
    }

    m68k_areg (regs, 7) = get_long (0x00f80000);
    m68k_setpc (get_long (0x00f80004));
    fill_prefetch_0 ();
    regs.stopped = 0;
    regs.s = 1;
    regs.m = 0;
    regs.t1 = 0;
    regs.t0 = 0;
    SET_ZFLG (0);
    SET_XFLG (0);
    SET_CFLG (0);
    SET_VFLG (0);
    SET_NFLG (0);
    regs.intmask = 7;
    regs.vbr = regs.sfc = regs.dfc = 0;
    regs.fpcr = regs.fpsr = regs.fpiar = 0;
}

unsigned long REGPARAM2 op_illg (uae_u32 opcode)
{
    uaecptr pc = m68k_getpc ();
    
	
    compiler_flush_jsr_stack ();

    if ((opcode & 0xF000) == 0xF000) {
		Exception(0xB,0);
		return 4;
    }
    if ((opcode & 0xF000) == 0xA000) {
		Exception(0xA,0);
		return 4;
    }
    write_log ("Illegal instruction: %04x at %0" PRIx32 "\n", opcode, pc);
    Exception (4,0);
    return 4;
}

void mmu_op(uae_u32 opcode, uae_u16 extra)
{
    if ((extra & 0xB000) == 0) { /* PMOVE instruction */

    } else if ((extra & 0xF000) == 0x2000) { /* PLOAD instruction */
    } else if ((extra & 0xF000) == 0x8000) { /* PTEST instruction */
    } else
	op_illg (opcode);
}

static int n_insns = 0, n_spcinsns = 0;

static uaecptr last_trace_ad = 0;

static __inline__ void do_trace (void)
{
}

static int do_specialties (void)
{
    /*n_spcinsns++;*/
    do_trace ();

    return 0;
}

static void m68k_run_1 (void)
{
    for (;;) {
        unsigned long cycles;
		uae_u32 opcode = GET_OPCODE;
		#if 0
		if (get_ilong (0) != do_get_mem_long (&regs.prefetch)) {
		    debugging = 1;
		    return;
		}
		#endif
		/* assert (!regs.stopped && !(regs.spcflags & SPCFLAG_STOP)); */
		/*	regs_backup[backup_pointer = (backup_pointer + 1) % 16] = regs;*/
		#if COUNT_INSTRS == 2
		if (table68k[cft_map (opcode)].handler != -1)
		    instrcount[table68k[cft_map (opcode)].handler]++;
		#elif COUNT_INSTRS == 1
		instrcount[opcode]++;
		#endif
		cycles = (*cpufunctbl[opcode])(opcode);
		/*n_insns++;*/
	    if (quit_program) break;
    }
}

#define m68k_run1 m68k_run_1


//int in_m68k_go = 0;


int m68k_os9trace;
int m68k_disp= 0;

#undef PROBLEM

ushort debugwait( void ); // WIL

enum {
    M68K_CPU_TYPE_INVALID,
    M68K_CPU_TYPE_68000,
    M68K_CPU_TYPE_68010,
    M68K_CPU_TYPE_68EC020,
    M68K_CPU_TYPE_68020,
    M68K_CPU_TYPE_68EC030,
    M68K_CPU_TYPE_68030,
    M68K_CPU_TYPE_68EC040,
    M68K_CPU_TYPE_68LC040,
    M68K_CPU_TYPE_68040,
    M68K_CPU_TYPE_SCC68070
};

unsigned int m68k_disassemble(char* str_buff, unsigned int pc, unsigned int cpu_type);

static void musashi_disassemble() {
    char disass[0x100];
    m68k_disassemble(disass, regs.pc_p, M68K_CPU_TYPE_68020);

    printf("%08x    %-40s", regs.pc_p, disass);

    printf("D: ");
    for (int i = 0; i < 8; i++)
        printf("%08x ", regs.regs[REGS_D + i]);

    printf("A: ");
    for (int i = 0; i < 8; i++)
        printf("%08x ", regs.regs[REGS_A + i]);

    printf("\n");
}

// special emulator call, runs up to next os9_running=0 assignment
unsigned long m68k_os9go(void)
{
	static uae_u32 lasta2;
	
    if (in_m68k_go) {
		write_log ("Bug! m68k_go is not reentrant.\n");
		abort ();
    }
    // prevent recursion
    in_m68k_go++;
    // stay in 68k emu until TRAP or exception occurs
    m68_os9go_result=0;
    os9_running=1;
    while (os9_running) {
		uae_u32 opcode = GET_OPCODE;
		(*cpufunctbl[opcode])(opcode);
		if (m68k_disp)
			upe_printf( "%8x %4x\n", regs.pc_p,opcode );
		if (m68k_os9trace) {
			uaecptr n;
		   m68k_dumpstate(&n,0);
			if (debugwait()) {
				printf("ExtraHalt:\n");
			}
		}
    }
    in_m68k_go--;
    // make sure PC is updated
    m68k_setpc(m68k_getpc());
    return m68_os9go_result;
}


void m68k_go (int may_quit)
{
    if (in_m68k_go || !may_quit) {
		write_log ("Bug! m68k_go is not reentrant.\n");
		abort ();
    }


    in_m68k_go++;
    for (;;) {
		if (quit_program > 0) {
		    if (quit_program == 1)
				break;
		    quit_program = 0;
		    m68k_reset ();
		}
		m68k_run1 ();
    }
    in_m68k_go--;
}

static void m68k_verify (uaecptr addr, uaecptr *nextpc)
{
    uae_u32 opcode, val;
    struct instr *dp;

    opcode = get_iword_1(0);
    last_op_for_exception_3 = opcode;
    m68kpc_offset = 2;

    if (cpufunctbl[cft_map (opcode)] == op_illg_1) {
	opcode = 0x4AFC;
    }
    dp = table68k + opcode;

    if (dp->suse) {
	if (!verify_ea (dp->sreg, dp->smode, dp->size, &val)) {
	    Exception (3, 0);
	    return;
	}
    }
    if (dp->duse) {
	if (!verify_ea (dp->dreg, dp->dmode, dp->size, &val)) {
	    Exception (3, 0);
	    return;
	}
    }
}

void m68k_disasm (uaecptr addr, uaecptr *nextpc, int cnt)
{
    uaecptr newpc = 0;
    m68kpc_offset = addr - m68k_getpc ();
    while (cnt-- > 0) {
	char instrname[20],*ccpt;
	int opwords;
	uae_u32 opcode;
	struct mnemolookup *lookup;
	struct instr *dp;

	printf("%0" PRIx32 ": ", m68k_getpc () + m68kpc_offset);
	for (opwords = 0; opwords < 5; opwords++){
	    printf("%04x ", get_iword_1 (m68kpc_offset + opwords*2));
	}
	opcode = get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	if (cpufunctbl[cft_map (opcode)] == op_illg_1) {
	    opcode = 0x4AFC;
	}
	dp = table68k + opcode;
	for (lookup = lookuptab;lookup->mnemo != dp->mnemo; lookup++)
	    ;

	strcpy (instrname, lookup->name);
	ccpt = strstr (instrname, "cc");
	if (ccpt != 0) {
	    strncpy (ccpt, ccnames[dp->cc], 2);
	}
	printf("%s", instrname);
	switch (dp->size){
	 case sz_byte: printf(".B "); break;
	 case sz_word: printf(".W "); break;
	 case sz_long: printf(".L "); break;
	 default: printf("   "); break;
	}

	if (dp->suse) {
	    newpc = m68k_getpc () + m68kpc_offset;
	    newpc += ShowEA (dp->sreg, dp->smode, dp->size, 0);
	}
	if (dp->suse && dp->duse)
	    printf(",");
	if (dp->duse) {
	    newpc = m68k_getpc () + m68kpc_offset;
	    newpc += ShowEA (dp->dreg, dp->dmode, dp->size, 0);
	}
	if (ccpt != 0) {
	    if (cctrue(dp->cc))
		printf(" == %0" PRIx32 " (TRUE)", newpc);
	    else
		printf(" == %0" PRIx32 " (FALSE)", newpc);
	} else if ((opcode & 0xff00) == 0x6100) /* BSR */
	    printf(" == %0" PRIx32 , newpc);
	printf("\n");
    }
    if (nextpc)
	*nextpc = m68k_getpc () + m68kpc_offset;
}

void m68k_dumpstate (uaecptr *nextpc, int to_logfile)
{
    int i;
    
    for (i = 0; i < 8; i++){
	printf("D%d: %0" PRIx32 " ", i, m68k_dreg(regs, i));
	if ((i & 3) == 3) printf("\n");
    }
    for (i = 0; i < 8; i++){
	printf("A%d: %0" PRIx32 " ", i, m68k_areg(regs, i));
	if ((i & 3) == 3) printf("\n");
    }
    if (regs.s == 0) regs.usp = m68k_areg(regs, 7);
    if (regs.s && regs.m) regs.msp = m68k_areg(regs, 7);
    if (regs.s && regs.m == 0) regs.isp = m68k_areg(regs, 7);
    printf("USP=%0" PRIx32 " ISP=%0" PRIx32 " MSP=%0" PRIx32 " VBR=%0" PRIx32 "\n",
	    regs.usp,regs.isp,regs.msp,regs.vbr);
    printf("T=%d%d S=%d M=%d X=%d N=%d Z=%d V=%d C=%d IMASK=%d\n",
	    regs.t1, regs.t0, regs.s, regs.m,
	    GET_XFLG, GET_NFLG, GET_ZFLG, GET_VFLG, GET_CFLG, regs.intmask);
    for (i = 0; i < 8; i++){
	printf("FP%d: %g ", i, regs.fp[i]);
	if ((i & 3) == 3) printf("\n");
    }
    printf("N=%d Z=%d I=%d NAN=%d\n",
		(regs.fpsr & 0x8000000) != 0,
		(regs.fpsr & 0x4000000) != 0,
		(regs.fpsr & 0x2000000) != 0,
		(regs.fpsr & 0x1000000) != 0);

    m68k_disasm(m68k_getpc (), nextpc, 1);
    m68k_disasm(m68k_getpc (), nextpc, 1);
    if (nextpc)
	printf("next PC: %0" PRIx32 "\n", *nextpc);
}

#ifdef NEVER_DEFINED // %%% LuZ no save/restore of CPU
/* CPU save/restore code */

#define CPUTYPE_EC 1
#define CPUMODE_HALT 1

uae_u8 *restore_cpu(uae_u8 *src)
{
int i,model,flags;
uae_u32 l;

model=restore_long();
switch(model)
	{
	case 68000:
	currprefs.cpu_level=0;
	break;
	case 68010:
	currprefs.cpu_level=1;
	break;
	case 68020:
	currprefs.cpu_level=2;
	break;
	default:
	write_log("Unknown cpu level %d\n",model);
	break;
}
flags=restore_long();
currprefs.address_space_24=0;
if(flags&CPUTYPE_EC) currprefs.address_space_24=1;
for(i=0;i<15;i++) regs.regs[i]=restore_long();
regs.pc=restore_long();
regs.prefetch=restore_long();
regs.usp=restore_long();
regs.isp=restore_long();
regs.sr=restore_word();
l=restore_long();
if(l&CPUMODE_HALT) regs.stopped=1; else regs.stopped=0;
if(model>=68010) {
	regs.dfc=restore_long();
	regs.sfc=restore_long();
	regs.vbr=restore_long();
}
if(model>=68020) {
	caar=restore_long();
	cacr=restore_long();
	regs.msp=restore_long();
}
write_log("CPU %d%s%03d, PC=%08.8X\n",model/1000,flags&1?"EC":"",model%1000,regs.pc);

return src;
}

static int cpumodel[]={68000,68010,68020,68020};

uae_u8 *save_cpu(int *len)
{
uae_u8 *dstbak,*dst;
int model,i;

dstbak=dst=malloc(256);
model=cpumodel[currprefs.cpu_level];
save_long(model);				/* MODEL */
save_long(currprefs.address_space_24?1:0);	/* FLAGS */
for(i=0;i<15;i++) save_long(regs.regs[i]);	/* D0-D7 A0-A6 */
save_long(m68k_getpc());			/* PC */
save_long(regs.prefetch);			/* prefetch */
MakeSR();
save_long(!regs.s ? regs.regs[15] : regs.usp);	/* USP */
save_long(regs.s ? regs.regs[15] : regs.isp);	/* ISP */
save_word(regs.sr);				/* SR/CCR */
save_long(regs.stopped?CPUMODE_HALT:0);		/* flags */
if(model>=68010) {
	save_long(regs.dfc);			/* DFC */
	save_long(regs.sfc);			/* SFC */
	save_long(regs.vbr);			/* VBR */
}
if(model>=68020) {
	save_long(caar);			/* CAAR */
	save_long(cacr);			/* CACR */
	save_long(regs.msp);			/* MSP */
}
*len=dst-dstbak;
return dstbak;
}

#endif // LuZ %%%
