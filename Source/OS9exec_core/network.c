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
 *    Revision 1.15  2003/05/07 19:23:20  bfo
 *    Error 000:164 for raw socket open w/o permission
 *
 *    Revision 1.14  2003/04/25 20:04:56  bfo
 *    Use geteuid / set user ID only to 0 for raw sockets
 *
 *    Revision 1.13  2003/04/25 19:37:50  bfo
 *    Setuid/seteuid support for Linux
 *
 *    Revision 1.12  2003/04/20 23:10:41  bfo
 *    Various adaptions for "ping" support on all 3 platforms
 *
 *    Revision 1.11  2003/04/14 21:52:14  bfo
 *    Release buffer (accept) at pNClose fixed
 *
 *    Revision 1.10  2003/04/12 21:56:26  bfo
 *    Name server support
 *
 *    Revision 1.9  2002/10/28 22:18:30  bfo
 *    network close with corrected release_mem for net->b_local
 *
 *    Revision 1.8  2002/10/27 23:39:02  bfo
 *    Release memory at pNclose done in every case
 *    get_mem/release_mem without param <mac_asHandle>
 *
 *    Revision 1.7  2002/10/27 23:32:34  bfo
 *    Release memory at pRclose done in every case
 *    get_mem/release_mem without param <mac_asHandle>
 *
 *    Revision 1.6  2002/09/11 16:59:19  bfo
 *    Import "os9exec_incl.h" as the first file
 *
 *    Revision 1.5  2002/08/09 22:39:20  bfo
 *    New procedure set_os9_state introduced and adapted everywhere
 *
 *
 */



#include "os9exec_incl.h"


/* specific network definitions */
#ifdef macintosh
  #include <OTDebug.h>
  #include <Threads.h>
#endif

#ifdef USE_CARBON
  OTClientContextPtr *otContext;
  
#endif

#ifdef windows32
  #include <WinSock2.h>
  #define WSA_ASYNC WM_USER+1
#endif

#ifdef linux
  #include <sys/utsname.h>
  #include <netdb.h>
  #include <netinet/in.h>
  #include <sys/socket.h>
  #include <asm/ioctls.h>
  #define INVALID_SOCKET (-1)
  #define SOCKET_ERROR   (-1)
  
  typedef struct sockaddr_in SOCKADDR_IN;
#endif



/* --- local procedure definitions for object definition ------------------- */
void   init_Net ( fmgr_typ* f );

os9err pNopen   ( ushort pid, syspath_typ* spP, ushort *modeP, char* pathname );
os9err pNclose  ( ushort pid, syspath_typ* spP );
os9err pNread   ( ushort pid, syspath_typ* spP, ulong  *lenP,  char* buffer );
os9err pNreadln ( ushort pid, syspath_typ* spP, ulong  *lenP,  char* buffer );
os9err pNwrite  ( ushort pid, syspath_typ* spP, ulong  *lenP,  char* buffer );
os9err pNwriteln( ushort pid, syspath_typ* spP, ulong  *lenP,  char* buffer );

os9err pNopt    ( ushort pid, syspath_typ* spP,                byte* buffer );
os9err pNpos    ( ushort pid, syspath_typ* spP, ulong  *posP );
os9err pNready  ( ushort pid, syspath_typ* spP, ulong  *n    );

       /* network specific functions */
os9err pNbind   ( ushort pid, syspath_typ* spP, ulong  *n,     byte* ispP );
os9err pNlisten ( ushort pid, syspath_typ* spP );
os9err pNconnect( ushort pid, syspath_typ* spP, ulong  *n,     byte* ispP );
os9err pNaccept ( ushort pid, syspath_typ* spP, ulong  *d1 );
os9err pNrecv   ( ushort pid, syspath_typ* spP, ulong  *d1,    
                                                ulong  *d2,    char* *a0  );
os9err pNsend   ( ushort pid, syspath_typ* spP, ulong  *d1,    
                                                ulong  *d2,    char* *a0  );
os9err pNGNam   ( ushort pid, syspath_typ* spP, ulong  *d1,
                                                ulong  *d2,    byte* ispP );
os9err pNSOpt   ( ushort pid, syspath_typ* spP, ulong  *d1,    ulong *d2  );

os9err pNgPCmd  ( ushort pid, syspath_typ *spP, ulong  *a0 );
os9err pNsPCmd  ( ushort pid, syspath_typ *spP, ulong  *a0 );
/* ------------------------------------------------------------------------- */



void init_Net( fmgr_typ* f )
/* install all procedures of the RBF file manager */
{
    gs_typ* gs= &f->gs;
    ss_typ* ss= &f->ss;
    
    /* main procedures */
    f->open        = (pathopfunc_typ)pNopen;
    f->close       = (pathopfunc_typ)pNclose;
    f->read        = (pathopfunc_typ)pNread;
    f->readln      = (pathopfunc_typ)pNreadln;
    f->write       = (pathopfunc_typ)pNwrite;
    f->writeln     = (pathopfunc_typ)pNwriteln;
    f->seek        = (pathopfunc_typ)pBadMode; /* not allowed */


    /* getstat */
    gs->_SS_Size   = (pathopfunc_typ)pUnimp;   /* -- not used */
    gs->_SS_Opt    = (pathopfunc_typ)pNopt;
    gs->_SS_DevNm  = (pathopfunc_typ)pSCFnam;
    gs->_SS_Pos    = (pathopfunc_typ)pNpos;
    gs->_SS_EOF    = (pathopfunc_typ)pNop;         /* ignored */
    gs->_SS_Ready  = (pathopfunc_typ)pNready;

    gs->_SS_PCmd   = (pathopfunc_typ)pNgPCmd;  /* network spefic function */

    gs->_SS_Undef  = (pathopfunc_typ)pUnimp;   /* -- not used, if any other function */


    /* setstat */
    ss->_SS_Size   = (pathopfunc_typ)pNop;         /* ignored */
    ss->_SS_Opt    = (pathopfunc_typ)pNop;         /* ignored */
    ss->_SS_Attr   = (pathopfunc_typ)pNop;         /* ignored */
    
    ss->_SS_Bind   = (pathopfunc_typ)pNbind;   /* network spefic functions */
    ss->_SS_Listen = (pathopfunc_typ)pNlisten;
    ss->_SS_Connect= (pathopfunc_typ)pNconnect;
    ss->_SS_Accept = (pathopfunc_typ)pNaccept;
    ss->_SS_Recv   = (pathopfunc_typ)pNrecv;
    ss->_SS_Send   = (pathopfunc_typ)pNsend;
    ss->_SS_GNam   = (pathopfunc_typ)pNGNam;
    ss->_SS_SOpt   = (pathopfunc_typ)pNSOpt;
    ss->_SS_SendTo = (pathopfunc_typ)pNsend;
    ss->_SS_PCmd   = (pathopfunc_typ)pNsPCmd;

    ss->_SS_Undef  = (pathopfunc_typ)pNop;         /* ignored, if any other function */
} /* init_Net */

/* ----------------------------------------------- */



