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



/* OS9exec/nt calling of Windows/DOS command line commands */
/* ======================================================= */

/* global includes */
#include "os9exec_incl.h"

#ifdef windows32

static void usage(char *name, ushort pid)
{
    usrpath_printf(pid,usrStderr,"# Usage:    %s <Windows Consoler/dos command name> [args...]\n",name);
} /* usage */



/* call of external Win/DOS commands for OS9exec/nt */
os9err int_wincmd(ushort pid, int argc, char **argv)
{   
    int k;
    
    /* show args for debug */
    if (debugcheck(dbgUtils,dbgNorm)) {
        upe_printf("# os9dir: argc=%d",argc);
        for (k=0; k<argc; k++) upe_printf(", argv[%d]='%s'",k,argv[k]);
        upe_printf("\n");
    }
    if (argc<2) {
        usage(argv[0],pid);
        return(1);
    }
    /* Now call windows command */
    return call_hostcmd("",pid,argc-1,&argv[1]);
} /* int_wincmd */


#endif