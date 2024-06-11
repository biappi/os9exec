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
/*             O S 9 E x e c / NT             */
/*  Cooperative-Multiprocess OS-9 emulation   */
/*         for Apple Macintosh and PC         */
/*                                            */
/* (c) 1993-2007 by Lukas Zeller, CH-Zuerich  */
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
 *    Revision 1.26  2007/01/28 21:36:21  bfo
 *    __GLOBDEF definitions added
 *
 *    Revision 1.25  2007/01/06 01:13:44  bfo
 *    "MACOSX" defined as equivalent of __MACH__
 *
 *    Revision 1.24  2006/06/26 22:08:24  bfo
 *    <setjmp.h> included
 *
 *    Revision 1.23  2006/06/12 10:52:09  bfo
 *    MAX_PATH added
 *
 *    Revision 1.22  2006/06/07 16:06:28  bfo
 *    XCode / IntelMacs supported now
 *
 *    Revision 1.21  2006/02/19 16:08:44  bfo
 *    Some sytem definitons changes
 *
 *    Revision 1.20  2005/07/15 22:24:00  bfo
 *    <math.h> / <unistd.h> and NULL adaptions for OS9exec/PtoC
 *
 *    Revision 1.19  2005/07/06 21:09:44  bfo
 *    defined UNIX
 *
 *    Revision 1.18  2005/06/30 15:46:13  bfo
 *    Adapted for universal use
 *
 *    Revision 1.17  2005/06/30 15:39:33  bfo
 *    Adapted for universal use
 *
 *    Revision 1.16  2005/06/30 15:22:44  bfo
 *    Adapted for universal use
 *
 *    Revision 1.15  2005/06/30 11:53:47  bfo
 *    Mach-O support
 *
 *    Revision 1.14  2005/01/22 16:12:55  bfo
 *    Renamed to ifdef MACOS9
 *
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

// This is OS9exec !!
#define OS9EXEC 1 


/* XCode is MACH, but does not know about macintosh */
#if !defined macintosh && defined __MACH__
  #define MACOSX
  #define macintosh
  
  // Support Intel Macs
  #ifndef __BIG_ENDIAN__
    #define __INTEL__
  #endif
#endif

/* WINTEL can't be separated :-) */
#ifdef __INTEL__
  // except for the "bright future"
  #ifndef macintosh
    #define windows32
  #endif
#endif

/* Support only for Linux on PC */
/* makes life easier for them moment ... */
#ifdef linux
  #define __INTEL__
#endif 

/* the UNIX systems */
#if defined linux || defined MACOSX
  #define UNIX
#endif

/* either windows or linux */
#if defined windows32 || defined linux
  #define win_linux
#endif

/* all modern systems combined */
#if defined win_linux || defined UNIX
  #define win_unix
#endif


// Decide about the system
#if !defined macintosh && !defined __INTEL__ && !defined __GNUC__
  /* make sure that this can also be compiled for MPW */
  /* when "macintosh" is not defined */
  #define macintosh
#endif

#if !defined powerc && defined MACOSX && !defined __INTEL__
  #define powerc
#endif

/* define a special label for 68k/MacOS9/Carbon software */
/* which is not compiled for the MACH kernel */
#if defined macintosh && !defined MACOSX
  #define MACOS9
  
    #define USE_CLASSIC
#endif


// ----------------------------------------------------------------------------
// the target specific things
#include "target_options.h"
// ----------------------------------------------------------------------------


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


/* these platforms are supported to use TCP/IP sockets */
//#ifdef NET_SUPPORT
//  #if defined powerc || defined win_linux
//    #define WITH_NETWORK 1
//  #endif
//#endif


#ifdef windows32
  // #define PARTIALRETURNREGS /* returning 16/8-bit values only affects loword/lobyte of register */
  // #define NODEBUG /* cause all debug statements to be omitted (but debugwait() still exists) */
  #define WINFILES /* use Mac OS Fs-calls instead of C-level f-calls for File I/O */
  #define WINMEM      /* use Mac Memory manager calls instead of C-level calloc()/free() */
#endif

#if defined __INTEL__ || defined UNIX
  #define TICKS_PER_SEC 100 /* Number of Ticks per sec (as reported by GetTickCount()) */
#endif



/* platform includes for all project modules */
/* ========================================= */


#include <math.h>

#ifdef win_unix
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
  //#define  MAX_PATH 1024
    
    #ifndef __MACH__
      struct __dirstream {}; /* not visible */
    #endif
    
    #include <dirent.h>
    #include <sys/stat.h>
    
    #ifndef __MACH__
      #include <unistd.h>
    #endif
  #endif
#endif

typedef struct dirent dirent_typ;

#ifdef __MACH__
  #define  NULL 0L
  #include <stddef.h>
  
  #ifdef __MWERKS__
    #include <stdio.h>
	#include <mw_stdarg.h>    // N.B. Not cdstarg!
  //#include <cstdio>
    #include <stat.h>
  #endif
#endif

#ifndef UNIX
  /* C library include files */
  #include <CType.h>
  
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
      
      #ifdef NET_SUPPORT
        #include <OpenTransport.h>
        #include <OpenTptInternet.h>
      #endif
    #endif

  #include <Signal.h>
#endif

#ifdef TERMINAL_CONSOLE
  #ifndef MACOSX
    #include <unistd.h>
  #endif
#endif

/* setjmp/longjmp support */
#include <setjmp.h>



// define type shortcuts
#ifndef  __GLOBDEF
#define  __GLOBDEF
  typedef unsigned char  byte;
  typedef unsigned short os9err;

  #if defined __INTEL__ || defined __MACH__
    typedef char Boolean;
    #define true  1
    #define false 0
  #endif

  #if defined __MACH__
    typedef unsigned long ulong;
  #endif


  #ifndef UNIX
    typedef unsigned      int uint;
    typedef unsigned long int ulong;
  #endif

  #if defined __INTEL__ || defined __MACH__
    typedef int OSErr;
  #endif
#endif //__GLOBDEF


#endif // OS9MAIN_INCL_PRECOMP_H

/* eof */
