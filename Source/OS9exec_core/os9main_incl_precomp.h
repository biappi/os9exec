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

/**********************************************/
/*           os9main_incl_precomp.h           */
/*                                            */
/*                                            */
/*             O S 9 E x e c / NT             */
/*  Cooperative-Multiprocess OS-9 emulation   */
/*    main include file for all versions      */
/*                                            */
/*                                            */
/* (c) 1993-2002 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        forsterb@dial.eunet.ch              */
/**********************************************/

// precompilable includes (files that are not part of
// OS9exec itself, such as platform and lib includes)
// ==================================================

#ifndef OS9MAIN_INCL_PRECOMP_H
#define OS9MAIN_INCL_PRECOMP_H

// Identifier for CW Pro 7 identification
// 0xXXYY meaning Compiler Version X.X, rev YY
#define CW7_MWERKS 0x2405 // V 2.4.5 = CW Pro 7


/* make sure that this can also be compiled for MPW */
/* when "macintosh" is not defined */
#if !defined(macintosh) && !defined(__INTEL__) && !defined(linux)
  #define macintosh
#endif


#ifndef TERMINAL_CONSOLE
  #define MPW
#endif


#ifdef macintosh
  /* global feature switches */
  // #define PARTIALRETURNREGS /* returning 16/8-bit values only affects loword/lobyte of register */
  // #define NODEBUG /* cause all debug statements to be omitted (but debugwait() still exists) */
  #define MACFILES /* use Mac OS Fs-calls instead of C-level f-calls for File I/O */
  #define MACMEM      /* use Mac Memory manager calls instead of C-level calloc()/free() */

  #ifdef TERMINAL_CONSOLE
    #define MACTERMINAL /* often used in this combination */
  #endif

  #define TRAPAVAIL_UNAVAILABLE

  // #define MPW_FILTERS                /* uses filters for output of compilers */

  #ifdef MPW
    /* global pragmas */
    /* for OS9app, mpwc_newline must be off due to SIOUX-compat, so it's only */
    // set on the command line when compiling as MPW tool from now on.
    // #pragma mpwc_newline on
    #pragma fourbyteints on
  #endif
#endif

/* WINTEL can't be separated :-) */
#ifdef __INTEL__
  #define windows32
#endif

/* Support only for Linux on PC */
/* makes life easier for them moment ... */
#ifdef linux
  #define __INTEL__
#endif 

/* either windows or linux */
#if defined(windows32) || defined linux
  #define win_linux
#endif


/* these platforms are supported to use TCP/IP sockets */
#ifdef NET_SUPPORT
  #if defined powerc || defined win_linux
    #define WITH_NETWORK
  #endif
#endif


#ifdef windows32
  // #define PARTIALRETURNREGS /* returning 16/8-bit values only affects loword/lobyte of register */
  // #define NODEBUG /* cause all debug statements to be omitted (but debugwait() still exists) */
  #define WINFILES /* use Mac OS Fs-calls instead of C-level f-calls for File I/O */
  #define WINMEM      /* use Mac Memory manager calls instead of C-level calloc()/free() */
#endif

#ifdef __INTEL__
  #define TICKS_PER_SEC 100 /* Number of Ticks per sec (as reported by GetTickCount()) */
#endif

#ifdef macintosh
  #define TICKS_PER_SEC CLOCKS_PER_SEC 
#endif


/* platform includes for all project modules */
/* ========================================= */

#ifdef macintosh
  /* make sure apple extensions are enabled */
  #define __useAppleExts__ 1
  #define __xlC__ 1
  
  typedef int DIR; /* just dummy definitions */
#endif

#ifdef win_linux
  #ifdef windows32
    #if __MWERKS__ < CW7_MWERKS
      // we need the dirent port
      #define _WIN32
      #define  USE_DIRENT_PORT
      #define  MSDIR_WIN32_INTERFACE
      #include "msdir.h"
    #else
      // Note: the _WIN32 and WIN32_LEAN_AND_MEAN are required 
      // or windows.h will generate errors
      #define _WIN32
      #define  WIN32_LEAN_AND_MEAN 
      // just include windows.h, so we have all the windows stuff there
      #include <windows.h>
      // We need sockets, so include them here as well
      #include <ws2spi.h>
    #endif

  #else
    #define  MAX_PATH 1024
    struct   __dirstream {}; /* not visible */
    #include <dirent.h>
    #include <sys/stat.h>
    #include <unistd.h>
  #endif
#endif

typedef struct dirent dirent_typ;

#ifndef linux
  /* C library include files */
  #include <Types.h>
  #include <stdarg.h>
  #include <stddef.h> // %%% added luz 2002-02-04
  #include <errno.h>  // %%% added luz 2002-02-04
  #include <StdIO.h>
  #include <StdLib.h>
  #include <Time.h>
  #include <String.h>

  #ifdef macintosh
    #include <Strings.h>
  #endif

  #include <CType.h>

  #ifdef macintosh
    #include "FCntl.h"
    #include "IntEnv.h"
    #include <OpenTransport.h>
    #include <OpenTptInternet.h>
  #endif

  #include <Signal.h>
#endif

#ifdef TERMINAL_CONSOLE
    #include <unistd.h>
#endif

#ifdef macintosh
  #ifdef MPW
        /* MPW include files */
    #pragma push
    #pragma align=mac68k
    #pragma d0_pointers on
    #pragma enumsalwaysint on

    #pragma mpwc on
    #include <CursorCtl.h>
    #include <ErrMgr.h>
    #pragma pop
  #endif

  /* Mac OS include files */
  // #include <MacRuntime.h>
  #include <Memory.h>
  #include <Resources.h>
  #include <Errors.h>
  #include <Script.h> /* required for smSystemScript */
#endif


#ifdef __INTEL__
  typedef short Boolean;
  #define true  1
  #define false 0

  typedef int OSErr;
#endif


// define type shortcuts
#ifndef linux
  typedef unsigned int      uint;
  typedef unsigned long int ulong;
#endif

#if !defined(windows32) || __MWERKS__ >= CW7_MWERKS
  typedef unsigned char     byte;
#endif


#endif // OS9MAIN_INCL_PRECOMP_H

/* eof */