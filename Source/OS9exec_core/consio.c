// 
//    OS9exec,   OS-9 emulator for Mac OS, Windows and Linux 
//    Copyright (C) 2002 Lukas Zeller / Beat Forster
//    Available under http://www.synthesis.ch/os9exec
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
/*         for Apple Macintosh, PC, Linux     */
/*                                            */
/* (c) 1993-2002 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        forsterb@dial.eunet.ch              */
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
 */

#include "os9exec_incl.h"

/* Console I/O routines */
/* ==================== */



/* --- local procedure definitions for object definition ------------------- */
void   init_Cons ( fmgr_typ* f );
os9err pCopen    ( ushort pid, syspath_typ*, ushort  *modeP, char* pathname );
os9err pCclose   ( ushort pid, syspath_typ* );
os9err pConsIn   ( ushort pid, syspath_typ*, ulong *maxlenP, char* buffer );
os9err pConsInLn ( ushort pid, syspath_typ*, ulong *maxlenP, char* buffer );
os9err pConsOut  ( ushort pid, syspath_typ*, ulong *maxlenP, char* buffer );
os9err pConsOutLn( ushort pid, syspath_typ*, ulong *maxlenP, char* buffer );

os9err pCopt     ( ushort pid, syspath_typ*,                 byte* buffer );
os9err pCpos     ( ushort pid, syspath_typ*, ulong *posP );
os9err pCready   ( ushort pid, syspath_typ*, ulong *n );
os9err pCsetopt  ( ushort pid, syspath_typ*,                 byte* buffer );

void   init_NIL  ( fmgr_typ* f );
os9err pEOF      ( ushort pid, syspath_typ*, ulong *maxlenP, char* buffer );

void   init_SCF  ( fmgr_typ* f );
os9err pSopen    ( ushort pid, syspath_typ*, ushort  *modeP, char* pathname );
os9err pSBlink   ( ushort pid, syspath_typ*, ulong      *d2 );
/* ------------------------------------------------------------------------- */

void init_Cons( fmgr_typ* f )
/* install all procedures of the console file manager */
{
    gs_typ* gs= &f->gs;
    ss_typ* ss= &f->ss;
    
    /* main procedures */
    f->open      = (pathopfunc_typ)pCopen;
    f->close     = (pathopfunc_typ)pCclose;
    f->read      = (pathopfunc_typ)pConsIn;
    f->readln    = (pathopfunc_typ)pConsInLn;
    f->write     = (pathopfunc_typ)pConsOut;
    f->writeln   = (pathopfunc_typ)pConsOutLn;
    f->seek      = (pathopfunc_typ)pBadMode; /* not allowed */
    
    /* getstat */
    gs->_SS_Size = (pathopfunc_typ)pUnimp;   /* -- not used */
    gs->_SS_Opt  = (pathopfunc_typ)pCopt;
    gs->_SS_DevNm= (pathopfunc_typ)pSCFnam;
    gs->_SS_Pos  = (pathopfunc_typ)pCpos;
    gs->_SS_EOF  = (pathopfunc_typ)pNop;         /* ignored */
    gs->_SS_Ready= (pathopfunc_typ)pCready;

    /* setstat */
    ss->_SS_Size = (pathopfunc_typ)pNop;         /* ignored */
    ss->_SS_Opt  = (pathopfunc_typ)pCsetopt;
    ss->_SS_Attr = (pathopfunc_typ)pNop;         /* ignored */
} /* init_Cons */


void init_NIL( fmgr_typ* f )
{
    gs_typ* gs= &f->gs;
    ss_typ* ss= &f->ss;
    
    /* main procedures */
    f->open      = (pathopfunc_typ)pNop;         /* ignored */
    f->close     = (pathopfunc_typ)pNop;         /* ignored */
    f->read      = (pathopfunc_typ)pEOF;      /* as in OS-9 */
    f->readln    = (pathopfunc_typ)pEOF;      /* as in OS-9 */
    f->write     = (pathopfunc_typ)pNop;         /* ignored */
    f->writeln   = (pathopfunc_typ)pNop;         /* ignored */
    f->seek      = (pathopfunc_typ)pBadMode; /* not allowed */

    /* getstat */
    gs->_SS_Size = (pathopfunc_typ)pUnimp;   /* -- not used */
    gs->_SS_Opt  = (pathopfunc_typ)pSCFopt;
    gs->_SS_DevNm= (pathopfunc_typ)pSCFnam;
    gs->_SS_Pos  = (pathopfunc_typ)pBadMode; /* not allowed */
    gs->_SS_EOF  = (pathopfunc_typ)pNop;         /* ignored */
    gs->_SS_Ready= (pathopfunc_typ)pBadMode; /* not allowed */

    /* setstat */
    ss->_SS_Size = (pathopfunc_typ)pNop;         /* ignored */
    ss->_SS_Opt  = (pathopfunc_typ)pNop;         /* ignored */
    ss->_SS_Attr = (pathopfunc_typ)pNop;         /* ignored */
} /* init_NIL */


