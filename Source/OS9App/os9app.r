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
/*         for MacOS, Windows and Linux       */
/*                                            */
/* (c) 1993-2006 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        bfo@synthesis.ch                    */
/**********************************************/

/* This Rez source generates OS9C resources for the
 * common OS9 libraries used by OS9exec/NT based tools
 */

#include "mactypes.r"
 
resource 'vers' (1, purgeable) {
    0x3,
    0x34,
    final,
    0x1,
    verGrSwiss,
    "3.34",
    "OS9exec V3.34   © 1993-2006   luz@synthesis.ch / bfo@synthesis.ch"
};
