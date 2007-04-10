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
/*         for Apple Macintosh and PC         */
/*                                            */
/* (c) 1993-2007 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        bfo@synthesis.ch                    */
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
 *    Revision 1.59  2007/04/07 09:05:14  bfo
 *    pStart will be displayed as process state "b"
 *
 *    Revision 1.58  2007/03/31 12:18:34  bfo
 *    - LINKED_HASH support and bug fixes (NULL initialisation)
 *    - extended <liCnt> range
 *
 *    Revision 1.57  2007/03/24 13:02:45  bfo
 *    - "HashF" hash function (using CRC) added
 *    - Hash function prepared for LINKED_HASH
 *    - Speedup for TFS directory reading
 *    - "StrReplace" function for Windows "con" problem ( => ".con" )
 *
 *    Revision 1.56  2007/03/10 12:41:13  bfo
 *    Use "malloc" instead "get_mem" here (avoid memory scattering)
 *
 *    Revision 1.55  2007/02/24 14:06:57  bfo
 *    - FD_ID: Additional params <dirid> and <fName>
 *    - MacOSX no longer based on Inodes (same as Linux)
 *    - Consistent m->ident key handling for MacOS9
 *
 *    Revision 1.54  2007/01/28 21:33:40  bfo
 *    'ustrcmp' made invisible / DirEntry dbgNorm -> dbgDetail
 *
 *    Revision 1.53  2007/01/07 13:17:25  bfo
 *    Use MACOS9 define
 *
 *    Revision 1.52  2006/12/02 12:13:33  bfo
 *    global <lastsyscall> eliminated
 *
 *    Revision 1.51  2006/12/01 20:05:44  bfo
 *    "MountDev" with <devCopy> support ( -d )
 *
 *    Revision 1.50  2006/10/12 19:50:23  bfo
 *    Extended for ".DS_Store" support
 *
 *    Revision 1.49  2006/08/20 18:35:29  bfo
 *    "noecho" mode is default again
 *    (Changes done by Martin Gregorie)
 *
 *    Revision 1.48  2006/07/23 14:21:43  bfo
 *    Quick return if no state change
 *
 *    Revision 1.47  2006/07/21 07:12:54  bfo
 *    task switch dbg display is now dbgDetail only
 *
 *    Revision 1.46  2006/06/13 19:17:29  bfo
 *    /p /p1 are recognized as printer devices
 *
 *    Revision 1.45  2006/06/11 22:02:32  bfo
 *    set_os9_state with 3rd param <callingProc>
 *
 *    Revision 1.44  2006/06/10 10:21:21  bfo
 *    t1..t49 no longer visible for Linux
 *
 *    Revision 1.43  2006/05/16 13:11:20  bfo
 *    Linux full path name adaption
 *
 *    Revision 1.42  2006/02/20 21:40:31  bfo
 *    no echo for Linux /term ; cTime/cDate assignment implented in a clean way
 *
 *    Revision 1.41  2006/02/19 16:36:23  bfo
 *    use <isIntUtil> / echo mode off by default
 *
 *    Revision 1.40  2005/07/06 21:05:19  bfo
 *    defined UNIX
 *
 *    Revision 1.39  2005/07/02 14:20:36  bfo
 *    Adapted for Mach-O / time correction for files / Volume name catch extension
 *
 *    Revision 1.38  2005/06/30 11:45:17  bfo
 *    Mach-O support
 *
 *    Revision 1.37  2005/04/15 11:40:44  bfo
 *    RBF_ImgSize implemented here now
 *
 *    Revision 1.36  2005/01/22 16:13:38  bfo
 *    Renamed to ifdef MACOS9
 *
 *    Revision 1.35  2004/12/04 00:07:26  bfo
 *    MacOSX MACH adaptions / small transferBuffer for Linux
 *
 *    Revision 1.34  2004/11/27 12:08:58  bfo
 *    _XXX_ introduced
 *
 *    Revision 1.33  2004/11/20 11:44:08  bfo
 *    Changed to version V3.25 (titles adapted)
 *
 *    Revision 1.32  2004/10/22 23:00:25  bfo
 *    Most of the "pragma unused" eliminated
 *
 *    Revision 1.31  2003/05/05 17:57:02  bfo
 *    Activate most of the ramDisk things even without RAM_SUPPORT
 *
 *    Revision 1.30  2003/04/25 19:31:30  bfo
 *    Handle Netatalk IDs out of range correctly now (using FD_ID)
 *
 *    Revision 1.29  2002/11/06 20:08:23  bfo
 *    zero is zero for os9_word/os9_long
 *
 *    Revision 1.28  2002/10/27 23:16:39  bfo
 *    get_mem/release_mem no longer with param <mac_asHandle>
 *
 *    Revision 1.27  2002/10/15 17:58:21  bfo
 *    'CutUp' visible again (for linuxfiles.c)
 *
 *    Revision 1.26  2002/10/02 18:52:54  bfo
 *    GetScreen function is now defined at "utilstuff"
 *
 *    Revision 1.25  2002/09/22 20:54:34  bfo
 *    CutUp handling corrected /
 *    Min nr of dir entries is 2 ( ".." and "." )
 *
 *    Revision 1.24  2002/09/19 21:59:39  bfo
 *    "CutUp" invisible, "CutUp"/"Eatback" adapted for Win/Mac + OS9
 *
 *    Revision 1.23  2002/09/11 17:22:51  bfo
 *    os9_long_inc with unsigned int* param
 *
 *    Revision 1.22  2002/08/13 21:24:17  bfo
 *    Some more variables defined at the real procid struct now.
 *
 *    Revision 1.21  2002/08/13 15:15:23  bfo
 *    The state <dead> will be handled correctly now (state=0x9100)
 *
 *    Revision 1.20  2002/08/09 22:39:21  bfo
 *    New procedure set_os9_state introduced and adapted everywhere
 *
 *    Revision 1.19  2002/08/06 07:59:45  luz
 *    Fixed "Retry/Abort" pop-up dialog
 *
 *    Revision 1.18  2002/07/30 16:45:37  bfo
 *    E-Mail address beat.forster@ggaweb.ch is updated everywhere
 *
 *    Revision 1.17  2002/07/24 22:33:53  bfo
 *    Timer synchronisation enhanced
 *
 *    Revision 1.16  2002/07/23 19:40:33  bfo
 *    go back to the 1/60th seconds system, synchronise with <sycorr>
 *
 *    Revision 1.15  2002/07/21 15:11:13  bfo
 *    Up to date again
 *
 *    Revision 1.14  2002/07/21 14:37:16  bfo
 *    Extended 'Get_Time' function
 *
 *    Revision 1.13  2002/07/07 22:08:21  bfo
 *    take the RAM disk size from the descriptor (if available)
 *
 *    Revision 1.12  2002/07/06 15:56:42  bfo
 *    ram disks (also other names than /r0) can be mounted/unmounted with "mount"
 *    the size -r=<size> is prepared, but not yet active
 *
 *    Revision 1.11  2002/07/05 22:45:54  uid82848
 *    E_FULL problem fixed
 *
 *    Revision 1.10  2002/07/03 21:41:47  bfo
 *    RAM disk is now operational ( currently fixed to 2MB )
 *
 *    Revision 1.9  2002/07/02 18:55:39  bfo
 *    Preparations done for RAM disk /r0 implementation
 *
 *    Revision 1.8  2002/07/01 21:28:11  bfo
 *    p2cstr and c2pstr implemented correctly for Carbon version
 *
 *    Revision 1.7  2002/06/30 10:41:56  bfo
 *    check, if KeyToBuffer might cause an overflow -> seems not to be the case
 *
 *    Revision 1.6  2002/06/25 20:44:33  luz
 *    Added /lp printer support under windows. Not tested or even compiled for Mac
 *
 */

#include "os9exec_incl.h"

/* General Utility routines */
/* ======================== */


#include <utime.h>

                

char* nullterm( char* s1, const char* s2, ushort max )
/* create null terminated version of s2. All chars<=SPACE will terminate s2
 * returns updated pointer (first char after end of string)
 */
{
    char *res= s1;
    ushort  n=  0;
    
    regcheck( currentpid,"nullterm inptr",(ulong)s2,RCHK_ARU+RCHK_MEM );    
    while(*s2>' ') {
        if (n++<max) *s1++= *s2++; /* don't use max-- structure any more */
        else                 s2++; /* don't copy more, simply increment */
    } /* while */
   
    *s1= NUL; /* terminate */
    trigcheck( "nullterm",res ); /* check for name trigger */
    return (char*)s2;
} /* nullterm */


/*
int ustrcmp( const char *s1,const char *s2 )
// case insensitive version of strcmp
//  Input  : *s1,*s2 = strings to be compared
//  Result : -1:s1<s2, 0:s1=s2, 1:s1>s2
//
{
    int  diff;
    char c;

    do {
        if ((diff=toupper(c=*(s1++))
                 -toupper(  *(s2++)))!=0) return diff>0 ?1:-1;
    } while (c!=NUL);

    return 0; // equal
} // ustrcmp
*/


int pustrcmp( const char *s1,const char *s2 )
/* case insensitive version of strcmp
 *  using PASCAL notation
 *  Input  : *s1,*s2 = strings to be compared
 *  Result : -1:s1<s2, 0:s1=s2, 1:s1>s2
 */
{
   int  len, diff, n;
   int  len1= s1[0];
   int  len2= s2[0];
   char c;

   if (len1<len2) len= len1;
                  len= len2;

   *(s1++); /* start with 1st char */
   *(s2++);
   
   for (n=1; n<=len; n++) {
      if ((diff=toupper(c=*(s1++))
               -toupper(  *(s2++)))!=0) return diff>0 ? 1:-1;
   }

   if (len1<len2) return -1;
   if (len1>len2) return  1;

   return 0; /* equal */
} /* pustrcmp */



int ustrncmp( const char *s1, const char *s2, ushort n )
/* case insensitive version of strcmp
 *  Input  : *s1,*s2 = strings to be compared
 *  Result : -1:s1<s2, 0:s1=s2, 1:s1>s2
 */
{
    int diff;
    char c;

    while (n-- > 0) {
        if ((diff=toupper(c=*(s1++))
                 -toupper(  *(s2++)))!=0) return diff>0 ? 1:-1;
        if (c==NUL) break;
    }
   
    return 0; /* equal */
} /* ustrncmp */



