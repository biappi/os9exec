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
/*               os9exec_incl.h               */
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


/* includes for all project modules */
/* ================================ */

#ifndef OS9EXEC_INCL_H
#define OS9EXEC_INCL_H

// include global options (external file to make
// things more clear to look at, specific for each target)
#include "target_options.h"

// Note: Inclusion of all non-OS9exec includes has been moved
//       into separate _precomp file to make them precompilable

// include precompilables
#include "os9main_incl_precomp.h"

/* low_level magic */
/* one single lowlevel include file for all types */
/* use either UAE emulator */
/* or  built-in interface to Mac 68k emulator */
#include "os9_ll.h"

/*
// Note: These have been renamed to OS9_Exxx to avoid potential clashes
//       with host (MSL) codes.

// error variables shouldn't be declared twice
#undef EWOULDBLOCK
#undef ENOTSOCK
#undef EDESTADDRREQ
#undef EMSGSIZE
#undef EPROTOTYPE
#undef ENOPROTOOPT
#undef EPROTONOSUPPORT
#undef EOPNOTSUPP
#undef EPFNOSUPPORT
#undef EAFNOSUPPORT
#undef EADDRINUSE
#undef EADDRNOTAVAIL
#undef ENETDOWN
#undef ENETUNREACH
#undef ENETRESET
#undef ECONNABORTED
#undef ECONNRESET
#undef ENOBUFS
#undef EISCONN
#undef ENOTCONN
#undef ESHUTDOWN
#undef ETOOMANYREFS
#undef ETIMEDOUT
#undef ECONNREFUSED
#undef EALREADY
#undef EINPROGRESS
*/

/* original OS-9 include files */
#ifdef MPW
/* search path for MPW is different */
  #include ":os9defs:procid.h"
  #include ":os9defs:errno.h"
  #include ":os9defs:sgstat.h"
#else  
  #include "procid.h"
  #include "errno.h"
  #include "sgstat.h"
#endif



/* project headers */
/* --------------- */

/* OS9 main loop */
#include "os9main.h"

/* OS9 specific definitions */
#include "os9funcs.h"

/* OS9 emulator core */
#include "os9exec_nt.h"
#include "alarms.h"
#include "consio.h"
#include "debug.h"
#include "events.h"
#include "funcdispatch.h"
#include "modstuff.h"
#include "memstuff.h"
#include "procstuff.h"
#include "telnetaccess.h"
#include "vmod.h"
#include "utilstuff.h"

#ifdef macintosh
  #include "macfiles.h"
#elif defined linux
  #include "linuxfiles.h"
#endif

#include "filestuff.h"
#include "fcalls.h"
#include "icalls.h"
#include "filters.h"

/* external utilities */
#include "intcommand.h"

#endif
/* eof */

