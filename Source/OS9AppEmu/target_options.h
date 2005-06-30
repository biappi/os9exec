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


/* WINTEL can't be separated :-) */
#ifdef __INTEL__
  #define windows32
#endif

/* Support only for Linux on PC */
/* makes life easier for them moment ... */
#ifdef linux
  #define __INTEL__
#endif 

/* the UNIX systems */
#if defined linux || defined __MACH__
  #define unix
#endif

/* either windows or linux */
#if defined windows32 || defined linux
  #define win_linux
#endif

/* all modern systems combined */
#if defined win_linux || defined unix
  #define win_unix
#endif


// Decide about the system
#if !defined macintosh && !defined __INTEL__ && !defined linux
  /* make sure that this can also be compiled for MPW */
  /* when "macintosh" is not defined */
  #define macintosh
  
  #ifndef __MACH__
    #define MPW
  #endif
#endif

/* define a special label for 68k/MacOS9/Carbon software */
/* which is not compiled for the MACH kernel */
#if defined macintosh && !defined __MACH__
  #define MACOS9
  
  #ifndef USE_CARBON
    #define USE_CLASSIC
  #endif
#endif


// Target options

#define USE_UAEMU            // use UAE-68k emulator

#ifdef macintosh
   #define REUSE_MEM         // do not deallocate memory after use
#endif
  
#define INT_CMD              // internal commands       supported
#define CON_SUPPORT          // conole and TTYs         supported
#define PIP_SUPPORT          // pipes  and PTYs         supported



#ifdef windows32
  #define TERMINAL_CONSOLE   // can do single char I/O
  #define TFS_SUPPORT        // transparent file system supported
  #define RBF_SUPPORT        // RBF acesss              supported
//#define RAM_SUPPORT        // RAM disk                supported
  #define NET_SUPPORT        // TCP/IP sockets          supported
  #define PRINTER_SUPPORT    // LPT printers            supported
#endif

#ifdef USE_CLASSIC
  #define TERMINAL_CONSOLE   // can do single char I/O
  #define TFS_SUPPORT        // transparent file system supported
  #define RBF_SUPPORT        // RBF acesss              supported
  #define RAM_SUPPORT        // RAM disk                supported
  #define NET_SUPPORT        // TCP/IP sockets          supported
  #define PRINTER_SUPPORT    // LPT printers            supported
#endif

#ifdef USE_CARBON
  #define TERMINAL_CONSOLE   // can do single char I/O
  #define TFS_SUPPORT        // transparent file system supported
  #define RBF_SUPPORT        // RBF acesss              supported
  #define RAM_SUPPORT        // RAM disk                supported
  #define NET_SUPPORT        // TCP/IP sockets          supported
  #define PRINTER_SUPPORT    // LPT printers            supported
#endif

#ifdef __MACH__
  #define TERMINAL_CONSOLE   // can do single char I/O
  #define TFS_SUPPORT        // transparent file system supported
  #define RBF_SUPPORT        // RBF acesss              supported
  #define RAM_SUPPORT        // RAM disk                supported
  #define NET_SUPPORT        // TCP/IP sockets          supported
  #define PRINTER_SUPPORT    // LPT printers            supported
#endif
