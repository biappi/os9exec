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
/* (c) 1993-2002 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        beat.forster@ggaweb.ch              */
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
 *    Revision 1.6  2003/07/31 14:42:02  bfo
 *    "get_locations": Additional parameter for sectorsize
 *
 *    Revision 1.5  2003/05/17 21:36:31  bfo
 *    rename to the same file name on RBF is no longer possible now
 *
 *    Revision 1.4  2002/10/09 20:46:14  bfo
 *    uphe_printf => upe_printf
 *
 *
 */


/* OS9exec/nt internal rename */
/* ========================== */

/* global includes */
#include "os9exec_incl.h"



static void usage( char* name, ushort /* pid */ )
{
    upe_printf( "Syntax:   %s [<opts>] <path> <name> [<opts>]\n", name );
    upe_printf( "Function: rename a file or directory\n" );
    upe_printf( "Options:\n" );
    upe_printf( "     -x       path starts from execution dir\n" );
} /* usage */



/* internal "rename" command for OS9exec/nt */
os9err int_rename(ushort cpid, int argc, char **argv)
{
    #ifdef macintosh
    /* special mac includes */
    #include <Finder.h>
    FSSpec     theFSSpec;
    #endif

    char *p, *sv;
    int nargc=0, h;
    #define     MAXARGS 2
    char *nargv[MAXARGS];
    
    os9err     err;
    OSErr      oserr = 0;
    int        exedir= 0;
    ushort     path;
    ptype_typ  type;
    char       *nameP, *qq;
    char       nmS     [OS9PATHLEN];
    char       nmD     [OS9PATHLEN];
    char       newName [OS9PATHLEN];
    ulong      fd, dfd, dcp, dcpD, sSct, len;
    Boolean    asDir;
    char       oldPath [OS9PATHLEN];

    #ifdef win_linux
      char*    pp;
      char     newPath [OS9PATHLEN]; 
      char     adaptOld[OS9PATHLEN];
      char     adaptNew[OS9PATHLEN];
    #endif
    
    
    debugprintf(dbgUtils,dbgNorm,( "# rename\n" ));
    
    /* get arguments and options */
    for (h=1; h<argc; h++) {
        p= argv[h];    
        if (*p=='-') { 
            p++;
            switch (tolower(*p)) {
                case '?' :  usage(argv[0],cpid); return 0;
                case 'x' :  exedir= 1; break;
                default  :  upe_printf("Error: unknown option '%c'!\n",*p); 
                            usage(argv[0],cpid); return 1;
            }   
        }
        else {
            if (nargc>=MAXARGS) { 
                upe_printf("Error: no more than %d arguments allowed\n",MAXARGS); return 1;
            }
            nargv[nargc++]= argv[h];
        }
    }   
        
    if (nargc!=2) {
        upe_printf("Error: two arguments required\n"); return 1;
    }


    /* first get file/dir to be renamed */
               nameP= nargv[0];
    strcpy(nmS,nameP);
    
    err= parsepath( cpid, &nameP,oldPath, exedir ); if (err) return err;
    
    /* ... and new name */
             strcpy( newName, nargv[1] );
        qq=  strstr( newName,PSEP_STR ); /* no path delimiters allowed */
    if (qq!=NULL) return _errmsg( E_BPNAM,"can't rename to \"%s\"",newName);

        len= strlen( newName );
    if (len>DIRNAMSZ)
                  return _errmsg( E_BPNAM,"name too long \"%s\"", newName );


        type= IO_Type( cpid,nmS, 0x00 ); /* get the device type: Mac/PC or RBF */
    if (type==fRBF) {
        /* do it the same way as the OS-9 rename */
            err= get_locations( cpid,type, nmS,false, &asDir, &fd,&dfd,&dcp,&sSct );
        if (err) return err;
        
        p = (char*)&nmS;
        sv= p; sv--;
        
        while  (*p!=NUL) {
            if (*p==PSEP) sv= p; /* search for the last one */
            p++;
        }
        
        if      (sv==nmS) return E_FNA; /* this is the root path */
        if      (sv >nmS) *sv= NUL;     /* cut the file name at the end */
        else strcpy( nmS, "." );        /* or take the current path */
        
        strcpy( nmD,nmS );
        strcat( nmD,PSEP_STR );
        strcat( nmD,newName );
        	 err= get_locations  ( cpid,type, nmD,false, &asDir, &fd,&dfd,&dcpD,&sSct );
        if (!err && dcp!=dcpD) return E_CEF;   /* file already exists with different name ? */
                                     /* case sensitive changes of the same name are allowed */
        
        err= usrpath_open   ( cpid,&path, type, nmS,     0x83);   if (err) return err;
        err= usrpath_seek   ( cpid, path, dcp );                  if (err) return err;
        LastCh_Bit7( newName, true );      len++;              /* write also NUL terminator */
        err= usrpath_write  ( cpid, path, &len, newName, false ); if (err) return err;
        err= usrpath_close  ( cpid, path );                       if (err) return err;
    }
    else {
        #ifdef macintosh
          /* now do the real stuff */
          /* first make FSSpec for file/dir to be renamed */
              err= getFSSpec( cpid,oldPath, exedir ?_exe:_data, &theFSSpec ); 
          if (err) return err;
    
          /* now rename it */       c2pstr(newName);
              oserr= FSpRename( &theFSSpec,newName);
          if (oserr==dupFNErr) _errmsg(0,"can't rename to \"%#s\"",newName);
          err= host2os9err(oserr,E_PNNF);
    
        #elif defined win_linux
                           pp=  oldPath;
          err= AdjustPath( pp, adaptOld, false ); 
          pp=                  adaptOld;
          if (err) return _errmsg( err,"can't open \"%s\"",oldPath );
          
          strcpy   ( newPath,pp );
          qq=        newPath+strlen(newPath)-1;
          while  (qq>newPath && *qq!=PATHDELIM) qq--; /* cut the existing file name */
          *++qq= NUL;
          strcat  ( newPath,newName );
          
                           qq=  newPath;
          err= AdjustPath( qq, adaptNew, false ); 
          if (!err) return _errmsg( E_CEF,"can't rename to \"%s\"",newName );
        
          err= host2os9err( rename( pp,qq ),E_CEF );
          debugprintf(dbgUtils,dbgNorm,( "# rename from: '%s'\n",        pp      ));
          debugprintf(dbgUtils,dbgNorm,( "# rename to:   '%s' err=%d\n", qq, err ));
          if  (err) return _errmsg( err,"can't rename to \"%s\"",newName );
        #endif
    }
    
    return err;
} /* int_rename */


/* eof */
