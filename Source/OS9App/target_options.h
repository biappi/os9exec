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

#ifdef macintosh
  #define REUSE_MEM      // do not deallocate memory after use
#endif

#define INT_CMD          // internal commands       supported
#define CON_SUPPORT      // conole and TTYs         supported
#define PIP_SUPPORT      // pipes and PTYs          supported

#define TERMINAL_CONSOLE // can do single char I/O
#define TFS_SUPPORT      // transparent file system supported
#define RBF_SUPPORT      // RBF acesss is           supported
#define RAM_SUPPORT      // RAM disk                supported

#ifdef powerc
  #define NET_SUPPORT    // TCP/IP sockets          supported
#endif

#define PRINTER_SUPPORT  // LPT printers Win        supported   

