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
