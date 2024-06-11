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
/* (c) 1993-2004 by Lukas Zeller, CH-Zuerich  */
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
 *    Revision 1.5  2003/05/17 10:49:06  bfo
 *    (CVS header included)
 *
 *
 */


/* "serialaccess.c"
 * Support for the Apple Macintosh serial interfaces
 *
 *
 * 00/05/07 bfo  Initial version
 * 00/05/08 bfo  Read activated
 * 00/05/09 bfo  0a0d correctly handled, with "DevReady"
 * 00/05/11 bfo  "yes" debugged (still running as Debug)
 * 00/05/20 bfo  running quite stable (also standalone)
 * 00/06/18 bfo  devIsReady support for re-entrancy
 * 00/07/13 bfo  Support for both serial lines
 * 00/07/15 bfo  RemoveConsole: installed= false at the end
 */

#include "Serial.h"
#include "os9exec_incl.h"

#define  MAXSERIAL 3 /* serial[ 0 ] is not existing */
ttydev_typ mac_serial[ MAXSERIAL ];     



/* locally defined procedures */
void  InitSerials();
short InstallSerial                               ( int serialID );
void  RemoveSerial                                ( int serialID );
OSErr WriteToSerial(ttydev_typ* mco, Ptr theData, Size *lengthOfData);
long  WriteCharsToSerial ( char *buffer, long    n, int serialID );
void  CheckInBufferSerial                         ( int serialID );
void  CheckInputBuffersSerial();
Boolean                  DevReadySerial( long *cnt, int serialID );
long  ReadCharsFromSerial( char *buffer, long    n, int serialID );
/* -------------------------- */



void InitSerials()
/* there are 2 serial devices supported */
/* initialize them all */
{
    ttydev_typ* mco;
    int      k;

    for (k=1; k<MAXSERIAL; k++) {
        mco= &mac_serial[k];
        mco->installed = false;
        mco->inBufUsed =     0;
        mco->holdScreen= false;
    }
} /* InitSerials */



short InstallSerial( int serialID )
/* initialize access to Mac's terminal consoles */
/* <serialID> = 1 is .aIn/.aOut (Printer Port, also supported by Stealth Board */
/*            = 2 is .bIn/.bOut (Modem   Port) */
{
    #define     TIMEOUT 100
    OSErr       oserr= 0;
    char        *nIn, *nOut;
    SerShk      shk_field;
    SerShk*     shk;
    int         ii;
    ttydev_typ* mco;

        
    if (serialID<1 || 
        serialID>2) return 1; /* wrong ID */
    
        mco= &mac_serial[serialID];
    if (mco->installed) return 0; /* it is already open */
        mco->spP= g_spP;

    /* which of two serial lines is it ? */    
    switch (serialID) {
        case  1: nIn = "\p.aIn";           /* use PASCAL notation  */
                 nOut= "\p.aOut"; break;
                
        case  2: nIn = "\p.bIn";
                 nOut= "\p.bOut"; break;
    }

    /* try to open input, using some timeout */
    ii= 0;
    do {    oserr= OpenDriver( nIn, &mco->rIn );
        if (oserr!=0)  CloseDriver(  mco->rIn );
        ii++; if (ii>TIMEOUT) return 1;
    }
    while  (oserr!=0);
    
    
    /* try to open output, using some timeout */
    ii= 0;
    do {    oserr= OpenDriver( nOut, &mco->rOut );
        if (oserr!=0)    CloseDriver( mco->rOut ); 
        ii++; if (ii>TIMEOUT) return 1;
    }
    while  (oserr!=0);

    
    /* activate software flow control */  
    shk= &shk_field;
    shk->fXOn= 1;
    shk->fCTS= 0;
    shk->xOn = mco->spP->opt[ PD_XON  ];
    shk->xOff= mco->spP->opt[ PD_XOFF ];
    shk->errs= 0;
    shk->evts= 0;
    shk->fInX= 1;
    shk->fDTR= 0;
    oserr= SerHShake( mco->rIn,  shk );
    oserr= SerHShake( mco->rOut, shk );
    oserr= SerReset ( mco->rIn,  baud9600+data8+stop10+noParity );
    oserr= SerReset ( mco->rOut, baud9600+data8+stop10+noParity );
 
    /* it is now installed */
    mco->installed= true; /* it is already open */    
    return 0;
} /* InstallSerial */



