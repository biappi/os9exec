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


/* "macfiles.c" */
/* specific mac file routines which are not available */
/* on other systems */



#include "os9exec_incl.h"



os9err getCipb( CInfoPBRec* cipbP, FSSpec *fspP )
/* get cipb from FSSpec */
{
    OSErr  oserr;
    Str255 fName;
    
    /* get info record for the FSSpec itself */
    memcpy( fName,fspP->name, 32 );
    
    cipbP->hFileInfo.ioCompletion= 0L;
    cipbP->hFileInfo.ioNamePtr   = fName;
    cipbP->hFileInfo.ioVRefNum   = fspP->vRefNum;
    cipbP->hFileInfo.ioFDirIndex = 0;   /* use the dir & vRefNum */
    cipbP->hFileInfo.ioDirID     = fspP->parID;
    oserr= PBGetCatInfoSync(cipbP);
    
    return host2os9err(oserr,E_PNNF);
} /* getCipb */


os9err setCipb( CInfoPBRec* cipbP, FSSpec *fspP )
/* set cipb from FSSpec */
{
    OSErr  oserr;
    Str255 fName;
    
    /* get info record for the FSSpec itself */
    memcpy( fName,fspP->name, 32 );
    
    cipbP->hFileInfo.ioCompletion= 0L;
    cipbP->hFileInfo.ioNamePtr   = fName;
    cipbP->hFileInfo.ioVRefNum   = fspP->vRefNum;
    cipbP->hFileInfo.ioFDirIndex = 0;   /* use the dir & vRefNum */
    cipbP->hFileInfo.ioDirID     = fspP->parID;
    oserr= PBSetCatInfoSync(cipbP);
    
    return host2os9err(oserr,E_PNNF);
} /* setCipb */



os9err Resolved_FSSpec( short volID, long dirID, char* pathname, 
                        Boolean *isFolder, FSSpec *fsP, FSSpec *afsP )
/* Get alias resolved FSSpec */
{
    os9err  err;
    OSErr   oserr;
    Str255  pasmac;
    char    cstr[OS9PATHLEN];
    char    rstr[OS9PATHLEN];
    char    tmp [OS9PATHLEN];
    char*   p;
    Boolean wasAliased;
    int     ii;

    if (pathname==NULL) strcpy( cstr,""       );
    else                strcpy( cstr,pathname ); /* make a local copy */
                        strcpy( rstr,""       );

    debugprintf(dbgFiles,dbgNorm,("# Resolved_FSSpec(Vol=%d, Dir=%ld, Name='%s')\n",
                                     volID,dirID, cstr ));

    /* Get FSSpec for requested file */
    while (true) {
        strcpy( pasmac,cstr );
        c2pstr( pasmac );
        oserr= FSMakeFSSpec( volID,dirID, pasmac, fsP );
        
        if    (!oserr) {
                memcpy( afsP,fsP, sizeof(*fsP) );
                oserr=  ResolveAliasFile( fsP,true, isFolder,&wasAliased );
            if (oserr ||             /* an error is always bad */
                rstr[0]==NUL) break; /* no sub path to reconnect */
             
            if (!*isFolder) {
                err= RBF_Rsc( fsP ); if (err) return err;
                break; /* file with sub path inpossible except if RBF image */
            }
            
            volID= fsP->vRefNum; /* volume can change also !! */
            dirID= fsP->parID;
            
            strcpy( cstr,""  );
            p=      rstr;
            
            if (*isFolder) {
                if (dirID==1) { /* root dir does not start with ":" on Mac */
                     while ( *p==':') p++; /* no up paths allowed for root */
                }
                else strcpy( cstr,":"  );
            }
            
            p2cstr     ( fsP->name ); /* used in C notation */
            strcat( cstr,fsP->name);
            strcat( cstr,":" );
            strcat( cstr,p   );
            strcpy( rstr,""  ); /* don't use it again */
        }
        else {
            if (oserr!=dirNFErr) break;

            if (strcmp( cstr,"::" )==0) {
                strcpy( cstr,":"  );
            }
            else {
                       ii= strlen(cstr)-2;
                while (ii>0) { /* cut the last piece of path */
                    if (cstr[ii]==':') {
                        strcpy( tmp,   rstr );
                        strcpy( rstr, &cstr[ii+1] );
                        strcat( rstr,  tmp );
                        cstr[ii+1]= NUL; 
                        break; 
                    }
                    ii--;
                } /* while (ii>0) */
            }
        }
    } /* while true */
    
    debugprintf(dbgFiles,dbgNorm,("# Resolved_FSSpec(Vol=%d, Dir=%ld, Name='%s') returns oserr=%d\n",
                                     volID,dirID, cstr, oserr));
    return host2os9err(oserr,E_PNNF);
} /* Resolved_FSSpec */


os9err getFSSpec( ushort pid, char* pathname, defdir_typ defdir, FSSpec *fsP )
/* make FSSpec from mac path / exe flag */
{
    short   volID;
    long    dirID;
    Boolean isFolder;
    FSSpec  afs;
    
    switch (defdir) {
        case _start: volID=         startVolID; dirID=         startDirID; break;
        case _appl : volID=          applVolID; dirID=          applDirID; break;
        case _root : volID= procs[ 1 ].d.volID; dirID= procs[ 1 ].d.dirID; break;
        case _rootX: volID= procs[ 1 ].x.volID; dirID= procs[ 1 ].x.dirID; break;
        case _data : volID= procs[pid].d.volID; dirID= procs[pid].d.dirID; break;
        case _exe  : volID= procs[pid].x.volID; dirID= procs[pid].x.dirID; break;
        case _mdir : volID=         mdir.volID; dirID=         mdir.dirID; break;
        default :    return os9error(E_FNA);
    }

    return Resolved_FSSpec( volID,dirID, pathname, &isFolder,fsP,&afs );    
} /* getFSSpec */



os9err get_dirid( short *volID_P, long *dirID_P, char* pathname)
/* get ID of directory */
{
    FSSpec     fs;
    os9err     err;
    Boolean    isFolder;
    CInfoPBRec cipb;    
    FSSpec     afs;
    
    err= Resolved_FSSpec( *volID_P,*dirID_P, pathname==NULL ? NULL : pathname, &isFolder, &fs,&afs);
    debugprintf(dbgFiles,dbgDetail,("# get_dirid: MakeFSSpec(Vol=%d, Dir=%ld, Name='%s') returns err=%d\n",
                                       *volID_P,*dirID_P,
                                       pathname==NULL ? "<NULL>" : pathname, err) );
    if (err) return err;
    
    /* now get Dir ID of this file, not its parent */
    err= getCipb( &cipb, &fs ); if (err) return err;
    if   (!(cipb.hFileInfo.ioFlAttrib & ioDirMask)) return os9error(E_FNA); /* it's a file, return error */ 

    /* it is a directory, return it's ID */
    *volID_P= fs.vRefNum;
    *dirID_P= cipb.dirInfo.ioDrDirID; /* copy the sucker */
    debugprintf(dbgFiles,dbgDetail,("# get_dirid: VolId=%d, DirId=%ld\n",fs.vRefNum,fs.parID));
    return 0;
} /* get_dirid */


/* eof */