void init_SCF( fmgr_typ* f )
/* currently implemented for support of the /vmod driver only */
{
    gs_typ* gs= &f->gs;
    ss_typ* ss= &f->ss;
    
    /* main procedures */
    f->open       = (pathopfunc_typ)pSopen;
    f->close      = (pathopfunc_typ)pNop;
    f->read       = (pathopfunc_typ)pBadMode; /* not allowed */
    f->readln     = (pathopfunc_typ)pBadMode; /* not allowed */
    f->write      = (pathopfunc_typ)pBadMode; /* not allowed */
    f->writeln    = (pathopfunc_typ)pBadMode; /* not allowed */
    f->seek       = (pathopfunc_typ)pBadMode; /* not allowed */

    /* getstat */
    gs->_SS_Size  = (pathopfunc_typ)pUnimp;   /* -- not used */
    gs->_SS_Opt   = (pathopfunc_typ)pSCFopt;
    gs->_SS_DevNm = (pathopfunc_typ)pSCFnam;
    gs->_SS_Pos   = (pathopfunc_typ)pBadMode; /* not allowed */
    gs->_SS_EOF   = (pathopfunc_typ)pNop;         /* ignored */
    gs->_SS_Ready = (pathopfunc_typ)pBadMode; /* not allowed */
    gs->_SS_Undef = (pathopfunc_typ)pVMod;

    /* setstat */
    ss->_SS_Size  = (pathopfunc_typ)pNop;         /* ignored */
    ss->_SS_Opt   = (pathopfunc_typ)pNop;         /* ignored */
    ss->_SS_Attr  = (pathopfunc_typ)pNop;         /* ignored */
    ss->_SS_LBlink= (pathopfunc_typ)pSBlink;     /* specific */
    ss->_SS_Undef = (pathopfunc_typ)pNop;         /* ignored */
} /* init_SCF */

/* --------------------------------------------------------- */



/* standard output write. This routine calls the external function writeline
 * which may contain application-specific filtering to
 * convert error messages into MPW format
 */
static long stdwrite(ushort pid, byte *p, long cnt, FILE* stream, Boolean wrln)
{
    /* %%% not soooo nice stuff here, but will change with new filters anyway */
    #define MAXLINELEN 300
    static char linebuffer[MAXLINELEN+1];
    static char *lbp = linebuffer;
    long k,i;
    process_typ* cp= &procs[pid];
   
    debugprintf(dbgTerminal,dbgDeep,("# stdwrite cnt=%d, from=$%lX\n",cnt,(ulong)p));
    i=lbp-linebuffer; /* number of chars in linebuffer */
    for (k=0; k<cnt; k++) {
        if (i==MAXLINELEN) {
            /* we have a problem here with the buffer, write two separate lines */
            fprintf(stream,"# **** OS9exec/nt: Line longer than %d chars, truncated on console\n",MAXLINELEN);
            i++;
        }
        if (i<MAXLINELEN) {
          *lbp=*p; /* copy char */
        }
      
      if (*p==CR) {
         /* end of line: now call filter on a line-per line basis */
            *lbp= 0; /* terminate line */
         
            /* --- call appropriate filter routine */
            if   (cp->stdoutfilter==NULL) writeline( linebuffer,stream );
            else (cp->stdoutfilter)                ( linebuffer,stream, cp->filtermem );

            lbp= linebuffer; /* reset buffer pointer */
            if (wrln) {
                cnt= ++k;  /* just as much */
                break; /* don't go further */
            }
      }
      else if (i<MAXLINELEN) { i++; lbp++; }
      p++;
   }
   
   return cnt;
} /* stdwrite */