/*
 * Command sequences for the network connections:
 *
 * ftp/telnet:   
 * (SS_GNam)    [telnet only]
 *  SS_Resv      -
 *  SS_Connect   port 21/23
 * (SS_GNam)    [ftp only]
 *
 *
 * ftp DATA
 *  SS_Resv      -
 *  SS_Bind
 *  SS_GNam
 *  SS_Listen
 *  SS_Accept
 *
 *-------------------------------------------------
 *
 * ftpd/telnetd
 * (SS_GNam)
 *  SS_Resv      -
 *  SS_Bind
 *  SS_Listen
 *  SS_Accept    LOOP
 *
 *  SS_GNam     (ftpdc/telnetdc)
 *  SS_GNam     [ftpd only]
 *  
 *
 * ftpd DATA     
 *  SS_Resv      -
 *  SS_Opt
 *  SS_Bind
 *  SS_Connect   port 20
 *
 */



/* Connection from Internet Support Package (ISP) */
/* to the host system's network connection */

static NetInstalled= false;
enum { kTransferBufferSize = 4096 };
// #define MAXCNT 10000


/* definitions for ICMP (ping) */
#define kOurMagic 0x5175696E /* enum { kOurMagic = 'Quin' }; */
#define ICMP_ECHOREPLY     0
#define ICMP_ECHO          8


#ifdef win_linux
  #define MAX_PACKET 1024 // Max ICMP packet size
#endif


typedef struct _icmphdr 
{
    byte   i_type;
    byte   i_code;    // Type sub code
    ushort i_cksum;
    ulong  i_ISP;     /* use this instead of UInt16 pID/pSeqNum */
    ulong  i_magic;
} IcmpHeader;


// #ifdef macintosh
// struct PingPacket {
//    UInt8   pType;
//    UInt8   pCode;
//    UInt16  pChecksum;
//    ulong   pISP_ID;  /* use this instead of UInt16 pID/pSeqNum */
//    OSType  pMagic;
// };
// typedef struct PingPacket PingPacket, *PingPacketPtr;
// #endif


#ifdef win_linux
/* IP header structure */
typedef struct _iphdr 
{
    unsigned int   h_len:4;        // Length of the header
    unsigned int   version:4;      // Version of IP
    unsigned char  tos;            // Type of service
    unsigned short total_len;      // Total length of the packet
    unsigned short ident;          // Unique identifier
    unsigned short frag_and_flags; // Flags
    unsigned char  ttl;            // Time to live
    unsigned char  proto;          // Protocol (TCP, UDP etc)
    unsigned short checksum;       // IP checksum

    unsigned int   sourceIP;
    unsigned int   destIP;
} IpHeader;

//
// ICMP header structure
//
// typedef struct _icmphdr 
// {
//     BYTE   i_type;
//     BYTE   i_code;                 // Type sub code
//     USHORT i_cksum;
//     ulong  i_id;
//     ulong  i_seq;
//       
// //  USHORT i_id;
// //  USHORT i_seq;
//     // This is not the standard header, but we reserve space for time
// //  ULONG  timestamp;
// } IcmpHeader;
#endif



#ifdef macintosh
static pascal void YieldingNotifier(void* contextPtr, OTEventCode code, 
                                       OTResult result, void* cookie)
// This simple notifier checks for kOTSyncIdleEvent and
// when it gets one calls the Thread Manager routine
// YieldToAnyThread.  Open Transport sends kOTSyncIdleEvent
// whenever it's waiting for something, eg data to arrive
// inside a sync/blocking OTRcv call.  In such cases, we
// yield the processor to some other thread that might
// be doing useful work.
{
    #pragma unused(contextPtr,result,cookie)
    OSStatus junk;
    
    switch (code) {
        case kOTSyncIdleEvent:
            junk = YieldToAnyThread();
            OTAssert("YieldingNotifier: YieldToAnyThread failed", junk == noErr);
            break;
        default:
            // do nothing
            break;
    } /* switch */
}
#endif



#ifdef windows32
int ioctl( IN SOCKET s, IN long cmd, IN OUT u_long FAR * argp )
{   return ioctlsocket( s,cmd, argp );
}
#endif


static void SetDefaultEndpointModes( SOCKET s )
// This routine sets the supplied endpoint into the default
// mode used in this application.  The specifics are:
// blocking, synchronous, and using synch idle events with
// the standard YieldingNotifier.
{
    #ifdef powerc
      OSStatus junk;
    
      junk = OTSetNonBlocking   ( s);
//    junk = OTSetBlocking      ( s);
      OTAssert("SetDefaultEndpointModes: Could not set blocking",         junk==noErr);
      junk = OTSetSynchronous   ( s);
      OTAssert("SetDefaultEndpointModes: Could not set synchronous",      junk==noErr);
      junk = OTInstallNotifier  ( s, &YieldingNotifier, nil);
      OTAssert("SetDefaultEndpointModes: Could not install notifier",     junk==noErr);
      junk = OTUseSyncIdleEvents( s, true );
      OTAssert("SetDefaultEndpointModes: Could not use sync idle events", junk==noErr);
    
    #elif defined win_linux
      ulong nonblocking= true;
      int err= ioctl( s, FIONBIO,  &nonblocking );
    
    #else
      #pragma unused(s)
    #endif
} /* SetDefaultEndpointModes */



static os9err NetInstall(void)
{
    #ifdef powerc
      OSStatus   err;
      InetSvcRef inet_services= nil;
    #endif
    
    #ifdef windows32
      WORD    wVersionRequested= MAKEWORD(2,2);
      WSADATA wsaData;
    #endif

    
    if (NetInstalled) return 0; /* already done, ok */
    
    #ifdef powerc
      #ifdef USE_CARBON
          err= InitOpenTransportInContext( kInitOTForApplicationMask, &otContext );
      #else
          err= InitOpenTransport();
      #endif
          
      if (err) return E_UNIT;
      
      #ifdef USE_CARBON
        inet_services= OTOpenInternetServicesInContext( kDefaultInternetServicesPath, 0,&err, otContext );
      #else
        inet_services= OTOpenInternetServices         ( kDefaultInternetServicesPath, 0,&err );
      #endif
          
      if (err) { 
          #ifdef USE_CARBON
            CloseOpenTransportInContext( otContext ); 
          #else
            CloseOpenTransport(); 
          #endif
          
          return E_UNIT; 
      }

    #elif defined(windows32)
      /* now it works also for Windows !! */
      WSAStartup( wVersionRequested, &wsaData );
    
    #elif defined linux
      // do nothing here

    #else
      return OS9_ECONNREFUSED;
    #endif
        
    NetInstalled= true; /* it is ok now */
    return 0;
} /* NetInstall */



