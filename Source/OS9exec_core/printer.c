//
//    OS9exec,   OS-9 emulator for Mac OS, Windows and Linux
//    Copyright (C) 2002 Lukas Zeller / Beat Forster
//    Available under http://www.synthesis.ch/os9exec
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
/*         for Apple Macintosh, PC, Linux     */
/*                                            */
/* (c) 1993-2006 by Lukas Zeller, CH-Zuerich  */
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
 *    Revision 1.9  2004/11/27 12:09:30  bfo
 *    _XXX_ introduced
 *
 *    Revision 1.8  2004/11/20 11:44:08  bfo
 *    Changed to version V3.25 (titles adapted)
 *
 *    Revision 1.7  2004/10/22 22:51:12  bfo
 *    Most of the "pragma unused" eliminated
 *
 *    Revision 1.6  2002/09/11 17:03:59  bfo
 *    Adapted for the Linux version
 *
 *    Revision 1.5  2002/09/04 12:23:29  luz
 *    Implemented auto-linefeed for /lp (allowing direct printing of text files)
 *
 *    Revision 1.4  2002/07/30 17:16:34  bfo
 *    E-Mail adress beat.forster@ggaweb.ch is updated everywhere
 *
 *    Revision 1.3  2002/06/29 21:51:21  bfo
 *    GetDefaultPrinter not available for CW5 -> skip whole printer part for CW5
 *
 *    Revision 1.2  2002/06/26 21:24:39  bfo
 *    Mac version can be compiled with "printer.c"
 *
 *    Revision 1.1  2002/06/25 20:44:33  luz
 *    Added /lp printer support under windows. Not tested or even compiled for
 * Mac
 *
 */

#include "os9exec_incl.h"

void init_Printer(_fmgr_) {} /* do nothing */

/* eof */