void os9_long_inc( unsigned int* a, ulong increment )
/* increment ulong field <a> dependent on big/little endian */
{   *a= os9_long( os9_long( *a ) + increment );
} /* os9_long_inc */



void set_os9_state( ushort cpid, pstate_typ state, const char* callingProc )
/* convert the state into OS-9 notation */
{
    process_typ* cp= &procs[cpid];
    procid*      pd= &cp->pd;
    if   (state==cp->state) return; // no change, ignore
    
    debugprintf(dbgTaskSwitch,dbgDetail,("#    state %d -> %d '%s'\n", 
                                           cp->state, state, callingProc ));
                                           
            cp->state= state;
    switch (cp->state) {
        case pStart    : pd->_state= os9_word(0x8800); pd->_queueid= 'b'; break;
        case pUnused   : pd->_state= 0;                pd->_queueid= '-'; break;
        case pActive   : pd->_state= os9_word(0x8800); pd->_queueid= 'a'; break;
        case pDead     : pd->_state= os9_word(0x9100); pd->_queueid= '-'; break;
        case pSleeping : pd->_state= os9_word(0xA000); pd->_queueid= 's'; break;
        case pWaiting  : pd->_state= os9_word(0x8000); pd->_queueid= 'w'; break;
        case pSysTask  : pd->_state= 0;                pd->_queueid= 't'; break;
        case pWaitRead : pd->_state= os9_word(0xA000); pd->_queueid= 'r'; break;
        default        : pd->_state= 0;                pd->_queueid= '?';
    } // switch
    
    if (cp->isIntUtil) { pd->_state = 0;               pd->_queueid = 'i'; }
} /* set_os9_state */



/* generates OS9 error (and saves traceback values) */
os9err os9error(os9err err)
{
    int dbg;
    process_typ* cp;
    char* name;

    errpid= currentpid; /* remember */
    
    if (debugcheck(dbgErrors,dbgNorm)) {
            cp=&procs[errpid];  dbg= dbgNorm;
        if (cp->state==pWaitRead) dbg= dbgDetail;
        
        get_error_strings(err, &name,NULL);
        debugprintf(dbgErrors,dbg,("# ** Error #%03d:%03d (%s), lastsyscall=%s, currentpid=%d\n",
                                          err>>8, err & 0xFF,name, 
                                          get_syscall_name(cp->lastsyscall), errpid ));
        if (lastpathparsed!=NULL) {
            debugprintf(dbgErrors,dbgDetail,("#    last path parsed=%s\n",lastpathparsed));
        } // if
    } // if

    return err;
} /* os9error */



/* translates mac OS error into OS-9 error codes */
os9err host2os9err(OSErr hosterr,ushort suggestion)
{
    os9err err;
    Boolean known;
   
    known=true;

    #ifdef MACOS9
      if (hosterr==noErr) return 0;
    
      switch (hosterr) {
        case opWrErr            : /* same */
        case fBsyErr            : err=E_SHARE;  break;
        case fLckdErr           : err=E_FNA;    break;
        case dirNFErr           : /* same */
        case fnfErr             : err=E_PNNF;   break;
        case dskFulErr          : err=E_FULL;   break;
        case eofErr             : err=E_EOF;    break;
        case dirFulErr          : err=E_SLF;    break;
        case tmfoErr            : err=E_PTHFUL; break;
        case afpAccessDenied    : /* same */
        case permErr            : /* same */
        case wrPermErr          : err=E_FNA;    break;
        case vLckdErr           : /* same */
        case wPrErr             : err=E_WP;     break;
        case nsvErr             : err=E_UNIT;   break;
        case ioErr              : err=E_HARDWARE; break;
        case rfNumErr           : /* same */
        case fnOpnErr           : err=E_BPNUM;  break;
        case posErr             : err=E_NES;    break;
        case bdNamErr           : err=E_BPNAM;  break;
        case mFulErr            : err=E_NORAM;  break;
        case afpObjectTypeErr   : /* same */
        case dupFNErr           : err=E_CEF;    break;
        case gfpErr             : err=E_SEEK;   break;
        case readErr            : err=E_READ;   break;
        case abortErr           : err=E_PRCABT; break;
        default                 : err=suggestion; known=false;break;
      }
      debugprintf(dbgErrors,dbgNorm,("# ** Mac OSerr=%d%s\n",
                                      hosterr,known ? "" : "(not known, using suggestion)"));

    #elif defined(windows32)
      if     (hosterr==NOERROR) return 0;
      switch (hosterr) {
        case ERROR_PATH_NOT_FOUND: /* same */

        case ERROR_FILE_NOT_FOUND:         err=E_PNNF; break;

        case ERROR_NO_MORE_FILES:
        case ERROR_TOO_MANY_OPEN_FILES:    err=E_PTHFUL; break;

        case ERROR_OPEN_FAILED:
        case ERROR_NETWORK_ACCESS_DENIED:
        case ERROR_INVALID_ACCESS:
        case ERROR_SHARING_VIOLATION:
        case ERROR_NOACCESS:
        case ERROR_CANTOPEN:
        case ERROR_FILE_INVALID:
        case ERROR_ACCESS_DENIED:          err=E_FNA; break;

        case ERROR_INVALID_HANDLE:         err=E_BPNUM; break;

        case ERROR_INSUFFICIENT_BUFFER:
        case ERROR_BUFFER_OVERFLOW:
        case ERROR_STACK_OVERFLOW:
        case ERROR_OUTOFMEMORY:
        case ERROR_NOT_ENOUGH_MEMORY:      err=E_NORAM; break;

        case ERROR_PATH_BUSY:
        case ERROR_LOCK_VIOLATION:         err=E_SHARE; break;

        case ERROR_WRITE_PROTECT:          err=E_WP; break;

        case ERROR_DUP_NAME:
        case ERROR_FILE_EXISTS:            err=E_CEF; break;

        case ERROR_BAD_NETPATH:            err=E_BPNAM; break;

        case ERROR_NOT_READY:              err=E_NOTRDY; break;

        case ERROR_DRIVE_LOCKED:
        case ERROR_BUSY_DRIVE:             err=E_DEVBSY; break;

        case ERROR_SEEK:
        case ERROR_NEGATIVE_SEEK:
        case ERROR_SEEK_ON_DEVICE:
        case ERROR_INVALID_DRIVE:
        case ERROR_INVALID_BLOCK:
        case ERROR_SECTOR_NOT_FOUND:
        case ERROR_BAD_FORMAT:             err=E_SEEK; break;

        case ERROR_CRC:                    err=E_CRC; break;

        case ERROR_CANTWRITE:
        case ERROR_WRITE_FAULT:            err=E_WRITE; break;

        case ERROR_CANTREAD:
        case ERROR_READ_FAULT:             err=E_READ; break;

        case ERROR_HANDLE_EOF:             err=E_EOF; break;

        case ERROR_DISK_FULL:
        case ERROR_HANDLE_DISK_FULL:       err=E_FULL; break;

        case ERROR_DISK_CHANGE:            err=E_DIDC; break;

        case ERROR_CALL_NOT_IMPLEMENTED:   err=E_UNKSVC; break;

        case ERROR_UNRECOGNIZED_VOLUME:
        case ERROR_NO_VOLUME_LABEL:
        case ERROR_INVALID_NAME:           err=E_BNAM; break;

        case ERROR_DIR_NOT_EMPTY:          err=E_DNE; break;

        default: err=suggestion; known=false; break;
      }
      debugprintf(dbgErrors,dbgNorm,("# ** Win32 error=%d%s\n",
                                      hosterr,known ? "" : "(not known, using suggestion)"));

    #elif defined UNIX
      if     (hosterr==0) return 0;
      err= suggestion; known=false;
    
    #else
      #error Unknown Target OS, no error translation implemented
    #endif

    return os9error(err);
} /* host2os9err */



os9err c2os9err(int cliberr,ushort suggestion)
/* translates mac/C-library error into OS-9 error codes */
{
   os9err err;
   
   if (!cliberr) return 0;
   switch (cliberr) {
      #if !defined(__MWERKS__) && !defined(linux)
        /* %%% seems not to have ANY usable error codes ! */
        case EPERM  : err=E_FNA;      break;
        case ENOENT : err=E_PNNF;     break;
        case ENOSPC : err=E_FULL;     break;
        case EIO    : err=E_HARDWARE; break;
      #endif

      default       : err=suggestion; break;
   }
    debugprintf(dbgErrors,dbgNorm,("# ** c2os9err: C-lib-err=%d\n",cliberr));
   return os9error(err);
} /* c2os9err */



ulong j_date(int d, int m, int y)
/* this routine returns the number of days
 * since January 1, 4713 B.C.              
 * it is used by the F$Time call.
 * expected <d>: 1..31 / <m>: 1..12 / <y>: XXXX 
 */
{ 
    long fct= 365*y + 31*(m-1) + d;
    int  yb = y-1; /* the year before */;

    if (m==1 || m==2) fct+= (yb/4) - ( 3*((yb/100)+1) /4);
    else              fct+= (y /4) - ((3*((y /100)+1))/4) - (4*m+23)/10;
    
    return fct + DAYS_SINCE_0000; 
} /* j_date */



void g_date(ulong jdn, int *dp, int *mp, int *yp )
/* this routine returns the date from julian day number */
{ 
  long fct;
  int  d, m, y, yb, fb;
  int  marr[ 12 ];
  
  
  fct= jdn - DAYS_SINCE_0000;    /* see above */
  y  = 4*(fct+15) / ( 4*365+1 ); /* 100/400 year adaption: 15 days */
  yb = y-1;                      /* the year before */
  fct= fct - 365*y - yb/4 + yb/100 - yb/400; 
  
  fb=28; if ( (y % 4)==0 ) fb=29;
  
  marr[  1 ]= 31; /* not so sophisticated as the julian conversion */
  marr[  2 ]= fb;
  marr[  3 ]= 31;
  marr[  4 ]= 30;
  marr[  5 ]= 31;
  marr[  6 ]= 30;
  marr[  7 ]= 31;
  marr[  8 ]= 31;
  marr[  9 ]= 30;
  marr[ 10 ]= 31;
  marr[ 11 ]= 30;
/*marr[ 12 ]= 31*/;
  
  m= 1; d= fct;
  while ( ( m<12 ) & ( d>marr[ m ] ) ) {
                    d= d-marr[ m ]; m++; }
   
  *yp= y;
  *mp= m;
  *dp= d;
} /* g_date */



