// 
// Ê ÊOS9exec,   OS-9 emulator for Mac OS, Windows and Linux 
// Ê ÊCopyright (C) 2002 Ê[ Lukas Zeller / Beat Forster ]
//	  Available under http://www.synthesis.ch/os9exec
// 
// Ê ÊThis program is free software; you can redistribute it and/or 
// Ê Êmodify it under the terms of the GNU General Public License as 
// Ê Êpublished by the Free Software Foundation; either version 2 of 
// Ê Êthe License, or (at your option) any later version. 
// 
// Ê ÊThis program is distributed in the hope that it will be useful, 
// Ê Êbut WITHOUT ANY WARRANTY; without even the implied warranty of 
// Ê ÊMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// Ê ÊSee the GNU General Public License for more details. 
// 
// Ê ÊYou should have received a copy of the GNU General Public License 
// Ê Êalong with this program; if not, write to the Free Software 
// Ê ÊFoundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
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

void customreset(void) {
    write_log("Custom reset called\n");
}


void *xmalloc(size_t n)
{
    void *a = malloc (n);
    if (a == NULL) {
        write_log( "Virtual memory exhausted!\n" );
        abort ();
    }
    return a;
}


void console_out (const char *format,...)
{
    va_list parms;

    va_start (parms, format);
    vfprintf(stderr,format,parms);
    va_end (parms);
}

void write_log (const char *format,...)
{
    va_list parms;

    va_start (parms, format);
    vfprintf(stderr,format,parms);
    va_end (parms);
}

#endif

// main

int main( void ) {
    printf("Hello World.\n");

    #ifndef TEST
    savestate_wanted = 0;
    currprefs.cpu_level = 2; // 68020;
    
    init_m68k();
    compiler_init ();
    // will crash!
    quit_program = 0; // no reset!
    //m68k_go(1);
    #endif
    
    return 0;
}