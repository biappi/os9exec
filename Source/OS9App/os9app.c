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
    TermWinScrolls     = 1;      // use scroll bars
    gConsoleQuickInput = true;   // get single chars
    gConsoleEcho       = 0;      // no echo
    gConsoleNLExpand   = false;  // no NL expand

    os9_main(argc,argv,envp);
}
/* eof */