void TConv( time_t u, struct tm* tim )
/* time conversion, seems to be buggy under CW7 -> 70 year correction */
{
	struct tm *tp;

	#if __MWERKS__ >= CW7_MWERKS && !defined __MACH__
	  u -= (72*365+17)*SecsPerDay; /* corrected bug of CW7, leap years included */
	#endif
	
    tp= localtime( (time_t*)&u );
    memcpy( tim,tp, sizeof(struct tm) ); /* copy it, as it might be overwritten */
	
	#if __MWERKS__ >= CW7_MWERKS && !defined __MACH__
	  tim->tm_year += 2; 
	#endif
} /* TConv */



time_t UConv( struct tm* tim )
/* time conversion, seems to be buggy under CW7 -> 70 year correction */
{
	time_t u;
	
	#if __MWERKS__ >= CW7_MWERKS && !defined __MACH__
	  tim->tm_year -= 2; 
	#endif

    u= mktime( tim );              /* set modification time */

	#if __MWERKS__ >= CW7_MWERKS && !defined __MACH__
	  u += (72*365+17)*SecsPerDay; /* corrected bug of CW7, leap years included */
	#endif
	
	return u;
} /* UConv */



void GetTim( struct tm* tim )
/* time conversion, seems to be buggy under CW7 -> 70 year correction */
{
	time_t     u;
	struct tm* tp;
	
    time                   ( &u );       /* get the current time, it seems to be   */
    tp = localtime( (time_t*)&u );       /* based on 1900, not 1904, as file dates */
    memcpy( tim,tp, sizeof(struct tm) ); /* copy it, as it might be overwritten    */
} /* GetTim */



void Get_Time( ulong *cTime, ulong *cDate, int *dayOfWk, int *currentTick,
               Boolean asGregorian, Boolean withTicks )
{
    struct tm tim; /* Important Note: internal use of <tm> as done in OS-9 */
  //byte   tc[4];
  //ulong* tcp= (ulong*)&tc[0];
    ulong  ct0;
    int    y, m, d, tsm, ssm, syTick;
    
  //process_typ* cp= &procs[ currentpid ];
    
    /* Get consistent time/date and ticks */
    if      (withTicks) { syTick= GetSystemTick(); 
                    *currentTick= (syTick+syCorr) % TICKS_PER_SEC; }
    else            *currentTick= 0;
    
    do {
        if  (withTicks) ct0= *currentTick;
        
        GetTim( &tim );
        y=       tim.tm_year+1900;
        m=       tim.tm_mon +   1;
        d=       tim.tm_mday;
        
        if  (withTicks) { syTick= GetSystemTick();
                    *currentTick= (syTick+syCorr) % TICKS_PER_SEC; }
    } while (withTicks && *currentTick<ct0); // must be within the same second

    ssm= tim.tm_hour*3600
        +tim.tm_min *  60
        +tim.tm_sec;        /* seconds since midnight */
        
  //if (withTicks && cp->state!=pIntUtil) {
    if (withTicks) {
            tsm= (syTick+syCorr)/TICKS_PER_SEC;
        if (tsm>ssm) { 
        //upe_printf( "> %10d %10d %10d %10d %10d\n", tsm,ssm, syTick,syCorr,*currentTick );
          syCorr= (ssm+1)*TICKS_PER_SEC - syTick-1;
          *currentTick= (syTick+syCorr) % TICKS_PER_SEC;
        //upe_printf( "  %10d %10d %10d %10d %10d\n", tsm,ssm, syTick,syCorr,*currentTick );
        } // if
          
        if (tsm<ssm) {   
        //upe_printf( "< %10d %10d %10d %10d %10d\n", tsm,ssm, syTick,syCorr,*currentTick );
          syCorr=  ssm*   TICKS_PER_SEC - syTick;
          *currentTick= (syTick+syCorr) % TICKS_PER_SEC;
        //upe_printf( "  %10d %10d %10d %10d %10d\n", tsm,ssm, syTick,syCorr,*currentTick );
        } // if
    } // if
   
    if (asGregorian) {
		/* gregorian format */
		    *cTime= 0x10000*tim.tm_hour + 
		              0x100*tim.tm_min  +
		                    tim.tm_sec;
		    /*
        tc[0]= 0;
      	tc[1]= tim.tm_hour;
      	tc[2]= tim.tm_min;
      	tc[3]= tim.tm_sec;
      	*cTime= os9_long( *tcp );
        */
        
		    *cDate= 0x10000*y + 
		              0x100*m +
		                    d;
        /*
      	*((ushort*) &tc[0])= os9_word( y );
      	tc[2]= m;
      	tc[3]= d;
      	*cDate= os9_long( *tcp );
      	*/
      //debugprintf(dbgSysCall,dbgNorm,
      //       ("here we are: hour=%d %08X\n", tim.tm_hour, *cTime));      
    }
    else {
      	/* julian format */
      	*cTime= ssm;             /* seconds since midnight */
      	*cDate= j_date( d,m,y ); /* julian date, intenral clock starts 1904 */
    } // if

    *dayOfWk= tim.tm_wday; /* day of week, 0=sunday, 1=monday... */
} /* Get_Time */



ulong GetScreen( char mode )
/* Get screen dimensions: 'w'=width, 'h'=height */
{
    int r= 0; // no information, return zero (full screen)
    
    #ifdef windows32
      HWND dwh = GetDesktopWindow();
      RECT screenrec;

      if (GetWindowRect(dwh,&screenrec)) {
    	  switch (mode) {
    		  case 'w': r= screenrec.right -screenrec.left; break; /* return width  */ 
    		  case 'h': r= screenrec.bottom-screenrec.top;  break; /* return height */
    	  } 
      }
    #else
	  switch (mode) {
		  case 'w': r= 1600; break; /* return width  */ 
		  case 'h': r= 1200; break; /* return height */
	  } 
    #endif
    
    return (ulong)r;
} /* GetScreen */



/* ------------------------------------------------------------------------ */
#ifdef USE_CARBON
  char* p2cstr( unsigned char* s )
  {
      int ii;
      int n= (int)s[0];
      
      for  (ii=0;  ii<n; ii++) {
          s[ii]= s[ii+1];
      }
      s[n]= 0;
      
      return s;
  } /* p2cstr */


  unsigned char* c2pstr( char* s )
  {
      int ii;
      int n= 0;
      
      while (s[n]!=0) n++;
      for  (ii=n;  ii>0; ii--) {
          s[ii]= s[ii-1];
      }
      s[0]= (char)n;
      
      return s;
  } /* c2pstr */
#endif



ulong Min( ulong a, ulong b )
/* returns the smaller of two ulongs */
{   if (a<b) return a;
    else     return b;
} /* Min */


ulong Max( ulong a, ulong b )
/* returns the larger of two ulongs */
{   if (a>b) return a;
    else     return b;
} /* Max */



char* StrBlk_Pt( char* s, int n )
{   
    if (strlen( s )>n) { s[n-2]= NUL; strcat( s,".." ); }
    return s;
} /* StrBlk_Pt */



Boolean IsRead( ushort mode )
/* returns true, if <mode> has read bit set */
{   return (mode & poRead)!=0;
} /* IsRead */

Boolean IsWrite( ushort mode )
/* returns true, if <mode> has write bit set */
{   return (mode & poWrite)!=0;
} /* IsWrite */

Boolean IsRW( ushort mode )
/* returns true, if <mode> has read or write bit set */
{   return IsRead(mode) && IsWrite(mode);
} /* IsRW */

Boolean IsExec( ushort mode )
/* returns true, if <mode> has execution bit set */
{   return (mode & poExec)!=0;
} /* IsExec */

Boolean IsDir( ushort mode )
/* returns true, if <mode> has directory bit set */
{   return (mode & poDir)!=0;
} /* IsDir */

Boolean IsCrea( ushort mode )
/* returns true, if <mode> has creation bit set */
{   return (mode & poCreateMask)!=0;
} /* IsCrea */



Boolean IsTrDir( ushort umode )
/* returns true, if <mode> has dir bit set */
{
    #ifdef windows32
      return umode==0x4E00;
    #else
      return S_ISDIR( umode ); /* it is a directory ? */
    #endif
} /* IsTrDir */



/* ------------------------------------------------------------------------ */
/* default options for Console/non-Console SCF I$GetStt */
static struct _sgs init_consoleopts = {
        0,      /* PD_DTP   file manager class code 0=SCF */
        0,      /* PD_UPC   0 = upper and lower cases, 1 = upper case only */
        1,      /* PD_BSO   0 = BSE, 1 = BSE-SP-BSE */
        0,      /* PD_DLO   delete sequence */
        1,      /* PD_EKO   0 = no echo */
        1,      /* PD_ALF   0 = no auto line feed */
        0,      /* PD_NUL   end of line null count */
        0,      /* PD_PAU   0 = no end of page pause */
        24,     /* PD_PAG   lines per page */
        0x08,   /* PD_BSP   backspace character */
        0x18,   /* PD_DEL   delete line character */
        CR,     /* PD_EOR   end of record character */
        0x1B,   /* PD_EOF   end of file character */
        0x04,   /* PD_RPR   reprint line character */
        CtrlA,  /* PD_DUP   duplicate last line character */
        0x17,   /* PD_PSC   pause character */
        CtrlC,  /* PD_INT   keyboard interrupt character */
        CtrlE,  /* PD_QUT   keyboard abort character */
        0x08,   /* PD_BSE   backspace echo character */
        0x07,   /* PD_OVF   line overflow character (bell) */
        0,      /* PD_PAR   device initialization (parity) */
        0x0F,   /* PD_BAU   baud rate */
        0x0000, /* PD_D2P   offset to output device name string */
        XOn,    /* PD_XON   x-on char */
        XOff,   /* PD_XOFF  x-off char */
        0x09,   /* PD_TAB   tab character */
        4,      /* PD_TABS  tab size */
    0x00000000, /* PD_TBL   Device table address (copy) */
        0x0001, /* PD_Col   Current column number */
        0,      /* PD_Err   most recent error status */
        0       /*          _sgs_spare[93]; spare bytes */
};