#ifdef TERMINAL_CONSOLE
  /* put char to console and perform CR/LF expansion etc. */
  void ConsPutc( char c )
  {   
      /* save this info in terminal interface system */
      gLastwritten_pid= currentpid;
    
      if (gConsoleID>=TTY_Base) {
          #ifdef PIP_SUPPORT
            WriteCharsToPTY( &c,1, gConsoleID, false );
          #endif
          return;
      }

      write( 1,&c,1 );
      #ifdef windows32
        lw_pid( &main_mco ); /* assign for later use */
      #endif
  } /* ConsPutc */



  void ConsPutcEdit( char c, Boolean alf, char eorch )
  /* put char to console and perform CR/LF expansion etc. */
  {
      ConsPutc ( c );          /* Auto LF */
      if (alf && c!=NUL && c==eorch) ConsPutc( LF );
  } /* ConsPutcEdit */
#endif



#ifdef TERMINAL_CONSOLE
/* get char from console and perform CR/LF conversion */
Boolean ConsGetc( char* c )
{
    int n;
    if (gConsoleID>=TTY_Base) {
        n= ReadCharsFromPTY( c,1, gConsoleID);
        return (n>0) && devIsReady;
    } /* if */

    #if defined(windows32) // || defined linux
      HandleEvent();
      n= ReadCharsFromTerminal( c,1, &main_mco );
      return (n>0) && devIsReady;
    #endif

    /* as a UNIX-function, EOLN is delivered as 0x0A */
//      *c= getchar();
//  if (*c==0x00 || !devIsReady) return false;

    if (fread( c,1,1,stdin )!=1 || !devIsReady) return false;
    debugprintf(dbgTerminal,dbgDetail,("# ConsGetc: returns=%X\n",*c));

    /* now swap 0x0A and 0x0D */
    if      (*c==LF) *c= CR; /* convert LF to OS-9 style CR */
    else if (*c==CR) *c= LF; /* convert CR to LF, in case we need it */

//  printf( ">> '%c' %x\n", *c, *c );
    return true;
} /* ConsGetc */


static os9err ConsRead( ushort pid, syspath_typ* spP, 
                        ulong *maxlenP, char* buffer, Boolean edit, char endchar )
{
    os9err        err= 0; /* no err so far */
    long          cnt= 0;
    char          c;
    ulong         inputticks= GetSystemTick();
    process_typ*  cp= &procs[pid];
    struct _sgs*  ot= (struct _sgs*)&spP->opt; /* path opt table */
    Boolean       alf= ot->_sgs_alf;
    Boolean       dupMode= false;
    pipechan_typ* k;
    
    interactivepid=pid; /* set focus to this process */
    fflush(stdout);     /* ensure all is written out */
    clearerr(stdin);    /* make sure we are not stuck with a Cmd-. */
    
    if (cp->state==pWaitRead) {
        cp->state= cp->saved_state;
        cnt=       cp->saved_cnt;
    }

    while (cnt<*maxlenP) {
        if (!dupMode && !ConsGetc(&c)) { 
            err= E_READ;
            if (!devIsReady) {
                k=  spP->u.pipe.pchP;
                if (spP->type==fTTY &&
                            k!=NULL && k->broken) {
                    err= E_EOF; break; /* pipe is broken */
                }
                    
                cp->saved_state= cp->state;
                cp->state      = pWaitRead;
                cp->saved_cnt  = cnt;
                arbitrate= true;
            }
            break; 
        }

        if (c!=NUL) { /* no check on NUL char for every sgs option */       
            /* check options */
            if (c==ot->_sgs_eofch) { err= E_EOF;    break; }
        
            #ifdef MPW /* for the terminal consoles it is supported now */
            if (c==ot->_sgs_kbich) { err= S_Intrpt; break; };
            if (c==ot->_sgs_kbach) { err= S_Abort;  break; };
            #endif
            
            if (edit && c==ot->_sgs_dulnch) {
                dupMode= true; /* duplicate last line */
            }
        } /* if not NUL char */
        
        if (dupMode) { 
            c=  *(buffer+cnt); 
            if (*(buffer+cnt+1)==NUL) dupMode= false; }
        else    *(buffer+cnt)= c;

        if (edit) {
            /* basic line editing  */
            if (c!=NUL && c==ot->_sgs_eofch) {
                ConsPutcEdit( CR, alf,ot->_sgs_eorch );
                err= E_EOF;
                break;
            }
            
            else if (c!=NUL && c==ot->_sgs_bspch) {
                /* backspace */
                if (cnt>0) {
                    cnt--;
                    if (ot->_sgs_echo) {
                        /* backspace echo */
                        ConsPutc(ot->_sgs_bsech);
                        if (ot->_sgs_backsp) {
                            /* BSP-Space-BSP Sequence wanted */
                            ConsPutc(' ');
                            ConsPutc(ot->_sgs_bsech);
                        }
                    }
                }
            }
            
            else if (c!=NUL && c==ot->_sgs_dlnch) {
                /* clear line */
                if (ot->_sgs_echo) {
                    while (cnt>0) {
                        ConsPutc(ot->_sgs_bsech);
                        if (ot->_sgs_backsp) {
                            /* BSP-Space-BSP Sequence wanted */
                            ConsPutc(' ');
                            ConsPutc(ot->_sgs_bsech);
                        };
                        cnt--;                  
                    }
                }
                cnt=0; /* clear anyway */
            }
            
            else {
                cnt++;
                if (ot->_sgs_echo) ConsPutcEdit( c, alf,ot->_sgs_eorch );
            }
        }
        else {
            /* without Editing */
            cnt++;
            if (ot->_sgs_echo) ConsPutc( c );
        }
        fflush(stdout); /* ensure all is written out */
        if (endchar!=0 && endchar==c) break;
    }
    
    *maxlenP = cnt;

    rw__idleticks+= GetSystemTick()-inputticks;
    return os9error(err);   
} /* ConsRead */
#endif


