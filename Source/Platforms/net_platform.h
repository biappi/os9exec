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
 *
 */

#ifndef __net_platform_h
#define __net_platform_h


#include "os9exec_incl.h"



static netInstalled= false;
/* Connection from Internet Support Package (ISP) */
/* to the host system's network connection */

os9err NetInstall(void);
/* Create the network connection, if not already done */



os9err MyInetAddr( ulong *inetAddr, ulong *dns1Addr,
                                    ulong *dns2Addr, char* domainName );
/* Returns <inetAddr>, DNS addresses and <domainName> of this host */




OSStatus netReadBlock( ushort pid, net_typ* net, ulong *nBytes );
/* Read <nBytes> of data at <net->transferBuffer> from <net->ep> */
/* Result is <err> >0 -> number of bytes read                    */
/*                 =0 -> no data                                 */
/*                 <0 -> error (OS dependent)                    */
/* <net->closeit> will be set to true in case of EOF             */


OSStatus netWriteBlock( ushort pid, net_typ* net, ulong *nBytes );
/* Write <nBytes> of data of <net->transferBuffer> to <net->ep>  */
/* Result is <err> >0 -> number of bytes written                 */
/*                 =0 -> no data                                 */
/*                 <0 -> error (OS dependent)                    */
/* <net->closeit> will be set to true in case of EOF             */


#endif // __net_platform_h
/* eof */

