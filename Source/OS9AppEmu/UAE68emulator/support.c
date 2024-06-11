 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * Miscellaneous machine dependent support functions and definitions
  *
  * Copyright 1996 Bernd Schmidt
  */


#include "sysconfig.h"
#include "sysdeps.h"

#include "config.h"
#include "options.h"
#include "m68k.h"
//#include "events.h"

struct flag_struct regflags;

int fast_memcmp(const void *foo, const void *bar, int len)
{
    return memcmp(foo, bar, len);
}

int memcmpy(void *foo, const void *bar, int len)
{
    int differs = memcmp(foo, bar, len);
    memcpy(foo, bar, len);
    return differs;
}


/* All the Win32 configurations handle this in od-win32/win32.c */
#if !defined _WIN32 && !defined __MWERKS__ && !defined __GNUC__

#include <signal.h>

static volatile frame_time_t last_time, best_time;
static volatile int loops_to_go;

#ifdef __cplusplus
static RETSIGTYPE alarmhandler(...)
#else
static RETSIGTYPE alarmhandler(int foo)
#endif
{
    frame_time_t bar;
    bar = read_processor_time ();
    if (bar - last_time < best_time)
	best_time = bar - last_time;
    if (--loops_to_go > 0) {
	signal (SIGALRM, alarmhandler);
	last_time = read_processor_time();
	alarm (1);
    }
}

#ifdef __cplusplus
static RETSIGTYPE illhandler(...)
#else
static RETSIGTYPE illhandler(int foo)
#endif
{
    rpt_available = 0;
}

void machdep_init (void)
{
    rpt_available = 1;
    signal (SIGILL, illhandler);
    read_processor_time ();
    signal (SIGILL, SIG_DFL);
    if (! rpt_available) {
	fprintf (stderr, "Your processor does not support the RDTSC instruction.\n");
	return;
    }
    fprintf (stderr, "Calibrating delay loop.. ");
    fflush (stderr);
    best_time = (frame_time_t)-1;
    loops_to_go = 5;
    signal (SIGALRM, alarmhandler);
    /* We want exact values... */
    sync (); sync (); sync ();
    last_time = read_processor_time();
    alarm (1);
    while (loops_to_go != 0)
	usleep (1000000);
    fprintf (stderr, "ok - %.2f BogoMIPS\n",
	     ((double)best_time / 1000000), best_time);
    vsynctime = best_time / 50;
}

#endif

