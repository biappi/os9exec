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
 *    Revision 1.1  2005/06/30 11:01:35  bfo
 *    Moved to the platform dependent things
 *
 *    Revision 1.5  2004/11/20 11:44:07  bfo
 *    Changed to version V3.25 (titles adapted)
 *
 *    Revision 1.4  2003/05/17 11:27:41  bfo
 *    (CVS header included)
 *
 *
 */

/* Mac specific I/O routines */
typedef enum { _start, _appl, _root, _rootX, _data, _exe, _mdir } defdir_typ;

os9err getCipb(CInfoPBRec *cipbP, FSSpec *fspP);
os9err setCipb(CInfoPBRec *cipbP, FSSpec *fspP);

os9err Resolved_FSSpec(short    volID,
                       long     dirID,
                       char    *pathname,
                       Boolean *isFolder,
                       FSSpec  *fsP,
                       FSSpec  *afsP);

os9err getFSSpec(ushort pid, char *pathname, defdir_typ defdir, FSSpec *fsP);

os9err get_dirid(short *volID_P, long *dirID_P, char *pathname);
os9err check_vod(short *volID_P, long *objID_P, long *dirid_P, char *pathname);

/* eof */