os9err pNopen(ushort pid, syspath_typ* spP, ushort *modeP, char* pathname)
{
    #ifndef linux
    #pragma unused(pid,modeP)
    #endif
    
    OSStatus err;
    net_typ* net= &spP->u.net;
    
    #ifndef WITH_NETWORK
      return E_UNKSVC;
    #endif
        
    strncpy(spP->name,&pathname[1],OS9NAMELEN); /* assign the name */
    err= NetInstall(); if (err) return err;
    
    /* not yet bound, install the struct */
    net->ls       = nil;
    net->ep       = nil;
    net->bound    = false;
    net->accepted = false;
    net->connected= false;
    net->listen   = true;
    net->check    = false;
    net->closeIt  = false;
    
    #ifdef powerc
      /* First allocate a buffer for storing the data as we read it. */
      #ifdef USE_CARBON
          net->transferBuffer= OTAllocMemInContext( kTransferBufferSize, otContext );
      #else
          net->transferBuffer= OTAllocMem         ( kTransferBufferSize );
      #endif    
          
      if (net->transferBuffer==nil)  return E_NORAM;

    #elif defined win_linux
          net->transferBuffer= get_mem( kTransferBufferSize );
      if (net->transferBuffer==NULL) return E_NORAM;
    #endif
      
        net->b_local= get_mem( kTransferBufferSize );
    if (net->b_local==NULL) return E_NORAM;
      
    net->bsize= 0;
    return 0;
} /* pNopen */



os9err pNclose( ushort pid, syspath_typ* spP )
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    OSStatus err= 0;
    net_typ* net= &spP->u.net;
    
    #ifdef powerc
      OSStatus junk;
    #endif


    #ifndef WITH_NETWORK
      return E_UNKSVC;
    #endif
            
    if (net->bound && net->ep!=nil) {
        #ifdef powerc
                    err= OTSndOrderlyDisconnect(net->ep);
          if (!err) err= OTRcvOrderlyDisconnect(net->ep);
        
          junk= OTUnbind       ( net->ep );
          junk= OTCloseProvider( net->ep );
          if (!net->accepted) OTFreeMem( net->transferBuffer );
       
        #elif defined(windows32)
          err= closesocket( net->ep );
          if (!net->accepted) ( net->transferBuffer );
        
        #elif defined linux
          net->closeIt= true;
      //  err= shutdown( net->ep, SHUT_RDWR );
          err= close   ( net->ep );
        #endif
        
        if (!net->accepted) release_mem( net->b_local );
        net->bound= false;
    } /* if */

    return 0;
} /* pNclose */



static os9err netRead( ushort pid, syspath_typ* spP, ulong *lenP, 
                                   char* buffer, Boolean lnmode )
{
    OSStatus     err= 0;
    net_typ*     net= &spP->u.net;
    process_typ* cp = &procs[pid];
    int          ii;

    #ifdef powerc
      OTFlags  junkFlags;
      OTResult lookResult;
    #endif
    
    #ifdef win_linux
      #ifdef windows32
        WSANETWORKEVENTS ev;
        ulong arg;
      #endif
      
      int flags= 0;
    #endif


    #ifndef WITH_NETWORK
      return E_UNKSVC;
    #endif
    
    if (net->bsize==0) {
        if (cp->state==pWaitRead) set_os9_state( pid, cp->saved_state );
        
        #ifdef powerc
          /* keep interface fast for at least one minute */
          gNetActive= GetSystemTick()+60*TICKS_PER_SEC;
          HandleEvent(); /* allow cooperative multitasking */
        
          /* read not more than the transfer buffer size */
          err= OTRcv( net->ep, (void *)net->transferBuffer, 
                                           kTransferBufferSize, &junkFlags );

        #elif defined win_linux
          #ifdef windows32
                 err= WSAEnumNetworkEvents( net->ep, net->hEventObj, &ev );
            if (!err && (ev.lNetworkEvents & FD_CLOSE)!=0)
                 net->closeIt= true; /* close event: close after last read */
                
                 err= ioctl( net->ep, FIONREAD, &arg );
            if  (err>=0 && arg>0) 
          #endif
                 err= recv( net->ep, net->transferBuffer, 
                                         kTransferBufferSize, flags );
              
          #ifdef linux
            if (!err) net->closeIt= true;
          #endif
        #endif
                             
        if (err<=0) { /* error status */
            #ifdef powerc
              if (err==kOTNoDataErr) err= 0;
              if (err==kOTLookErr) {
                      lookResult= OTLook(net->ep);
                  if (lookResult==T_ORDREL) net->closeIt= true;
              }
            #endif
            
            if (net->closeIt) return E_EOF;
            
            cp->saved_state= cp->state;
            set_os9_state( pid, pWaitRead );
            return E_NOTRDY;
        }
                
        net->bsize= (ulong) err; /* bytesReceived */
                net->bpos= net->b_local;
        memcpy( net->bpos, net->transferBuffer, net->bsize );
    }
        
    if (*lenP>net->bsize) *lenP= net->bsize;
        
    if (lnmode) {
        for (ii=0; ii<*lenP; ii++) {
            if (net->bpos[ii]==CR) { *lenP= ii+1; break; }
        }
    }
    memcpy( buffer, net->bpos, *lenP );
    
    net->bpos += *lenP;
    net->bsize-= *lenP;
    return 0;
} /* netRead */


os9err pNread( ushort pid, syspath_typ* spP, ulong *lenP, char* buffer )
{   return netRead( pid,spP, lenP,buffer, false );
}

os9err pNreadln( ushort pid, syspath_typ* spP, ulong *lenP, char* buffer )
{   return netRead( pid,spP, lenP,buffer, true  );
}



static os9err netWrite( ushort pid, syspath_typ* spP, ulong *lenP, 
                                    char* buffer, Boolean lnmode )
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    OSStatus     err= 0;
    net_typ*     net= &spP->u.net;
    ulong        remain, blk;
    int          ii;
    int          flags= 0;

    #ifdef powerc
      OTResult   lookResult;
    #endif
    
    #ifdef windows32
      Boolean    ok;
      int        loopit;   
    #endif


    #ifndef WITH_NETWORK
      return E_UNKSVC;
    #endif

    if (lnmode) {
        for (ii=0; ii<*lenP; ii++) {
            if (buffer[ii]==CR) { *lenP= ii+1; break; }
        }
    }
    
    /* keep interface fast for at least one minute */
    #ifdef powerc
      gNetActive= GetSystemTick()+60*TICKS_PER_SEC;
    #endif
        
             remain= *lenP;
    while   (remain>0) {
        blk= remain;
        if (blk>kTransferBufferSize) blk= kTransferBufferSize;
        memcpy( net->transferBuffer,buffer, blk );
    
        while (true) {
            HandleEvent(); /* allow cooperative multitasking */
            
            #ifdef powerc       
                  err= OTSnd( net->ep, (void *)net->transferBuffer, blk,0 );
              if (err>0) break; /* ok */
        
              /* this is not an error */
              if (err==kOTNoDataErr) { err= 0; break; }
              if (err==kOTLookErr) {
                      lookResult= OTLook(net->ep);
                  if (lookResult==T_ORDREL) {
                      *lenP= 0;
                      return 0;
                  }
              }

              if (err!=kOTFlowErr) return E_WRITE;

            #elif defined win_linux
              if (net->closeIt) {
                  *lenP= 0;
                  return 0;
              }
              
                  err= send( net->ep, net->transferBuffer, blk, flags );
              if (err>=0) break;
            
              #ifdef windows32
                ok= false;
                              
                loopit= 10;
                while (true) {
                    err= WSAGetLastError();
                    
                //  upe_printf( "%d %d\n", err, blk ); 
                    if (!err) { ok= true; break; }
                    if (loopit<=0)        break;
                    Sleep( 1 );
                        
                        err= send( net->ep, net->transferBuffer, blk, flags );
                //  upe_printf( "%d %d --\n", err, blk );
                    if (err>=0) { ok= true; break; }
                    loopit--;
                } /* while */
                if (ok) break;
                
                switch (err) {
                    case WSAECONNABORTED: *lenP= 0; return 0;
                    case WSAECONNRESET  :
                    case WSAENOTCONN    : *lenP= 0; send_signal( pid, S_HangUp);
                                                    return E_WRITE;
                } /* switch */
              #endif
            #endif          
        } /* while */
        
        buffer += err; /* bytesSent */
        remain -= err;
    }
                          
    return 0;
} /* netWrite */


