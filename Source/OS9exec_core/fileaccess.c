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
 *
 */



#include "os9exec_incl.h"

#ifdef win_linux
  #include <utime.h>
#endif


/* Macintosh, PC and Linux File System access */
/* ========================================== */

/* Plain file I/O operations */
/* ------------------------- */

/* --- local procedure definitions for object definition ------------------- */
void   init_File ( fmgr_typ* f );
os9err pFopen    ( ushort pid, syspath_typ*, ushort *modeP,  const char* pathname );
os9err pFclose   ( ushort pid, syspath_typ* );
os9err pFread    ( ushort pid, syspath_typ*, ulong  *n,      char* buffer );
os9err pFreadln  ( ushort pid, syspath_typ*, ulong  *n,      char* buffer );
os9err pFwrite   ( ushort pid, syspath_typ*, ulong  *n,      char* buffer );
os9err pFwriteln ( ushort pid, syspath_typ*, ulong  *n,      char* buffer );
os9err pFseek    ( ushort pid, syspath_typ*, ulong   *posP );
os9err pFdelete  ( ushort pid, syspath_typ*, ushort *modeP,  char* pathname );

os9err pFsize    ( ushort pid, syspath_typ*, ulong  *sizeP );
os9err pFopt     ( ushort pid, syspath_typ*,                 byte* buffer );
os9err pHvolnam  ( ushort pid, syspath_typ*,                 char* volname );
os9err pFpos     ( ushort pid, syspath_typ*, ulong   *posP );
os9err pFeof     ( ushort pid, syspath_typ* );
os9err pFready   ( ushort pid, syspath_typ*, ulong  *n );
os9err pHgetFD   ( ushort pid, syspath_typ*, ulong  *maxbytP,byte* buffer );
os9err pHgetFDInf( ushort pid, syspath_typ*, ulong  *maxbytP, 
                                             ulong  *fdinf,  byte* buffer );

os9err pFsetsz   ( ushort pid, syspath_typ*, ulong  *sizeP );
os9err pHsetFD   ( ushort pid, syspath_typ*,                 byte* buffer );



void   init_Dir  ( fmgr_typ* f );
os9err pDopen    ( ushort pid, syspath_typ*, ushort *modeP,  const char* pathname );
os9err pDclose   ( ushort pid, syspath_typ* );
os9err pDread    ( ushort pid, syspath_typ*, ulong  *n,      char* buffer );
os9err pDseek    ( ushort pid, syspath_typ*, ulong   *posP );
os9err pDchd     ( ushort pid, syspath_typ*, ushort *modeP,  char* pathname );
os9err pDmakdir  ( ushort pid, syspath_typ*, ushort *modeP,  char* pathname );

os9err pDsize    ( ushort pid, syspath_typ*, ulong  *sizeP );
os9err pDpos     ( ushort pid, syspath_typ*, ulong   *posP );
os9err pDeof     ( ushort pid, syspath_typ* );
os9err pDsetatt  ( ushort pid, syspath_typ*, ulong   *attr );
/* ------------------------------------------------------------------------- */

void init_File( fmgr_typ* f )
/* install all procedures of the transparent file manager for files */
{
    gs_typ* gs= &f->gs;
    ss_typ* ss= &f->ss;
    
    /* main procedures */
    f->open       = (pathopfunc_typ)pFopen;
    f->close      = (pathopfunc_typ)pFclose;
    f->read       = (pathopfunc_typ)pFread;
    f->readln     = (pathopfunc_typ)pFreadln;
    f->write      = (pathopfunc_typ)pFwrite;
    f->writeln    = (pathopfunc_typ)pFwriteln;
    f->seek       = (pathopfunc_typ)pFseek;
    f->delete     = (pathopfunc_typ)pFdelete;
    f->makdir     = (pathopfunc_typ)pDmakdir; /* access to directory will be done via fFile */
    
    /* getstat */
    gs->_SS_Size  = (pathopfunc_typ)pFsize;
    gs->_SS_Opt   = (pathopfunc_typ)pFopt;
    gs->_SS_DevNm = (pathopfunc_typ)pHvolnam;
    gs->_SS_Pos   = (pathopfunc_typ)pFpos;
    gs->_SS_EOF   = (pathopfunc_typ)pFeof;
    gs->_SS_Ready = (pathopfunc_typ)pFready;
    gs->_SS_FD    = (pathopfunc_typ)pHgetFD;
    gs->_SS_FDInf = (pathopfunc_typ)pHgetFDInf;

    /* setstat */
    ss->_SS_Size  = (pathopfunc_typ)pFsetsz;
    ss->_SS_Opt   = (pathopfunc_typ)pNop;    /* ignored */
    ss->_SS_Attr  = (pathopfunc_typ)pNop;
    ss->_SS_FD    = (pathopfunc_typ)pHsetFD;
    ss->_SS_WTrk  = (pathopfunc_typ)pUnimp; /* not used */
} /* init_File */


void init_Dir( fmgr_typ* f )
/* install all procedures of the transparent file manager for directories */
{
    gs_typ* gs= &f->gs;
    ss_typ* ss= &f->ss;
    
    /* main procedures */
    f->open      = (pathopfunc_typ)pDopen;
    f->close     = (pathopfunc_typ)pDclose;
    f->read      = (pathopfunc_typ)pDread;
    f->readln    = (pathopfunc_typ)pDread;   /* the same as read */
    f->write     = (pathopfunc_typ)pBadMode; /* not allowed */
    f->writeln   = (pathopfunc_typ)pBadMode; /* not allowed */
    f->seek      = (pathopfunc_typ)pDseek;
    f->chd       = (pathopfunc_typ)pDchd;
    f->makdir    = (pathopfunc_typ)pDmakdir;
    
    /* getstat */
    gs->_SS_Size = (pathopfunc_typ)pDsize;
    gs->_SS_Opt  = (pathopfunc_typ)pRBFopt;
    gs->_SS_DevNm= (pathopfunc_typ)pHvolnam;
    gs->_SS_Pos  = (pathopfunc_typ)pDpos;
    gs->_SS_EOF  = (pathopfunc_typ)pDeof;
    gs->_SS_Ready= (pathopfunc_typ)pUnimp;      /* not used */
    gs->_SS_FD   = (pathopfunc_typ)pHgetFD;
    gs->_SS_FDInf= (pathopfunc_typ)pHgetFDInf;

    /* setstat */
    ss->_SS_Size = (pathopfunc_typ)pBadMode; /* not allowed */
    ss->_SS_Opt  = (pathopfunc_typ)pNop;         /* ignored */
    ss->_SS_Attr = (pathopfunc_typ)pDsetatt;
    ss->_SS_FD   = (pathopfunc_typ)pHsetFD;
    ss->_SS_Lock = (pathopfunc_typ)pNop;         /* ignored */
    ss->_SS_WTrk = (pathopfunc_typ)pUnimp;      /* not used */
} /* init_Dir */

/* -------------------------------------------------------- */



#ifndef USE_UAEMU
  static void assert( Boolean b )
  {   if (!b) printf( "Assert Error\n" ); 
  }
#endif



/* input from file */
os9err pFread(ushort pid, syspath_typ* spP, ulong *n, char* buffer)
{
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    long cnt, k;
    
    #ifdef MACFILES
      long  effpos;
      OSErr oserr;
    #elif defined(windows32)
      fpos_t tmp_pos;
    #endif
    
    assert( buffer!=NULL );
  
    if     (spP->rawMode) {
        if (spP->rawPos    > STD_SECTSIZE) return E_EOF; /* finished */
        if (spP->rawPos+*n > STD_SECTSIZE) *n= STD_SECTSIZE-spP->rawPos;
        
        memcpy( buffer, spP->rw_sct+spP->rawPos, *n );
        spP->rawPos+= *n;
        return 0;
    }
  
    #ifdef MACFILES
      cnt= *n; if (cnt==0) return 0; /* null read returns w/o error */
      oserr= FSRead(spP->u.disk.u.file.refnum, &cnt, (void*)buffer);
      debugprintf(dbgFiles,dbgDeep,("# pFread: FSRead requested=%ld, returned=%ld, oserr=%d\n",
                                       *n, cnt, oserr));
      if (oserr && oserr!=eofErr) return host2os9err(oserr,E_READ);
      if (cnt==0) return os9error(E_EOF); /* if nothing read, this is EOF */

      /* show read for debug */
      if (debugcheck(dbgFiles,dbgDeep)) {
          effpos=0x12345678; /* um Himmels willen Lukas, was soll denn das ? */
          GetFPos(spP->u.disk.u.file.refnum, &effpos);
          uphe_printf("pFread: read $%lX bytes, ending pos now=$%lX: ",cnt,effpos);
      }
    #else     
          cnt= fread( (void*)buffer, 1,*n, spP->stream );
      if (cnt==0 && feof(spP->stream)) return os9error(E_EOF);

      #ifdef windows32
        /* fflush doesn't work here: it synchronises read/write but jumps to end of file */
        fgetpos( spP->stream, &tmp_pos );   /* save current position */
        fsetpos( spP->stream, &tmp_pos );   /* restore position */
        /* now it is synchronised again */
      #endif
    #endif
  
    /* show read for debug */
    if (debugcheck(dbgFiles,dbgDetail)) {
        uphe_printf("%08X: ", cnt );
        for  (k=0; k<16 && k<cnt; k=k+2) {
                         upe_printf( "%02X" , (byte)buffer[k  ] );
            if (k+1<cnt) upe_printf( "%02X ", (byte)buffer[k+1] );
        }
        upe_printf("...\n");
    }
  
    /* ok, return # of chars read to caller */
    *n= cnt; return 0;
} /* pFread */



