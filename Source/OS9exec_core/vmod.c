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
 *    Revision 1.9  2006/02/19 15:21:52  bfo
 *    Header changed to 2006
 *
 *    Revision 1.8  2005/06/30 11:43:38  bfo
 *    Mach-O support
 *
 *    Revision 1.7  2004/11/27 12:06:27  bfo
 *    _XXX_ introduced
 *
 *    Revision 1.6  2004/11/20 11:44:08  bfo
 *    Changed to version V3.25 (titles adapted)
 *
 *    Revision 1.5  2004/10/22 22:36:31  bfo
 *    Most of the "pragma unused" eliminated
 *
 *    Revision 1.4  2003/05/17 10:10:41  bfo
 *    Disabled for USE_CARBON
 *
 *
 */

#include "os9exec_incl.h"

os9err pVMod(ushort pid, _spP_, uint32_t *d1, uint32_t *d2)
/* These routine makes the direct jump into the Macintosh Toolbox */
/* It emulates the "/vmod" driver of the Spectrapot system */
{
#if defined windows32 || defined MACOSX
#pragma unused(pid, d1, d2)
#endif

    return 0; /* no errors */
}

/* eof */