os9err pNwrite( ushort pid, syspath_typ* spP, ulong *lenP, char* buffer )
{   return netWrite( pid,spP, lenP,buffer, false );
}

os9err pNwriteln( ushort pid, syspath_typ* spP, ulong *lenP, char* buffer )
{   return netWrite( pid,spP, lenP,buffer, true  );
}



#ifdef macintosh
static os9err reUse( ushort pid, syspath_typ *spP )
{
    #pragma unused(pid)

    net_typ* net= &spP->u.net;

    UInt8       buf[kOTFourByteOptionSize]; // define buffer for fourByte Option size
    TOption*    opt;                        // option ptr to make items easier to access
    TOptMgmt    req;
    TOptMgmt    ret;
    OSStatus    err;

    opt = (TOption*)buf;                      // set option ptr to buffer
    req.opt.buf = buf;
    req.opt.len = sizeof(buf);
    req.flags   = T_NEGOTIATE;                // negotiate for option

    ret.opt.buf = buf;
    ret.opt.maxlen = kOTFourByteOptionSize;

    opt->level  = INET_IP;                    // dealing with an IP Level function
    opt->name   = IP_REUSEADDR;
    opt->len    = kOTFourByteOptionSize;
    opt->status = 0;
    *(UInt32*)opt->value = true; // set the desired option level, true or false

    err= OTOptionManagement(net->ls, &req, &ret);

    // if no error then return the option status value
    if    (err==kOTNoError && opt->status!=T_SUCCESS) err= opt->status;
    return err;
} /* reUse */
#endif


os9err MyInetAddr( ulong *inetAddr, ulong *dns1Addr,
                                    ulong *dns2Addr, char* domainName )
{
    #ifdef powerc
      InetInterfaceInfo iinfo;

    #elif defined(windows32)
      #define     Unk 0x7f7f7f7f
      HOSTENT*    h;
      ulong       *a, *a0, *a1, *a2;
      
    #elif defined linux
      SOCKADDR_IN        a;
      int                af, s;
      socklen_t          len;
      struct utsname     sysname= { 0 };
      struct hostent     *hostPtr;  
      struct sockaddr_in serverName= { 0 };
      
      FILE*   stream;
      char*   p;
      char    c;
      int     cnt;
      Boolean isEOF;
      #define LINELEN 132
      char    buffer[ LINELEN ];
      char*   v;
      int     n= 1;
      char*   dom;
      char    hostName[ OS9PATHLEN ];
    #endif

        
    OSStatus err= NetInstall(); if (err) return err;

    *inetAddr= 0; /* default values */
    *dns1Addr= 0;
    *dns2Addr= 0;
    strcpy( domainName, "" );

    #ifdef powerc
      err= OTInetGetInterfaceInfo( &iinfo,0 ); if (err) return OS9_ENETDOWN;
      *inetAddr= iinfo.fAddress;
      *dns1Addr= iinfo.fDNSAddr;
      *dns2Addr= 0;
      strcpy( domainName, &iinfo.fDomainName );
    
    #elif defined(windows32)
          h= gethostbyname( "" ); /* my own host */
      if (h==NULL) return OS9_ENETDOWN;
          a0= h->h_addr_list[0]; 
          a1= h->h_addr_list[1]; 
          a2= h->h_addr_list[2]; 

      /* Windows allows more than one host address */   
                              a= a2; /* e.g. PPP under Win2000 */
      if (a==NULL || *a==Unk) a= a1; /* e.g. LAN */
      if (a==NULL || *a==Unk) a= a0; /* if no other one */

      if (a==NULL) return OS9_ENETDOWN;       

      *inetAddr= os9_long( *a );          /* take simply the last one */
      *dns1Addr= 0xd5a02802;             /* %%% currently fixed value */
      *dns2Addr= 0xd5a02822;             /* %%% currently fixed value */
      strcpy( domainName, "ggaweb.ch" ); /* %%% currently fixed value */
      
    #elif defined linux
      err=                  uname( &sysname);
          hostPtr = gethostbyname(  sysname.nodename );
      if (hostPtr==NULL) return OS9_ENETDOWN;

      memset( &serverName, 0, sizeof(serverName));
      memcpy( &serverName.sin_addr, hostPtr->h_addr,
                                    hostPtr->h_length );

      *inetAddr= os9_long( serverName.sin_addr.s_addr );

          stream= fopen( "/etc/resolv.conf","r" ); /* open "resolv.conv" file */
      if (stream!=NULL) {
                  isEOF= false;
          while (!isEOF) {
              p  = buffer;
              cnt= 0;
          
              while( cnt<LINELEN ) {
                  if ((c=fgetc(stream))==EOF) { isEOF= true; break; }
                  if ( c=='\n' ) break; /* abort on CR */
                  
                  *p++= c; /* save in the buffer */        
                  cnt++;
              } /* while */

              *p++= '\0'; 
          //  if (*buffer!='#') printf( "'%s'\n", buffer );
              
              v= "search ";
              if (strstr( buffer,v )!=NULL) {
                  v= buffer + strlen( v );
                  strcpy( domainName, v );
              }
              
              v= "nameserver ";
              if (strstr( buffer,v )!=NULL) {
                  v= buffer + strlen( v );
                  if (n==1) *dns1Addr= os9_long( inet_addr( v ) );
                  else      *dns2Addr= os9_long( inet_addr( v ) );
                  n++;
              }
         } /* loop */
          
          fclose( stream );
      } /* if */

      hostPtr= gethostbyaddr( (void*)&serverName.sin_addr.s_addr,sizeof(ulong), AF_INET );
      strcpy( hostName, hostPtr->h_name );
      dom= strstr( hostName,domainName );
      if (dom!=NULL && strlen(dom)==strlen(domainName)) {
        dom--; if (*dom=='.') *dom= '\0';
      }
//    printf( "'%s'\n", hostName );
      
//    printf( "i12d %08X %08X %08X '%s'\n", *inetAddr, *dns1Addr, *dns2Addr, domainName );
      
    #else
      return OS9_ENETDOWN;
    #endif
    
//  printf( "inet/dns %08X %08X\n", *inetAddr,*dns_Addr );
    return 0;
} /* MyInetAddr */