/* returns index for numbered descriptors like tty00,01,02... */
static Boolean ConsId( char* name, char* family, int range, int offs, int *result )
{
    int   flen= strlen(family);
    char* nInd;
    int   ii;

    if (ustrncmp( name,family, flen )!=0) return false; /* family name correct ? */

    nInd= &name[flen]; if (strcmp(nInd,"00")==0) { *result= offs; return true; }
    ii  = atoi(nInd);                          
    if (ii>0 && ii<range) { *result= ii+offs; return true; }

    return false;
} /* ConsId */



os9err pCopen(ushort pid, syspath_typ* spP, ushort *modeP, char* name)
/* routine for opening serial devices */
{
    #ifndef linux
    #pragma unused(modeP)
    #endif

    os9err err; 
    int    id;

    while (true) {
        #ifdef TERMINAL_CONSOLE /* decide which terminal id has to be taken */
          if (ConsId( name,"/t",  MAX_CONSOLE,    0, &id )) break; /* Multi-Strout /t1   ../t99   */
        #endif
        
        if (ConsId( name,"/tty",MAXTTYDEV, TTY_Base, &id )) break; /* ttys for MGR /tty00../tty99 */

        if (ustrcmp(name,MainConsole)==0) { id=    Main_ID; break; } /* /term */
        if (ustrcmp(name,SerialLineA)==0) { id= SerialA_ID; break; } /* /ts1  */
        if (ustrcmp(name,SerialLineB)==0) { id= SerialB_ID; break; } /* /ts2  */

        break;
    } /* end exit part */

    spP->term_id= id;
    strcpy( spP->name,&name[1] );

    /* for tty/pty pairs with the same name, the same pipe must be used */
    #ifdef PIP_SUPPORT
      if (spP->type==fTTY) {
          err= ConnectPTY_TTY( pid,spP );
          InstallTTY             ( spP,id );
      }
    #endif
    
    /* get the initialised path option table */
    err= pSCFopt( pid,spP, (byte*)&spP->opt ); /* no err returned */
    
    debugprintf( dbgTerminal,dbgDetail,("# pCopen (%s): successful, pid=%d\n",
                                           name, pid ));
    return 0;
} /* pCOpen */



os9err pSopen( ushort pid, syspath_typ* spP, ushort *modeP, char* name )
/* routine for opening SCF devices */
{   
    #ifndef linux
    #pragma unused(pid,modeP)
    #endif
    
	int k;

    if (*name!=NUL) {
    	strcpy            ( spP->name,&name[1] );
    	    k= link_mod_id( spP->name );
    	if (k!=MAXMODULES)  spP->mh= os9mod( k );
    }
    
    return 0;
} /* pSopen */



