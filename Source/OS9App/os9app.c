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
/*                                            */
/*  Main program for Apple Macintosh version  */
/*                                            */
/*                                            */
/*             O S 9 E x e c / NT             */
/*  Cooperative-Multiprocess OS-9 emulation   */
/*         for Apple Macintosh and PC         */
/*                                            */
/* (c) 1993-2002 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        forsterb@dial.eunet.ch              */
/**********************************************/

/* includes */
/* ======== */
#include "os9exec_incl.h"



void main() 
{
    const argc= 1;
    
    char *argv[]= {
          "os9app"
      // ,"-d"
      // ,"20"
      // ,"shell"
      // ,"startup"
    };

    char *envp[] = {
          NULL,
          NULL
    };


    /* set terminal options */
    #ifndef USE_CARBON
      TermWinScrolls     = 1;      // use scroll bars
      gConsoleQuickInput = true;   // get single chars
      gConsoleEcho       = 0;      // no echo
      gConsoleNLExpand   = false;  // no NL expand
    #endif
    
    os9_main(argc,argv,envp);
}
/* eof */