os9err pNbind( ushort pid, syspath_typ* spP, ulong *nn, byte *ispP )
{
    #ifndef linux
    #pragma unused(pid,nn)
    #endif

    OSStatus err= 0;
    net_typ* net= &spP->u.net;
    ushort   fPort;
    #define  ALLOC_PORT 1025
    
    #ifdef powerc
      TEndpointInfo info;
      TBind         bindReq;
      char*         kN;
    #endif
    
    #ifdef win_linux
      SOCKADDR_IN   name;
    #endif


    #ifndef WITH_NETWORK
      return E_UNKSVC;
    #endif
    
    memcpy        ( &net->ipAddress, ispP, sizeof(net->ipAddress) );
                     net->ipAddress.fHost= os9_long( my_inetaddr );
    fPort= os9_word( net->ipAddress.fPort ); /* little/big endian change */

    #ifdef powerc
      if (net->listen) kN= "tilisten,tcp";
      else             kN=  kTCPName;

      #ifdef USE_CARBON
        net->ls= OTOpenEndpointInContext( OTCreateConfiguration(kN), 0, &info, &err, otContext );                           
      #else
        net->ls= OTOpenEndpoint         ( OTCreateConfiguration(kN), 0, &info, &err );
      #endif
                               
      if (err) return E_FNA;

    /* %%% the "reUse", has still no effect, don't know why ... */
//  if (!net->listen) err= reUse( pid, spP ); if (err) return OS9_EADDRINUSE;
    
    #elif defined win_linux
          net->ls= socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
      if (net->ls==INVALID_SOCKET) return E_FNA;
    #endif

    SetDefaultEndpointModes(net->ls);
    
    /* do it the same way as OS-9: first free port is 1025 */
    if (net->ipAddress.fAddressType==0) fPort= ALLOC_PORT;
    
    #ifdef powerc
      bindReq.addr.buf= (UInt8 *) &net->ipAddress;
      bindReq.addr.len=     sizeof(net->ipAddress);
      bindReq.qlen= 1;
    #endif

    while (true) {
        #ifdef powerc
          net->ipAddress.fPort= os9_word(fPort); /* don't forget to save it back */
               
               err= OTBind( net->ls, &bindReq,nil ); if (!err) break;
          if  (err==kEADDRINUSEErr  ||
               err==kOTNoAddressErr ||
               net->ipAddress.fAddressType==0) ; /* do nothing */
          else break;

        #elif defined win_linux
          #ifdef windows32
            if (net->ipAddress.fAddressType==0)  /* Windows needs AF_INET info ! */
                net->ipAddress.fAddressType= os9_word(AF_INET);
          #endif
          
          name.sin_family     = os9_word(net->ipAddress.fAddressType); 
          name.sin_port       = os9_word(fPort);
          name.sin_addr.s_addr=          net->ipAddress.fHost;  /* my own address */
        
          err= bind( net->ls, &name,sizeof(name) );
          #ifdef windows32
            if     (err) {
                    err= WSAGetLastError();
                if (err==WSAEWOULDBLOCK) err= 0; /* this is not an error */
            }
          #endif
          
      //  upe_printf( "bind %d %d: %d %d %x\n", err, net->ls, 
      //                                             name.sin_family,
      //                                    os9_word(name.sin_port),
      //                                    os9_long(name.sin_addr.s_addr));
          if (!err) break;
        #endif
    
        if (net->ipAddress.fAddressType==0) {
            if  (fPort< ALLOC_PORT) /* dynamically */
                 fPort= ALLOC_PORT; 
            else fPort++;
        }
        else {
            if  (fPort< ALLOC_PORT 
          // || (fPort> 9900 && fPort<10000)
                )
                switch (fPort) { /* start it in parallel to existing ones */
                    case 20: fPort= ALLOC_PORT; break;
                    case 21: fPort= 9921;       break;
                    case 23: fPort= 9923;       break;
                    default: ; /* do nothing */
                }
            else fPort++;
        }

        #ifdef windows32
        if (fPort>=ALLOC_PORT &&
            fPort< 9900) net->ipAddress.fAddressType= 0;
        #endif
    } /* loop */
    
    #ifdef powerc   
      if (err==kEADDRNOTAVAILErr) return OS9_ENETUNREACH;
      if (err)                    return OS9_EADDRINUSE;
    #elif defined(windows32)
      SetDefaultEndpointModes(net->ls);
    #endif
    
    net->bound= true;
    net->fAddT=    net->ipAddress.fAddressType;
                   net->ipAddress.fPort= os9_word(fPort);
    memcpy( ispP, &net->ipAddress, sizeof(net->ipAddress) );
    return 0;
} /* pNbind */



os9err pNlisten( ushort pid, syspath_typ* spP )
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    OSStatus err;
    net_typ* net= &spP->u.net;
    process_typ*  cp= &procs[pid];

    #ifdef powerc
      InetAddress ipRemote;
    #endif


    #ifndef WITH_NETWORK
      return E_UNKSVC;
    #endif
    
    if (cp->state==pWaitRead) set_os9_state( pid, cp->saved_state );

    #ifdef powerc
      OTMemzero( &net->call,   sizeof(TCall) );
      net->call.addr.buf = (UInt8 *) &ipRemote;
      net->call.addr.maxlen =  sizeof(ipRemote);
    
          err= OTListen( net->ls, &net->call );
      if (err==kOTNoDataErr) err= 0; /* this is not an error */

    #elif defined win_linux
      err= listen( net->ls, SOMAXCONN );
    #endif

    if (err) {
        cp->saved_state= cp->state;
        set_os9_state( pid, pWaitRead );
        return E_NOTRDY;
    }
    
    return 0;
} /* pNlisten */