/* input line from file */
os9err pFreadln( ushort pid, syspath_typ* spP, ulong *n, char* buffer )
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    long cnt;

    #ifdef MACFILES
      long bytes,chunkbytes, cht, k;
      OSErr oserr;
      #define READCHUNKSZ 200
    #else
      #ifndef macintosh
        byte *p;
        char c;
      #endif
    #endif
      
    #if defined(windows32)
      fpos_t tmp_pos;
    #endif

    
    
    assert( buffer!=NULL );

    #ifdef MACFILES
      /* input not more than one line from a FILE */
      bytes= *n; if (bytes==0) return 0; /* null read returns w/o error */

      /* read in chunks */
      cnt= 0;
      cht= 0; /* new variable */
      while  (bytes>0) {
          if (bytes>READCHUNKSZ) chunkbytes=READCHUNKSZ; else chunkbytes=bytes;
          oserr=FSRead(spP->u.disk.u.file.refnum, &chunkbytes, (void*)buffer);
          if (debugcheck(dbgFiles,dbgDeep)) {
              int k;
              uphe_printf("pFreadLn: chunkread: FSRead returned=%ld, oserr=%d: '",chunkbytes,oserr);
              for (k=0; k<chunkbytes; k++) putc(buffer[k],stderr);
              debugprintf(dbgFiles,dbgDeep,("'\n"));            
          }
          if (oserr!=0 && oserr!=eofErr) return host2os9err(oserr,E_READ);
        
          /* now check what we've got */
          if (chunkbytes==0) break; /* nothing more to read */
        
          /* search for a CR */
          cht+=chunkbytes;              /* total bytes (bfo) */
          for (k=0; ++k<=chunkbytes;) { /* don't forget to read the whole buffer <= (bfo) */
              if (*buffer++==CR) {
                  cnt+= k; /* update total number of bytes actually read */
                  /* now pull back file pointer (if required) */
                  if (chunkbytes>k) {
                      /* seek back */
                          oserr=SetFPos(spP->u.disk.u.file.refnum, fsFromMark, k-chunkbytes);
                      if (oserr) return host2os9err(oserr,E_SEEK);
                  }
                  goto readlnok; /* exit both loops, cnt is updated */
              }
          }
        
          bytes-=k;
          cnt  +=k;
          if (oserr==eofErr) goto readlnok; /* don't try to read more, because this is EOF, even if there's no CR */
      }
    
      if (cnt==0) return os9error(E_EOF);
      /* on exit from while: cnt=# of chars actually read */

    readlnok:
      /* cnt is always >0 here, so no EOF check is needed */
      if (cnt>cht) cnt=cht;  /* this must be adapted also (bfo) */
      
    #else
      /* input not more than one line from a FILE */
      #ifdef macintosh
        if     (*n <2) {
            if (*n!=0) {
                /* single char only */
                cnt= fread( (void*)buffer, 1,1, spP->u.disk.u.file.stream );
            }
            else return 0; /* nothing to read at all */
        }
        /* more than 1 char */
        if (fgets((void *)buffer, *n-1, spP->u.disk.u.file.stream)==NULL) { /* read all but one char */
            if (feof(spP->u.disk.u.file.stream)) return os9error(E_EOF);
            return c2os9err(errno,E_READ);
        }
        
            cnt=strlen(buffer); /* find size of string read */
        if (cnt==*n-1) {
            /* all read */
            if (buffer[cnt-1]!=CR) {
                /* not stopped by a CR, read one more, if possible */
                cnt+= fread( (void*)&buffer[cnt], 1,1, spP->u.disk.u.file.stream );
            }
        }
        if ((cnt==0) && feof(spP->u.disk.u.file.stream)) return os9error(E_EOF);

      #else
        /* we cannot rely on fgets() to stop on 0x0D on non-Mac */
        /* single char for now */
        p=buffer;
        cnt=0;
        while(cnt<*n) {
            if ((c=fgetc(spP->stream))==EOF) {
                *n= cnt;
                return cnt==0 ? os9error(E_EOF) : 0; /* return EOF only if on first char */
            }
            *p++=c; /* save in the buffer */        
            cnt++;
            if (c==CR) break; /* abort on CR */
        }
    
        #ifdef windows32
          /* fflush doesn't work here: it synchronises read/write but jumps to end of file */
          fgetpos( spP->stream,  &tmp_pos );   /* save current position */
          fsetpos( spP->stream,  &tmp_pos );   /* restore position */
          /* now it is synchronised again */
        #endif
      #endif
    #endif
    
    debugprintf( dbgFiles,dbgDetail,("# pFreadLn: requested=%ld, returned=%ld\n", *n, cnt ));
    
    *n= cnt;
    return 0;
} /* pFreadln */



#ifdef MACFILES
/* output to file */
os9err pFwrite(ushort pid, syspath_typ* spP, ulong *n, char* buffer)
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    OSErr oserr;
    long  cnt, effpos, k;


    assert( buffer!=NULL );
    
    /* output to a file */
    if ((cnt=*n)==0) return 0; /* null read returns w/o error */
    
    /* show what we write for debug */
    if (debugcheck(dbgFiles,dbgDeep)) {
        effpos=0x12345678;
        GetFPos(spP->u.disk.u.file.refnum, &effpos);
        uphe_printf("pFwrite: writing $%lX bytes at pos=$%lX: ",cnt,effpos);
        for (k=0; k<8 && k<cnt; k++) {
            upe_printf("$%02X ",(byte) buffer[k]);
        }
        upe_printf("...\n");
    }
    /* actual write */
        oserr=FSWrite(spP->u.disk.u.file.refnum, &cnt, (void*)buffer);
    if (oserr) return host2os9err(oserr,E_WRITE);
    
    debugprintf(dbgFiles,dbgDeep,("# pFwrite: requested=%ld, written=%ld, oserr=%d\n",*n,cnt,oserr));   
    *n= cnt;
    return 0;
} /* pFwrite */

#else
/* output to file */
os9err pFwrite(ushort pid, syspath_typ* spP, ulong *n, char* buffer)
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    long  cnt;


    assert( buffer!=NULL );
    
    /* output to a FILE */
    cnt= fwrite( (void*)buffer, 1,*n, spP->stream );
    fflush( spP->stream ); /* don't forget this */

    debugprintf(dbgFiles,dbgDeep,("# pFwrite: requested=%ld, written=%ld, ferror=%d, errno=%d\n",*n,cnt,ferror(spP->stream),errno));
    if (cnt<0) return c2os9err(errno,E_WRITE); /* default: general write error */
    *n= cnt;
    return 0;
} /* pFwrite */
#endif


#ifdef MACFILES
/* output to file */
os9err pFwriteln(ushort pid, syspath_typ* spP, ulong *n, char* buffer)
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    long cnt,effpos,k;
    OSErr oserr;


    assert( buffer!=NULL );

    /* output line to a file */
    cnt= *n; if (cnt==0) return 0; /* null read returns w/o error */

    /* limit number of bytes to one line, max */
    for (cnt=0; cnt<*n; cnt++) if (buffer[cnt]==CR) { ++cnt; break; }

    /* show what we write for debug */
    if (debugcheck(dbgFiles,dbgDeep)) {
        effpos=0x12345678;
        GetFPos(spP->u.disk.u.file.refnum, &effpos);
        uphe_printf("pFwriteln: writing $%lX bytes at pos=$%lX: ",cnt,effpos);
        for (k=0; k<8 && k<cnt; k++) {
            upe_printf("$%02X ",(byte) buffer[k]);
        }
        upe_printf("...\n");
    }
    /* actual write */
        oserr=FSWrite(spP->u.disk.u.file.refnum, &cnt, (void*)buffer);
    if (oserr) return host2os9err(oserr,E_WRITE);
    
    debugprintf(dbgFiles,dbgDeep,("# pFwrite: requested=%ld, written=%ld, oserr=%d\n",*n,cnt,oserr));   
    *n= cnt; return 0;
} /* pFwriteln */

#else
os9err pFwriteln(ushort pid, syspath_typ* spP, ulong *n, char* buffer)
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    long cnt,ii;


    assert( buffer!=NULL );
    
    /* output line to a FILE */
    for (ii=0; ii<*n; ii++) if (buffer[ii]==CR) { ++ii; break; }
    cnt=fwrite((void *)buffer, 1,ii, spP->stream);
    fflush( spP->stream ); /* don't forget this */
    
    if (cnt<0) return c2os9err(errno,E_WRITE); /* default: general write error */
    *n= cnt; return 0;
} /* pFwriteln */
#endif



os9err pFopt( ushort pid, syspath_typ* spP, byte *buffer )
{
    os9err err= pRBFopt( pid,spP, buffer );
    
    #ifdef macintosh
      FSSpec*    spc= &spP->u.disk.spec;
      CInfoPBRec cipb;
    
      short  volid;
      long   objid;
      ulong  fd;
      ulong* l;
      
      err= getCipb( &cipb,spc );  
    
            volid= spc->vRefNum; /* get volume ID */
      fd=  (volid & VOLIDMASK) << IDSHIFT;
            objid= cipb.hFileInfo.ioDirID;
      fd |= objid & IDMASK;

      l= &buffer[ PD_FD ]; *l= os9_long(fd)<<BpB; /* LSN of file */
    #endif
    
    return err;
} /* pFopt */



/* check ready */
os9err pFready( ushort pid, syspath_typ* spP, ulong *n )
{
    #ifndef linux
    #pragma unused(pid,spP)
    #endif

    *n= 1; return 0;
} /* pFready */


/* get device name from HFS object */
os9err pHvolnam(ushort pid, syspath_typ* spP, char* volname)
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    
    #ifdef macintosh
      long  free= 0;
      short volid= spP->u.disk.spec.vRefNum;
      
      #ifdef USE_CARBON
        strcpy( volname,"" );
      #else
        OSErr oserr= GetVInfo(0, volname, &volid, &free);
        if   (oserr) return host2os9err(oserr,E_BPNUM);
        p2cstr( volname );
      #endif
      
      debugprintf(dbgFiles,dbgDetail,("# pFvolnam: name='%s', free bytes=%ld\n",volname,free));

    #elif defined(windows32)
      volname[0]= toupper( spP->fullName[0] ); /* first char only */       
      volname[1]= NUL;     
    
    #elif defined linux
      strcpy( volname,"" ); /* none for Linux, top directory structure is different */
    
    #else
      return E_UNKSVC;
    #endif
    
    return 0;
} /* pHvolnam*/



#ifdef win_linux
  static void Set_FileDate( syspath_typ* spP, time_t t )
  /* Set (Windows/Linux) file date */
  {
      #ifdef windows32
        Boolean    isFolder= false;
        int        hh;
        HANDLE     hFile;
        LPWIN32_FIND_DATA data;
        FILETIME   cr, lastA, lastW, lastL;
        SYSTEMTIME sy;
        struct tm  tim;

        /* open as file or as current directory */
            hh= _lopen( spP->fullName, OF_READWRITE );
        if (hh!=-1) hFile= (HANDLE) hh;
        else {
            hFile= NULL;
            if (PathFound(spP->fullName)) {
                    isFolder= true;
                    hFile= FindFirstFile( spP->fullName, &data);
                if (hFile!=NULL) {
                    FindNextFile( hFile, &data);
                }
            }
        }
        if (hFile==NULL) return;
        
        GetFileTime( hFile, &cr, &lastA, &lastW );
        FileTimeToLocalFileTime( &lastW, &lastL );
        FileTimeToSystemTime   ( &lastL, &sy );
        
        if (t==0) GetTim  ( &tim );
        else      TConv( t, &tim );
        
        sy.wYear  = tim.tm_year+1900;
        sy.wMonth = tim.tm_mon+1;
        sy.wDay   = tim.tm_mday;
        sy.wHour  = tim.tm_hour;
        sy.wMinute= tim.tm_min;
        sy.wSecond= tim.tm_sec;
        
        SystemTimeToFileTime   ( &sy,    &lastL );
        LocalFileTimeToFileTime( &lastL, &lastW );
        SetFileTime( hFile, &cr, &lastA, &lastW );

        if (isFolder) FindClose( hFile );
        else       _lclose( (int)hFile );

      #else
        struct utimbuf buf;
      
        buf.actime = t;
        buf.modtime= t;
        utime( spP->fullName, &buf );
      #endif
  } /* Set_FileDate */
