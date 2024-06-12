//
//    OS9exec,   OS-9 emulator for Mac OS, Windows and Linux
//    Copyright (C) 2002 Lukas Zeller / Beat Forster
//	  Available under http://www.synthesis.ch/os9exec
//
//    This program is free software; you can redistribute it and/or
//    modify it under the terms of the GNU General Public License as
//    published by the Free Software Foundation; either version 2 of
//    the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//    See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

// A minimal Win32 console app

#undef TEST

#ifndef TEST
#include "sysconfig.h"
#include "sysdeps.h"

#include "config.h"
#include "options.h"
#include "memory.h"
#include "readcpu.h"
#include "newcpu.h"
#include "compiler.h"
#define extern
#include "luzstuff.h"
#undef extern

// Functions used by CPU emu

void customreset(void) { write_log("Custom reset called\n"); }

void *xmalloc(size_t n)
{
    void *a = malloc(n);
    if (a == NULL) {
        write_log("Virtual memory exhausted!\n");
        abort();
    }
    return a;
}

void console_out(const char *format, ...)
{
    va_list parms;

    va_start(parms, format);
    vfprintf(stderr, format, parms);
    va_end(parms);
}

void write_log(const char *format, ...)
{
    va_list parms;

    va_start(parms, format);
    vfprintf(stderr, format, parms);
    va_end(parms);
}

#endif

// main

int main(void)
{
    printf("Hello World.\n");

#ifndef TEST
    savestate_wanted    = 0;
    currprefs.cpu_level = 2; // 68020;

    init_m68k();
    compiler_init();
    // will crash!
    quit_program = 0; // no reset!
// m68k_go(1);
#endif

    return 0;
}
