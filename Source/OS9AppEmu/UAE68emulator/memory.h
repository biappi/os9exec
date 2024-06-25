 /*
  * UAE - The Un*x Amiga Emulator
  *
  * memory management
  *
  * Copyright 1995 Bernd Schmidt
  */

#include "maccess.h"

static __inline__ uae_u32 get_long(uaecptr addr)
{
    return do_get_mem_long((uae_u32 *)addr);
}
static __inline__ uae_u32 get_word(uaecptr addr)
{
    return do_get_mem_word((uae_u16 *)addr);
}
static __inline__ uae_u32 get_byte(uaecptr addr)
{
    return do_get_mem_byte((uae_u8  *)addr);
}
static __inline__ void put_long(uaecptr addr, uae_u32 l)
{
    do_put_mem_long((uae_u32 *)addr, l);
}
static __inline__ void put_word(uaecptr addr, uae_u32 w)
{
    do_put_mem_word((uae_u16 *)addr, w);
}
static __inline__ void put_byte(uaecptr addr, uae_u32 b)
{
    do_put_mem_byte((uae_u8  *)addr, b);
}

static __inline__ uae_u8 *get_real_address(uaecptr addr)
{
 /* return get_mem_bank(addr).xlateaddr(addr); */
    return (uae_u8 *)addr; /* no translation ever %%% */
}

static __inline__ int valid_address( uaecptr addr, uae_u32 size )
{
 
 /* return get_mem_bank(addr).check(addr, size); */
    return 1; /* all addresses can be translated (as there is no translation! %%% */
}