#endif



static os9err touchfile( ushort pid, syspath_typ* spP )
{
    struct tm tim; /* Important Note: internal use of <tm> as done in OS-9 */
    byte      fdbeg[16]; /* buffer for preparing FD */
    ulong     maxbyte = 16;
    int       k;
    
    for (k=0; k<16; k++) fdbeg[ k]= 0; /* initialize it */
    
    GetTim ( &tim );
    fdbeg[3]= tim.tm_year;
    fdbeg[4]= tim.tm_mon+1; /* somewhat different month notation */
    fdbeg[5]= tim.tm_mday;
    fdbeg[6]= tim.tm_hour;
    fdbeg[7]= tim.tm_min;
    
    return pHsetFD( pid,spP, fdbeg );
} /* touchfile */



#ifdef MACFILES
  typedef struct {
      char*   p;
      OSType* creator;
      OSType* type;
  } VType;
  

  static Boolean TCSuff( VType* v, char* suffix, OSType creator, OSType type )
  {
	  if (ustrcmp( suffix,v->p )==0 ||
	  	  ustrcmp( suffix,""   )==0) {
	  	  *v->creator= creator;
	      *v->type   = type; return true;
	  }
	            
  	  return false;
  } /* TCSuff */


  static Boolean TCCWIE( VType* v, char* suffix )
  {   return TCSuff( v,suffix, 'CWIE','TEXT' ); /* CodeWarrior text */
  } /* TCCWIE */
  
  static Boolean TCRBFi( VType* v, char* suffix )
  {   return TCSuff( v,suffix, 'os9a','RBFi' ); /* CodeWarrior text */
  } /* TCRBFi */


  static void Get_Creator_And_Type( FSSpec* spc, OSType *creator, OSType *type )
  {

	  VType   v;
 	  char*   q;
	  char    fName[32];
      memcpy( fName, spc->name,32 );
	  p2cstr( fName );

	  v.creator= creator; /* basic assignment */
	  v.type   = type;
	  
	  /* get the suffix */
	  v.p= &fName;
	  while (true) {
	  	  q  = strstr( v.p,"." ); if (q==NULL) break;
	  	  v.p= q+1;
	  } /* loop */

	  if (v.p==&fName) { /* no suffix */
	      if (TCSuff( &v, "Read_Me", 'MPS ','TEXT' )) return;
	  }
	  else {
	  	      v.p--; /* making e.g. ".c" */
	  	  if (v.p==&fName) { /* starting with "." */
	          TCSuff( &v, "",        'os9a','PROG' ); return; // ".*"
	      }
	                           
	      if (TCCWIE( &v, ".c" ))                     return; // C source
	      if (TCCWIE( &v, ".h" ))                     return; // include files
	      if (TCCWIE( &v, ".p" ))                     return; // Pascal source
	      if (TCCWIE( &v, ".m" ))                     return; // Pascal module source
	      if (TCCWIE( &v, ".f" ))                     return; // Pascal include files
	      if (TCCWIE( &v, ".a" ))                     return; // Assembler
	      if (TCCWIE( &v, ".d" ))                     return; // Assembler include files
	      if (TCSuff( &v, ".r",      'CWIE','MPLF' )) return; 
	      if (TCSuff( &v, ".x",      'CWIE','MPLF' )) return;
	      if (TCSuff( &v, ".tm",     'CWIE','MPLF' )) return;

								  /* creator type */
	      if (TCSuff( &v, ".exe",    'CWIE','DEXE' )) return; // executables

	      if (TCSuff( &v, ".pic",    '8BIM','PICT' )) return; // PICT
	      if (TCSuff( &v, ".psd",    '8BIM','8BPS' )) return; // Photoshop
	      if (TCSuff( &v, ".jpg",    '8BIM','JPEG' )) return; // JPEG
	      if (TCSuff( &v, ".tif",    '8BIM','TIFF' )) return; // TIFF
	      if (TCSuff( &v, ".gif",    '8BIM','GIFf' )) return; // GIF
	      if (TCSuff( &v, ".pdf",    'CARO','PDF ' )) return; // PDF
	      if (TCSuff( &v, ".rm",     'PNst','PNRM' )) return; // RealMovie

	      if (TCSuff( &v, ".lzh",    'LARC','LHA ' )) return; // lzh
	      if (TCSuff( &v, ".sit",    'SIT!','SIT!' )) return; // StuffIt
	      if (TCSuff( &v, ".zip",    'ZIP ','ZIP ' )) return; // zip

	      if (TCSuff( &v, ".doc",    'MSWD','W8BN' )) return; // MS Word
	      if (TCSuff( &v, ".xls",    'XCEL','XLS8' )) return; // MS Excel
	      if (TCSuff( &v, ".ppt",    'PPT3','SLD8' )) return; // MS Powerpoint

	      if (TCSuff( &v, ".Me",     'MPS ','TEXT' )) return; // text file
	      if (TCSuff( &v, ".txt",    'ttxt','TEXT' )) return; // text file
      }

      if   ( RBF_Rsc( spc ) && TCRBFi( &v, "" ) )     return; // RBF images
   			  TCSuff( &v, "",        'os9a','PROG' );         // default
  } /* Get_Creator_And_Type */
#endif 


os9err pFopen( ushort pid, syspath_typ* spP, ushort *modeP, const char* pathname )
{
/* open/create file */
    os9err    err;
    char*     pastpath;
    char      hostpath[OS9PATHLEN];
    char      ploc    [OS9PATHLEN];
    Boolean   isW   = IsWrite(*modeP);
    Boolean   exedir= IsExec (*modeP);
    Boolean   cre   = IsCrea (*modeP);
    file_typ* f= &spP->u.disk.u.file;
    char*     p;
    char*     pp;
    char*     vn;
    int       len;
    
    #ifdef MACFILES
      OSErr      oserr;
      CInfoPBRec cipb;
      defdir_typ defdir;
      FSSpec*    spc= &spP->u.disk.spec;
      OSType     creator, type;

    #else
      FILE* stream;
      
      #ifdef win_linux
        char adapted[OS9PATHLEN];
      #endif
    #endif

          strcpy( ploc,pathname );
    if          (*ploc==NUL) return E_BPNAM;
    while (IsRaw( ploc )) {
        spP->rawMode= true;
        len= strlen(ploc); 
        ploc[len-1]= NUL;
    } /* while */
        
    
    pastpath= ploc;
    err     = parsepath( pid, &pastpath,hostpath, exedir ); if (err) return err;
    pp      = hostpath;

    if (spP->rawMode) {        /* rawmode allows only reading of 1st sector */
        spP->rw_sct = get_mem( STD_SECTSIZE,false ); /* for some info procs */
        spP->rawPos = 0;
                     vn= &spP->rw_sct[31];
        VolInfo( pp, vn );    /* this is the correct position */      
        
        #ifdef MACFILES
          if (*vn==NUL) {             defdir= exedir ?_exe:_data;   
              err= getFSSpec( pid,pp, defdir, spc );
              pHvolnam      ( pid,spP, vn );
          }
        #endif

        return 0;
    } /* if IsRaw */

    
    #ifdef MACFILES
  //  if (strlen(loc)>DIRNAMSZ) return os9error(E_BPNAM);
    
      /* Get FSSpec for requested file */
                              defdir= exedir ? _exe:_data;
      err= getFSSpec( pid,pp, defdir, spc );
      debugprintf(dbgFiles,dbgDetail,("# pFopen: getFSSpec returned err=%d\n",err));
      if (err==E_FNA) return err; /* seems to be a file instead of a sub-direcotry */

      /* now see if create or open */
      if (cre) {
          /* --- create */
          debugprintf( dbgFiles,dbgDetail,( "# pFopen:  trying to create '%s'\n",pp ));
          /* check if file exists */
          if (err==0)      return os9error(E_CEF); /* no error on MakeFSSpec -> object already exists */
          if (err!=E_PNNF) return err;
        
          /* file does not exist yet, create it first */
          Get_Creator_And_Type( spc, &creator, &type );
              oserr=FSpCreate ( spc,  creator,  type, smSystemScript);
          if (oserr) return host2os9err(oserr,E_ILLARG);

          /* now open it */     
              oserr=FSpOpenDF( spc, fsRdWrPerm, &f->refnum);
          if (oserr) return host2os9err(oserr,E_PNNF);
      }
      else {
          /* --- open it */
          debugprintf(dbgFiles,dbgDetail,("# pFopen: trying to open '%s', mode=$%04hX\n",
                                             pp,*modeP));
          if (!err) {
              /* object exists, but make sure it is not a directory */
              getCipb( &cipb, spc );/* it's a directory, return error */ 
              if       (cipb.hFileInfo.ioFlAttrib & ioDirMask) return os9error(E_FNA);
          }
                 
              oserr=FSpOpenDF( spc, isW ? fsRdWrPerm:fsRdPerm, &f->refnum);
          if (oserr) return host2os9err(oserr,E_PNNF);

          /* and adapt file date if open for writing */
          if (isW) { err= touchfile( pid, spP ); if (err) return err; }
      }
      debugprintf(dbgFiles,dbgDetail,("# pFopen: open/create successful, refnum=%d\n",
                                         f->refnum));

    #else
      debugprintf(dbgFiles,dbgNorm,("# pFopen: trying to %s '%s', mode=$%04hX\n",
                                     cre ? "create":"open", pp,*modeP));

      err= AdjustPath( pp,adapted, cre ); if (err) return err;
      pp=                 adapted; 

      if (cre) {
          /* --- create */
          /* check if file exists */
          if (FileFound( pp )) return os9error(E_CEF); /* create existing file not allowed */
          #ifdef windows32
          if (GetLastError()==ERROR_NOT_READY) return os9error(E_NOTRDY);
          #endif

              stream= fopen( pp,"wb+" ); /* create for update, use binary mode (bfo) ! */
          if (stream==NULL) return c2os9err(errno,E_FNA); /* default: file not accessible in this mode */  
      }
      else {
          /* --- open */
          #ifdef win_linux
            /* object exists, but make sure it is not a directory */
            #ifdef windows32
            SetLastError(ERROR_SUCCESS);
            #endif
            if (PathFound( pp )) return os9error(E_FNA);
            #ifdef windows32
            if (GetLastError()==ERROR_NOT_READY) return os9error(E_NOTRDY);
            #endif
          #endif
        
              stream= fopen( pp,"rb" ); /* try to open for read, use binary mode */
          if (stream==NULL) return os9error(E_PNNF); /* file not found */

          if (isW) { /* open (also) for write */
              fclose(stream);  /* close the read-only path again */
                  stream= fopen( pp,"rb+" ); /* open for update, use binary mode */
              if (stream==NULL) {
                  return c2os9err(errno,E_FNA); /* default: file no access in this mode */
              }
          }
      }
      
      spP->stream= stream; /* assign stream */
    
      #ifdef win_linux
                spP->dDsc= NULL; /* this is not a directory */
        strcpy( spP->fullName, pp );
      #endif
    #endif
    
    p= (char*)ploc+strlen(ploc)-1; /* only the filename itself, no path name */
    while (p>=ploc && *p!=PSEP) p--;
    p++;
    
    strcpy( spP->name,p );
    
    f->moddate_changed= false;
    #ifdef windows32
      if (isW) {
          f->moddate        = 0;
          f->moddate_changed= true;
      }
    #endif
    
    return 0;
} /* pFopen */



