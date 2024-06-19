 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Memory management
  *
  * (c) 1995 Bernd Schmidt
  * (c) 1999 Brian King - added enforcer-style checking for location zero in chip-memory
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "config.h"
#include "options.h"
//#include "uae.h"
#include "memory.h"
//#include "ersatz.h"
//#include "zfile.h"
//#include "savestate.h"
//#include "gui.h"

//#ifdef USE_MAPPED_MEMORY
//#include <sys/mman.h>
//#endif

//int ersatzkickfile = 0;

#include <luzstuff.h>

#ifdef SAVE_MEMORY_BANKS
addrbank *mem_banks[65536];
#else
addrbank mem_banks[65536];
#endif

#ifdef NO_INLINE_MEMORY_ACCESS
__inline__ uae_u32 longget (uaecptr addr)
{
    return call_mem_get_func (get_mem_bank (addr).lget, addr);
}
__inline__ uae_u32 wordget (uaecptr addr)
{
    return call_mem_get_func (get_mem_bank (addr).wget, addr);
}
__inline__ uae_u32 byteget (uaecptr addr)
{
    return call_mem_get_func (get_mem_bank (addr).bget, addr);
}
__inline__ void longput (uaecptr addr, uae_u32 l)
{
    call_mem_put_func (get_mem_bank (addr).lput, addr, l);
}
__inline__ void wordput (uaecptr addr, uae_u32 w)
{
    call_mem_put_func (get_mem_bank (addr).wput, addr, w);
}
__inline__ void byteput (uaecptr addr, uae_u32 b)
{
    call_mem_put_func (get_mem_bank (addr).bput, addr, b);
}
#endif

uae_u32 chipmem_mask, kickmem_mask, bogomem_mask, a3000mem_mask;
static uae_u8 *restored_chipmem,*restored_bogomem;

/* A dummy bank that only contains zeros */

static uae_u32 dummy_lget (uaecptr) REGPARAM;
static uae_u32 dummy_wget (uaecptr) REGPARAM;
static uae_u32 dummy_bget (uaecptr) REGPARAM;
static void dummy_lput (uaecptr, uae_u32) REGPARAM;
static void dummy_wput (uaecptr, uae_u32) REGPARAM;
static void dummy_bput (uaecptr, uae_u32) REGPARAM;
static int dummy_check (uaecptr addr, uae_u32 size) REGPARAM;

uae_u32 REGPARAM2 dummy_lget (uaecptr addr)
{
    if (currprefs.illegal_mem)
	write_log ("Illegal lget at %08lx\n", addr);

    return 0;
}

uae_u32 REGPARAM2 dummy_wget (uaecptr addr)
{
    if (currprefs.illegal_mem)
	write_log ("Illegal wget at %08lx\n", addr);

    return 0;
}

uae_u32 REGPARAM2 dummy_bget (uaecptr addr)
{
    if (currprefs.illegal_mem)
	write_log ("Illegal bget at %08lx\n", addr);

    return 0;
}

void REGPARAM2 dummy_lput (uaecptr addr, uae_u32 l)
{
    if (currprefs.illegal_mem)
	write_log ("Illegal lput at %08lx\n", addr);
}
void REGPARAM2 dummy_wput (uaecptr addr, uae_u32 w)
{
    if (currprefs.illegal_mem)
	write_log ("Illegal wput at %08lx\n", addr);
}
void REGPARAM2 dummy_bput (uaecptr addr, uae_u32 b)
{
    if (currprefs.illegal_mem)
	write_log ("Illegal bput at %08lx\n", addr);
}

int REGPARAM2 dummy_check (uaecptr addr, uae_u32 size)
{
    if (currprefs.illegal_mem)
	write_log ("Illegal check at %08lx\n", addr);

    return 0;
}

/* A bank that is mapped to a 64-bit host memory */

static uae_u32 sixtyfour_lget (uaecptr) REGPARAM;
static uae_u32 sixtyfour_wget (uaecptr) REGPARAM;
static uae_u32 sixtyfour_bget (uaecptr) REGPARAM;
static void sixtyfour_lput (uaecptr, uae_u32) REGPARAM;
static void sixtyfour_wput (uaecptr, uae_u32) REGPARAM;
static void sixtyfour_bput (uaecptr, uae_u32) REGPARAM;
static int sixtyfour_check (uaecptr addr, uae_u32 size) REGPARAM;
static uae_u8 *sixtyfour_bank_map[65536];

uae_u32 REGPARAM2 sixtyfour_lget (uaecptr addr)
{
    uae_u16 bank_index = ((uae_u32)addr >> 16) & 0xffff;
    uae_u16 offset = addr & 0xffff;
    return do_get_mem_long((uae_u32*)(sixtyfour_bank_map[bank_index] + offset));
}

uae_u32 REGPARAM2 sixtyfour_wget (uaecptr addr)
{
    uae_u16 bank_index = ((uae_u32)addr >> 16) & 0xffff;
    uae_u16 offset = addr & 0xffff;
    return do_get_mem_word((uae_u16*)(sixtyfour_bank_map[bank_index] + offset));
}