os9err pSBlink( ushort pid, syspath_typ* spP, ulong *d2 )
/* specific "/L2" blink command, as defined in "led_Drv" */
{
    #ifndef linux
    #pragma unused(pid,spP)
    #endif
    
     byte*   bb= (byte  *)*d2;
     ushort* ww= (ushort*)*d2;
        	
     l2.col1  =           *(bb+0); /* assign values as done in the "led_drv" */
     l2.ratio1= os9_word( *(ww+1) );
     l2.col2  =           *(bb+4);
     l2.ratio2= os9_word( *(ww+3) );
     return 0;
} /* pSBlink */



os9err pCclose( ushort pid, syspath_typ* spP )
{
    int           k, pb;
    syspath_typ   *spK, *spB;
    pipechan_typ* p= spP->u.pipe.pchP;
    pipechan_typ* b;

    g_spP     = spP;
    gConsoleID= spP->term_id;

    if (spP->type!=fTTY) {
        #ifdef MACTERMINAL
          /* now it can really be de-initialized */
          RemoveConsole();
        #endif
          
        return 0;
    } /* if */
    
                    if (p==NULL) return 0; /* no pipe available => return */          
    pb= p->sp_lock; if (pb==0)   return 0;

       spB= get_syspath( pid,pb );  /* take the according PTY */
    b= spB->u.pipe.pchP;

    /* paths shouldn't be closed if same console is still open */
    /* searching if there is a console with the same name */
    for (k=0; k<MAXSYSPATHS; k++) {
            spK= get_syspath( pid,k ); 
        if (spK!=NULL                     && 
            spK->linkcount>0              && /* still open by another path */
            spK->type==fTTY               &&
            spK->u.pipe.pchP!=NULL        &&
            spK->u.pipe.pchP->sp_lock==pb && b!=NULL) { b->sp_lock= spK->nr; return 0; }
    } /* for */

    /* syspath no longer in use, disconnect the pipe */
    if (b!=NULL) {
        b->broken= true;
        b->sp_lock= spB->nr;
        
        #ifdef PIP_SUPPORT
          releasePipe( pid, spP );
        #endif
    }
    
    RemoveTTY( gConsoleID ); 
    return 0;
} /* pCclose */



/* input character wise from console */
os9err pConsIn( ushort pid, syspath_typ* spP, ulong *maxlenP, char* buffer )
{
    #ifdef TERMINAL_CONSOLE
      gConsoleID= spP->term_id;
      g_spP     = spP;
      return ConsRead( pid,spP, maxlenP,buffer,false, 0);
    
    #else
      return pUnimp  ( pid,spP );
    #endif
} /* pConsIn */



os9err pConsInLn( ushort pid, syspath_typ* spP, ulong *maxlenP, char* buffer )
/* input line from console */
{
	os9err err= 0;
	
    #ifdef TERMINAL_CONSOLE
      gConsoleID= spP->term_id;
      g_spP     = spP;
      err= ConsRead( pid,spP,maxlenP,buffer,true,CR );
    
    #else
      long cnt,i;
      #define RDBUFLEN 500
      static char readbuf[RDBUFLEN];
      int cerr;
      ulong inputticks= GetSystemTick();


      /* interactive input line from stdin */
      cnt=*maxlenP; /* max number of chars to get */
      interactivepid=pid; /* set focus to this process */
      stdwrite(pid,"\n",1,stdout,false); /* begin input on a free line */
      fflush(stdout); /* ensure all is written out */
      clearerr(stdin); /* make sure we are not stuck with a Cmd-. */

      if (fgets(readbuf,RDBUFLEN,spP->stream)==NULL) {
          cerr= errno;
          clearerr(stdin);
          debugprintf(dbgTerminal,dbgNorm,("# pConsInLn: fgets returned NULL, errno=%d\n",cerr));   

          if      (cerr==22)    return          S_Abort;
          else if (feof(stdin)) return os9error(E_EOF);
          return                       os9error(E_READ);
      }

      i= strlen(readbuf);
      if (i>0) if (*(readbuf+i-1) =='\n') i--; /* forget newline if there is one */
      i=i<cnt ? i : cnt-1;
      *(readbuf+i++)='\n'; /* at end of string returned, there will be a CR anyway */

      debugprintf( dbgTerminal,dbgNorm,("# pConsIn len=%d, string='%s'\n",i,readbuf ));
      debug_halt ( dbgTerminal );
      strncpy( buffer,readbuf,i );
      *maxlenP=i;

      rw__idleticks+= GetSystemTick()-inputticks;
    #endif
    
    arbitrate= true;
    return err;
} /* pConsInLn */