/* get options from SCF device */
os9err pSCFopt( _pid_, _spP_, byte* buffer )
{   memcpy( buffer, &init_consoleopts, OPTSECTSIZE ); return 0;
} /* pSCFopt */



/* ------------------------------------------------------------------------ */
/* constant option sections for RBF I$GetStt */
               /* dev dsc / path */
const byte rbfstdopts[OPTSECTSIZE]= 
              { 1, /* $48   $80  PD_DTP: 1=RBF */
                0, /* $49   $81  PD_DRV: drive number */
                0, /* $4A   $82  PD_STP */
                0, /* $4B   $83  PD_TYP: floppy type */
                2, /* $4C   $84  PD_DNS */
                0, /* $4D   $85  reserved */
            0,128, /* $4E   $86  PD_CYL */
                1, /* $50   $88  PD_SID */
                0, /* $51   $89  PD_VFY */
   0, DEFAULT_SCT, /* $52   $8A  PD_SCT: ignored, because PD_Cntl is used */
   0, DEFAULT_SCT, /* $54   $8C  PD_T0S: ignored, because PD_Cntl is used */
   0,RBF_MINALLOC, /* $56   $8E  PD_SAS: segment allocaton size */
                0, /* $58   $90  PD_ILV */
                0, /* $59   $91  PD_TFM */
                0, /* $5A   $92  PD_TOffs */
                0, /* $5B   $93  PD_SOffs */
              1,0, /* $5C   $94  PD_SSize default: set to 256 */
              0,9, /* $5E   $96  PD_Cntl: automatically */
                              
              0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,
              0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,
              0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0 };
              
os9err pRBFopt( _pid_, _spP_, byte* buffer )
/* get standard options for RBF file */
{   memcpy( buffer, rbfstdopts, OPTSECTSIZE ); return 0;
} /* pRBFopt */

/* ------------------------------------------------------------------------ */



/* fill <key> to buffer and make special key handling */
Boolean KeyToBuffer( ttydev_typ* mco, char key )
{
    char pd_int = mco->spP->opt[ PD_INT  ]; /* get special chars from opt sct */
    char pd_qut = mco->spP->opt[ PD_QUT  ];
    char pd_xon = mco->spP->opt[ PD_XON  ];
    char pd_xoff= mco->spP->opt[ PD_XOFF ];

    int lwp= mco->spP->lastwritten_pid;        /* where to send the signal ? */
    if     ( mco->inBufUsed >= INBUFSIZE-1 ) {
    /*  printf( "buffer is full %s\n", mco->spP->name ); */
        return false; /* buffer is full */
    }
    
    /* these characters will be eaten before they reach the input buffer */
    /* treatment for special chars */
    if     (key!=NUL) {
        if (key==pd_int)  { if (lwp) send_signal( lwp, S_Intrpt ); return 0; }
        if (key==pd_qut)  { if (lwp) send_signal( lwp, S_Abort  ); return 0; }
        if (key==pd_xon)  { mco->holdScreen= false;                return 0; }
        if (key==pd_xoff) { mco->holdScreen=  true;                return 0; }
    }
            
    mco->inBuf[ mco->inBufUsed++ ]= key; /* update the buffer */
    return true;
} /* KeyToBuffer */



void LastCh_Bit7( char* name, Boolean setIt )
/* adapt the dir entry's last char to normal/dir style depending on <setIt> */
{
    char*    c= name;
    if     (*c==NUL) return; /* do nothing for the empty string */
    while  (*c!=NUL) c++;    /* search for the last char in string */
    
                c--;
    if (setIt) *c= *c | 0x80;
    else       *c= *c & 0x7f;
} /* LastCh_Bit7 */



void Console_Name( int term_id, char* consname )
{
    char    *p;
    Boolean isTTY;
    
    switch(term_id) {         /* allowed terminal names */
        case    Main_ID: p="term"; break;
        case SerialA_ID: p="ts1";  break;
        case SerialB_ID: p="ts2";  break;
        case   VModBase: p="vmod"; break;

        default        : p= consname;  /* memory must exist */
              isTTY= (term_id>=TTY_Base);
          if (isTTY) sprintf( p,"%s%02d","tty",term_id ); /* tty     */
          else       sprintf( p,"%s%d"  ,"t",  term_id ); /* console */
    }

    strcpy(consname,p);
} /* Console_Name */



char* OS9exec_Name( void )
{
    static char theName[15];
    
    sprintf( theName, "OS9exec V%x.%02x", exec_version,exec_revision );
    return   theName;
} /* OS9exec_Name */


ulong Pipe_NReady( pipechan_typ* p )
{
  ulong  n= p->pwp-p->prp;
  if       (p->pwp<p->prp) n+= p->size; /* wrapper */
  return n;
} /* Pipe_NReady */



/* --------------------------------------------------------------- */
Boolean AbsPath( const char* pathname )
/* returns true if <pathname> is an absolute path */
{   return *pathname==PSEP;
} /* AbsPath */



void GetOS9Dev( const char* pathname, char* cmp_entry )
/* gets the next subpath of <p> into <cmp_entry> */
{
    char*       c= cmp_entry;
    const char* p= pathname;
    
    if (AbsPath( p )) { /* only searching absolute paths */
        p++;
        while  (*p!=NUL && *p!=PSEP && *p!='@') {
            *c= *p; 
            c++; p++;
        } /* while */
    }
    
    *c= NUL;
} /* GetOS9Dev */



Boolean IsWhat( const char* pathname, Boolean isRoot )
/* Test if <pathname> is a root/raw path */
{   
    char* p= (char*)pathname;

    /* root/raw must be absolute path */
    if (!isRoot && strcmp( p,"@" )==0) return true; /* my own device raw */ 
    if (!AbsPath( p )) return false;
    
    p++;
    while  (*p!=NUL) { /* further slashes are not allowed */
        if (*p=='@') { p++; break; } /* will be decremented again */
        if (*p==PSEP) return false;
        p++;
    } /* while */

    p--;
    if (isRoot) return *p!='@';
    else        return *p=='@';
} /* IsWhat */


Boolean IsRoot( const char* pathname )
/* Test if <pathname> is a root path */
{   return IsWhat( pathname,true );
} /* IsRoot */


Boolean IsRaw( const char* pathname )
/* Test if <pathname> is a raw path */
{   return IsWhat( pathname,false );
} /* IsRaw */



/* --------------------------------------------------------------- */
Boolean VolInfo( const char* pathname, char* volname )
{
    Boolean ok= true;

    #ifdef macintosh
      #pragma unused(pathname)
      strcpy( volname,"" );
      
    #elif defined linux
      strcpy( volname,"/" );

    #elif defined(windows32)
      char    sysname[OS9NAMELEN];
      DWORD   serno,maxComp,sysFlags;
      
      ok= GetVolumeInformation( pathname,  volname, OS9NAMELEN, 
               &serno,&maxComp,&sysFlags, &sysname, OS9NAMELEN );
      if (!ok ||  *volname==NUL)
          sprintf( volname, "%c:", toupper(pathname[0]) );  
    #endif
    
    return ok;
} /* VolInfo */



Boolean OpenTDir( const char* pathname, DIR** d )
/* Open Directory with special treatment of empty root dir on windows */
{
    #ifdef MACOS9
      struct stat info;

      *d= NULL; /* not used */
      return stat_( pathname, &info )==0 && IsTrDir(info.st_mode);

    #else
      Boolean ok= false;
      
      #ifdef TFS_SUPPORT
        #ifdef windows32
          char volname[OS9NAMELEN];
        #endif
        
             *d= (DIR*)opendir( pathname ); /* try to open */
        ok= (*d!=NULL);

        #ifdef windows32
          /* special handling for empty root directory */
          if (!ok) ok= VolInfo( pathname, &volname );
        #endif
      #endif
      
      return ok;
    #endif
} /* OpenTDir */



dirent_typ* ReadTDir( DIR* d )
{
    #ifdef MACOS9
      #pragma unused(d)
      return NULL;
    
    #else
      if (d==NULL) return NULL;
      return readdir( d );
    #endif
} /* ReadTDir */



Boolean PathFound( const char* pathname )
/* Check if this entry is a directory */
{
    DIR* d;
    Boolean ok= OpenTDir( pathname, &d );
    
    #if defined win_unix && defined TFS_SUPPORT
      if (d!=NULL) closedir( d );
    #endif
    
    debugprintf( dbgFiles,dbgNorm,("# PathFound %s '%s'\n", 
                                      ok ?" (ok)":"(err)", pathname ));
    return ok;
} /* PathFound */



Boolean FileFound( const char* pathname )
/* Check if this entry is a file */
{
    #ifdef windows32
    DWORD winerr;
    #endif
    FILE*        f= fopen( pathname,"rb" ); /* try to open for read */
    Boolean ok= (f!=NULL);
    if     (ok) fclose( f ); /* don't use it any more */

    #ifdef windows32
      if (!ok) {
        winerr=GetLastError();
        if (winerr==ERROR_SHARING_VIOLATION) ok= true;
      }
    #endif
    
    debugprintf( dbgFiles,dbgNorm,("# FileFound %s '%s'\n", 
                                      ok ?" (ok)":"(err)", pathname ));
    return ok;
} /* FileFound */



#ifdef linux    
  void include_2e( char* filename, char* pos )
  {
      char  tmp[OS9PATHLEN];
    
      *(pos)= NUL;
      strcpy( tmp,&pos[1] );
      strcat( filename,L_P );
      strcat( filename,tmp );
  } /* include_2e */
#endif