/* close a file */
os9err pFclose(ushort pid, syspath_typ* spP )
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    #ifdef MACFILES
      OSErr      oserr;
      FSSpec*    spc= &spP->u.disk.spec;
      CInfoPBRec cipb;
    #endif 
      
    file_typ* f= &spP->u.disk.u.file;
      
    if (spP->rawMode) {
        release_mem( spP->rw_sct, false ); 
        spP->rw_sct= NULL;
        spP->rawMode= false;
        return 0;
    }
    
    #ifdef MACFILES
            oserr=FSClose( f->refnum );
      if   (oserr) return host2os9err(oserr,E_WRITE);
      
      if (f->moddate_changed) {
              oserr= getCipb( &cipb, spc );
          if (oserr) return host2os9err(oserr,E_WRITE);
      
          cipb.hFileInfo.ioFlMdDat= (ulong)f->moddate-OFFS_1904;
      
              oserr= setCipb( &cipb, spc ); /* and write it back */
          if (oserr) return host2os9err(oserr,E_WRITE);
      } /* if */

            oserr=FlushVol( NULL, spc->vRefNum );
      if   (oserr) return host2os9err(oserr,E_UNIT);
      
    #else
      if (fclose(spP->stream)<0) return c2os9err(errno,E_WRITE);    
      if (f->moddate_changed) Set_FileDate( spP, f->moddate );
    #endif
    
    return 0;
} /* pFclose */


os9err pFseek( ushort pid, syspath_typ* spP, ulong *posP )
/* seek within a file */
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    #ifdef MACFILES
      OSErr oserr;
      ulong effpos;
    #else
      int   fildes;
    #endif


    if (spP->rawMode) {
        if (*posP>STD_SECTSIZE) return E_SEEK;
        spP->rawPos= *posP;
        return 0;
    }

    #ifdef MACFILES
      /* try to set position */
      oserr= SetFPos(spP->u.disk.u.file.refnum, fsFromStart, (long) *posP);
      debugprintf(dbgFiles,dbgDetail,("# pFseek: tried to seek to $%08lX, oserr=%d\n",*posP,oserr));
      if (oserr==eofErr) {
          /* tried to seek beyond currend EOF -> try to extend file */
          oserr=SetEOF(spP->u.disk.u.file.refnum, (long) *posP);
          debugprintf(dbgFiles,dbgDetail,("# pFseek: Tried to extend file to size=$%08lX, oserr=%d\n",*posP,oserr));
          if (oserr==wrPermErr) {
              debugprintf(dbgFiles,dbgDetail,("# pFseek: File not writable, oserr=wrPermErr\n"));
              oserr=0; /* file not open for write, can't extend, simply ignore */
          }
          if (!oserr) {
              /* now as file is extended, seek to new LEOF */
              oserr=SetFPos(spP->u.disk.u.file.refnum, fsFromLEOF, 0);
              debugprintf(dbgFiles,dbgDeep,("# pFseek: Extended file to size $%08lX (setting pos -> oserr=%d\n",*posP,oserr));
          }
      }
      
      /* show result of seek */
      if (debugcheck(dbgFiles,dbgDeep)) {
          effpos=0x12345678;
          GetFPos(spP->u.disk.u.file.refnum, &effpos);
          uphe_printf("pFseek: Actual pos (GetFPos)=$%lX\n",effpos);
          if (effpos!=*posP) {
              uphe_printf("pFseek: FATAL: seek did not reach desired position ($%lX)\n",*posP);
              debugwait();
          }
          effpos=0x12345678;
          GetEOF(spP->u.disk.u.file.refnum, &effpos);
          uphe_printf("pFseek: Actual EOF (GetEOF)=$%lX\n",effpos);
      }
      /* now seek should be done or failed definitely */
      return host2os9err(oserr,E_SEEK);

    #else
      if (fseek( spP->stream, (long)*posP, SEEK_SET )==0) return 0;

      debugprintf(dbgFiles,dbgDetail,("# pFseek: tried to seek to $%08lX, got errno=%d\n",*posP,errno));

      #if !defined(__MWERKS__) && !defined(linux)
      if (errno==ESPIPE || errno==ENXIO)
      #else
      if (errno!=0) /* %%% rude version, if seek fails, assume that we must enlarge the file */
      #endif
      {
          /* try extending file */
          debugprintf(dbgFiles,dbgDetail,("# pFseek: Trying to extend file to size=$%08lX\n",*posP));
          fflush(spP->stream); /* unbuffer everything */
          fildes= fileno( spP->stream );

          #ifdef macintosh
            return c2os9err(errno,E_SEEK); /* %%% FIOSETEOF is not available in MSL on Mac: use MACFILES-Version normally !! */
            /* if (ioctl(fildes, FIOSETEOF, (long *) (*sizeP))<0) return c2os9err(errno,E_SEEK); */
          #else
            return c2os9err(errno,E_SEEK); /* %%% FIOSETEOF is not available in MSL !! */
          #endif

          if (fseek(spP->stream, (long int) *posP, SEEK_SET)!=0) return c2os9err(errno,E_SEEK);
      } 
      else return c2os9err(errno,E_SEEK);
    
      fflush( spP->stream );  
      return 0;
    #endif
} /* pFseek */



os9err pFdelete( ushort pid, syspath_typ* none, ushort *modeP, char* pathname )
{
    #ifndef linux
    #pragma unused(none)
    #endif

    os9err  err;    
    OSErr   oserr= 0;
    char*   pastpath;
    char    pp[OS9PATHLEN];
    Boolean exedir= IsExec(*modeP);
    
    #ifdef MACFILES
      FSSpec     delSpec;
      CInfoPBRec cipb;
      Boolean    isDir;
      int        kk;
    #endif
    
    #ifdef windows32
      char cmd[OS9PATHLEN];
    #endif
        
    #ifdef win_linux
      char adapted[OS9PATHLEN];
    #endif
    

    pastpath= pathname;
    err     = parsepath( pid, &pastpath,pp, exedir ); if (err) return err;  
    pathname= pp;

    debugprintf( dbgFiles,dbgNorm,( "# I$Delete: pathname=%s, err=%d\n", pathname,err ));
    if (err) return err;

    #ifdef MACFILES
          err= getFSSpec( pid,pathname, exedir ?_exe:_data, &delSpec );
      if (err) return err;
                  
      getCipb( &cipb,&delSpec);
      isDir=  ( cipb.hFileInfo.ioFlAttrib & ioDirMask ); if (isDir) return E_FNA;

      /* unfortunately the Mac system needs some time to be ready   */
      /* for deleting an empty directory => make some timeout loops */
      for (kk=0; kk<MAXTRIES; kk++) {
               oserr= FSpDelete( &delSpec );
          if (!oserr) break;
          HandleEvent();
      }
    
    #elif defined win_linux
      err     = AdjustPath( pathname,adapted, false ); if (err) return err;
      pathname= adapted;
      
      #ifdef windows32
//      if (!DeleteFile( adapted )) oserr= GetLastError();
        sprintf( cmd, "del %s", pathname );
        err= call_hostcmd( cmd, pid, 0,NULL ); if (err) return err;

      #else
        oserr= remove( pathname );
      #endif  

    #else
      #error I_Delete not yet implemented
    #endif
    
    return host2os9err(oserr,E_SHARE);  
} /* pFdelete */



/* get file position */
os9err pFpos( ushort pid, syspath_typ* spP, ulong *posP )
{   
    #ifndef linux
    #pragma unused(pid)
    #endif


    #ifdef MACFILES
      return host2os9err( GetFPos(spP->u.disk.u.file.refnum, (long *)posP),E_SEEK );

    #else
      *posP= (ulong) ftell( spP->stream );
      return 0;
    #endif
} /* pFpos */



/* get file size */
os9err pFsize( ushort pid, syspath_typ* spP, ulong* sizeP )
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    os9err err= 0;
    
    #ifdef win_linux
      int    fd= fileno( spP->stream );
      struct stat info;
    #endif

    
    #ifdef MACFILES
      OSErr oserr= GetEOF( spP->u.disk.u.file.refnum, (long*)sizeP );
      err= host2os9err( oserr,E_SEEK );

    #elif defined win_linux
          err= fstat( fd,&info );
      if (err) return E_SEEK;
      *sizeP= info.st_size;
      
    #else
      fpos_t tmp_pos;
     
      fgetpos( spP->stream,  &tmp_pos );   /* save current position */
      fseek  ( spP->stream,0,SEEK_END );   /* go to EOF */
      *sizeP= (ulong)ftell( spP->stream ); /* get position now = file size */
      fsetpos( spP->stream,  &tmp_pos );   /* restore position */
    #endif
      
    return err;
} /* pFsize */



