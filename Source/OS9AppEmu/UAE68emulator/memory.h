 /*
  * UAE - The Un*x Amiga Emulator
  *
  * memory management
  *
  * Copyright 1995 Bernd Schmidt
  */

static __inline__ uae_u32 get_long(uaecptr addr)
{
    uae_u8 *a = (uae_u8 *)addr;
    uae_u8 *b = (uae_u8 *)a;

    return (*b << 24) | (*(b+1) << 16) | (*(b+2) << 8) | (*(b+3));
}
static __inline__ uae_u32 get_word(uaecptr addr)
{
    uae_u8 *a = (uae_u8 *)addr;
    uae_u8 *b = (uae_u8 *)a;

    return (*b << 8) | (*(b+1));
}
static __inline__ uae_u32 get_byte(uaecptr addr)
{
    uae_u8 *a = (uae_u8 *)addr;
    return *a;
}
static __inline__ void put_long(uaecptr addr, uae_u32 v)
{
    uae_u8 *a = (uae_u8 *)addr;
    uae_u8 *b = (uae_u8 *)a;
    
    *b = v >> 24;
    *(b+1) = v >> 16;    
    *(b+2) = v >> 8;
    *(b+3) = v;
}
static __inline__ void put_word(uaecptr addr, uae_u32 v)
{
    uae_u8 *a = (uae_u8 *)addr;
    uae_u8 *b = (uae_u8 *)a;
    
    *b = v >> 8;
    *(b+1) = (uae_u8)v;
}
static __inline__ void put_byte(uaecptr addr, uae_u32 v)
{
    uae_u8 *a = (uae_u8 *)addr;
    *a = v;
}