void CutUp( char* pathname, const char* prev )
/* cut out /xxxx/../ sequences */
{
    char *v, *q, *qs;
    Boolean inc;

    v= pathname;
    while (true) {
        q =   strstr( v,prev ); if (q==NULL) break; /* search the string */
        qs= q+strlen(   prev );
        
        inc= false;
        switch (*qs) {
            case NUL      : *q= NUL; break; /* cut "/."  at the end */
            
            /* avoid duplicate definition */
            #ifndef UNIX
            case PATHDELIM:
            #endif
            case PSEP     : *q= NUL; strcat( pathname,qs ); break; /* cut "/./" anywhere */

            case '.'      : v= qs;
                            while  (*(++v)=='.') {};
                            switch (*v) {
                                case NUL:

                                /* avoid duplicate definition */
                                #ifndef UNIX
                                case PATHDELIM:
                                #endif
                                case PSEP     : while   (q>pathname) {
                                                         q--;
                                                    if (*q==PATHDELIM || *q==PSEP) break;
                                                } /* while */

                                                *q= NUL; /* concatenate at the new position */
                                                strcat( pathname,++qs );
                                                break;

                                default:        q++; inc= true;
                            } /* switch */
                            break;
                            
            default:        q++; inc= true;
        } /* switch */
      
        #ifdef linux
          if (inc) include_2e( pathname, q );
        #endif
        
        debugprintf( dbgFiles,dbgNorm,("# AdjustPath REDU '%s'\n", pathname ));
        v= q;
    } /* while */
} /* CutUp */



void EatBack( char* pathname )
{
    #define Prev  "/."
    #define PrevW "\\." /* Windows32 version */
    char*   p;
    
    int     eat = 0;
    int     eat0= 0;
    Boolean searchP= true;
    
    p= pathname+strlen(pathname)-1; /* start at end of string */

    while (p>pathname) {
        switch (*p) {
            case '.'      : if (searchP)    eat++;                      break;
            
            /* avoid duplicate definition */
            #ifndef UNIX
            case PATHDELIM:
            #endif
            case PSEP     : *p= NUL; eat--; eat0= eat;  searchP=  true; break;
            
            #ifdef windows32
              case ':'    : p++;     eat=      0;                       break;
            #endif
            
            default       :          eat=   eat0;       searchP= false; break;
        } /* switch */

        p--;
        if (eat<=0) break;
    }
    
    #ifdef windows32
      if (*p==':') { *++p= PATHDELIM; *++p= NUL; }
    #endif
          
    CutUp( pathname, Prev ); /* support also for RBF OS-9 paths */
    
    #ifdef windows32
      CutUp( pathname, PrevW );
    #endif
} /* EatBack */


// Take the CRC algorithm as hash function
//static int HashF( char* name, char* fName )
static int HashF( char* name )
{
  const ulong AccStart= 0xffffffff;
  ulong rslt;
  int  i, len;  
  char ups[MAX_PATH]; // let the original be case sensitive for Linux
  
               len= strlen( name );  
  for (i= 0; i<len+1; i++) {
    ups[ i ]= toupper( name[ i ] ); // make comparisons more "the same"
  } // for
  
  
  rslt = AccStart;
  rslt = calc_crc( (byte*)&ups, len,  rslt );
  rslt^= AccStart;
  rslt = rslt % MAXDIRS;
  
  if    (rslt==0) rslt++; // do not allow 0
  return rslt;
} // HashF


os9err FD_ID( syspath_typ* spP, const char* pathname, dirent_typ* dEnt, 
              ulong *id, long dirid, char* fName, Boolean isFirst, Boolean useInodes )
{
  #ifdef MACOS9
    #pragma unused(spP,dEnt,isFirst,useInodes)
  #endif

  #define ATTR_DIR 0x0010
  char      tmp[MAX_PATH];
  int       ii, hh, n;
  direntry* m;
  Boolean   doit;
  ulong     liCnt= 0;
  
  #define MAXLICNT ( 0x00800000 / MAXDIRS )
    
//upe_printf( "REIN name='%s'\n", pathname );
  #if defined UNIX
    // .. when using Inodes
    if (useInodes) {
      if (spP!=NULL) {
        while (dEnt!=NULL && isFirst && strcmp( dEnt->d_name,"." )!=0) {
               dEnt= ReadTDir( spP->dDsc ); 
        } // while
      } // if
      
      if (dEnt==NULL) *id= 0;
      else            *id= dEnt->d_ino;
    
      #if defined __MACH__
        if (*id==1) *id= 2; // adapt for top dir searching
      #endif
          
      return 0;
    }
  #endif

  strcpy( tmp,pathname );
      
  #if defined win_unix
    if     ( tmp[strlen( tmp )-1]!=PATHDELIM ) strcat( tmp,PATHDELIM_STR );
    strcat ( tmp, dEnt->d_name );
    EatBack( tmp );
  #endif
//upe_printf( "REIN name='%s'\n", tmp );
                
//if (dEnt->data.dwFileAttributes!=ATTR_DIR) return id;
      
  *id= 0; /* undefined */
    //hh= HashF( tmp, fName );
      hh= HashF( tmp );
  ii= hh;
//upe_printf( "id=%08X <= name='%s' START\n", ii, tmp );
      
  n= 1; m= &dirtable[ ii ];
  while (true) {
  //upe_printf( "m->ident=%08X\n", m->ident );
    if (m->ident==NULL) {
                m->ident= malloc( strlen( tmp )+1 );
      strcpy  ( m->ident,                 tmp );
              
      #ifdef MACOS9
                m->fName= malloc( strlen( fName )+1 );
        strcpy( m->fName,                 fName );
      #endif
          
      if (liCnt==0) hittable[ 0 ]--; // adapt statistics
                    hittable[ n ]++;            
    } /* if */
          
    #ifdef MACOS9
      if (dirid!=0) m->dirid= dirid;
    #endif
          
    if (ustrcmp( m->ident, tmp )==0) {
      *id= (ulong)ii;
      break;
    } /* if */

    #ifdef LINKED_HASH
                liCnt++;
          doit= liCnt>=MAXLICNT;
      if (doit) liCnt= 0;
    #else
      doit= true;
    #endif
  //upe_printf( "liCnt=%d\n", liCnt );
    
    if (doit) {    
          ii++;
      if (ii==MAXDIRS) ii= 1;
      if (ii==hh) break;
      m= &dirtable[ ii ];
    } 
    else {
      #ifdef LINKED_HASH
            doit= m->next==NULL;
        if (doit) m->next= malloc( sizeof( direntry ) );
        m=        m->next;
        
        if (doit) {
          m->ident= NULL;

          #ifdef MACOS9
          m->fName= NULL;
          #endif

          m->next = NULL;
        } // if
      #endif
    } // if
    
    if (n<MAXDIRHIT-1) n++; 
  } /* loop */
  if (*id==0) return E_NORAM;
  
  *id+= liCnt*MAXDIRS;
    
//upe_printf( "id=%08X <= name='%s'\n", *id, tmp );
  return 0;
} /* FD_ID */



os9err FD_Name( long fdID, char** pathnameP )
// get back the real <volID> and <objID> for Mac file system
{
  os9err    err= 0;
  direntry* m;
  long      id= fdID;
  
  #ifdef LINKED_HASH
    int   i;
    ulong liCnt= fdID / MAXDIRS;
          fdID = fdID % MAXDIRS;
  #endif

//upe_printf( "REIN id=%08X, liCnt=%d fdID=%08X\n", id, liCnt, fdID );
   
  *pathnameP= NULL;
  if   (fdID>0 && fdID<MAXDIRS) {       // if the range is correct
    m= &dirtable[ fdID ]; // get the entry directly
  
    #ifdef LINKED_HASH
      for (i= 0; i<liCnt; i++) {
        if (m) m= m->next;
      } // for
    #endif
    
    if      (m && m->ident!=NULL) {
      *pathnameP= m->ident;
    } // if
  } // if
  
  if (*pathnameP==NULL) err= E_PNNF;  
//upe_printf( "id=%08X => name='%s' err=%d\n", fdID, *pathnameP ? *pathnameP : "", err );
  return err;
} // FD_Name

  
  
#ifdef win_unix
os9err DirNthEntry( syspath_typ* spP, int n, dirent_typ** dEnt )
/* prepare directory to read the <n>th entry */
{   
//int m= n;
  int i= 0;
  
  /*  
  debugprintf(dbgFiles,dbgDetail,("# DirEntry: ---\n" )); 
  seekD0( spP );
  while (m>0) {
        *dEnt= ReadTDir( spP->dDsc ); 
    if (*dEnt==NULL) break;
    debugprintf(dbgFiles,dbgDetail,("# DirEntry: '%s'\n", (*dEnt)->d_name )); 
    m--;
  } // while
  debugprintf(dbgFiles,dbgDetail,("# DirEntry: ---\n" )); 
  */
  
  do {
    if (n>2) {
             i=            spP->svD_n;
      if (n==i  ) { *dEnt= spP->svD_dEnt; break; } // still the same
      if (n!=i+1) i= 0;                            // not the next one
    } // if
  
  //if (i==0) { seekD0( spP ); i= 1; }  // start at the beginning

    if (n<=2 || i==0) { 
      seekD0( spP );
      i= 1; // ignore ".." and "." entries for n>=2
    } // if
    
    if (n==0) {     // search for ".."
      do     *dEnt= ReadTDir( spP->dDsc ); 
      while (*dEnt!=NULL && strcmp( (*dEnt)->d_name,".." )!=0);

      break;
    } // if

    if (n==1) {     // search for "."
      do     *dEnt= ReadTDir( spP->dDsc ); 
      while (*dEnt!=NULL && strcmp( (*dEnt)->d_name,"."  )!=0);

      break;
    } // if
    
  //#ifdef windows32 // missing top dir entry
    /*
    if (n==2 || i==0) { 
      seekD0( spP );
      i= 1; // ignore ".." and "." entries
    } // if
    */
  //#endif
    
    // starting after ".." and "."
    do {  *dEnt= ReadTDir( spP->dDsc );
      if (*dEnt==NULL) break;
      
      if (strcmp( (*dEnt)->d_name,".."  )!=0 &&
          strcmp( (*dEnt)->d_name,"."   )!=0 &&
         ustrcmp( (*dEnt)->d_name,AppDo )!=0 &&
         ustrcmp( (*dEnt)->d_name,DsSto )!=0) i++;
    } while ( i<n );
  } while (false);
  
  spP->svD_n   =     n;
  spP->svD_dEnt= *dEnt;
  
  if (*dEnt==NULL) return E_EOF;
  else             return 0;
} /* DirNthEntry */



