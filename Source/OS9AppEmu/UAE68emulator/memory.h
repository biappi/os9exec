 /*
  * UAE - The Un*x Amiga Emulator
  *
  * memory management
  *
  * Copyright 1995 Bernd Schmidt
  */

static __inline__ uae_u16 do_get_mem_word(uae_u16 *a)
{
    uae_u8 *b = (uae_u8 *)a;
    
    return (*b << 8) | (*(b+1));
}

static __inline__ uae_u8 do_get_mem_byte(uae_u8 *a)
{
    return *a;
}

static __inline__ void do_put_mem_long(uae_u32 *a, uae_u32 v)
{
    uae_u8 *b = (uae_u8 *)a;
    
    *b = v >> 24;
    *(b+1) = v >> 16;    
    *(b+2) = v >> 8;
    *(b+3) = v;
}

static __inline__ void do_put_mem_word(uae_u16 *a, uae_u16 v)
{
    uae_u8 *b = (uae_u8 *)a;
    
    *b = v >> 8;
    *(b+1) = (uae_u8)v;
}

static __inline__ void do_put_mem_byte(uae_u8 *a, uae_u8 v)
{
    *a = v;
}

static __inline__ uae_u32 get_long(uaecptr addr)
{
    uae_u8 *a = (uae_u8 *)addr;
    uae_u8 *b = (uae_u8 *)a;
    
    return (*b << 24) | (*(b+1) << 16) | (*(b+2) << 8) | (*(b+3));
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