os9err pNconnect( ushort pid, syspath_typ* spP, ulong *n, byte *ispP)
{
    #ifndef linux
    #pragma unused(pid,n)
    #endif

    #define CONNECT_TIMEOUT 100000
    
    OSStatus err= 0;
    net_typ* net= &spP->u.net;
    process_typ*  cp= &procs[pid];
    ushort        fPort;
    
    #ifdef powerc
      TCall sndCall;
      char* kN;
    #endif
    
    #ifdef win_linux
      int          af, ty, proto; 
      SOCKADDR_IN  name;
      
      #ifdef linux
        int  id, u_err;
      #endif
    #endif
   
    #ifndef WITH_NETWORK
      return E_UNKSVC;
    #endif

    if  (cp->state==pWaitRead) {
         cp->state= cp->saved_state;
         cp->wTimeOut--;
    }
    else cp->wTimeOut= CONNECT_TIMEOUT;


    memcpy( &net->ipRemote, ispP, sizeof(net->ipRemote) );
    fPort= os9_word(net->ipRemote.fPort); /* little/big endian change */
    
    if (net->bound) {
        net->ep= net->ls;
        net->ipRemote.fAddressType= net->fAddT;
    }
    else {
        #ifdef powerc
          if (fPort==0) kN= kRawIPName;
          else          kN= kTCPName;
        
          #ifdef USE_CARBON
            net->ep= OTOpenEndpointInContext( OTCreateConfiguration(kN), 0,nil, &err, otContext );
          #else
            net->ep= OTOpenEndpoint         ( OTCreateConfiguration(kN), 0,nil, &err );
          #endif
          
          if (err) return E_FNA;
        
          SetDefaultEndpointModes( net->ep );
          err= OTBind( net->ep, nil,nil ); if (err) return OS9_ECONNREFUSED;
          
        #elif defined win_linux
          af= os9_word(net->ipRemote.fAddressType);
          
          if (fPort==0) { ty= SOCK_RAW;    proto= IPPROTO_ICMP; }
          else          { ty= SOCK_STREAM; proto= IPPROTO_TCP;  }

          #ifdef linux
            if (fPort==0) {
                id= geteuid();        // printf( "%d\n",     id    );
                u_err= seteuid( 0 );  // printf( "err=%d\n", u_err );
            }
          #endif
          
          net->ep= socket( af, ty, proto );
          
          #ifdef linux
            if (fPort==0) {
                                      // printf(  "ep=%d %d\n", net->ep, fPort );
                u_err= seteuid( id ); // printf( "err=%d\n", u_err   );
            }
          #endif
          
          debugprintf(dbgSpecialIO,dbgNorm, ( "connect %d %d %d %d %d\n", net->ep, af, ty, proto, fPort ));
          if (net->ep==INVALID_SOCKET)
              if (fPort==0) return E_PERMIT;
              else          return E_FNA;
        #endif
            
        net->bound= true;
        if (fPort==0) return 0;
    }

//  /* the domain service is now implemented */
//  if (fPort==53) return OS9_ENETUNREACH;
    
    #ifdef powerc   
      sndCall.addr.buf = (UInt8*) &net->ipRemote;
      sndCall.addr.len =    sizeof(net->ipRemote);
      sndCall.opt.buf  = nil;       // no connection options
      sndCall.opt.len  = 0;
      sndCall.udata.buf= nil;       // no connection data
      sndCall.udata.len= 0;
      sndCall.sequence = 0;     // ignored by OTConnect
      err= OTConnect( net->ep, &sndCall, nil ); 

    #elif defined win_linux
      name.sin_family     = os9_word(net->ipRemote.fAddressType);
      name.sin_port       =          net->ipRemote.fPort;
      name.sin_addr.s_addr=          net->ipRemote.fHost;        /* no big/little endian change */

      err= connect( net->ep, &name,sizeof(name) );
    #endif

    debugprintf(dbgSpecialIO,dbgNorm, ( "connect %d %d: %d %d %x\n", err, net->ep, 
                                         os9_word(net->ipRemote.fAddressType),
                                         os9_word(net->ipRemote.fPort),
                                         os9_long(net->ipRemote.fHost) ));

    #ifdef win_linux
      #ifdef windows32
        if     (err) {
                err= WSAGetLastError();
            if (err==WSAEWOULDBLOCK) err= 0; /* this is not an error */
        }
      #endif

      #ifdef linux      
        if (err && cp->wTimeOut>0) {
        //  pNclose( pid, spP );
            cp->saved_state= cp->state;
            cp->state=       pWaitRead;
            return E_NOTRDY;
      } /* if */
      #endif
      
      #ifdef windows32
        if (!err) {
            net->hEventObj= WSACreateEvent();
            err= WSAEventSelect( net->ep, net->hEventObj, 
                                 FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE );
        }
      #endif
    #endif

    if (err) return OS9_ECONNREFUSED;
    
    net->connected= true;
    return 0;
} /* pNconnect */



os9err pNaccept( ushort pid, syspath_typ* spP, ulong *d1 )
{
    OSStatus     err= 0;
    net_typ*     net= &spP->u.net;
    process_typ* cp = &procs[pid];
    syspath_typ* spN;
    ushort       up, path;
    
    #ifdef powerc
      OSStatus       state;
      OTResult       lookResult;
    #endif
    
    #ifdef win_linux
      SOCKADDR_IN    name;
      int            len;
    #endif

    #ifndef WITH_NETWORK
      return E_UNKSVC;
    #endif
    

    if (cp->state==pWaitRead) set_os9_state( pid, cp->saved_state );

    #ifdef powerc
              state= OTGetEndpointState( net->ls );
      switch (state) {
          case T_IDLE:      err= pNlisten( pid, spP ); 
                        if (err) return err;
                        break;
          case T_INCON: break;
          default:      return OS9_ECONNREFUSED;
      }

      if (state!=T_INCON) {
          cp->saved_state= cp->state;
          set_os9_state( pid, pWaitRead );
          return E_NOTRDY;
      }
    
      #ifdef USE_CARBON
        net->ep= OTOpenEndpointInContext( OTCreateConfiguration(kTCPName), 0, nil, &err, otContext );
      #else
        net->ep= OTOpenEndpoint         ( OTCreateConfiguration(kTCPName), 0, nil, &err );
      #endif
      
      if (err) return E_FNA;
    
      SetDefaultEndpointModes( net->ep );
          err= OTAccept( net->ls, net->ep, &net->call);
      if (err==kOTLookErr) {
              lookResult= OTLook(net->ls);
          if (lookResult==T_LISTEN) {
              OTMemzero( &net->call,sizeof(TCall) );
              net->call.addr.buf = (UInt8 *) &net->ipRemote;
              net->call.addr.maxlen =  sizeof(net->ipRemote);
              err= OTListen( net->ls, &net->call );

              SetDefaultEndpointModes( net->ep );
                  err= OTAccept( net->ls, net->ep, &net->call);
              if (err) return OS9_ECONNREFUSED;
          }
      }
    
    #elif defined win_linux
      #ifdef windows32
        net->hEventObj= WSACreateEvent();
        err= WSAEventSelect( net->ls, net->hEventObj, 
                             FD_ACCEPT|FD_READ|FD_WRITE|FD_CLOSE );
      #endif
                                           len= sizeof(name);
          net->ep= accept( net->ls, &name,&len );
      if (net->ep==INVALID_SOCKET) {
          cp->saved_state= cp->state;
          set_os9_state( pid, pWaitRead );
          return E_NOTRDY;
      }
          
      net->ipRemote.fAddressType= os9_word(name.sin_family);
      net->ipRemote.fPort       =          name.sin_port;        /* no big/little endian change */
      net->ipRemote.fHost       =          name.sin_addr.s_addr; /* no big/little endian change */

      SetDefaultEndpointModes( net->ep );   
    #endif

    
    if (err) return E_FNA;
    net->bound   = true;
    net->accepted= true;

    /* "accept" returns a new path */
    err = usrpath_new( pid, &up, fNET );
    path= procs[pid].usrpaths[up]; /* this is the new syspath var */
    spN = get_syspath( pid,path );

    strncpy( spN->name,spP->name, OS9NAMELEN );  /* assign the name */
    memcpy( &spN->u.net, net, sizeof(net_typ) ); /* don't forget to inherit */
    net->bound= false;                           /* don't disconnect in close */
    *d1= up; 

    return err;
} /* pNaccept */




os9err pNrecv( ushort pid, syspath_typ* spP, ulong *d1, ulong *d2, char** a0 )
/* for TCP protocol, 2 additional bytes with length info will be received */
{
    #ifndef linux
    #pragma unused(d2)
    #endif
    
    os9err err;
    ulong  lenB=   2;
    ulong  len = *d2;
    ushort n; /* the length fill be filled in here */
    
    err= netRead( pid, spP, &lenB, (char*)&n, false );
    err= netRead( pid, spP, &len,  (char*)a0, false );

    if   (!err) *d1= len;
    return err;
} /* pNrecv */