void RemoveSerial(int serialID)
/* de-initalize serial interface */
{
    OSErr       oserr= 0;
    SerShk      shk_field;
    SerShk*     shk;
    ttydev_typ* mco;

    /* at the end, serial line status should be switched back to default */    
    /* which of two serial lines is it ? */    
    if (serialID<1 || 
        serialID>2) return; /* wrong ID */
        
         mco= &mac_serial[serialID];
    if (!mco->installed) return; /* still open ? */
    
    /* switch back to normal state */
    shk= &shk_field;
    shk->fXOn= 0;
    shk->fCTS= 1;
    shk->xOn = 0;
    shk->xOff= 0;
    shk->errs= 0;
    shk->evts= 0;
    shk->fInX= 0;
    shk->fDTR= 1;
    oserr= SerHShake( mco->rIn,  shk );
    oserr= SerHShake( mco->rOut, shk );
    
    CloseDriver( mco->rIn  );
    CloseDriver( mco->rOut );
    
    /* it is now de-installed */
    mco->installed= false;   
} /*  RemoveSerial */



OSErr WriteToSerial(ttydev_typ* mco, Ptr theData, Size *lengthOfData)
{
    while (mco->holdScreen) CheckInputBuffers(); /* XOn/XOff flow control */
    return FSWrite( mco->rOut, lengthOfData,theData );
} /* WriteToSerial */



long WriteCharsToSerial(char *buffer, long n, int serialID)
/* write characters to console */
{
    long        ii, a= 1;
    char        lf;
    char        *tmp;
    static      int writtenLF= false;
    ttydev_typ* mco;

    if (serialID<1 || 
        serialID>2) return 0; /* wrong ID */

            mco= &mac_serial[serialID];
    lw_pid( mco ); /* assign for later use ! */

    if (!gConsoleNLExpand) {
      writtenLF= false;
      return WriteToSerial( mco, buffer,&n );
    }

    tmp= buffer;
    while (n>0) {   /* ignore it if in combination */
        if (writtenLF && (tmp[0]=='\n')) { tmp++; n--; }
        writtenLF= false; 
      
        for (ii=0;  (ii<n) &&  (!writtenLF); ii++) {
            if ( tmp[ii]=='\r' ) writtenLF= true;
        }
      
        if (WriteToSerial( mco, tmp,&ii ) != noErr) return -1;
      
        if (writtenLF) {
            lf= '\n'; /* add a line feed after CR */
            if (WriteToSerial( mco, &lf,&a ) != noErr) return -1;
        }
      
        n  = n-ii;
        tmp= &tmp[ii];
    }
    
    return n;
} /* WriteCharsToSerial */



void CheckInBufferSerial( int serialID )
{
    OSErr       oserr;
    long        cnt= 0;
    char        key;
    ttydev_typ* mco;
    
    if (serialID<1 || 
        serialID>2) return; /* wrong ID */

         mco= &mac_serial[serialID];
    if (!mco->installed) return;
   
      oserr= SerGetBuf( mco->rIn, &cnt );
    
    if (cnt==0) return; /* currently nothing to read from serial line */
    
    cnt  = 1;
    oserr= FSRead( mco->rIn, &cnt, &key );
    KeyToBuffer  ( mco,             key );
} /* CheckInBufferSerial */


void CheckInputBuffersSerial(void)
{
    CheckInBufferSerial( 1 );
    CheckInBufferSerial( 2 );
} /* CheckInputBuffersSerial */




Boolean DevReadySerial( long *cnt, int serialID )
/* true, if next character(s) can be read */
/* used by "ReadCharsFromConsole" and "GetStat.SS_Ready" */
{
    Boolean     ok;
    ttydev_typ* mco= &mac_serial[serialID];

    mco->pid= currentpid;
    CheckInputBuffersSerial();
    if (serialID<1 || 
        serialID>2) return 0; /* wrong ID */
    
    /* how many characters are currently ready in input buffer ? */
           ok=       mco->inBufUsed; 
    if    (ok) *cnt= mco->inBufUsed;
    return ok; 
} /* DevReadySerial */



long ReadCharsFromSerial(char *buffer, long n, int serialID)
/* read characters from console */
{
    long        cnt;
    ttydev_typ* mco= &mac_serial[serialID];

    /* if not yet ready handle other events */
    if (mco->holdScreen || (!DevReadySerial( &cnt,serialID ))) {      
      devIsReady= false;
      *buffer= NUL; return 1; /* if 0, it would not return to caller */
    }

    devIsReady= true;
    /* which of two serial lines is it ? */    
    if (serialID<1 || 
        serialID>2) {
        *buffer= NUL; return 1; /* wrong ID */
    }

    if       (mco->inBufUsed) { // got some chars to return...
        cnt= (mco->inBufUsed<n ? mco->inBufUsed : n); // return this many chars
        MoveBlk( buffer, mco->inBuf, cnt );

        if (cnt<mco->inBufUsed) {
             // didn't return all chars; shift buffer contents down
             MoveBlk( mco->inBuf, mco->inBuf+cnt, mco->inBufUsed-cnt );
             mco->inBufUsed -= cnt; /* copy <inBuf>, and not <buffer> !!! */
        } 
        else mco->inBufUsed  = 0;
        return cnt;
    }
    
    return 0;
} /* ReadCharsFromSerial */


/* eof */