os9err RemoveAppledouble( syspath_typ* spP )
{
    os9err      err;
    DIR*        app_d;
    dirent_typ* dEnt;
    char        app [OS9PATHLEN];
    char        fnam[OS9PATHLEN];
    
    err= DirNthEntry( spP,2, &dEnt ); if (err) return err; // start after "." and ".."

    while (dEnt!=NULL) {
      if (ustrcmp( dEnt->d_name,AppDo )!=0 ) return 0; /* there are still more entries */
      dEnt= ReadTDir( spP->dDsc ); 
    } /* loop */

    strcpy( app,spP->fullName );
    strcat( app,PATHDELIM_STR );
    strcat( app,AppDo ); /* full .AppleDouble path */
    
    app_d= opendir( app ); if (app_d==NULL) return 0; /* already removed */

    while (!err) { /* delete all entries of the .AppleDouble directory */
            dEnt= ReadTDir( app_d );
        if (dEnt==NULL) break;
        
        if (strcmp( dEnt->d_name,"."  )!=0 && /* ignore "." and ".." */
            strcmp( dEnt->d_name,".." )!=0) {
        	strcpy( fnam,app );
        	strcat( fnam,PATHDELIM_STR );
        	strcat( fnam,dEnt->d_name );
        	err= host2os9err( remove(fnam),E_PNNF ); if (err) break;
        }
    } /* while */

    closedir( app_d );
    if (err) return err; /* from previous error */
    
    /* and remove .AppleDouble itself */
    err= host2os9err( remove(app),E_PNNF ); if (err) return err;
    return 0;
} /* RemoveAppledouble */
  
  
  
void seekD0( syspath_typ* spP )
{   if (spP->dDsc!=NULL) rewinddir( spP->dDsc ); /* start at the beginning */
} /* seekD0 */



ulong DirSize( syspath_typ* spP )
/* get the virtual OS-9 dir size in bytes */
{
    int         cnt= 0;
    dirent_typ* dEnt;
    
    #ifdef linux
      int sv= telldir( spP->dDsc );
    #endif
    
    seekD0( spP );       /* start at the beginning */
    while (true) {       /* search for the nth entry */
            dEnt= ReadTDir( spP->dDsc ); 
        if (dEnt==NULL) break;
        if (ustrcmp( dEnt->d_name,AppDo )!=0) cnt++; /* ignore ".AppleDouble" */
    } /* loop */

    #ifdef linux
      seekdir( spP->dDsc,sv );
    #endif
    
                                      /* avoid also 1 entry !!! */
    if    (cnt<2) cnt= 2; /* at least two entries are there !!! */
    return cnt*DIRENTRYSZ;
} /* DirSize */
#endif



int stat_( const char* pathname, struct stat *buf )
/* slightly adapted version for Windows */
{
    int err= -1 /* default */;
    
    #if defined macintosh || defined UNIX
      /* do it the "normal" way */
      err= stat( pathname, buf );

    #elif defined windows32
      /* 'stat' at windows has a bug, it does not close correctly */
      FILE* stream;
      int   fd, cer;
      
      /* is it a file ?? */
          stream= fopen( pathname,"rb" );
      if (stream==NULL) {
        if (GetLastError()==ERROR_SHARING_VIOLATION) 
            err= stat( pathname, buf ); /* if error, try normal way */
      }
      else {
          /* stat seems not to close correctly under Windows */
          /* use fstat instead of stat */
          fd = fileno( stream );
          err= fstat ( fd,buf ); 
          cer= fclose( stream ); if (err==0) err= cer; /* close always */
      
          /* is it a dir ? then try it the "normal" way */
          if (err && PathFound( pathname ))
              err=        stat( pathname, buf );
      }
    #endif 
    
    return err;
} /* stat_ */



Boolean DirName( const char* pathname, ulong fdsect, char* result, Boolean useInodes )
// Only Linux StartDir uses useInodes = true
{
  Boolean ok= false;

  #ifdef MACOS9
    #pragma unused(pathname,fdsect,result,useInodes)
  #endif
    
  #ifdef win_unix
    DIR*        d;
    dirent_typ* dEnt;
    ulong       fd;

    #ifdef __MACH__
      char   p [OS9PATHLEN];
      char   q [OS9PATHLEN];
      char   sv[OS9PATHLEN];
      struct stat qInfo;
      struct stat pInfo;
      int    len, err;
      
      strcpy( sv, result ); 
    #endif
     
        d= (DIR*)opendir( pathname ); /* search for the current inode */
    if (d!=NULL) {
      while (true) {
            dEnt= ReadTDir( d );
        if (dEnt==NULL) break;
      //printf( "DirName='%s'\n", pathname );
        FD_ID( NULL,pathname,dEnt, &fd, 0, "", false,useInodes );
              
      //upo_printf( "fd/fdsect=%d/%d '%s'\n", fd, fdsect, dEnt->d_name );
        if (fd==fdsect && strcmp( dEnt->d_name,".." )!=0) { /* this is it */
          strcpy( result,         dEnt->d_name );
          ok= true; break;
        }   
      } // loop
          
      closedir( d );
      
      
      if (!ok) {
        strcpy( result, "?" ); // in case of not found
      
        #ifdef __MACH__
          do {
            // prepare comparison of one higher inode
            strcpy( q,  pathname );
            len= strlen(pathname); if (len<=6) break;
            q[ len-6 ]= 0;
          
          //upo_printf( "q='%s' sv='%s'\n", q, sv );
		    err= stat_ ( q, &qInfo ); if (err) break;
          //upo_printf( "q='%s' ino=%d err=%d\n", q, qInfo.st_ino, err );
              
                d= (DIR*)opendir( pathname ); /* search for the current inode */
            if (d!=NULL) {
              while (true) {
                    dEnt= ReadTDir( d );
                if (dEnt==NULL) break;
              //upo_printf( "name='%s'\n", dEnt->d_name );
		                        
                strcpy( p, pathname );
                strcat( p, PATHDELIM_STR );
                strcat( p, dEnt->d_name );
                strcat( p, PATHDELIM_STR );
                strcat( p, sv );
		          
                // get inode and compare it
                     err= stat_( p, &pInfo );
                if (!err) {
               // upo_printf( "name='%s' ino=%d\n", p, pInfo.st_ino );
                  if (pInfo.st_ino==qInfo.st_ino) { 
                    strcpy( result, dEnt->d_name );
                    ok= true;
                    break;
                  } // if
                } // if
              } // loop
        
              closedir( d );
            } // if
          } while (false);
        #endif
      } // if
      
    //upo_printf( "'%s' %s => '%s'\n\n", pathname, ok ? "OK":"NOT OK", result ); 
    } /* if */
  #endif
  
  return ok;
} /* DirName */



ulong My_Ino( const char* pathname )
{
    ulong ino= 0;

    #ifdef macintosh
      #pragma unused(pathname)
    #endif
    
    #ifdef win_linux
      char        p[OS9PATHLEN];
      char*       q;
      DIR*        d;
      dirent_typ* dEnt;

      strcpy( p,pathname );
      q= p+strlen(p)-1;
      while (q>p && *q!=PATHDELIM) q--;
      *q++= NUL;

          d= opendir( p ); /* search for the current inode */
      if (d!=NULL) {
          while (true) {
                  dEnt= ReadTDir( d );
              if (dEnt==NULL) break;
              if (ustrcmp(dEnt->d_name,q)==0) {
              //printf( "MyIno='%s'\n", p );
                FD_ID( NULL, p,dEnt, &ino, 0, "", false,false );
                break;
              }
          } /* while */
          
          closedir( d );
      } /* if */
    #endif
  
    return ino;
} /* MyIno */



void MakeOS9Path( char* pathname )
{
  char  tmp[OS9PATHLEN];
  char* q= pathname;
  int   i= 0;
    
  if (*q!=PSEP) {
    if (q[ 1 ]==':' &&
       (q[ 2 ]==PATHDELIM ||
        q[ 2 ]=='\0')) {   /* windows */
        q[ 1 ]= q[ 0 ];    /* adapt it OS-9-like, same length */
        q[ 0 ]= PSEP;
    }
    else {
      tmp[ 0 ]= PSEP; /* start with a slash */
      i= 1;
    } // if
  } // if

  strcpy( &tmp[ i ],q ); q= tmp;
            
  while  (*q!=NUL) { /* replace slashes */
    if (*q==PATHDELIM) *q= PSEP;
    q++;
  } /* while */
    
  q--; if (*q==PSEP) *q= NUL;
  strcpy( pathname,tmp );
} /* MakeOS9Path */



static void CutRaw( char** pP )
/* expect OS-9 notation */
{
    char*    p= *pP;
    while  (*p!=NUL) { /* cut raw device name or sub paths */
        if (*p=='@') { *p= NUL; break; }
        p++;
    }
} /* CutRaw */



Boolean SamePathBegin( const char* pathname, const char* cmp )
{
    int    len= strlen(cmp);
    return len>0 
       &&  ustrncmp( pathname,cmp, len )==0
       && (pathname[len]==NUL || 
           pathname[len]=='@' || 
           pathname[len]==PSEP);
} /* SamePathBegin */



Boolean IsDesc( const char* dvn, mod_dev** mod, char** p )
{
    int    mid= find_mod_id( dvn );
    ushort mty;
    
    if (mid==MAXMODULES) return false; /* no such module found */
    
       *mod= (mod_dev*)os9mod( mid );      
        mty= os9_word( (*mod)->_mh._mtylan )>>BpB;
    if (mty!=MT_DEVDESC) return false; /* not the right type */

    *p= (char*)*mod + os9_word((*mod)->_mfmgr);
    return true;    
} /* IsDesc */



Boolean SCSI_Device( const char* os9path,
                     short *scsiAdapt, short *scsiBus, int *scsiID, short *scsiLUN,
                     ushort *scsiSsize, ushort *scsiSas, byte *scsiPDTyp,
                     ptype_typ *typeP )