os9err pNsend( ushort pid, syspath_typ* spP, ulong *d1, ulong *d2, char** a0 )
/* for TCP protocol, 2 additional bytes with length info must be sent */
{
    os9err err;
    ulong  lenB=   2;
    ulong  len = *d2;
    ushort n   = os9_word( (ushort)len );
    
    err= netWrite( pid, spP, &lenB, (char*)&n, false );
    err= netWrite( pid, spP, &len,  (char*)a0, false );
 
    if   (!err) *d1= len;
    return err;
} /* pNsend */



os9err pNGNam( ushort pid, syspath_typ* spP, ulong *d1, ulong *d2, byte *ispP )
/* Still some hardcoded things here */
/* mode will be:    accepted / connected */
/* ftpd/telnetd:      yes         no     */
/* ftp                no          yes    */
/* ftp data           no          no     */
{   
    #ifndef linux
    #pragma unused(pid)
    #endif
    
	char* c;
    
    net_typ* net= &spP->u.net;
//  upe_printf( "accepted connected %d %d %8X %8X\n", net->accepted, net->connected, ispP, *d2 );
    
    if (net->accepted) {
        *d1= 16; /* std */
        memcpy( ispP, &net->ipAddress, sizeof(net->ipAddress) );
        return 0;
    } /* if */
    
    if (*d2==0) return 0;
    

  	c= (char*)ispP; /* type casting */
    #ifdef powerc
      strcpy( c,"bfomac" );
    #elif defined(windows32)
      strcpy( c,"bfowin" );
    #elif defined linux
      strcpy( c,"bfolinux" );
    #else
      strcpy( c,"unknown" );
    #endif

//  upe_printf( "gnam '%s'\n", c );
    return 0;
} /* pNGNam */



os9err pNSOpt(ushort pid, syspath_typ *spP, ulong *d1, ulong *d2)
/* don't know what this is really good for (bfo) */
{
    #ifndef linux
    #pragma unused(pid,spP)
    #endif

    net_typ* net= &spP->u.net;

    if (*d2==4) {
        net->listen= false;
        net->check = true;
    } /* if */

    *d1=   0;
    return 0;
} /* pNSOpt */



//  #ifdef macintosh
//  /* checksum calculator for "ping" */
//  static UInt16 ChecksumBuffer( UInt16* buf, size_t len )
//  {
//      // This checksum implementation requires the buffer to be an even number of bytes long.
//      UInt32 sum;
//      size_t nwords;
//      
//      nwords = len / 2;
//      sum = 0;
//      while (nwords > 0) {
//          sum+= *buf;
//          buf++;
//          nwords-= 1;
//      } /* while */
//      
//      sum =  (sum>>16) + (sum & 0xffff);
//      sum+=  (sum>>16);
//      return ~sum;
//  } /* ChecksumBuffer */
//  #endif
//  
//  
//  
//  #ifdef win_linux
// 
// Function: FillICMPData
//
// Description:
//    Helper function to fill in various fields for our ICMP request
//
//  void FillICMPData(char *icmp_data, int datasize)
//  {
//      IcmpHeader *icmp_hdr = NULL;
//      char       *datapart = NULL;
//  
//      icmp_hdr = (IcmpHeader*)icmp_data;
//      icmp_hdr->i_type = ICMP_ECHO;        // Request an ICMP echo
//      icmp_hdr->i_code = 0;
//  //  icmp_hdr->i_id   = (USHORT)GetCurrentProcessId();
//      icmp_hdr->i_cksum= 0;
//  //  icmp_hdr->i_seq  = 0;
//    
//      datapart= icmp_data + sizeof(IcmpHeader);
//  
//      // Place some junk in the buffer
//      memset( datapart,'E', datasize - sizeof(IcmpHeader));
//  } /* FillICMPData */
//  #endif
//  



static ushort checksum( ushort *buffer, int size) 
/* This function calculates the 16-bit one's complement sum */
/* of the supplied buffer (ICMP) header */
{
    ulong cksum= 0;

    while ( size>1 ) {
        cksum+= *buffer++;
        size -=  sizeof(ushort);
    } /* while */

    if (size>0) cksum+= *(byte*)buffer; /* odd handling */
    
    cksum =         (cksum>>16) + (cksum & 0xffff);
    cksum+=         (cksum>>16);
    return (ushort)(~cksum);
} /* checksum */



os9err pNsPCmd( ushort pid, syspath_typ *spP, ulong *a0 )
{   
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    OSStatus   err= 0;
    net_typ*   net= &spP->u.net;
    ulong*     u;
    byte*      h;
    IcmpHeader icmp;

    #ifdef powerc
      TUnitData udata;
      
    #elif defined win_linux
//    #define DEF_PACKET_SIZE  32        // Default packet size

      struct sockaddr_in name;
      
//    char icmp_data[ MAX_PACKET ];
//    int datasize = DEF_PACKET_SIZE + sizeof(IcmpHeader); ;
    #endif

    #ifndef WITH_NETWORK
      return E_UNKSVC;
    #endif

    
              u= &net->ipRemote.fHost;
    h= (byte*)u;
    upe_printf( "(%d.%d.%d.%d)\n", h[0],h[1],h[2],h[3] );

    icmp.i_type = ICMP_ECHO;
    icmp.i_code = 0;
    icmp.i_cksum= 0;   // dummy checksum of 0 for purposes of checksum calculation
    icmp.i_ISP  = *a0;
    icmp.i_magic= kOurMagic;
    icmp.i_cksum= checksum( (ushort*)&icmp, sizeof(icmp));
    
    #ifdef powerc
      udata.addr.buf = (byte*)&net->ipRemote;
      udata.addr.len =  sizeof(net->ipRemote);
      udata.opt.buf  = nil;
      udata.opt.len  = 0;
      udata.udata.buf= (UInt8*)&icmp;
      udata.udata.len=   sizeof(icmp);

      err= OTSndUData(net->ep, &udata);
      
    #elif defined win_linux
      memset( (char*)&name, NUL, sizeof(name));
      name.sin_family     = os9_word(net->ipRemote.fAddressType);
      name.sin_port       =          net->ipRemote.fPort;
      name.sin_addr.s_addr=          net->ipRemote.fHost;        /* no big/little endian change */

      err=     sendto( net->ep,   (byte*)&icmp, sizeof(icmp), 0, 
                       (struct sockaddr*)&name, sizeof(name) );

      if (err<0) {
          err= sendto( net->ep,   (byte*)&icmp, sizeof(icmp), 0, 
                       (struct sockaddr*)&name, sizeof(name) );
      }

      upe_printf( "wait a little bit ...\n" );
//    upe_printf( "sendto err=%d\n", err );
      if (err>0) err= 0;
    #endif

    debugprintf(dbgSpecialIO,dbgNorm, ( "NsPCmd %d=%d %d: %d %d %x\n", err,sizeof(IcmpHeader), net->ep, 
                                         os9_word(net->ipRemote.fAddressType),
                                         os9_word(net->ipRemote.fPort),
                                         os9_long(net->ipRemote.fHost) ));
    return err;
} /* pNsPCmd */



