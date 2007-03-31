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
/* (c) 1993-2007 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        bfo@synthesis.ch                    */
/**********************************************/


/* "macfiles.c" */
/* specific mac file routines which are not available */
/* on other systems */

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
 *    Revision 1.5  2007/03/24 12:38:39  bfo
 *    CVS statistics added
 *
 *
 *
 */


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
    
    union {
        CInfoPBRec    cipb;
        HParamBlockRec hpb;
    } pbu;
    Str255 fName;

    Boolean resolveAlias= true;
    
    // No alias resolver for MacOS <= 9.0.4
  //#define NO_ALIAS_RESOLVER
    #ifdef  NO_ALIAS_RESOLVER
      resolveAlias= false;
    #endif

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

        /* speciality for "netatalk": activate the directory */
        if (oserr==fnfErr && fsP->name[0]!=0 
                          && fsP->vRefNum!=0 
                          && fsP->parID  !=0) {
            ii= 0;
            do {
                fName[0]= 0;
                pbu.cipb.hFileInfo.ioCompletion = 0;
                pbu.cipb.hFileInfo.ioNamePtr    = fName;
                pbu.cipb.hFileInfo.ioVRefNum    = fsP->vRefNum;
                pbu.cipb.hFileInfo.ioDirID      = fsP->parID;
                pbu.cipb.hFileInfo.ioFDirIndex  = ii++;
                oserr = PBGetCatInfoSync(&pbu.cipb);
            } while (!oserr);
            if (ii>1 && oserr==fnfErr) /* and try it again */
                        oserr= FSMakeFSSpec( volID,dirID, pasmac, fsP );
        } /* if */

        
 /*     strcpy( tmp,cstr );
        while ((oserr==fnfErr ||
                oserr==nsvErr) && strstr( tmp,":" )!=NULL ) {
            upe_printf( "==> '%s'\n", tmp );
 
            ii= 0;
            do {
                fName[0]= 0;
                pbu.cipb.hFileInfo.ioCompletion = 0;
                pbu.cipb.hFileInfo.ioNamePtr    = fName;
                pbu.cipb.hFileInfo.ioVRefNum    = fsP->vRefNum;
                pbu.cipb.hFileInfo.ioDirID      = fsP->parID;
                pbu.cipb.hFileInfo.ioFDirIndex  = ii++;
                oserr = PBGetCatInfoSync(&pbu.cipb);
                p2cstr( fName );
                upe_printf( "yyy '%s' %d %d %d\n", fName, 
                            pbu.cipb.hFileInfo.ioFlAttrib & ioDirMask,
                            pbu.cipb.dirInfo.ioDrDirID,
                            pbu.cipb.hFileInfo.ioDirID );
                           
            } while (!oserr);
            if (ii>1 && oserr==fnfErr) oserr= 0;
            
            for (ii=strlen(tmp); ii>0; ii--) {
                if (tmp[ii]==':') { tmp[ii]= NUL; break; }
            }
                    
            strcpy( pastmp,tmp );
            c2pstr( pastmp ); oserr= FSMakeFSSpec( volID,dirID, pastmp, fsP );
            if (!oserr) {
            //  oserr= getCipb( &cipb, fsP );
            
            ii= 0;
            do {
                fName[0]= 0;
                pbu.cipb.hFileInfo.ioCompletion = 0;
                pbu.cipb.hFileInfo.ioNamePtr    = fName;
                pbu.cipb.hFileInfo.ioVRefNum    = fsP->vRefNum;
                pbu.cipb.hFileInfo.ioDirID      = fsP->parID;
                pbu.cipb.hFileInfo.ioFDirIndex  = ii++;
                oserr = PBGetCatInfoSync(&pbu.cipb);
                p2cstr( fName );
                upe_printf( "'%s' %d %d %d\n", fName, 
                            pbu.cipb.hFileInfo.ioFlAttrib & ioDirMask,
                            pbu.cipb.dirInfo.ioDrDirID,
                            pbu.cipb.hFileInfo.ioDirID );
                           
            } while (!oserr);
            if (ii>1 && oserr==fnfErr) oserr= 0;
            
            //  fName[0]= 0;
            //  pbu.hpb.fidParam.ioNamePtr  = fsP->fName;
            //  pbu.hpb.fidParam.ioVRefNum  = fsP->vRefNum;
            //  pbu.hpb.fidParam.ioSrcDirID = fsP->parID;
            //  oserr= PBCreateFileIDRefSync(&pbu.hpb);
            }
            
            if (oserr==0 || 
                oserr==dirNFErr) oserr= FSMakeFSSpec( volID,dirID, pasmac, fsP );
        } // while
*/
        
        if (!oserr) {
            memcpy( afsP,fsP, sizeof(*fsP) );
            
            // No resolver support for MacOS <= 9.0.4
            if (resolveAlias) oserr= ResolveAliasFile( fsP,true, isFolder,&wasAliased );
            else              oserr= fnfErr;
            
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
    
    if (oserr==fnfErr) {
      if (pathname==NULL) strcpy( cstr,""       );
      else                strcpy( cstr,pathname ); /* make a local copy */

      strcpy( pasmac,cstr );
      c2pstr( pasmac );
      oserr= FSMakeFSSpec( volID,dirID, pasmac, fsP );
      
      debugprintf(dbgFiles,dbgNorm,("# Resolved_FSSpec(Vol=%d, Dir=%ld, Name='%s') returns oserr=%d\n",
                                       volID,dirID, cstr, oserr));
    } // if
                                     
    return host2os9err( oserr,E_PNNF );
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
    } // switch

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



os9err check_vod( short* volID_P, long* objID_P, long* dirID_P, char* pathname )
// get back the real <volID> and <objID> and name for Mac file system
{
  os9err    err= E_PNNF;
  direntry* m;
  char*     a;
  char*     b;
  char      v[ 20 ];
  long      fdID= *objID_P;
  
  #ifdef LINKED_HASH
    int   i;
    ulong liCnt= fdID / MAXDIRS;
          fdID = fdID % MAXDIRS;
          
    *volID_P= 0; // don't use it directly for this case
  #endif
    
  if (*volID_P==0 && fdID<MAXDIRS) {   // if the range is correct
    m=    &dirtable[ fdID ];           // get the entry directly
    
    #ifdef LINKED_HASH
      for (i= 0; i<liCnt; i++) {
        if (m) m= m->next;
      } // for
    #endif
    
    if (m && m->ident!=NULL) {
      a=     m->ident;
      b= strstr( a," " ); b++;         // abd divide it into its pieces
        
      memset( v, 0, 20    );
      memcpy( v, a, b-a-1 ); 
        
      *volID_P= -atoi( v );            // convert them back to numers
      *objID_P=  atoi( b );
      *dirID_P=         m->dirid;
      strcpy( pathname, m->fName );                       
     
      err= 0;                          // yes, it's fine
    } // if
  } // if
    
  return err;
} // check_vod



/* eof */