/* set file size */
os9err pFsetsz(ushort pid, syspath_typ* spP, ulong *sizeP )
{
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    os9err err= 0;

    #ifdef windows32
      #define N_Tries 100
      int     ii, n, cnt;
      char    name[OS9NAMELEN];
      FILE*   tmp;
      byte    a;
      Boolean doRead;
      
    #elif defined linux
      int fd;
    #endif
    

    
    #ifdef MACFILES
      OSErr oserr= SetEOF( spP->u.disk.u.file.refnum, (long)*sizeP );
      err= host2os9err( oserr,E_SEEK );

    #else
      fpos_t tmp_pos;
      
      fgetpos( spP->stream, &tmp_pos );  /* save current position */
      
      #ifdef windows32
        /* create a temporary file and copy until length */
        ii= 1;
        for (ii=0; ii<N_Tries; ii++) { /* try some tmp files */
            sprintf( name,".tmp.%d", ii++ );
            
                tmp= fopen( name,"wb+" );
            if (tmp!=NULL) break;
        }
        if (tmp==NULL) return E_PNNF;
        
                               n= 0;
        fsetpos( spP->stream, &n );
        
        doRead= true;
        for (ii=0; ii<*sizeP; ii++) { /* copy the file content */
            if (doRead) { /* fill with NUL, if longer */
                    cnt= fread( &a, 1,1, spP->stream );
                if (cnt==0) { a= NUL; doRead= false; }
            } /* if */
            
            cnt= fwrite( &a, 1,1, tmp );
        } /* for */

        
        fclose( spP->stream ); /* close them both */
        fclose( tmp );
    
        /* and change to original name */
        err= remove(        spP->fullName ); if (err) return E_PNNF;
        err= rename( name,  spP->fullName ); if (err) return E_PNNF;
        spP->stream= fopen( spP->fullName, "rb+" );

      #elif defined linux
        fflush     ( spP->stream );        /* unbuffer everything */
        fd = fileno( spP->stream );  /* <fd> used for "ftruncate" */
        err= ftruncate( fd, *sizeP ); /* cut the file at <*sizeP> */
      #endif
      
      if     (tmp_pos!=*sizeP) {
          if (tmp_pos >*sizeP)  tmp_pos= *sizeP;
          fsetpos(spP->stream, &tmp_pos); /* restore position */
      }
    #endif
    
    return err;
} /* pFsetsz */
 



os9err pFeof( ushort pid, syspath_typ* spP )
/* check for EOF */
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    #ifdef MACFILES
      long  curpos,curend;
      OSErr oserr;
    
          oserr= GetEOF (spP->u.disk.u.file.refnum, &curend);
      if (oserr) return host2os9err(oserr,E_SEEK);

          oserr= GetFPos(spP->u.disk.u.file.refnum, &curpos);
      if (oserr) return host2os9err(oserr,E_SEEK);
    
      if (curpos>curend)     return os9error(E_EOF);

    #else   
      if (feof(spP->stream)) return os9error(E_EOF);
    #endif

    return 0;
} /* pFeof */


// attempt to improve things a little
#define NEW_LUZ_FD_IMPL 1

/* prepare a FD from a cipb */
static void getFD( void* fdl, ushort maxbyt, byte *buffer )
{
    #define   FDS 16
    byte      fdbeg[FDS];                  /* buffer for preparing FD */
    byte*     att     = (byte*) &fdbeg[0]; /* the position of the attr field */
    ulong*    sizeP   = (ulong*)&fdbeg[9]; /* the position of the size field */
    Boolean   isFolder= false;
    Boolean   uDir    = false;
    ulong     u       = 0;
    struct tm tim;
        
    #ifdef macintosh
      CInfoPBRec* cipbP= (CInfoPBRec*)fdl;

    #elif defined win_linux
      char*       pathname;
      struct stat info;
      syspath_typ spRec;
      Boolean     ok;
      
      #ifdef windows32
        HANDLE hFile;
        SYSTEMTIME sy;
        // This was the bad thing: declared pointer instead of data structure !
        // LPWIN32_FIND_DATA data;
        WIN32_FIND_DATA data;
        FILETIME cr, lastA, lastW, lastL;        
          mode_t v;
      #else
        __mode_t v;
      #endif
      
    #else
      #pragma unused(fdl)
    #endif

    /* fill up with 0 as far as needed, used as default */
    memset( buffer,0, maxbyt );
    memset( fdbeg, 0, FDS    );
    
    #if defined NEW_LUZ_FD_IMPL && defined windows32
      #pragma unused(v,lastA,tim)
      // use FindFirstFile for everything
      pathname= (char*)fdl;
      
          ok= (pathname!=NULL && ustrcmp( pathname,"" )!=0);
      if (ok) {
          hFile= FindFirstFile( pathname, &data );
          ok= ( (int)hFile>0 ); /* is -1, if error -> compare on >= !! */
      }
        
      if (ok) { 
          FindClose( hFile );
          // now extract all the data from WIN32_FIND_DATA structure
          // - is it a folder?
          isFolder= (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0;
          
          // - basic r or rw
          *att=poRead | (poRead<<3); // always readable
          if ((data.dwFileAttributes & FILE_ATTRIBUTE_READONLY)==0)
              *att |= poWrite | poWrite<<3; // if not write protected, also writeable
          // - always executable
          *att |= poExec | (poExec<<3);
          
          // - dirs are special
          if (isFolder) {
              *att |= 0x3F; // all permissions
              *att |= 0x80; // and dir bit
          }
          
          // - owner
          fdbeg[1] =0;
          fdbeg[2] =0; /* owner = superuser */
          
          // - get moification dates
          lastW = data.ftLastWriteTime;  
          // - convert to systemtime
          FileTimeToLocalFileTime ( &lastW, &lastL );
          FileTimeToSystemTime    ( &lastL, &sy );
          fdbeg[3]= sy.wYear-1900;
          fdbeg[4]= sy.wMonth;
          fdbeg[5]= sy.wDay;
          fdbeg[6]= sy.wHour;
          fdbeg[7]= sy.wMinute;

          // link count
          fdbeg[8]= 1; /* link count = 1 */
          
          // creation date
          cr = data.ftCreationTime;
          FileTimeToLocalFileTime ( &cr, &lastL );
          FileTimeToSystemTime    ( &lastL, &sy );
          fdbeg[13]= sy.wYear-1900;
          fdbeg[14]= sy.wMonth;
          fdbeg[15]= sy.wDay;
          
          // size of file
          // NOTE: only lower 32 bits are used. NT returns 64bits here...
          // NOTE: this is still a little hacky, as we use the info record only for size
          //       by now.
          info.st_size = data.nFileSizeLow; 
      }
      else {
          isFolder= true; /* assuming it is the top directory */

//     // file does not exist (any more)
//        *att= 0; // not accessible
      }
    
//    printf( "fd: " );
//    for (ii=0; ii<FDS; ii++) {
//        printf( "%02X ", fdbeg[ii] );
//    }
//    printf( "- %d '%s'\n", hFile, pathname );
    
    #else
    // conventional ifdef haystack :-)

    /* fill in constants */
    *att= 0x3F; /* default: peprpwerw (exe always set for now, %%% later: check file type!) */         

    #ifdef macintosh
      /* now get info from CInfoPBRec */
      isFolder= cipbP->hFileInfo.ioFlAttrib & ioDirMask; /* is it a folder */
      if (cipbP->hFileInfo.ioFlAttrib & 0x01) *att &= 0xED; /* clear write attrs */
      
    #elif defined win_linux
      pathname= (char*)fdl;
            
      #ifdef windows32
        isFolder= PathFound( pathname ); /* don't call stat_ for directories under Windows */
      #endif
      
      *att= 0x00; 
      if (!isFolder) {
          stat_( pathname, &info );
          v= info.st_mode;
      
          if (v & S_IRUSR)    *att|= poRead;
          if (v & S_IWUSR)    *att|= poWrite;
       /* if (v & S_IXUSR) */ *att|= poExec; /* always */

          if (v & S_IROTH)    *att|= 0x08;
          if (v & S_IWOTH)    *att|= 0x10;
       /* if (v & S_IXOTH) */ *att|= 0x20;   /* always */
      
          #ifdef windows32
            *att|= 0x03; /* workaround because currently not visible */
          #endif
      
          isFolder= IsTrDir(v);
      }
      
      if (isFolder) *att|= 0x3F; /* all attr for directory */
    #endif
    
    if (isFolder) *att|= 0x80; /* set if it is a directory */
    

    fdbeg[1] =0;
    fdbeg[2] =0; /* owner = superuser */

    /* file dates */
    #ifdef macintosh
      u= cipbP->hFileInfo.ioFlMdDat + OFFS_1904; /* get modification time */
    
    #elif defined win_linux
      u= info.st_mtime;
      
      #ifdef windows32
        if (isFolder) {
            // used to crash here because data was wrong type
            hFile= FindFirstFile( pathname, &data);
            if (hFile!=NULL) {
                debugprintf(dbgFiles,dbgNorm,("# getFD: '%s'\n",pathname));
                FindNextFile( hFile, &data);
                GetFileTime ( hFile, &cr, &lastA, &lastW );
                FileTimeToLocalFileTime ( &lastW, &lastL );
                FileTimeToSystemTime    ( &lastL, &sy );
                        
                uDir= true;
                u   = 0;
                
                FindClose( hFile );
            }
        }
      #endif
    #endif

    TConv( u, &tim );
//  tp = localtime( (time_t*)&u );
//  tim= *tp; /* copy it, as it might be overwritten */

    #ifdef windows32
      if (uDir) {
          tim.tm_year= sy.wYear-1900;
          tim.tm_mon = sy.wMonth-1; /* somewhat different month notation */
          tim.tm_mday= sy.wDay;
          tim.tm_hour= sy.wHour;
          tim.tm_min = sy.wMinute;
      }
    #endif
    
    fdbeg[3]= tim.tm_year;
    fdbeg[4]= tim.tm_mon+1; /* somewhat different month notation */
    fdbeg[5]= tim.tm_mday;
    fdbeg[6]= tim.tm_hour;
    fdbeg[7]= tim.tm_min;
    
    fdbeg[8]= 1; /* link count = 1 */
    debugprintf(dbgFiles,dbgNorm,("# getFD: %02d/%02d/%02d %02d:%02d\n", 
                tim.tm_year % 100,tim.tm_mon+1,tim.tm_mday, tim.tm_hour,tim.tm_min));
    
    #ifdef macintosh
      u= cipbP->hFileInfo.ioFlCrDat + OFFS_1904; /* get creation time */
    #elif defined linux
      #ifdef windows32
        if (isFolder) info.st_ctime= 0;
      #endif
      
      u= info.st_ctime;
    #endif

    TConv( u, &tim );
//  tp = localtime( (time_t*)&u );
//  tim= *tp; /* copy it, as it might be overwritten */

    fdbeg[13]= tim.tm_year;
    fdbeg[14]= tim.tm_mon+1;
    fdbeg[15]= tim.tm_mday;

    /* file length */
//  *sizeP= 0; /* by default */
    #endif // NEW_LUZ_FD_IMPL

        
    #ifdef macintosh
      if (isFolder) { /* virtual size is number of items plus 2 for . and .. */
          *sizeP= (ulong)(cipbP->dirInfo.ioDrNmFls+2)*DIRENTRYSZ;
      }
      else {          /* file size is the data fork size */
          *sizeP= (ulong) cipbP->hFileInfo.ioFlLgLen;
      }

    #elif defined win_linux
      if (isFolder) {
          *sizeP= 0; /* by default */
          
              ok= (pathname!=NULL && ustrcmp( pathname,"" )!=0);
          if (ok) {
                        strcpy( spRec.fullName, (char*)fdl );
                  ok= OpenTDir( spRec.fullName, &spRec.dDsc );
              if (ok) {
                  *att  = 0x80 | 0x3F;
                  *sizeP= os9_long( 2*DIRENTRYSZ ); /* if no entries */   
              
                  if (spRec.dDsc!=NULL) {
                      *sizeP= os9_long( DirSize(&spRec) );
                      closedir( spRec.dDsc );
                  }
              }
    	  } /* if (ok) */
      }
      else *sizeP= os9_long(info.st_size);
        
//    printf( "%d %10d '%s'\n", isFolder, os9_long(*sizeP), pathname );
    #endif
    
    
    /* copy FD beginning to caller's buffer */
    memcpy(buffer,fdbeg,maxbyt>FDS ? FDS : maxbyt);

//  /* fill up with 0 as far as needed */
//  if (maxbyt>16) {
//      memset( &buffer[16],0, maxbyt-16 );
//  }
} /* getFD */



static void setFD( syspath_typ* spP, void* fdl, byte *buffer )
/* adapt cipb with info of FD */
// void setFD(CInfoPBRec *cipbP, ushort maxbyt, byte *buffer)
{
    byte fdbeg[16]; /* buffer for preparing FD */
    struct tm tim;
    time_t u;
    
    #ifdef MACFILES  
      CInfoPBRec* cipbP= fdl;
    #else
      #ifndef linux
      #pragma unused(fdl)
      #endif
    #endif
    
    memcpy(fdbeg,buffer,16);  /* the size IS 16 */  
    tim.tm_year= fdbeg[ 3]; 
    tim.tm_mon = fdbeg[ 4]-1; /* the month correction */
    tim.tm_mday= fdbeg[ 5];
    tim.tm_hour= fdbeg[ 6];
    tim.tm_min = fdbeg[ 7];
    tim.tm_sec = 0;           /* no seconds supported */
    
    u= UConv( &tim );

    spP->u.disk.u.file.moddate        = u;
    spP->u.disk.u.file.moddate_changed= true;

    #ifdef MACFILES
      cipbP->hFileInfo.ioFlMdDat= (ulong)u-OFFS_1904; /* fill it into Mac's record */
    #elif defined win_linux
      Set_FileDate( spP, u );
    #endif


    if (fdbeg[13]==0) return; /* don't change it for invalid year */
    
    tim.tm_year= fdbeg[13];
    tim.tm_mon = fdbeg[14]-1; /* the month correction */
    tim.tm_mday= fdbeg[15];
    tim.tm_hour= 0;
    tim.tm_min = 0;
    tim.tm_sec = 0;           /* no seconds supported */

    u= UConv( &tim );
	
    #ifdef MACFILES
      cipbP->hFileInfo.ioFlCrDat= (ulong)u-OFFS_1904; /* fill it into Mac's record */
    #endif
} /* setFD */


/* get file descriptor for HFS object */
os9err pHgetFD(ushort pid, syspath_typ* spP, ulong *maxbytP, byte *buffer)
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    void* fdl;


    #ifdef macintosh
      os9err err;
      CInfoPBRec cipb;  

      /* get the cipb and prepare an FD from it */
      err= getCipb( &cipb, &spP->u.disk.spec ); if (err) return err;
      fdl=          &cipb;
    
    #elif defined win_linux
      fdl= &spP->fullName; /* use this for linux */
    
    #else
      #pragma unused(spP)
      /* no FD sector read possible w/o native OS access */
      return E_UNKSVC;
    #endif
    
    getFD( fdl, loword(*maxbytP),buffer );
    debugprintf(dbgFiles,dbgNorm,("# pHgetFD: no longer alive %d\n"));
    return 0;
} /* pHgetFD */


