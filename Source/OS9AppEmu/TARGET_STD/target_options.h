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

// Target options
#define USE_UAEMU            // use UAE-68k emulator

                             // Support for:
                             // ============
#define INT_CMD              //      internal commands
//#define PTOC_SUPPORT       // PtoC internal commands

#define RBF_SUPPORT          // RBF access
#define RAM_SUPPORT          // RAM disk (RBF)
#define TFS_SUPPORT          // transparent file system

#define CON_SUPPORT          // console and TTYs
#define PIP_SUPPORT          // pipes   and PTYs
#define NET_SUPPORT          // TCP/IP sockets
#define TERMINAL_CONSOLE     // can do single char I/O


// Mac (4 different versions)
#ifdef macintosh
  #define REUSE_MEM          // do not deallocate memory after use
//#define PRINTER_SUPPORT    // LPT printers

  #ifdef MACOS9
    //
    

  #endif
  
  #ifdef MACOSX
    #define NATIVE_SUPPORT   // allows native DLLs
    
    #ifdef __GNUC__          // XCode (universal binaries)
    //
    #else                    // CW Mach
    //
    #endif
  #endif
#endif

// Windows
#ifdef windows32
  #define PRINTER_SUPPORT    // LPT printers
  #define NATIVE_SUPPORT     // allows native DLLs
#endif

// Linux
#ifdef linux
//#define PRINTER_SUPPORT    // LPT printers
  #define NATIVE_SUPPORT     // allows native DLLs
#endif
