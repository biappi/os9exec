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

/**********************************************/
/*           os9main_incl_precomp.h           */
/*                                            */
/*                                            */
/*             O S 9 E x e c / NT             */
/*  Cooperative-Multiprocess OS-9 emulation   */
/*    main include file for all versions      */
/*                                            */
/*                                            */
/* (c) 1993-2004 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        bfo@synthesis.ch                    */
/**********************************************/

/*
 *  CVS:
 *    $Author$
 *    $Date$
 *    $Revision$
 *    $Source$
 *    $State$
 *    $Name$ (Tag)
 *    $Locker$ (who has reserved checkout)
 *  Log:
 *    $Log$
 *    Revision 1.13  2004/12/04 00:11:43  bfo
 *    MacOSX MACH adaptions
 *
 *    Revision 1.12  2004/11/20 13:31:00  bfo
 *    Compileable for Mac Classic again
 *
 *    Revision 1.11  2004/11/20 11:44:08  bfo
 *    Changed to version V3.25 (titles adapted)
 *
 *    Revision 1.10  2003/05/24 11:34:13  bfo
 *    Some more MACH-O adaptions, still not able to compile
 *
 *    Revision 1.9  2003/05/21 20:27:43  bfo
 *    #define CW8_MWERKS 0x3000 // V 3.0   = CW Pro 8
 *
 *    Revision 1.8  2003/05/17 10:22:59  bfo
 *    Clean Carbon import
 *
 *    Revision 1.7  2002/07/30 16:46:14  bfo
 *    E-Mail adress bfo@synthesis.ch       is updated everywhere
 *
 *    Revision 1.6  2002/07/23 19:37:15  bfo
 *    go back to the 1/60th second system
 *
 *    Revision 1.5  2002/07/22 22:05:36  bfo
 *    Using "UpTime" as time base, still somewhat strange
 *
 *    Revision 1.4  2002/06/25 20:44:33  luz
 *    Added /lp printer support under windows. Not tested or even compiled for Mac
 *
 */

// precompilable includes (files that are not part of
// OS9exec itself, such as platform and lib includes)
// ==================================================

#ifndef OS9MAIN_INCL_PRECOMP_H
#define OS9MAIN_INCL_PRECOMP_H

// Identifier for CW Pro identification
// 0xXXYY meaning Compiler Version X.X, rev YY
// Use the predefined __MWERKS__ macro.
// It has the form (major version <<12)|(minor version<<8)|patch revision.
// The "version" here is based on our internal versioning scheme (reflected in
// plugins' "version" resource).  
//  2.3   (0x2300) = Pro5
//  2.4   (0x2400) = Pro6
//  2.4.5 (0x2405) = Pro7
//  2.4.7 (0x2407) = Pro7.2
//  3.0   (0x3000) = Pro8

#define CW7_MWERKS 0x2405 // V 2.4.5 = CW Pro 7
#define CW8_MWERKS 0x3000 // V 3.0   = CW Pro 8


/* make sure that this can also be compiled for MPW */
/* when "macintosh" is not defined */
#if !defined macintosh && !defined __INTEL__ && !defined linux
  #define macintosh
#endif

/* define a special label for 68k/MacOS9/Carbon software */
/* which is not compiled for the MACH kernel */
#if defined macintosh && !defined __MACH__
  #define MACOS9
#endif

#ifndef TERMINAL_CONSOLE
  #define MPW
#endif


#ifdef MACOS9
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
#if defined windows32 || defined linux
  #define win_linux
#endif


/* these platforms are supported to use TCP/IP sockets */
#ifdef NET_SUPPORT
  #if defined powerc || defined win_linux
    #define WITH_NETWORK 1
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
  #if __MWERKS__ >= CW7_MWERKS
    #define TICKS_PER_SEC 60 /* CLOCKS_PER_SEC seems to buggy in CW7 */
  #else
    #define TICKS_PER_SEC CLOCKS_PER_SEC
  #endif 
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
      // We need printing support
      #include <winspool.h>
      
      #if __MWERKS__ >= CW8_MWERKS
        #include <dirent.h>
      #endif
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

#ifdef __MACH__
//#include <stdarg.h>
//#include <stddef.h> // %%% added luz 2002-02-04
//#include <StdIO.h>
//#include <StdLib.h>
//#include <Time.h>
//#include <String.h>
//#include <cstdarg>

  #define _STDARG_H
  #include <va_list.h>
  #include <stdarg.h>    // N.B. Not cdstarg!
  #include <cstdio>
//#include <cstdlib>
  #include <stat.h>
#endif

#if !defined linux && !defined __MACH__
  /* C library include files */
  #include <CType.h>
  
  #ifdef USE_CARBON
	  #include <Carbon.h>
  #else
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
      #include "FCntl.h"
      
      #ifdef MPW
        #include "IntEnv.h"
      #endif
      
      #include <OpenTransport.h>
      #include <OpenTptInternet.h>
    #endif
  #endif

  #include <Signal.h>
#endif

#ifdef TERMINAL_CONSOLE
  #ifndef __MACH__
    #include <unistd.h>
  #endif
#endif

#ifdef MACOS9
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
  #include <DriverServices.h>
#endif


#if defined __INTEL__ || defined __MACH__
  typedef short Boolean;
  #define true  1
  #define false 0

  typedef int OSErr;
#endif


// define type shortcuts
#if !defined linux && !defined __MACH__
  typedef unsigned int uint;
#endif

#if !defined linux
  typedef unsigned long int ulong;
#endif

#if !defined(windows32) || __MWERKS__ >= CW7_MWERKS
  typedef unsigned char byte;
#endif


#endif // OS9MAIN_INCL_PRECOMP_H

/* eof */