/* expect OS-9 notation */
{
    char      tmp[OS9PATHLEN];
    char      *p, *dvn;
    int       ii;
    mod_dev*  mod;
    byte      lun;
    byte      id;
    ushort    *ssize, *sas;
    byte      *pdtyp;
    
    *typeP= fNone;         /* the default value */
    strcpy( tmp,os9path ); /* make a local copy */
         p= tmp;
    if (*p==PSEP) p++;
    dvn= p;             /* this is what we are looking for */
    while  (*p!=NUL) { /* cut raw device name or sub paths */
        if (*p=='@' || *p==PSEP) { *p= NUL; break; }
        p++;
    }

    if (*dvn==NUL) return false; /* no device */

    /* this is the official table of the SCSI entries */
    for (ii=0; ii<MAXSCSI; ii++) { /* returns true, if SCSI device */
        if (ustrcmp(dvn,scsi[ ii ].name)==0) {
            // found, return SCSI address 
            *scsiAdapt= scsi[ ii ].adapt;
            *scsiBus  = scsi[ ii ].bus;
            *scsiID   = scsi[ ii ].id;
            *scsiLUN  = scsi[ ii ].lun;
            *scsiSsize= scsi[ ii ].ssize;
            *scsiSas  = scsi[ ii ].sas;
            *scsiPDTyp= scsi[ ii ].pdtyp;
            *typeP    = fRBF;
            return true;
        }
    } /* for */

    if (!IsDesc( dvn, &mod, &p )) return false;
    
//        mid= find_mod_id( dvn );           /* no such module found */
//    if (mid==MAXMODULES) return false;
//    
//        mod= (mod_dev*)os9mod( mid );      /* not the right type */
//        mty= os9_word( mod->_mh._mtylan )>>BpB;
//    if (mty!=MT_DEVDESC) return false;
//
//    p= (char *)mod + os9_word(mod->_mfmgr);         
    if (ustrcmp( p,"RBF" )==0) {
        id   = *((byte*)(&mod->_mdtype + PD_CtrlID));
        lun  = *((byte*)(&mod->_mdtype + PD_LUN));

        // full SCSI address
        *scsiID   = id;
        *scsiLUN  = lun;
        *scsiAdapt= defSCSIAdaptNo; // bus and adaptor come from defaults
        *scsiBus  = defSCSIBusNo;
        ssize     = (ushort*)(&mod->_mdtype + PD_SSize); *scsiSsize= *ssize;
        sas       = (ushort*)(&mod->_mdtype + PD_SAS  ); *scsiSas  = *sas;
        pdtyp     = (byte  *)(&mod->_mdtype + PD_TYP  ); *scsiPDTyp= *pdtyp;
        
        // find empty scsi entry
        for (ii=0; ii<MAXSCSI; ii++) {
            if (scsi[ii].name[0]==NUL) {
                // empty entry, add stuff here
                // - name
                strcpy( scsi[ii].name, dvn );
                // - full SCSI address
                scsi[ ii ].id    = id;
                scsi[ ii ].lun   = lun;
                scsi[ ii ].adapt = defSCSIAdaptNo; // bus and adaptor come from defaults
                scsi[ ii ].bus   = defSCSIBusNo;
                // - params
                scsi[ ii ].ssize= os9_word (*ssize);
                scsi[ ii ].sas  = os9_word (*sas);
                scsi[ ii ].pdtyp=           *pdtyp;
                *typeP= fRBF;
                // done!
                return true;
            }
        }
        // no more room in SCSI table
        return false; 
    } // if
    
    // is false, but useful anyway
    if     (ustrcmp( p,  "SCF"     )==0) { *typeP= fSCF;
        if (ustrcmp( dvn,"vmod"    )==0)   *typeP= fVMod; return false; }
    if     (ustrcmp( p,  "SOCKMAN" )==0 ||
            ustrcmp( p,  "SFM"     )==0) { *typeP= fNET;  return false; }
    if     (ustrcmp( p,  "PKMAN"   )==0) { *typeP= fPTY;  return false; }
    
    return false;
} /* SCSI_Device */



#ifdef windows32
  static void StrReplace( char* dst, const char* src, const char* search, 
                                                      const char* replace )
  {
    int  i= 0;
    int  j= 0;
    int  k;
    char ch, v, w;
    Boolean found, foundOK= false;
    
    while (true) {
      ch= src[ i ]; if (ch=='\0') break;
      
      found= false; // <search> sequence ?
      if (i==0 || src[ i-1 ]=='\\') { // start or separator
        k= 0;
        while (true) {
          v= search[   k ]; 
          w= src   [ i+k ];
          
          if (v=='\0') {
            found= w=='\0' || 
                   w=='.'  ||
                   w=='\\'; // end or separator
            break;
          } // if
          if (toupper( v )!=toupper( w )) break;
          
          k++;
        } // loop
      } // if
      
      if (found) {
        i+= k; // skip <search> sequence
 
        k= 0; // insert <replace> sequence
        while (true) {
          v= replace[ k ]; 
          
          if (v=='\0') break;
          dst[ j ]= v;          
          
          j++;
          k++;
        } // loop
        
        foundOK= true;
      }
      else {
        dst[ j ]= ch;
        
        i++;
        j++;
      }
    } // loop
    
    dst[ j ]= '\0';
  //if (foundOK) upe_printf( "con src='%s' dst='%s'\n", src, dst );
  } // StrReplace
  
  
  
  os9err AdjustPath( const char* pathname, char* adname, Boolean creFile )
  /* do more or less nothing */
  { 
  //char* p;
      
    if ( *pathname==NUL ) return E_BPNAM;
      
    /* "con" is a problem in windows => convert it to ".con" */
    StrReplace( adname, pathname, "CON", ".CON" );
      
    /*
    strcpy( adname,pathname );
    if (ustrcmp( adname,   "con" )==0) 
        strcpy ( adname,  ".con" );
      
    p= adname + strlen( adname )-strlen( "\\con" );
    if (ustrcmp( p,      "\\con" )==0) {
        *p= NUL;
        strcat ( adname,"\\.con" );
    }
    */
      
  //upe_printf( "%s\n", adname );
    EatBack( adname );
      
    if (!creFile) { /* can't be here, if file not yet exists */
      if (!FileFound( adname )) {
        #ifdef windows32
          if (GetLastError()==ERROR_NOT_READY) return os9error(E_NOTRDY);
        #endif
        
        if (!PathFound( adname )) {
          #ifdef windows32
            if (GetLastError()==ERROR_NOT_READY) return os9error(E_NOTRDY);
          #endif
          
          return E_PNNF;
        } // if
      } // if
    } // if
      
    return 0;
  } /* AdjustPath */
#endif



#ifdef win_unix
  void GetEntry( dirent_typ* dEnt, char* name, Boolean do_2e_conv )
  /* Get the <name> of dir entry <dEnt> */
  /* Convert 2e string, if <do_2e_conv> is true */
  {
      char* q;
      const int L_Plen= strlen(L_P);
      
      strncpy( name, dEnt->d_name, DIRNAMSZ );
           
      if (ustrncmp( name,L_P, L_Plen )==0 && do_2e_conv) {
                    name[0]=  '.';  /* convert ".xx" string */
                    name[1]= NUL;
            strcat( name, &name[L_Plen] );
      }
      
      while (true) {
           q= strstr( name," " ); if (q==NULL) break; /* replace " " by "_" */
          *q= '_';
      } /* loop */
  } /* GetEntry */
#endif



Boolean RBF_ImgSize( long size )
/* Returns true, if it is a valid RBF Image size */
{
 //   if (size<8192 || (size % 2048)!=0)         return E_PNNF;
 //   if (size<1024 || (size % STD_SECTSIZE)!=0) return E_PNNF;
  return size>=1024 && (size % STD_SECTSIZE)==0;
} /* RBF_ImgSize */


#if defined MACOS9
  os9err RBF_Rsc( FSSpec *fs )
  {
      os9err     err;
      OSErr      oserr;
      CInfoPBRec cipb;
      ulong      size;
      short      refnum;
      int        cnt;
      char       bb[256]; /* one sector */
      
      err= getCipb( &cipb, fs ); if (err) return err;
      size= (ulong)  cipb.hFileInfo.ioFlLgLen;
      
      if (!RBF_ImgSize( size )) return E_PNNF;
            
          oserr=FSpOpenDF( fs, fsRdPerm, &refnum );
      if (oserr) return host2os9err(oserr,E_PNNF);

                               cnt= sizeof(bb);
      oserr= FSRead ( refnum, &cnt,       &bb );
      oserr= FSClose( refnum );     /* is this really an OS-9 partition ? */
      if (strcmp( &bb[CRUZ_POS],Cruz_Str )!=0) return E_PNNF; /* Cruzli check */
      
      return 0;
  } /* RBF_Rsc */



  os9err GetRBFName( char* os9path, ushort mode,
                     Boolean *isFolder, FSSpec *fs, FSSpec *afs )
  {
      os9err     err;
      char*      pp;
      Str255     nMac;
      char       sv[OS9PATHLEN];
        
      strcpy( sv, os9path );
      pp=     sv; CutRaw( &pp );
      err= parsepath( 0,  &pp, nMac, false ); if (err) return E_PNNF;

      #ifdef RBF_SUPPORT
        err= Resolved_FSSpec( startVolID,startDirID, nMac, isFolder, fs,afs );
        if (err)          return err;
        if (*isFolder)    return E_FNA;
        if (!IsDir(mode)) return E_FNA;

        err= RBF_Rsc( fs );
      
      #else
        #pragma unused(mode,isFolder,fs,afs)
        err= E_UNIT;
      #endif
      
      return err;
  } /* GetRBFName */