/* set file descriptor for HFS object */
/* %%% currently only the file date will be set */
os9err pHsetFD( ushort pid, syspath_typ* spP, byte *buffer )
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    os9err err= 0;
    void*  fdl;
    
    #ifdef MACFILES
      OSErr       oserr;
      CInfoPBRec  cipb;  
      FSSpec*     spc= &spP->u.disk.spec;
      FInfo       f;

      /* get the cipb */
                    fdl= &cipb;
      err= getCipb( fdl, spc ); if (err) return err;

    #elif defined win_linux
      fdl= NULL;

    #else
      /* no FD read possible w/o native OS access */
      return E_UNKSVC;
    #endif


    setFD( spP, fdl, buffer);


    #ifdef MACFILES
      err= setCipb( fdl, spc ); /* and write it back */

           oserr= FSpGetFInfo  ( spc, &f );
      if (!oserr) {           
           Get_Creator_And_Type( spc, &f.fdCreator, &f.fdType );
           oserr= FSpSetFInfo  ( spc, &f );
      }
    #endif

    return err;
} /* pHsetFD */
/* adapt time/date of a file */



/* get file descriptor for file specified by "sector" */
os9err pHgetFDInf(ushort pid, syspath_typ* spP, ulong *maxbytP,
                                                ulong *fdinf, byte *buffer)
{
    #ifndef linux
    #pragma unused(pid,spP)
    #endif
    
    void* fdl;
    
    #ifdef MACFILES
      union {
          CInfoPBRec    cipb;
          HParamBlockRec hpb;
      } pbu;
    
      Str255      fName;
      short       volid;
      long        objid, dirid;
      byte        isdir;
      OSErr       oserr;
      
    #elif defined win_linux
      char        name  [OS9PATHLEN];
      char        result[OS9PATHLEN];
    #endif

    
    #ifdef MACFILES
      /* extract volid and file/dirid */    /* if (volid & VOLIDSIGN) volid |= VOLIDEXT; */
      volid= (*fdinf >> IDSHIFT) & VOLIDMASK; if (volid != 0       ) volid |= VOLIDEXT;
      objid=  *fdinf &  IDMASK; if (objid & IDSIGN) objid |= IDEXT;

      /* try to locate the file/dir */
      /* --- first assume that it is a file and try to resolve ID */
      pbu.hpb.fidParam.ioVRefNum= volid;
      pbu.hpb.fidParam.ioFileID = objid;
      fName[0]=0; /* make sure PBResolveFileIDRef uses VRefNum, not a volume name */
      pbu.hpb.fidParam.ioNamePtr=fName;
      oserr = PBResolveFileIDRefSync(&pbu.hpb);
      debugprintf(dbgFiles,dbgDetail,("# pHgetFDsect: resolving volid=%d, fileid=$%lX returned oserr=%d\n",volid,objid,oserr));

      if (oserr==notAFileErr) {
          /* it must be a directory */
          dirid= objid;
          isdir= true;
      }
      else {
          /* it is a file */
          if (oserr) return host2os9err(oserr,E_SEEK); /* probably because no file ID ref has been created */
          dirid= pbu.hpb.fidParam.ioSrcDirID; /* get dir ID */
          isdir= false;
      }
      
      /* parent ID, vRefNum and (if not isdir, fName) are now set */
      /* --- now get cipb */
      pbu.cipb.hFileInfo.ioCompletion   = 0L;
      pbu.cipb.hFileInfo.ioNamePtr      = fName;
      pbu.cipb.hFileInfo.ioVRefNum      = volid;
      pbu.cipb.hFileInfo.ioFDirIndex    = isdir ? -1 : 0;   /* use the dir & vRefNum (and fName if file) */
      pbu.cipb.hFileInfo.ioDirID        = dirid;

      oserr = PBGetCatInfoSync(&pbu.cipb);
      debugprintf(dbgFiles,dbgDetail,("# pHgetFDsect: getting info for '%#s' returned oserr=%d\n",fName,oserr));
      if (oserr) return host2os9err(oserr,E_PNNF);

      /* prepare an FD from it */
      fdl= &pbu.cipb;
    
    #elif defined win_linux
      fdl= NULL;
      if      (DirName( spP->fullName, *fdinf, (char*)&result )) {
          strcpy( name, spP->fullName );
          strcat( name, PATHDELIM_STR );
          strcat( name, result );
          fdl=   &name; /* use this for windows and linux */
      }
          
    #else
      #pragma unused(spP,fdinf)
      /* no FD sector read possible w/o native OS access */
      return E_UNKSVC;
    #endif
    
    getFD( fdl, loword(*maxbytP),buffer );
    return 0;
} /* pHgetFDInf */



/* Directory file emulation */
/* ------------------------ */

/* open directory */
os9err pDopen( ushort pid, syspath_typ* spP, ushort *modeP, const char* pathname)
{
    os9err  err= 0;
    char*   pastpath;
    char    hostpath[OS9PATHLEN];
    char    ploc    [OS9PATHLEN];
    Boolean exedir= IsExec(*modeP);
    char*   p;
    char*   pp;
    
    #ifdef MACFILES
      CInfoPBRec cipb;
      defdir_typ defdir;
      FSSpec*    spc= &spP->u.disk.spec;
      
    #elif defined win_linux
      DIR*    d;
      char    adapted[OS9PATHLEN];
      Boolean ok;
    #endif

 /* if (strlen(pathname)>DIRNAMSZ) return os9error(E_BPNAM); */

    strcpy  ( ploc,pathname );
    if      (*ploc==NUL) return E_BPNAM;
    pastpath= ploc;
    err     = parsepath( pid, &pastpath,hostpath, exedir ); if (err) return err;
    pp      = hostpath;
    
    #ifdef macintosh
      /* make an FSSpec for the directory */
                                   defdir= exedir ?_exe:_data;  
           err= getFSSpec( pid,pp, defdir, spc );
      if (!err) {
          /* object exists, but make sure it is a directory */
          getCipb( &cipb, spc ); /* it's a file, return error */
          if     (!(cipb.hFileInfo.ioFlAttrib & ioDirMask)) return os9error(E_FNA);  

          /* OS9TCP/ftp requires open dir for write !?! */
          // if (*modeP & 0x02) return os9error(E_FNA); /* can't open dir for write */
      }

      debugprintf(dbgFiles,dbgDetail,("# pDopen: MakeFSSpec returned err=%d\n",err));

    #elif defined win_linux
      err= AdjustPath( pp,adapted, false ); if (err) return err;
      pp = adapted;

           ok= OpenTDir( pp, &d );
      if (!ok) {
        #ifdef windows32
        if (GetLastError()==ERROR_NOT_READY) return os9error(E_NOTRDY);
        #endif
        return E_FNA;
      }
      
              spP->dDsc= d;
      strcpy( spP->fullName, adapted ); /* for later use with stat function */

    #else
      /* no directory read possible w/o native OS access */
      return E_UNKSVC;
    #endif

    spP->u.disk.u.dir.pos= 0; /* rewind it ! */

    p= (char*)ploc+strlen(ploc)-1; /* only the filename itself, no path name */
    while (p>=ploc && *p!=PSEP) p--;
    p++;

    strcpy( spP->name,p );
    return err;
} /* pDopen */


