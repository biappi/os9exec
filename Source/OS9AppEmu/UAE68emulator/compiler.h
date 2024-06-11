 /*
  * UAE - The Un*x Amiga Emulator
  *
  * m68k -> i386 compiler
  *
  * (c) 1995 Bernd Schmidt
  */

typedef uaecptr (*code_execfunc)(void);

struct code_page {
    struct code_page *next;
    uae_u32 allocmask;
};

struct hash_block {
    struct hash_block *lru_next, *lru_prev;
    struct hash_entry *he_first;

    struct code_page *cpage;
    int alloclen;
    uae_u32 page_allocmask;
    char *compile_start;

    int nrefs;

    int translated:1;
    int untranslatable:1;
    int allocfailed:1;
};

struct hash_entry {
    code_execfunc execute; /* For the sake of the stubs in X86.S */
    struct hash_entry *next,*prev;
    struct hash_entry *next_same_block, *lru_next, *lru_prev;
    struct hash_block *block;

    uaecptr addr;
    uae_u32 matchword;
    int ncalls:8;
    int locked:1;
    int cacheflush:1;
};

extern int nr_bbs_start;
extern uae_u8 nr_bbs_to_run;
extern code_execfunc exec_me;


#define run_compiled_code() do { ; } while (0)
#define compiler_init() do { ; } while (0)
#define possible_loadseg() do { ; } while (0)
#define compiler_flush_jsr_stack() do { ; } while (0)

static __inline__ void m68k_do_rts(void)
{
    m68k_setpc(get_long(m68k_areg(regs, 7)));
    m68k_areg(regs, 7) += 4;
}

static __inline__ void m68k_do_bsr(uaecptr oldpc, uae_s32 offset)
{
    m68k_areg(regs, 7) -= 4;
    put_long(m68k_areg(regs, 7), oldpc);
    m68k_incpc(offset);
}

static __inline__ void m68k_do_jsr(uaecptr oldpc, uaecptr dest)
{
    m68k_areg(regs, 7) -= 4;
    put_long(m68k_areg(regs, 7), oldpc);
    m68k_setpc(dest);
}


