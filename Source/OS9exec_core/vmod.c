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

#include "os9exec_incl.h"

#ifdef macintosh
#ifndef MPW
#include "TermWindow.h"

/* /VMod access  */
/* ============= */


typedef struct {
            TermWindowPtr wPtr;
            TermWindowPtr wStore;
            Rect          r;
            char          title[81]; /* Pascal size */
            ulong         wSize;
            ushort        wIndex;        
            ushort        wTot;        
} newwin_type;
#endif
#endif


os9err pVMod( ushort pid, syspath_typ* spP, ulong *d1,ulong *d2 )
/* These routine makes the direct jump into the Macintosh Toolbox */
/* It emulates the "/vmod" driver of the Spectrapot system */
{
    #ifndef linux
    #pragma unused(spP)
    #endif


    #if defined macintosh && !defined MPW
      ushort       func;
      newwin_type* nw;
      GrafPtr      my_port;
      ushort       sp;
      syspath_typ* spS;
      PicHandle    ph;
    
      func=        loword(*d1); /* get the parameters */
      nw  = (newwin_type*)*d2;
    
      switch (func) {
          case 0xA874: sp = procs[pid].usrpaths[usrStdout]; /* GetPort */
                       spS= get_syspathd( pid,sp );   /* this is really my window */
                       nw->wPtr= (TermWindowPtr)SysPathWindow( spS );
                       break;

          case 0xA8F6: ph= (PicHandle)nw->wStore;  /* DrawPicture */
                       GetPort    ( &my_port    ); /* save current port */
                       SetPort    (  nw->wPtr   );
                       EraseRect  (      &nw->r ); /* erase all */
                       DrawPicture(  ph, &nw->r ); /* draw PCT */

                       UpdatePrms ( (ulong)nw->wPtr, /* give it to update handler */
                                    (ulong)ph, nw->wSize, nw->wIndex,nw->wTot );
                       SetPort    (  my_port  );   /* and restore previous window */
                       break;

          case 0xA913: gConsoleID= VModBase; /* NewWindow */
                       gRect     = &nw->r;
                       memcpy( gTitle,&nw->title, OS9NAMELEN );   /* convert to C */
                       p2cstr( gTitle );
                     
                       ConsPutc(NUL); /* open it */
                       nw->wPtr  = (TermWindowPtr)CurrentWindow();
                       nw->wStore= nw->wPtr;
                       break;
        
          case 0xA92D: gConsoleID= VModBase; /* CloseWindow */
                       memcpy( gTitle,&nw->title, OS9NAMELEN );   /* convert to C */
                       p2cstr( gTitle );

                       RemoveConsole();
                       break;
        
          case 0xA9C8: SysBeep( 1 ); break;  /* also here, as in the real vmod drv */
          default    : ; /* do nothing */
      }

    #elif defined(windows32)
      #pragma unused(pid,d1,d2)
    #endif
    
    return 0; /* no errors */
} /* pVMod */


/* eof */