os9err pEOF( ushort pid, syspath_typ* spP, ulong *maxlenP, char* buffer )
/* read operation for the nil device */
{   
    #ifndef linux
    #pragma unused(pid,spP,maxlenP,buffer)
    #endif
    
    return E_EOF;
} /* pEOF */



static os9err ConsoleOut( ushort pid, syspath_typ* spP, 
                          ulong *maxlenP, char* buffer, Boolean wrln )
/* output to console */
{
    long         cnt;
    char         c;
    ulong        outputticks= GetSystemTick();
    syspath_typ* spC=  spP;          /* default: no crossed path */
    struct _sgs* ot = (struct _sgs*)&spC->opt; /* path opt table */
    Boolean      do_lf= false;
    process_typ* cp= &procs[pid];

    gConsoleID=  spP->term_id;
    g_spP     =  spP;

    /* go directly if tty */
    if (gConsoleID>=TTY_Base) {
        if (wrln) {
                   cnt= 0; /* search if there is any CR */
            while (cnt<*maxlenP) {
                    c= buffer[cnt++];
            //  if (c==NUL) {
            //      *maxlenP= cnt-1;
            //      break;
            //  }
                
                if (c!=NUL && c==ot->_sgs_eorch) {
                    *maxlenP= cnt;
                    do_lf= ot->_sgs_alf;
                    break;
                }
            } /* while */
        }

        cnt=  WriteCharsToPTY( buffer,*maxlenP, gConsoleID, do_lf );
        if (cp->state==pSysTask) cnt= *maxlenP;
    }
    else {
        /* interactive output to console */
        #ifdef TERMINAL_CONSOLE
                 cnt=0;
          while (cnt<*maxlenP) {
              c= buffer[cnt++];
              ConsPutc( c );

              if (cp->state==pSysTask) { /* should never go to here */
                  cp->systask_offs= cnt-1; /* store it here !! */
                  cnt= *maxlenP;
                  break; /* tty/pty break */
              }

              if (wrln && c!=NUL && c==ot->_sgs_eorch) {
                  if (ot->_sgs_alf) ConsPutc( LF );
                  break;
              }
          }

        #else   
          cnt= stdwrite(pid,buffer,*maxlenP,spP->stream,false);
        #endif
    }

    rw__idleticks+= GetSystemTick()-outputticks;
    if (cnt<0) return c2os9err(errno,E_WRITE); /* default: general write error */

    *maxlenP= cnt;
    return 0;
} /* ConsoleOut */


/* output to console */
os9err pConsOut  ( ushort pid, syspath_typ* spP, ulong *maxlenP, char* buffer )
{   return    ConsoleOut( pid,spP, maxlenP, buffer, false );    
} /* pConsOut */


/* output line to console */
os9err pConsOutLn( ushort pid, syspath_typ* spP, ulong *maxlenP, char* buffer)
{   
	os9err err= ConsoleOut( pid,spP, maxlenP, buffer, true );
	arbitrate= true; /* create smoother output */
	return err;
} /* pConsOutLn */



/* get options from console */
os9err pCopt(ushort pid, syspath_typ* spP, byte* buffer)
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    memcpy( buffer,&spP->opt, OPTSECTSIZE);
    return 0;
} /* pCopt */



/* set console options */
os9err pCsetopt(ushort pid, syspath_typ* spP, byte* buffer)
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    memcpy( &spP->opt,buffer, OPTSECTSIZE );
    return 0;
} /* pCsetopt */



os9err pCpos(ushort pid, syspath_typ* spP, ulong *posP )
{
    #ifndef linux
    #pragma unused(pid,spP)
    #endif

    *posP= 0;
    return 0;
} /* pCpos */



os9err pCready( ushort pid, syspath_typ *spP, ulong *n )
/* check ready */
/* NOTE: is valid for outputs also, when using "dup" */
{
    #ifndef linux
    #pragma unused(pid,spP)
    #endif
          
    gConsoleID= spP->term_id;
    g_spP     = spP;

    if (gConsoleID>=TTY_Base) {
        if (DevReadyTTY( n, gConsoleID )) return 0;
    }
    else { 
        #ifdef TERMINAL_CONSOLE
          if (DevReady ( n ))             return 0;
        #endif
    }
    
    return os9error(E_NOTRDY);
} /* pCready */



/* eof */