/* not used for Mac */
os9err pDclose( ushort pid, syspath_typ* spP )
{
    os9err err= 0;

    #ifndef linux
    #pragma unused(pid,spP)
    #endif
    
    #ifdef win_linux
      DIR* d= spP->dDsc;
      if  (d!=NULL) { err= closedir( d ); spP->dDsc= NULL; }
      debugprintf( dbgFiles,dbgNorm,("# pDclose: '%s' err=%d\n", 
                                        spP->fullName,err ));
    #endif
    
    return err;
} /* pDclose */




#ifdef macintosh
static os9err get_dir_entry(ushort index, os9direntry_typ *deP, syspath_typ *spP)
/* get directory entry referred to by index
 *  index=0 returns parent     ("..")
 *  index=1 returns dir itself (".")
 */
{
    os9err  err;
    OSErr   oserr;
    FSSpec* spc= &spP->u.disk.spec;
    short   volid;
    long    dirid, objid;
    char*   q;

    union {
        CInfoPBRec cipb;
        HParamBlockRec hpb;
    } pbu;

    Str255 fName;
    ushort n;

    /* set the vrefnum of the file or dir */
    volid= spc->vRefNum; /* get volume ID */
                                       /* (volid>VOLIDSIGN-1 || volid<-VOLIDSIGN)) */
    if (debugcheck(dbgAnomaly,dbgNorm) && (volid>0           || volid< VOLIDMIN)) {
        uphe_printf("get_dir_entry: vRefnum=%hd out of 3-bit range\n",volid); 
    }
    deP->fdsect=((volid & VOLIDMASK) << IDSHIFT);
    memset(deP->name, 0,DIRNAMSZ); /* clear before using */

    /* now get the entry */
    if (index==0) {
        /* parent directory ist the FIRST entry (even if Dibble said it's the second!) */
        strcpy(deP->name,".\xAE");
        objid= spc->parID; /* save to check range later */
        if (objid==fsRtParID) {
            /* this points to the (nonexistant) "root of root" */
            objid=fsRtDirID; /* let parent entry point again to the root */
        }
        deP->fdsect |= (objid & IDMASK);
        debugprintf(dbgFiles,dbgDetail,("# get_dir_entry: index=%d: name='..', volid=%d, dirid=$%lX, fdsect=$%08lX\n",index,volid,objid,deP->fdsect));
    }
    else {
		    err= getCipb( &pbu.cipb, spc );
        if (err) return err;
        
        dirid= pbu.cipb.dirInfo.ioDrDirID; /* copy the sucker */
        /* now check for "." */
        if (index==1) {
            /* current directory is the SECOND entry */
            strcpy(deP->name,"\xAE");
            objid=dirid;
            deP->fdsect |= (objid & IDMASK);
            debugprintf(dbgFiles,dbgDetail,("# get_dir_entry: index=%d: name='.', volid=%d, dirid=$%lX, fdsect=$%08lX\n",index,volid,objid, deP->fdsect));
        }
        else {
            /* some entry within this dir */
            fName [0] = 0;
            pbu.cipb.hFileInfo.ioCompletion = 0L;
            pbu.cipb.hFileInfo.ioNamePtr    = fName;
            pbu.cipb.hFileInfo.ioVRefNum    = volid;
            pbu.cipb.hFileInfo.ioFDirIndex  = index-1;  /* use index, range 1..n */
            pbu.cipb.hFileInfo.ioDirID      = dirid;
            oserr = PBGetCatInfoSync(&pbu.cipb);
            if (oserr==fnfErr) return os9error(E_EOF); /* this is no error, but only no more items in the folder */

            if (oserr) return host2os9err(oserr,E_READ);
            else {
                /* there is an entry for this index */
                n=fName[0]>DIRNAMSZ ? DIRNAMSZ : fName[0];
                strncpy(deP->name,&fName[1], n); /* copy the name */
                
                q= deP->name;
                while (true) { /* convert the spaces in the file names to underlines */
  	  	            q= strstr( q," " ); if (q==NULL) break;
                    q[ 0 ]= '_';
                } /* loop */

                
                if (fetchnames) {
                    /* show slashes at filename beginnings as periods (that's what Fetch FTP does on OS9->mac */
                    if (deP->name[0]=='/') deP->name[0]='.';
                }
                deP->name[n-1] |= 0x80; /* set old-style terminator */
                if (pbu.cipb.hFileInfo.ioFlAttrib & ioDirMask) {
                    objid=pbu.cipb.dirInfo.ioDrDirID;
                    deP->fdsect |= (objid & IDMASK);                
                    debugprintf(dbgFiles,dbgDetail,("# get_dir_entry: index=%d: dirname='%#s', volid=%d, dirid=$%lX, fdsect=$%08lX\n",index,fName,volid,objid,deP->fdsect));
                }
                else {
                    objid=pbu.cipb.hFileInfo.ioDirID; /* get file ID, which returned in ioDirID by PBGetCatInfo */
                    /* make sure that a file ID reference exists to find the file by pHgetFDsect later */
                    pbu.hpb.fidParam.ioNamePtr  = fName;
                    pbu.hpb.fidParam.ioVRefNum  = volid;
                    pbu.hpb.fidParam.ioSrcDirID = dirid;
                    oserr=PBCreateFileIDRefSync(&pbu.hpb);
                    debugprintf(dbgFiles,dbgDetail,("# get_dir_entry: PBCreateFileIDRef returns oserr=%d\n",oserr));
                    /* now assign file id (for which a reference now should exist) */
                    deP->fdsect |= (objid & IDMASK);
                    debugprintf(dbgFiles,dbgDetail,("# get_dir_entry: index=%d: filename='%#s', volid=%d, fileid=$%lX, filerefid=$%lX, fdsect=$%08lX\n",index,fName,volid,objid,pbu.hpb.fidParam.ioFileID,deP->fdsect));
                }
            }
        }
    }
    if (objid>=IDSIGN || objid< -IDSIGN && debugcheck(dbgAnomaly,dbgNorm)) {
        uphe_printf("get_dir_entry: object ID=%ld is out of range %ld..%ld\n",objid,-IDSIGN,IDSIGN-1); /* better: (bfo) */
    }
    return 0;
} /* get_dir_entry */
#endif



/* read from (simulated) directory file */
os9err pDread( ushort pid, syspath_typ *spP, ulong *n, char* buffer )
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    os9err err;
    
    ulong* pos  = &spP->u.disk.u.dir.pos; /* current file position */
    ulong  offs = *pos & 0x1F;            /* offset    to start */
    ushort index= *pos >> 5;              /* dir index to start */
    char*  myb= buffer;
    
    ulong  cnt, nbytes;
    os9direntry_typ os9dirent;
    
    #ifdef win_linux
      dirent_typ*  dEnt;
      int          len;
      ulong        fdpos;
      Boolean      topFlag= false;
      Boolean      de_call;
      
      #ifdef windows32
        dirent_typ dField;
      #endif
    #endif
    
//  printf( "pos=%8d n=%4d '%s'\n", *pos, *n, spP->name );
    debugprintf(dbgFiles,dbgDetail,("# pDread: requests $%lX bytes\n",*n)); 
    cnt= *n;
    if  (*n==0) return 0;
        
    /* now copy remaining entries (if any) */
    /* now do it in one single step (bfo)  */
    while (cnt>0) {
        #ifdef MACFILES
          err= get_dir_entry( index, &os9dirent, spP );
          
        #elif defined win_linux
          err= 0;
          memset( &os9dirent, 0,DIRENTRYSZ ); /* clear before using */
          
      //  #ifdef windows32
            de_call= true;      /* do it always */
      //  #else
      //      de_call= (offs!=0); /* strange reading, e.g. in "dump" */
      //  #endif
          
          if (de_call) {
              err= DirNthEntry( spP,index ); if (err) return err;
          }
          
          /* ".." and "." are swapped in UXIX !! */
          if (*pos-offs==DIRENTRYSZ) { /* if 2nd entry */
               strncpy( (char*)&os9dirent.name,".", DIRNAMSZ );
               fdpos= spP->u.disk.u.dir.fdcur; /* get current dir info */
          }
          else {
              while (true) { /* ignore ".AppleDouble" */
                      dEnt= ReadTDir( spP->dDsc );
                  if (dEnt==NULL ||
                      ustrcmp( (char*)dEnt->d_name,AppDo )!=0 ) break;
              } /* while */
                  
              if (*pos-offs==0) { /* if 1st entry */
                  #ifdef windows32
                    if (dEnt==NULL) {
                        dEnt= &dField;
                        strcpy( dEnt->d_name,"" ); /* virtual field */
                    }
                    
                    if (dEnt!=NULL &&
                        ustrcmp( (char*)dEnt->d_name,"." )!=0 ) {
                        strcpy( dEnt->d_name,"." );
                        topFlag= true;
                    //  printf( "top '%s' '%s'\n", spP->fullName,dEnt->d_name );
                    }
                  #endif
                  
                  FD_ID( spP->fullName,dEnt, &spP->u.disk.u.dir.fdcur );
                  if (topFlag) strcpy( dEnt->d_name,".." );
                  else dEnt= ReadTDir( spP->dDsc );
              } /* if (*pos-offs==0) */
              
              if (dEnt!=NULL) {
                  GetEntry( dEnt,os9dirent.name, true );
                  FD_ID( spP->fullName,dEnt, &fdpos );                      
                  if (topFlag) { seekD0( spP ); topFlag= false; }
              }
              else err= E_EOF;
          } /* if (*pos-offs==DIRENTRYSZ) */
          
          if (!err) {
          //   printf( "==> '%s' %08X\n", os9dirent.name, fdpos );
               len= strlen(os9dirent.name);
               os9dirent.name[len-1] |= 0x80; /* set old-style terminator */
               os9dirent.fdsect= os9_long( fdpos );
          }
      
        #else
          /* %%% no directory read possible w/o native OS access */
          return E_UNKSVC;
        #endif
        
        if (!err) {
            nbytes = (offs+cnt)>DIRENTRYSZ ? DIRENTRYSZ-offs:cnt;
            memcpy(myb,(byte*)&os9dirent+offs, nbytes);
            
            cnt -= nbytes;
            myb += nbytes;
            offs = (offs+nbytes) & 0x1F;
            *pos+= nbytes;
            if (offs==0) index++;
        }
        else {
            if (err==E_EOF) {
                if (cnt==*n) return err;  /* nothing to be read */
                /* ok, no more to read, but something was there */
                break;
            }
            else return err;
        }
    } /* while */
    
    /* sucessful, set number of bytes actually read */
    *n-= cnt; /* adjust to show actually read # of bytes */
    debugprintf(dbgFiles,dbgDetail,("# pDread: returned $%lX bytes\n",*n)); 

