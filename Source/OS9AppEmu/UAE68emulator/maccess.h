 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * Memory access functions
  *
  * Copyright 1996 Bernd Schmidt
  */

/* %%% Bfo: Linux is Intel for the moment */
/* Note: It is not sufficient to define this in os9main_incl.h */
/* because this SW does not import this file */
	#define __INTEL__


/* %%% LuZ: changed to work with x86 as well as PPC */
#ifdef __INTEL__


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

// #define call_mem_get_func(func, addr) ((*func)(addr))
// #define call_mem_put_func(func, addr, v) ((*func)(addr, v))

#undef NO_INLINE_MEMORY_ACCESS
#undef MD_HAVE_MEM_1_FUNCS


/* %%% LuZ */
#endif // __INTEL__


/* %%% LuZ: changed to work with x86 as well as PPC */
#if (defined macintosh || defined __MACH__) && !defined __INTEL__

static __inline__ uae_u32 do_get_mem_long(uae_u32 *a)
{
    return *a;
}

static __inline__ uae_u16 do_get_mem_word(uae_u16 *a)
{
    return *a;
}

static __inline__ uae_u8 do_get_mem_byte(uae_u8 *a)
{
    return *a;
}

static __inline__ void do_put_mem_long(uae_u32 *a, uae_u32 v)
{
    *a = v;
}

static __inline__ void do_put_mem_word(uae_u16 *a, uae_u16 v)
{
    *a = v;
}

static __inline__ void do_put_mem_byte(uae_u8 *a, uae_u8 v)
{
    *a = v;
}

//#define call_mem_get_func(func, addr) ((*func)(addr))
//#define call_mem_put_func(func, addr, v) ((*func)(addr, v))

#undef NO_INLINE_MEMORY_ACCESS
#undef MD_HAVE_MEM_1_FUNCS

// %%% LuZ
#endif // macintosh