uae_u32 REGPARAM2 sixtyfour_bget (uaecptr addr)
{
    uae_u16 bank_index = ((uae_u32)addr >> 16) & 0xffff;
    uae_u16 offset = addr & 0xffff;
    return do_get_mem_byte((uae_u8*)(sixtyfour_bank_map[bank_index] + offset));
}

void REGPARAM2 sixtyfour_lput (uaecptr addr, uae_u32 l)
{
    uae_u16 bank_index = ((uae_u32)addr >> 16) & 0xffff;
    uae_u16 offset = addr & 0xffff;
    do_put_mem_long((uae_u32*)(sixtyfour_bank_map[bank_index] + offset), l);
}

void REGPARAM2 sixtyfour_wput (uaecptr addr, uae_u32 w)
{
    uae_u16 bank_index = ((uae_u32)addr >> 16) & 0xffff;
    uae_u16 offset = addr & 0xffff;
    do_put_mem_word((uae_u16*)(sixtyfour_bank_map[bank_index] + offset), w);
}

void REGPARAM2 sixtyfour_bput (uaecptr addr, uae_u32 b)
{
    uae_u16 bank_index = ((uae_u32)addr >> 16) & 0xffff;
    uae_u16 offset = addr & 0xffff;
    do_put_mem_byte((uae_u8*)(sixtyfour_bank_map[bank_index] + offset), b);
}

int REGPARAM2 sixtyfour_check (uaecptr addr, uae_u32 size)
{
    if (currprefs.illegal_mem)
	write_log ("Illegal check at %08lx\n", addr);

    return 0;
}

uae_u8 REGPARAM2 *sixtyfour_xlate (uaecptr a)
{
    uae_u16 bank_index = ((uae_u32)a >> 16) & 0xffff;
    uae_u16 offset = a & 0xffff;
    return (uae_u8*)(sixtyfour_bank_map[bank_index] + offset);
}

static void init_sixtyfour_map()
{
    for (int i = 0; i < 65536; i++)
    	sixtyfour_bank_map[i] = 0;
}

uae_u8 REGPARAM2 *default_xlate (uaecptr a)
{
    write_log("default_xlate called, no no no! a=%p", a);
    return NULL;
}


/* Address banks */

addrbank dummy_bank = {
    dummy_lget, dummy_wget, dummy_bget,
    dummy_lput, dummy_wput, dummy_bput,
    default_xlate, dummy_check
};

addrbank sixtyfour_bank = {
    sixtyfour_lget, sixtyfour_wget, sixtyfour_bget,
    sixtyfour_lput, sixtyfour_wput, sixtyfour_bput,
    sixtyfour_xlate, sixtyfour_check
};

char *address_space, *good_address_map;
int good_address_fd;

static void init_mem_banks (void)
{
    int i;
    for (i = 0; i < 65536; i++)
	put_mem_bank (i<<16, &dummy_bank);

    write_log("init_mem_banks     dummy_bank.xlateaddr=%p\n", dummy_bank.xlateaddr);
    write_log("init_mem_banks sixtyfour_bank.xlateaddr=%p\n", sixtyfour_bank.xlateaddr);
}

#define MAKE_USER_PROGRAMS_BEHAVE 1
void memory_init (void)
{
    init_mem_banks ();
    init_sixtyfour_map();
}

void memory_cleanup( void )
{
}

void map_banks (addrbank *bank, int start, int size)
{
    int bnr;
    unsigned long int hioffs = 0, endhioffs = 0x100;

    if (start >= 0x100) {
	for (bnr = start; bnr < start + size; bnr++)
	    put_mem_bank (bnr << 16, bank);
	return;
    }
    /* Some '020 Kickstarts apparently require a 24 bit address space... */
    if (currprefs.address_space_24)
	endhioffs = 0x10000;
    for (hioffs = 0; hioffs < endhioffs; hioffs += 0x100)
	for (bnr = start; bnr < start+size; bnr++)
	    put_mem_bank ((bnr + hioffs) << 16, bank);
}

void map_64bit_bank(uae_u8 *base, int size)
{
    if (size >= 0x10000) {
        write_log("TODO support mapping of more than 64k of memory (requested: %d / 0x%x)\n", size, size);
        exit(1);
    }

    uae_u16 sixtyfour_bank_index = ((uae_u64)base >> 16) & 0xffff;

    uae_u8 *new_bank = (uae_u8*)((uae_u64)base & 0xffffffffffff0000);
    uae_u8 *existing_bank = sixtyfour_bank_map[sixtyfour_bank_index];
    if (new_bank == existing_bank)
    {
        return;
    }

    if (existing_bank != 0)
    {
        write_log("map_64bit_bank: mapping 0x%04x is already mapped to %p (requested: %p)\n", sixtyfour_bank_index, existing_bank, base);
        exit(1);
    }

    sixtyfour_bank_map[sixtyfour_bank_index] = new_bank;
    uaecptr *truncated_base = (uaecptr*)((uae_u32)base & 0x00000000ffffffff);
    put_mem_bank(truncated_base, &sixtyfour_bank);

    write_log("Mapped bank index 0x%04x to 64-bit bank %p\n", sixtyfour_bank_index, new_bank);
}