os9err pNgPCmd( ushort pid, syspath_typ *spP, ulong *a0 )
{   
    #ifndef linux
    #pragma unused(pid)
    #endif
    
    OSStatus    err= 0;
    long        start_time;
    net_typ*    net= &spP->u.net;
    IcmpHeader  *icmp;

    #ifdef powerc
      TUnitData   udata;
      InetAddress src_addr;
      OTResult    lookResult;
  
      // we use this buffer to hold incoming ICMP packets
      UInt8 icmp_data[5000];
      
    #elif defined win_linux
      char icmp_data[ MAX_PACKET ];
      struct sockaddr_in   from;
      int fromlen = sizeof(from);
      int n;
      IpHeader   *iphdr;
      ushort      iphdrlen;
    #endif
    
    
    #ifndef WITH_NETWORK
      return E_UNKSVC;
    #endif

    
    start_time= GetSystemTick();
    do {
        #ifdef powerc
          // Set up the received...
          udata.addr.buf    = (UInt8*) &src_addr;
          udata.addr.maxlen =   sizeof( src_addr);
          udata.opt.buf     = nil;
          udata.opt.maxlen  = 0;
          udata.udata.buf   =          icmp_data;
          udata.udata.maxlen=   sizeof(icmp_data);
        
          // Look for a packet...
        
              err= OTRcvUData( net->ep, &udata, nil );
          if (err==noErr) {
                  icmp= (IcmpHeader*)&icmp_data[20];
              if (icmp->i_type==ICMP_ECHOREPLY
               && icmp->i_magic==kOurMagic) { *a0= icmp->i_ISP; return 0; }
          } 
          else {
              if (err==kOTNoDataErr) {
                  err= 0;
              }
              
              if (err==kOTLookErr) {
                  lookResult= OTLook(net->ep);
                  upe_printf( "lookResult=%d\n", lookResult );
              }
          }
          
        #elif defined win_linux
          do {   
              err= ioctl( net->ep, FIONREAD, &n );
          
              debugprintf(dbgSpecialIO,dbgDetail, ( "NgPCmd err=%d n=%d\n", err,n ));

              if (n>sizeof(IpHeader)+sizeof(IcmpHeader)) {
                  err= recvfrom( net->ep, icmp_data, n, 0, 
                                 (struct sockaddr*)&from, &fromlen);
                  if (err<=0) break;
              
                  debugprintf(dbgSpecialIO,dbgNorm, ( "NgPCmd %d %d: %d %d %x\n", err, net->ep, 
                                                         from.sin_family,
                                               os9_word( from.sin_port ),
                                               os9_long( from.sin_addr.s_addr )));
              
                  iphdr   = (IpHeader*)icmp_data; // Number of 32-bit words * 4 = bytes
                  iphdrlen= iphdr->h_len * 4;
                  icmp    = (IcmpHeader*)(icmp_data + iphdrlen);
             

               // upe_printf( "err=%d i_type=%d i_seq=%08X i_id=%08X\n", err,
               //                                               icmp->i_type,
               //                                               icmp->i_seq,
               //                                               icmp->i_id );
                                                           
                  if (err>0 && icmp->i_type==0
                            && icmp->i_magic==kOurMagic) {
                          *a0= icmp->i_ISP; return 0;
                   }
              } /* if */
              
              #ifdef linux
                 usleep( 1000000 ); /* sleep in milliseconds */
              #endif
          } while (GetSystemTick() < start_time+5*60);
          
        #else
          return E_UNKSVC;
          
        #endif
    } while (!err && GetSystemTick() < start_time+5*60);
    
    return OS9_ENETUNREACH;
} /* pNgPCmd */




/* constant option sections for SOCKET I$GetStt */
/* type 7 is important, because used by "telnetdc"/"ftpdc" to determine */
/* whether really an internet socket is connected */ 
const byte netstdopts[OPTSECTSIZE]= 
                  { 7, /* PD_DTP 7=SOCKET */
                    0,
                 0x01,
                    0,
                    0,
                 0x10,
                    
                       0,0,  0,0,0,0,0,0,0,0,
           0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,
           0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,
           0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,
              
           0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,
           0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,
           0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,
           0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0 };



/* get options from console */
os9err pNopt(ushort pid, syspath_typ *spP, byte *buffer)
{
    #ifndef linux
    #pragma unused(pid,spP)
    #endif

    memcpy(buffer,netstdopts,OPTSECTSIZE);
    return 0;
} /* pNopt */


os9err pNpos(ushort pid, syspath_typ* spP, ulong *posP )
/* get current file position */
{
    #ifndef linux
    #pragma unused(pid,spP)
    #endif

    *posP= 0;
    return 0;
} /* pRpos */


os9err pNready(ushort pid, syspath_typ *spP, ulong *n )
{
    #ifndef linux
    #pragma unused(pid,spP)
    #endif

    OSStatus err= 0;
    net_typ* net= &spP->u.net;

//  #ifdef windows32
//    WSANETWORKEVENTS ev;
//  #endif


    #ifdef powerc
      OTResult lookResult;
      
          err= OTCountDataBytes( net->ep, n );
      if (err==kOTLookErr) {
              lookResult= OTLook(net->ep);
          if (lookResult==T_DISCONNECT) {
               err= OTRcvDisconnect( net->ep, nil );
               *n= 1;
          }
      }

    #elif defined win_linux
      err= ioctl( net->ep, FIONREAD, n );
      if (*n==0) {
      //       err= WSAEnumNetworkEvents( net->ep, net->hEventObj, &ev );
      //  if (!err && (ev.lNetworkEvents & FD_CLOSE)!=0)
      //  if (ev.lNetworkEvents) upe_printf( "ready %d %x\n", err, ev.lNetworkEvents );
          err= -1;
      }
     
    #else
      return E_UNKSVC;
    #endif

    if (err) *n= 0; /* nothing in buffer if error ! */
    
        *n+= net->bsize; /* still something in buffer ? */
    if (*n>0) return 0;  /* return if not empty */
        
    return os9error(E_NOTRDY);
} /* pNready */



os9err pNask( ushort pid, syspath_typ* spP )
{
    net_typ* net= &spP->u.net;
    OSStatus state;
    os9err   sig, err;
    ulong    n;
    Boolean  ok;

    #ifndef WITH_NETWORK
      return E_UNKSVC;
    #endif

    if (net->check) {
        #ifdef powerc
               state= OTGetEndpointState( net->ls );
          ok= (state==T_INCON);

        #else
          #ifndef linux
          #pragma unused(state)
          #endif
          
          ok= true;
        #endif
    }
    else {
        err= pNready( pid,spP, &n );
        ok = (!err);
    }
        
    if (ok) {
        sig= spP->signal_to_send;    /* must be stored before clear */
             spP->signal_to_send= 0; /* remove the signal after used once */
        return sig;
    }
    return 0;
} /* pNask */

/* eof */