#elif defined win_unix
  os9err GetRBFName( char* os9path, ushort mode,
                     Boolean *isFolder, char* rbfname )
  {
      os9err err= 0;
      char   sv     [OS9PATHLEN];
      char   adjust [OS9PATHLEN];
      char   *pp, *qq;
      struct stat info;
      FILE*  stream;
      int    cnt;
      char   bb[STD_SECTSIZE]; /* one sector */


      strcpy    ( sv, os9path );
      pp= (char*)&sv; CutRaw( &pp );
      if (*pp==NUL || *pp!=PSEP) return E_PNNF; /* converted to an OS-9 path !! */

      err= parsepath( 0,  &pp,adjust, false ); if (err) return E_PNNF;
      strcpy( sv, adjust );
      pp =    sv;
      
      err= AdjustPath    ( pp,adjust, false ); if (err) return E_PNNF;
      pp =                    adjust;
      
      /* cut the path piece by piece (no sub paths within RBF images) */
      qq = pp+strlen(pp)-1;
      while (true) {
               *isFolder=    PathFound( pp );
          if (!*isFolder && !FileFound( pp )) err= E_PNNF;
      
          debugprintf( dbgFiles,dbgNorm,("# GetRBFName: '%s' mode=%d err=%d (%s)\n", 
                                            pp, mode,err, *isFolder ? "dir":"file" ));
          if (!err || pp==qq) break;

          while (*qq!=PATHDELIM && qq>pp) qq--;
          *qq= NUL;
      } /* loop */

      do {
          /* the first conditions for RBF image are min size and granularity */
          if (*isFolder)                               { err= E_FNA;  break; }
      
          /* allow to access the image as a normal file */
          if (ustrcmp(adjust,pp)==0 && !IsDir(mode))   { err= E_FNA;  break; }
      
          err= stat_( pp,  &info );           if (err) { err= E_PNNF; break; }
          if (!RBF_ImgSize( info.st_size ))            { err= E_FNA;  break; }

          stream= fopen( pp,"rb" );  if (stream==NULL) { err= E_PNNF; break; }
          cnt= fread( &bb, 1,sizeof(bb), stream );
          fclose( stream ); /* is this really an OS-9 partition ? => Cruzli check */
          if (strcmp( &bb[CRUZ_POS],Cruz_Str )!=0)     { err= E_FNA;  break; }   
      } while (false);

      qq = pp+strlen(pp);
      while (*qq!=PATHDELIM && qq>pp) qq--;
      qq++;
      
      strcpy( rbfname, qq );
      debugprintf( dbgFiles,dbgNorm,("# GetRBFName: '%s' err=%d\n", rbfname,err ));
      return err;
  } /* GetRBFName */
#endif



// #ifdef RAM_SUPPORT
Boolean RAM_Device( const char* os9path )
/* check, if it is a RAM device */
{
    char     cmp[OS9PATHLEN];
    mod_dev* mod;
    char*    p;
    
    GetOS9Dev( os9path, (char*)&cmp );
    if (mnt_ramSize>0 || strcmp( mnt_devCopy,"" )!=0) return true;
    
    if (IsDesc( cmp, &mod, &p )  && ustrcmp( p,"RBF" )==0) {
        p= (char*)mod + os9_word(mod->_mpdev);
        return ustrcmp( p,"ram" )==0;
    }
    
    return false;
} /* RAM_Device */
// #endif


static Boolean OS9_Device( char* os9path, ushort mode, ptype_typ *typeP )
/* Returns true, if <os9path> is an RBF device */
{
    os9err err= 0;
    int    id;
    short  lun;
    short  bus;
    short  adapt;
    ushort sas,ssize;
    byte   pdtyp;
    
    #ifdef MACOS9
      Boolean isFolder;
      FSSpec  fs,afs;

    #elif defined win_unix
      Boolean isFolder;
      char    rbfname[OS9PATHLEN];
    #endif
    
    #ifdef RBF_SUPPORT
      ushort cdv;
    #endif
    

    *typeP= fRBF; /* default value */
    #ifdef RBF_SUPPORT
      if (InstalledDev( os9path,"",false, &cdv )) return true; /* already ? */
      
      #ifdef RAM_SUPPORT
        if (RAM_Device( os9path )) return true;
      #endif
    #endif
    
    #ifdef MACOS9
                        err= GetRBFName(  os9path,   mode, &isFolder, &fs,&afs );
      if  (err==E_UNIT) err= GetRBFName( &os9path[1],mode, &isFolder, &fs,&afs );
      
    #elif defined win_unix
                        err= GetRBFName(  os9path,   mode, &isFolder, (char*)&rbfname );
      if  (err==E_UNIT) err= GetRBFName( &os9path[1],mode, &isFolder, (char*)&rbfname );

    #else 
      /* %%% some fixed devices defined currently */
      isFolder= false;
      if (ustrncmp( os9path,"/mt",3 )!=0 &&
          ustrncmp( os9path,"/c1",3 )!=0 &&
          ustrncmp( os9path,"/c2",3 )!=0 &&
          ustrncmp( os9path,"/c3",3 )!=0 &&
          ustrncmp( os9path,"/dd",3 )!=0) err= E_MNF;
    #endif
    
    /* try it again, it might be installed now */
    #ifdef RBF_SUPPORT
      if (err && InstalledDev( os9path,"",false, &cdv )) return true; /* already ? */
    #endif
    
    if (!IsDir(mode) && err==E_FNA) { *typeP= fNone; return false; }
    if (!err && !isFolder)          { *typeP= fRBF;  return true;  }

    /* searching for SCSI after searching file image !! */
    if (SCSI_Device( os9path, &adapt, &bus, &id, &lun, &ssize, &sas,&pdtyp, typeP ) || 
                                  *typeP!=fNone ) return true;
    return false;
} /* OS9_Device */



char* Mod_TypeStr( mod_exec* mod )
{
    char* nam;
    int   ty= os9_word(mod->_mh._mtylan)>>BpB;

    switch (ty) {
        case MT_ANY     : nam="    "; break;
        case MT_PROGRAM : nam="Prog"; break;
        case MT_SUBROUT : nam="Subr"; break;
        case MT_MULTI   : nam="Mult"; break;
        case MT_DATA    : nam="Data"; break;
        case MT_CSDDATA : nam="CSDD"; break;
        case MT_TRAPLIB : nam="Trap"; break;
        case MT_SYSTEM  : nam="Sys" ; break;
        case MT_FILEMAN : nam="Fman"; break;
        case MT_DEVDRVR : nam="Driv"; break;
        case MT_DEVDESC : nam="Desc"; break;
        default         : nam="????";
    }
    
    return nam;
} /* Mod_TypeStr */


char* PStateStr( process_typ* cp )
{
    char* nam;
    switch (cp->state) {
        case pUnused  : nam="pUnused";   break;
        case pActive  : nam="pActive";   break;
        case pDead    : nam="pDead";     break;
        case pSleeping: nam="pSleeping"; break;
        case pWaiting : nam="pWaiting";  break;
        case pSysTask : nam="pSysTask";  break;
        case pWaitRead: nam="pWaitRead"; break;
        default       : nam="unknown";
    } // switch
    
    if (cp->isIntUtil)  nam="pIntUtil";
    
    return nam;
} /* PStateStr */



char* TypeStr( ptype_typ type )
{
    char* nam;
    switch (type) {
        case fNone: nam="fNone"; break;
        case fCons: nam="fCons"; break;
        case fTTY : nam="fTTY";  break;
        case fNIL : nam="fNIL";  break;
        case fVMod: nam="fVMod"; break;
        case fSCF : nam="fSCF";  break;
        case fFile: nam="fFile"; break;
        case fDir : nam="fDir";  break;
        case fPipe: nam="fPipe"; break;
        case fPTY : nam="fPTY";  break;
        case fRBF : nam="fRBF";  break;
        case fNET : nam="fNET";  break;
        default   : nam="unknown";
    }
    
    return nam;
} /* TypeStr */



char* spP_TypeStr( syspath_typ* spP )
{
    char*          nam= "---";
    if (spP!=NULL) nam= TypeStr(spP->type);
    return         nam;
} /* spP_TypeStr */



ptype_typ IO_Type(ushort pid, char* os9path, ushort mode)
/* get the I/O type of <os9path> */
{
    ptype_typ    type;
    process_typ* cp= &procs[pid];
    
    debugprintf( dbgFMgrType,dbgNorm,("# IO_Type  (in): '%s'\n", os9path) );

    do {
        if (ustrncmp(os9path,">", 1)==0 ||        /* "sectorfiles" compatibility */
            ustrncmp(os9path,"<", 1)==0) { type= fNone; break; }
        if (!AbsPath(os9path) ) {
            if (IsExec(mode)) type= cp->x.type;   /* the types of exec/cur dir */
            else              type= cp->d.type;
        
            if (type==fDir && !(mode & 0x80)) type= fFile;
            break;
        } /* if */
    
        if  (ustrcmp (os9path,"/nil"   )==0) { type= fNIL;     break; }
        if  (ustrncmp(os9path,"/pipe",5)==0) { type= fPipe;    break; }
        if  (ustrncmp(os9path,"/lp",  3)==0 ||
             ustrcmp (os9path,"/p"     )==0 ||
             ustrcmp (os9path,"/p1"    )==0) { type= fPrinter; break; }

        #ifdef TERMINAL_CONSOLE
          /* there is more than one serial device possible now */
          if (ustrcmp(os9path,MainConsole)==0  /* for OS9serial, only this name is known */
    
          #ifndef SERIAL_INTERFACE
           || ustrcmp(os9path,SerialLineA)==0 /* ts1 */
           || ustrcmp(os9path,SerialLineB)==0 /* ts2 */
          
            #ifndef linux
           || (ustrcmp (os9path,"/t0") == 0)
           || (os9path[0]==PSEP &&
               os9path[1]=='t'  && atoi(&os9path[2])>= 1 && /* /t1 ../t49 */
                                   atoi(&os9path[2])<VModBase)
            #endif
          #endif
          ) { type= fCons; break; }

          if  (ustrcmp (os9path,"/tty00")==0  /* /tty0 */
           || (ustrncmp(os9path,"/tty",4)==0 
               && atoi(&os9path[4])>= 1       /* /tty1 .. /tty99 */
               && atoi(&os9path[4])<=99)) { type= fTTY; break; }

          if  (ustrcmp (os9path,"/pty00")==0  /* /pty0 */
           || (ustrncmp(os9path,"/pty",4)==0
               && atoi(&os9path[4])>= 1       /* /pty1 .. /pty99 */
               && atoi(&os9path[4])<=99)) { type= fPTY; break; }
        #endif
 
        /* "/vmod" is built-in */ 
        if    (ustrcmp( os9path,"/vmod" )==0) { type= fVMod; break; }
        if (OS9_Device( os9path,mode,          &type ))      break;
        
        if (IsDir(mode)) type= fDir;
        else             type= fFile;
    } while (false);

    debugprintf( dbgFMgrType,dbgNorm,("# IO_Type (out): '%s' %s\n", 
                                         os9path, TypeStr(type)) );
    return type;
} /* IO_Type */


/* eof */