//  printf( "buf=%8d n=%4d '%s'\n", buffer, *n, spP->name );
//  for (ii=0;ii<*n;ii++) {
//      printf( "%04X ", (byte)buffer[ii] );
//      if (ii%8==7) printf( "\n" );
//  }
    
    return 0;
} /* pDread */


/* get pointer position */
os9err pDpos( ushort pid, syspath_typ* spP, ulong *posP )
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    *posP= spP->u.disk.u.dir.pos;
    return 0;
} /* pDpos */



/* get size of directory file */
os9err pDsize( ushort pid, syspath_typ* spP, ulong *sizeP )
{
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    os9err err= 0;
    
    #ifdef MACFILES
      CInfoPBRec cipb;

      err= getCipb(&cipb, &spP->u.disk.spec); if (err) return err;
      *sizeP=(cipb.dirInfo.ioDrNmFls + 2) * DIRENTRYSZ; /* calc size (don't forget two more for . and .. */
      debugprintf(dbgFiles,dbgDetail,("# pDsize: number of files=%ld\n",cipb.dirInfo.ioDrNmFls));

    #elif defined win_linux
      *sizeP= DirSize( spP );
      
    #else
      #pragma unused(spP,sizeP)
      return E_UNKSVC;
    #endif
    
    return err;
} /* pDsize */



/* set read position */
os9err pDseek( ushort pid, syspath_typ* spP, ulong *posP )
{
    #ifdef macintosh
      ulong       size;
      os9err      err;
    #elif defined win_linux
      int         cnt, n;
      dirent_typ* dEnt;
    #else
      #pragma unused(pid)
    #endif
    
    
    #ifdef macintosh
      err= pDsize( pid,spP, &size ); if (err) return err;
      debugprintf(dbgFiles,dbgNorm,("# pDseek: dir size is = $%lX, seek request to $%lX\n",size,*posP));
      if (*posP>size) return os9error(E_SEEK);

    #elif defined win_linux
      seekD0( spP ); /* start at the beginning */

      n= 0;    cnt= *posP/DIRENTRYSZ;
      while (n<cnt) { /* search for the nth entry */
        dEnt= ReadTDir( spP->dDsc );
        
        /* special handling for the root directory */
        #ifdef windows32
          if (n==0 && (dEnt==NULL || ustrcmp( dEnt->d_name,"." )!=0)) {
              seekD0( spP ); break;
          }
        #endif
         
        if (dEnt==NULL) return os9error(E_SEEK);
        if (ustrcmp( dEnt->d_name,AppDo )!=0 ) cnt--; /* ignore ".AppleDouble" */
        n++;
      } /* while */

    #else
      return E_UNKSVC;
    #endif  

    spP->u.disk.u.dir.pos= *posP;
    return 0;
} /* pDseek */



os9err pDchd( ushort pid, syspath_typ* none, ushort *modeP, char* pathname )
{
    #ifndef linux
    #pragma unused(none)
    #endif
    
    os9err       err= 0;
    char*        pastpath;
    char         p[OS9PATHLEN]; 
    Boolean      exedir= IsExec(*modeP); /* check if it is chd or chx */
    process_typ* cp= &procs[pid];
    char*        defDir_s;
    
    #ifdef MACFILES
      short   vref;
      long    dirID;
      long*   xD;
      short*  xV;

    #elif defined win_linux
      char    adapted[OS9PATHLEN];
      
    #else
    #error I_Chgdir not yet implemented
    #endif
    
    /* now obtain volume and directory of new path */
    pastpath= pathname;
    err     = parsepath( pid, &pastpath,p, exedir);
    pathname= p;
     
    debugprintf( dbgFiles,dbgNorm,("# I$ChgDir: try to %s to '%s', err=%d\n",
                                      exedir ? "chx":"chd", pathname,err ));
    if (err) return err;
      
    /* get current default path */
    if (exedir) defDir_s= cp->x.path;
    else        defDir_s= cp->d.path;
        
    #ifdef MACFILES
      /* change based on default execution/data dir */
      if (exedir) { xV= &cp->x.volID; xD= &cp->x.dirID; }
      else        { xV= &cp->d.volID; xD= &cp->d.dirID; }
    
      vref= *xV;  dirID= *xD;
      err =get_dirid( &vref,&dirID,pathname ); if (err) return err;
      *xV = vref; *xD= dirID; /* now save these params as new default directory */

      debugprintf( dbgFiles,dbgNorm,("# I$ChgDir: ch%c: volID=%d, dirID=%ld (of new defdir)\n",
                   exedir ? 'x':'d', *xV,*xD ));
    
    #elif defined win_linux
      /* now do a OS changedir to "resolve" (and verify) location */
      err= AdjustPath( pathname,adapted, false ); if (err) return err;
      pathname=                 adapted;
            
      if  (!PathFound( pathname )) return E_FNA;    
      strcpy( defDir_s,pathname );
    #endif

    if (exedir) cp->x.type= fDir; /* adapt the type */
    else        cp->d.type= fDir;
    
    return 0; /* ok */
} /* pDchd */



os9err pDmakdir( ushort pid, syspath_typ* none, ushort *modeP, char* pathname )
{
    #ifndef linux
    #pragma unused(none)
    #endif
    
    os9err       err;
    OSErr        oserr= 0;
    char*        pastpath;
    char         p[OS9PATHLEN]; 
    Boolean      exedir= IsExec(*modeP); /* check if it is chd or chx */
    process_typ* cp= &procs[pid];

    #ifdef macintosh
      FSSpec     localFSSpec;
      defdir_typ defdir;
      long       newdirid;
    #elif defined linux
      char adapted[OS9PATHLEN];
    #endif


    pastpath= pathname;
    err     = parsepath( pid, &pastpath,p,exedir ); if (err) return err;
    pathname= p;
    
    #ifdef macintosh
      debugprintf(dbgFiles,dbgNorm,("# I$MakDir: Mac path=%s\n",pathname));

      /* --- create FSSpec, use default volume/dir */
                                     defdir= exedir ? _exe:_data;
      err= getFSSpec( pid, pathname, defdir, &localFSSpec );
    
      if (!err || err==E_PNNF) {
          debugprintf(dbgFiles,dbgDetail,("# I$MakDir: FSSpec for new dir: OSErr=%d, vRefNum=%d, dirID=%ld\n",
                                             err, localFSSpec.vRefNum,localFSSpec.parID));
          /* --- create the directory */
          oserr = FSpDirCreate(&localFSSpec, smSystemScript, &newdirid);
          debugprintf(dbgFiles,dbgNorm,  ("# I$MakDir: oserr=%d, Id of new dir is %ld\n",
                                             oserr,newdirid));
      }

    #elif defined(windows32)
      debugprintf(dbgFiles,dbgNorm,("# I$MakDir: Win path=%s\n",pathname));
      if (!CreateDirectory( pathname,NULL )) oserr=GetLastError();

    #elif defined linux
          err= AdjustPath( pathname,adapted, true );
      if (err) return err;
      debugprintf(dbgFiles,dbgNorm,("# I$MakDir: Linux path=%s\n",adapted));
      if (PathFound(adapted) || 
          FileFound(adapted)) return E_CEF;
          
      if (mkdir( adapted,0x01c0 )==0) return 0; /* rwx------ */
      return E_BPNAM;

    #else
      /* other OS */
      #error I_Makdir not yet implemented
    #endif

    return host2os9err(oserr,E_WRITE);
} /* pDmakdir */



os9err pDsetatt( ushort pid, syspath_typ* spP, ulong *attr )
{
    os9err err  = 0;
    OSErr  oserr= 0;
    ushort mode;
      
    #ifdef macintosh
      int    k;
      char*  pp= spP->name;
      char*  sv;
      char   pathname[OS9PATHLEN]; 
      FSSpec delSpec;
        
    #elif defined win_linux
      char*  pp= spP->fullName;
      
      #ifdef windows32
//      dirent_typ* dEnt;
        char cmd[OS9PATHLEN];
      #endif
    
    #else
      return E_UNKSVC;
    #endif
          

    if (*attr & 0x80 ) return 0; /* it is already a directory */
      
    #ifdef win_linux
          err= RemoveAppledouble( spP ); /* because this file is not visible */
      if (err) return err;
    #endif
      
    /* can't change attributes in Mac/Linux/Windows */
    /* But remove and recreate as file is possible */
    pDclose( pid, spP );
      
    #ifdef macintosh
      sv   = pp; /* 'parsepath' will change <pp> */
      err  = parsepath( pid,&pp, pathname, false); if (err) return err;
      oserr= getFSSpec( pid,     pathname, _data, &delSpec );
      pp   = sv;
        
      /* unfortunately the Mac system needs some time to be ready   */
      /* for deleting an empty directory => make some timeout loops */
      for (k=0; k<MAXTRIES; k++) {
             oserr= FSpDelete( &delSpec );
        if (!oserr) break;
        HandleEvent();
      }

    #elif defined(windows32)
//    spP->dDsc= opendir( spP->fullName );
//    while (true) {
//        dEnt= readdir( spP->dDsc ); if (dEnt==NULL) break;
//        printf( "'%s'\n", dEnt->d_name );
//    }
//    closedir( spP->dDsc );
    
      /* can't be removed at the moment */
//    if (!RemoveDirectory(pp)) err= E_DNE;
//    if (err) { 
//      spP->dDsc= opendir( spP->fullName );
//      printf( "%04X\n", GetFileAttributes( pp ) );
//      SetFileAttributes( pp, 0x0001 ); /* it's a little bit a bad joke */
//      return 0; 
//    }

      sprintf( cmd, "rmdir %s", pp );
      err= call_hostcmd( cmd, pid, 0,NULL ); if (err) return err;

    #elif defined linux
      oserr=  remove( pp ); if (oserr) err= host2os9err(oserr,E_DNE);
    #endif
      
    debugprintf( dbgFiles,dbgNorm,("# pDsetatt: '%s' remove err=%d\n", pp,err )); 
    if (err) return err;
      
    spP->type= fFile; /* change the file type now */
    mode= 0x03 | poCreateMask;
    err = pFopen( pid,spP, &mode, pp );
    
    return err;
} /* pDsetatt */



/* check for EOF */
os9err pDeof( ushort pid, syspath_typ* spP )
{
    ulong  n;
    os9err err= pDsize( pid,spP, &n ); if (err) return err;
    if    (n<=spP->u.disk.u.dir.pos) return os9error(E_EOF);
    
    return 0; 
} /* pDeof */


/* eof */

