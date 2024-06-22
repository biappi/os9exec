 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Stuff
  *
  * Copyright 1995, 1996 Ed Hanway
  * Copyright 1995-98 Bernd Schmidt
  */

#define UAEMAJOR 0
#define UAEMINOR 8
#define UAESUBREV 8



struct uae_prefs {

    int m68k_speed;
    int cpu_level;
    int cpu_compatible;
    int address_space_24;

};



extern int fast_memcmp(const void *foo, const void *bar, int len);
extern int memcmpy(void *foo, const void *bar, int len);

/*
 * You can specify numbers from 0 to 5 here. It is possible that higher
 * numbers will make the CPU emulation slightly faster, but if the setting
 * is too high, you will run out of memory while compiling.
 * Best to leave this as it is.
 */
#define CPU_EMU_SIZE 0

/* #define NEED_TO_DEBUG_BADLY */

#if !defined(USER_PROGRAMS_BEHAVE)
#define USER_PROGRAMS_BEHAVE 0
#endif



