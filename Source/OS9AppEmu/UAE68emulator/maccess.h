 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * Memory access functions
  *
  * Copyright 1996 Bernd Schmidt
  */

static __inline__ uae_u32 do_get_mem_long(uae_u32 *a)
{
    uae_u8 *b = (uae_u8 *)a;
    
    return (*b << 24) | (*(b+1) << 16) | (*(b+2) << 8) | (*(b+3));
}

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

