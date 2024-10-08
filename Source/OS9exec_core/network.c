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
 *    Revision 1.36  2006/06/11 22:07:45  bfo
 *    set_os9_state with 3rd param <callingProc>
 *
 *    Revision 1.35  2006/06/01 18:05:57  bfo
 *    differences in signedness (for gcc4.0) corrected
 *
 *    Revision 1.34  2006/02/19 16:10:34  bfo
 *    Some comments switched off again
 *
 *    Revision 1.33  2005/07/06 21:04:03  bfo
 *    defined UNIX
 *
 *    Revision 1.32  2005/06/30 13:13:23  bfo
 *    Mach-O adaptions
 *
 *    Revision 1.31  2005/04/17 20:46:49  bfo
 *    MPW is supported again
 *
 *    Revision 1.30  2005/01/22 16:20:54  bfo
 *    Unified OT calling interface for Classic and Carbon
 *
 *    Revision 1.29  2004/12/10 10:58:06  bfo
 *    zero reading correction
 *
 *    Revision 1.28  2004/12/04 00:15:37  bfo
 *    a lot of adaptions for MACH kernel
 *
 *    Revision 1.27  2004/11/28 13:54:46  bfo
 *    up to date
 *
 *    Revision 1.26  2004/11/27 12:16:15  bfo
 *    "net_platform" used / _XXX_ introduced
 *
 *    Revision 1.25  2004/11/20 13:29:59  bfo
 *    Compileable for Mac Classic again
 *
 *    Revision 1.24  2004/11/20 11:40:58  bfo
 *    Networking enhanced (for the demo httpd server).
 *
 *    Revision 1.23  2004/10/22 22:51:12  bfo
 *    Most of the "pragma unused" eliminated
 *
 *    Revision 1.22  2004/09/15 19:51:58  bfo
 *    Newer socket library types introduced
 *
 *    Revision 1.21  2004/02/19 19:50:02  bfo
 *    Network IP address bugfix for Windows XP (seems to be the only problem
 * with XP :-))
 *
 *    Revision 1.20  2003/08/01 11:16:52  bfo
 *    up to date
 *
 *    Revision 1.19  2003/05/21 20:34:25  bfo
 *    Preparations for seteuid on Mac OS X
 *
 *    Revision 1.18  2003/05/17 10:36:14  bfo
 *    Several adaptions for Carbon: MGR/telnet are running now.
 *    Not yet fully working: ftp data path, ftpd, telnetd: Permission for OTBind
 * ?
 *
 *    Revision 1.17  2003/05/12 16:30:48  bfo
 *    Define OT variable as static
 *
 *    Revision 1.16  2003/05/08 23:12:40  bfo
 *    MacOSX: Network activated for Carbon
 *
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
#include "net_platform.h"

/* specific network definitions */

#include <sys/utsname.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

// WIL
//  #ifdef linux
//    #include <asm/ioctls.h>
//  #endif

#include <arpa/inet.h>
#include <sys/ioctl.h>

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)

#ifdef MACOSX
#include <sys/filio.h>
#endif

typedef struct sockaddr_in SOCKADDR_IN;

/* --- local procedure definitions for object definition ------------------- */
void init_Net(fmgr_typ *f);

os9err pNopen(ushort pid, syspath_typ *spP, ushort *modeP, const char *pathname);
os9err pNclose(ushort pid, syspath_typ *spP);
os9err pNread(ushort pid, syspath_typ *spP, uint32_t *lenP, char *buffer);
os9err pNreadln(ushort pid, syspath_typ *spP, uint32_t *lenP, char *buffer);
os9err pNwrite(ushort pid, syspath_typ *spP, uint32_t *lenP, char *buffer);
os9err pNwriteln(ushort pid, syspath_typ *spP, uint32_t *lenP, char *buffer);

os9err pNopt(ushort pid, syspath_typ *spP, byte *buffer);
os9err pNpos(ushort pid, syspath_typ *spP, uint32_t *posP);
os9err pNready(ushort pid, syspath_typ *spP, uint32_t *n);

/* network specific functions */
os9err pNbind(ushort pid, syspath_typ *spP, uint32_t *n, byte *ispP);
os9err pNlisten(ushort pid, syspath_typ *spP);
os9err pNconnect(ushort pid, syspath_typ *spP, uint32_t *n, byte *ispP);
os9err pNaccept(ushort pid, syspath_typ *spP, uint32_t *d1);
os9err pNrecv(ushort pid, syspath_typ *spP, uint32_t *d1, uint32_t *d2, uint8_t *a0);
os9err pNsend(ushort pid, syspath_typ *spP, uint32_t *d1, uint32_t *d2, uint8_t *a0);
os9err pNGNam(ushort pid, syspath_typ *spP, uint32_t *d1, uint32_t *d2, byte *ispP);
os9err pNSOpt(ushort pid, syspath_typ *spP, uint32_t *buffer);

os9err pNgPCmd(ushort pid, syspath_typ *spP, ulong *a0);
os9err pNsPCmd(ushort pid, syspath_typ *spP, uint8_t *a0);
/* ------------------------------------------------------------------------- */

void init_Net(fmgr_typ *f)
/* install all procedures of the RBF file manager */
{
    gs_typ *gs = &f->gs;
    ss_typ *ss = &f->ss;

    /* main procedures */
    f->open    = pNopen;
    f->close   = pNclose;
    f->read    = pNread;
    f->readln  = pNreadln;
    f->write   = pNwrite;
    f->writeln = pNwriteln;
    f->seek    = IO_BadMode; /* not allowed */

    /* getstat */
    gs->_SS_Size  = IO_Unimp; /* -- not used */
    gs->_SS_Opt   = pNopt;
    gs->_SS_DevNm = pSCFnam;
    gs->_SS_Pos   = pNpos;
    gs->_SS_EOF   = IO_Nop; /* ignored */
    gs->_SS_Ready = pNready;

    gs->_SS_PCmd = pNgPCmd; /* network spefic function */

    gs->_SS_Undef =
        IO_Unimp; /* -- not used, if any other function */

    /* setstat */
    ss->_SS_Size = IO_Nop; /* ignored */
    ss->_SS_Opt  = IO_Nop; /* ignored */
    ss->_SS_Attr = IO_Nop; /* ignored */

    ss->_SS_Bind    = pNbind; /* network spefic functions */
    ss->_SS_Listen  = pNlisten;
    ss->_SS_Connect = pNconnect;
    ss->_SS_Accept  = pNaccept;
    ss->_SS_Recv    = pNrecv;
    ss->_SS_Send    = pNsend;
    ss->_SS_GNam    = pNGNam;
    ss->_SS_SOpt    = pNSOpt;
    ss->_SS_SendTo  = pNsend;
    ss->_SS_PCmd    = pNsPCmd;

    ss->_SS_Undef = IO_Nop; /* ignored, if any other function */
}

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

/* definitions for ICMP (ping) */
#define kOurMagic 0x5175696E /* enum { kOurMagic = 'Quin' }; */
#define ICMP_ECHOREPLY 0
#define ICMP_ECHO 8

#define MAX_PACKET 1024 // Max ICMP packet size

typedef struct _icmphdr {
    byte   i_type;
    byte   i_code; // Type sub code
    ushort i_cksum;
    ulong  i_ISP; /* use this instead of UInt16 pID/pSeqNum */
    ulong  i_magic;
} IcmpHeader;

/* IP header structure */
typedef struct _iphdr {
    unsigned int   h_len : 4;      // Length of the header
    unsigned int   version : 4;    // Version of IP
    unsigned char  tos;            // Type of service
    unsigned short total_len;      // Total length of the packet
    unsigned short ident;          // Unique identifier
    unsigned short frag_and_flags; // Flags
    unsigned char  ttl;            // Time to live
    unsigned char  proto;          // Protocol (TCP, UDP etc)
    unsigned short checksum;       // IP checksum

    unsigned int sourceIP;
    unsigned int destIP;
} IpHeader;

static void SetDefaultEndpointModes(SOCKET s)
// This routine sets the supplied endpoint into the default
// mode used in this application.  The specifics are:
// blocking, synchronous, and using synch idle events with
// the standard YieldingNotifier.
{
    ulong nonblocking = true;
    int   err         = ioctl(s, FIONBIO, &nonblocking);
}

static os9err GetBuffers(net_typ *net)
/* Get the transfer buffers for the net devices */
{
    net->transferBuffer = get_mem(kTransferBufferSize);
    if (net->transferBuffer.host == NULL)
        return E_NORAM;

    /* common for all operating systems */
    net->b_local = get_mem(kTransferBufferSize);
    if (net->b_local.host == NULL)
        return E_NORAM;

    return 0;
}

os9err pNopen(_pid_, syspath_typ *spP, _modeP_, const char *pathname)
{
    OSStatus err;
    net_typ *net = &spP->u.net;

#ifndef NET_SUPPORT
    return E_UNKSVC;
#endif

    strncpy(spP->name, &pathname[1], OS9NAMELEN); /* assign the name */
    err = NetInstall();
    if (err)
        return err;

    /* not yet bound, install the struct */
    net->ep        = nil;
    net->bound     = false;
    net->accepted  = false;
    net->connected = false;
    net->listen    = true;
    net->check     = false;
    net->closeIt   = false;
    net->bsize     = 0;

    return GetBuffers(net);
}

os9err pNclose(_pid_, syspath_typ *spP)
{
    OSStatus err = 0;
    net_typ *net = &spP->u.net;

#ifndef NET_SUPPORT
    return E_UNKSVC;
#endif

    // upe_printf( "Net Close: %s %d %d, %08X\n", spP->name, net->bound,
    //                                            net->accepted, net->ep );
    if (net->bound && net->ep != nil) {
        // err= shutdown( net->ep, SHUT_RDWR );
        err = close(net->ep);
        release_mem(net->transferBuffer);

        release_mem(net->b_local);
        net->closeIt = true;
        net->bound   = false;
    }

    return 0;
}

static os9err netRead(ushort       pid,
                      syspath_typ *spP,
                      uint32_t    *lenP,
                      void     *buffer,
                      Boolean      lnmode)
{
    ssize_t     err = 0;
    net_typ     *net = &spP->u.net;
    process_typ *cp  = &procs[pid];
    int          ii;
    ulong        askBytes = *lenP, mx;
    Boolean      CR_found = false; /* CR found for <lnmode> */
    ulong        nBytes   = kTransferBufferSize;

    *lenP = 0; /* how many bytes read ? 0 at the beginning */
    do {
        if (net->bsize == 0) { /* buffer is currently empty */
            if (cp->state == pWaitRead)
                set_os9_state(pid, cp->saved_state, "netRead");
            HandleEvent(); /* allow cooperative multitasking */

            err = netReadBlock(pid, net, &nBytes);
            if (err || nBytes == 0) {
                if (net->closeIt)
                    return E_EOF;

                cp->saved_state = cp->state;
                set_os9_state(pid, pWaitRead, "netRead");
                return E_NOTRDY;
            }

            net->bsize = nBytes; /* bytesReceived */
            net->bpos  = net->b_local.host;
            memcpy(net->bpos, net->transferBuffer.host, net->bsize);
        }

        // upe_printf( "netread: nBytes=%d *lenP=%d bsize=%d\n", nBytes, *lenP,
        // net->bsize );
        /* larger ? If no, the rest of the buffer can be taken in one "schlonz"
         */
        mx = net->bsize;
        if (*lenP + mx > askBytes)
            mx = askBytes - *lenP;

        if (lnmode) {
            for (ii = 0; ii < mx; ii++) {
                if (net->bpos[ii] == CR) {
                    CR_found = true;
                    mx       = ii + 1;
                    break;
                }
            }
        }
        memcpy(buffer, net->bpos, mx);

        net->bpos += mx; /* calculate remaining */
        net->bsize -= mx;
        *lenP += mx;
    } while (lnmode && !CR_found && *lenP < askBytes);

    return 0;
}

os9err pNread(ushort pid, syspath_typ *spP, uint32_t *lenP, char *buffer)
{
    return netRead(pid, spP, lenP, buffer, false);
}

os9err pNreadln(ushort pid, syspath_typ *spP, uint32_t *lenP, char *buffer)
{
    return netRead(pid, spP, lenP, buffer, true);
}

static os9err netWrite(ushort       pid,
                       syspath_typ *spP,
                       uint32_t    *lenP,
                       void        *buffer_,
                       Boolean      lnmode)
{
    ssize_t err = 0;
    net_typ *net = &spP->u.net;
    ulong    remain, nBytes;
    int      ii;
    char    *buffer = (char *)buffer_;
    char    *pp;

    if (lnmode) {
        for (ii = 0; ii < *lenP; ii++) {
            if (buffer[ii] == CR) {
                *lenP = ii + 1;
                break;
            }
        }
    }

    pp     = buffer; /* don't change buffer pos, make a copy */
    remain = *lenP;
    *lenP  = 0; /* start counting written bytes */

    while (remain > 0) {
        nBytes = remain;
        if (nBytes > kTransferBufferSize)
            nBytes = kTransferBufferSize; /* not more than buffer */
        memcpy(net->transferBuffer.host, pp, nBytes);

        HandleEvent(); /* allow cooperative multitasking */
        if (net->closeIt)
            return 0;

        err = netWriteBlock(pid, net, &nBytes);
        if (err)
            return err;

        pp += nBytes; /* bytesSent */
        remain -= nBytes;
        *lenP += nBytes;
    }

    return 0;
}

os9err pNwrite(ushort pid, syspath_typ *spP, uint32_t *lenP, char *buffer)
{
    return netWrite(pid, spP, lenP, buffer, false);
}

os9err pNwriteln(ushort pid, syspath_typ *spP, uint32_t *lenP, char *buffer)
{
    return netWrite(pid, spP, lenP, buffer, true);
}

os9err pNbind(_pid_, syspath_typ *spP, _d2_, byte *ispP)
{
    OSStatus err = 0;
    net_typ *net = &spP->u.net;
    ushort   fPort;
#define ALLOC_PORT 1025

    SOCKADDR_IN name;

#ifndef NET_SUPPORT
    return E_UNKSVC;
#endif

    memcpy(&net->ipAddress, ispP, sizeof(net->ipAddress));
    net->ipAddress.fHost = os9_long(my_inetaddr);
    fPort = os9_word(net->ipAddress.fPort); /* little/big endian change */

    net->ep = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (net->ep == INVALID_SOCKET)
        return E_FNA;

    SetDefaultEndpointModes(net->ep);

    /* do it the same way as OS-9: first free port is 1025 */
    debugprintf(dbgSpecialIO,
                dbgNorm,
                ("bind fAddressType=%d\n", net->ipAddress.fAddressType));
    if (net->ipAddress.fAddressType == 0)
        fPort = ALLOC_PORT;

    while (true) {
        name.sin_family      = os9_word(net->ipAddress.fAddressType);
        name.sin_port        = os9_word(fPort);
        name.sin_addr.s_addr = net->ipAddress.fHost; /* my own address */

        err = bind(net->ep, (const struct sockaddr *)&name, sizeof(name));

        // upe_printf( "bind %d %d: %d %d %x\n", err, net->ls,
        //                                           name.sin_family,
        //                                  os9_word(name.sin_port),
        //                                  os9_long(name.sin_addr.s_addr));
        if (!err)
            break;

        if (net->ipAddress.fAddressType == 0) {
            if (fPort < ALLOC_PORT) /* dynamically */
                fPort = ALLOC_PORT;
            else
                fPort++;
        }
        else {
            if (fPort < ALLOC_PORT)
                switch (fPort) { /* start it in parallel to existing ones */
                case 20:
                    fPort = ALLOC_PORT;
                    break;
                case 21:
                    fPort = 9921;
                    break;
                case 23:
                    fPort = 9923;
                    break;
                default:; /* do nothing */
                }
            else
                fPort++;
        }
    }

    net->bound           = true;
    net->fAddT           = net->ipAddress.fAddressType;
    net->ipAddress.fPort = os9_word(fPort);
    memcpy(ispP, &net->ipAddress, sizeof(net->ipAddress));
    debugprintf(dbgSpecialIO,
                dbgNorm,
                ("bind fAddT=%d fPort=%d\n", net->fAddT, fPort));
    return 0;
}

os9err pNlisten(ushort pid, syspath_typ *spP)
{
    OSStatus     err;
    net_typ     *net = &spP->u.net;
    process_typ *cp  = &procs[pid];

#ifndef NET_SUPPORT
    return E_UNKSVC;
#endif

    if (cp->state == pWaitRead)
        set_os9_state(pid, cp->saved_state, "pNlisten");

    err             = listen(net->ep, SOMAXCONN);

    if (err) {
        cp->saved_state = cp->state;
        set_os9_state(pid, pWaitRead, "pNlisten");
        return E_NOTRDY;
    }

    return 0;
}

os9err pNconnect(ushort pid, syspath_typ *spP, _d2_, byte *ispP)
{
#define CONNECT_TIMEOUT 100000

    OSStatus     err = 0;
    net_typ     *net = &spP->u.net;
    process_typ *cp  = &procs[pid];
    ushort       fPort;
    Boolean      isRaw;

    int         af, ty, proto;
    SOCKADDR_IN name;

    int id, u_err;

#ifndef NET_SUPPORT
    return E_UNKSVC;
#endif

    if (cp->state == pWaitRead) {
        cp->state = cp->saved_state;
        cp->wTimeOut--;
    }
    else
        cp->wTimeOut = CONNECT_TIMEOUT;

    memcpy(&net->ipRemote, ispP, sizeof(net->ipRemote));
    fPort = os9_word(net->ipRemote.fPort); /* little/big endian change */
    isRaw = (fPort == 0);

    if (net->bound) {
        net->ipRemote.fAddressType = net->fAddT;
    }
    else {
        af = os9_word(net->ipRemote.fAddressType);

        if (isRaw) {
            ty    = SOCK_RAW;
            proto = IPPROTO_ICMP;
        }
        else {
            ty    = SOCK_STREAM;
            proto = IPPROTO_TCP;
        }

        if (isRaw) {
            id    = geteuid();  // printf( "%d\n",     id    );
            u_err = seteuid(0); // printf( "err=%d\n", u_err );
        }

        net->ep = socket(af, ty, proto);

        if (isRaw) {             // printf(  "ep=%d %d\n", net->ep, fPort );
            u_err = seteuid(id); // printf( "err=%d\n", u_err   );
        }

        debugprintf(
            dbgSpecialIO,
            dbgNorm,
            ("connect %d %d %d %d %d\n", net->ep, af, ty, proto, fPort));
        if (net->ep == INVALID_SOCKET) {
            if (isRaw) {
                return E_PERMIT;
            }
            else {
                return E_FNA;
            }
        }


        net->bound = true;
        if (isRaw)
            return 0;
    }

    //  /* the domain service is now implemented */
    //  if (fPort==53) return OS9_ENETUNREACH;

    name.sin_family = os9_word(net->ipRemote.fAddressType);
    name.sin_port   = net->ipRemote.fPort;
    name.sin_addr.s_addr =
        net->ipRemote.fHost; /* no big/little endian change */

    err = connect(net->ep, (const struct sockaddr *)&name, sizeof(name));

    if (err && cp->wTimeOut > 0) {
        cp->saved_state = cp->state;
        cp->state       = pWaitRead;
        return E_NOTRDY;
    }

    debugprintf(dbgSpecialIO,
                dbgNorm,
                ("connect err=%d %d: %d %d %x\n",
                 err,
                 net->ep,
                 os9_word(net->ipRemote.fAddressType),
                 os9_word(net->ipRemote.fPort),
                 os9_long(net->ipRemote.fHost)));

    if (err)
        return OS9_ECONNREFUSED;

    net->connected = true;
    return 0;
}

os9err pNaccept(ushort pid, syspath_typ *spP, uint32_t *d1)
{
    OSStatus     err = 0;
    net_typ     *net = &spP->u.net;
    process_typ *cp  = &procs[pid];
    syspath_typ *spN;
    ushort       up, path;
    byte         c[4];
    ulong       *cpt;
    SOCKET       epNew;

    SOCKADDR_IN name;
    int         len;

#ifndef NET_SUPPORT
    return E_UNKSVC;
#endif

    if (cp->state == pWaitRead)
        set_os9_state(pid, cp->saved_state, "pNaccept");

    len   = sizeof(name);
    epNew = accept(net->ep, (struct sockaddr *)&name, (unsigned int *)&len);
    if (epNew == INVALID_SOCKET) {
        cp->saved_state = cp->state;
        set_os9_state(pid, pWaitRead, "pNaccept");
        return E_NOTRDY;
    }

    net->ipRemote.fAddressType = os9_word(name.sin_family);
    net->ipRemote.fPort = name.sin_port; /* no big/little endian change */
    net->ipRemote.fHost =
        name.sin_addr.s_addr; /* no big/little endian change */

    SetDefaultEndpointModes(epNew);

    if (err)
        return E_FNA;
    net->bound    = true;
    net->accepted = true;

    /* "accept" returns a new path in <d1> */
    err = usrpath_new(pid, &up, fNET);
    if (err)
        return err;
    path = procs[pid].usrpaths[up]; /* this is the new syspath var */
    spN  = get_syspath(pid, path);

    memcpy(&spN->u.net, net, sizeof(net_typ)); /* don't forget to inherit */
    net          = &spN->u.net; /* take the new struct from now on */
    net->ep      = epNew;
    net->closeIt = false;

    cpt  = (ulong *)&c;
    *cpt = net->ipRemote.fHost; /* get ip address as name */
    sprintf(spN->name, "%d.%d.%d.%d", c[0], c[1], c[2], c[3]);

    err = GetBuffers(net); /* the new path needs his own buffers !! */
    *d1 = up;              /* here we get the new path number */

    return err;
}

/* for TCP protocol, 2 additional bytes with length info will be received */
os9err pNrecv(ushort pid, syspath_typ *spP, uint32_t *d1, uint32_t *d2, uint8_t *a0)
{
    os9err err;
    uint32_t lenB = 2;
    uint32_t len  = *d2;
    ushort n; /* the length fill be filled in here */

    err = netRead(pid, spP, &lenB, (uint8_t *)&n, false);
    err = netRead(pid, spP, &len, a0, false);

    if (!err)
        *d1 = len;
    return err;
}

/* for TCP protocol, 2 additional bytes with length info must be sent */
os9err pNsend(ushort pid, syspath_typ *spP, uint32_t *d1, uint32_t *d2, uint8_t *a0)
{
    os9err err;
    uint32_t lenB = 2;
    uint32_t len  = *d2;
    ushort n    = os9_word((ushort)len);

    err = netWrite(pid, spP, &lenB, &n, false);
    err = netWrite(pid, spP, &len, a0, false);

    if (!err)
        *d1 = len;
    return err;
}

os9err pNGNam(_pid_, syspath_typ *spP, uint32_t *d1, uint32_t *d2, byte *ispP)
/* Still some hardcoded things here */
/* mode will be:    accepted / connected */
/* ftpd/telnetd:      yes         no     */
/* ftp                no          yes    */
/* ftp data           no          no     */
{
    char    *c;
    net_typ *net = &spP->u.net;
    //  upe_printf( "accepted connected %d %d %8X %8X\n", net->accepted,
    //  net->connected, ispP, *d2 );

    if (net->accepted) {
        *d1 = sizeof(net->ipAddress);
        memcpy(ispP, &net->ipAddress, sizeof(net->ipAddress));
        return 0;
    }

    if (*d2 == 0)
        return 0;

    c = (char *)ispP; /* type casting */
    strcpy(c, "bfolinux");

    //  upe_printf( "gnam '%s'\n", c );
    return 0;
}

;
/* os9err pNSOpt(_pid_, syspath_typ *spP, uint32_t *d1, uint32_t *d2) */
os9err pNSOpt(_pid_, syspath_typ *spP, uint32_t *buffer)
{
    /* wil: this function doesn't match the call site, commenting */

#if 0
    /* don't know what this is really good for (bfo) */
    uint32_t *d2 = (ulong*)buffer;
    net_typ *net = &spP->u.net;

    if (*d2 == 4) {
        net->listen = false;
        net->check  = true;
    }

    *d1 = 0;
#endif
    return 0;
}

static ushort checksum(ushort *buffer, int size)
/* This function calculates the 16-bit one's complement sum */
/* of the supplied buffer (ICMP) header */
{
    ulong cksum = 0;

    while (size > 1) {
        cksum += *buffer++;
        size -= sizeof(ushort);
    }

    if (size > 0)
        cksum += *(byte *)buffer; /* odd handling */

    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
    return (ushort)(~cksum);
}

os9err pNsPCmd(_pid_, syspath_typ *spP, uint8_t *a0)
{
    ssize_t    err = 0;
    net_typ   *net = &spP->u.net;
    in_addr_t *u;
    byte      *h;
    IcmpHeader icmp;

    struct sockaddr_in name;

#ifndef NET_SUPPORT
    return E_UNKSVC;
#endif

    u = &net->ipRemote.fHost;
    h = (byte *)u;
    upe_printf("(%d.%d.%d.%d)\n", h[0], h[1], h[2], h[3]);

    icmp.i_type = ICMP_ECHO;
    icmp.i_code = 0;
    icmp.i_cksum =
        0; // dummy checksum of 0 for purposes of checksum calculation
    icmp.i_ISP   = *a0;
    icmp.i_magic = kOurMagic;
    icmp.i_cksum = checksum((ushort *)&icmp, sizeof(icmp));

    memset((char *)&name, NUL, sizeof(name));
    name.sin_family = os9_word(net->ipRemote.fAddressType);
    name.sin_port   = net->ipRemote.fPort;
    name.sin_addr.s_addr =
        net->ipRemote.fHost; /* no big/little endian change */

    err = sendto(net->ep,
                 (byte *)&icmp,
                 sizeof(icmp),
                 0,
                 (struct sockaddr *)&name,
                 sizeof(name));

    if (err < 0) {
        err = sendto(net->ep,
                     (byte *)&icmp,
                     sizeof(icmp),
                     0,
                     (struct sockaddr *)&name,
                     sizeof(name));
    }

    //    upe_printf( "wait a little bit ...\n" );
    //    upe_printf( "sendto err=%d\n", err );
    if (err > 0)
        err = 0;

    debugprintf(dbgSpecialIO,
                dbgNorm,
                ("NsPCmd %d=%d %d: %d %d %x\n",
                 err,
                 sizeof(IcmpHeader),
                 net->ep,
                 os9_word(net->ipRemote.fAddressType),
                 os9_word(net->ipRemote.fPort),
                 os9_long(net->ipRemote.fHost)));
    return err;
}

os9err pNgPCmd(_pid_, syspath_typ *spP, ulong *a0)
{
    ssize_t err = 0;
    long     start_time;
    net_typ *net = &spP->u.net;

    //#ifdef NET_SUPPORT
    IcmpHeader *icmp;
    //#endif

    char               icmp_data[MAX_PACKET];
    struct sockaddr_in from;
    int                fromlen = sizeof(from);
    int                n;
    IpHeader          *iphdr;
    ushort             iphdrlen;

#ifndef NET_SUPPORT
#pragma unused(a0)
    return E_UNKSVC;
#endif

    start_time = GetSystemTick();
    do {

        do {
                        err = ioctl(net->ep, FIONREAD, &n);
                        debugprintf(dbgSpecialIO,
                        dbgDetail,
                        ("NgPCmd err=%d n=%d\n", err, n));

                        if (n > sizeof(IpHeader) + sizeof(IcmpHeader)) {
                            err = recvfrom(net->ep,
                               icmp_data,
                               n,
                               0,
                               (struct sockaddr *)&from,
                               (unsigned int *)&fromlen);
                            if (err <= 0)
                    break;

                debugprintf(dbgSpecialIO,
                            dbgNorm,
                            ("NgPCmd %d %d: %d %d %x\n",
                             err,
                             net->ep,
                             from.sin_family,
                             os9_word(from.sin_port),
                             os9_long(from.sin_addr.s_addr)));

                            iphdr =
                                (IpHeader *)icmp_data; // Number of 32-bit words * 4 = bytes
                            iphdrlen = iphdr->h_len * 4;
                            icmp     = (IcmpHeader *)(icmp_data + iphdrlen);

                            // upe_printf( "err=%d i_type=%d i_seq=%08X i_id=%08X\n", err,
                            //                                               icmp->i_type,
                            //                                               icmp->i_seq,
                            //                                               icmp->i_id );

                            if (err > 0 && icmp->i_type == 0 &&
                    icmp->i_magic == kOurMagic) {
                                *a0 = icmp->i_ISP;
                                return 0;
                }
            }

                        usleep(1000000); /* sleep in microseconds */
        } while (GetSystemTick() < start_time + 5 * 60);
    } while (!err && GetSystemTick() < start_time + 5 * 60);

    return OS9_ENETUNREACH;
}

/* constant option sections for SOCKET I$GetStt */
/* type 7 is important, because used by "telnetdc"/"ftpdc" to determine */
/* whether really an internet socket is connected */
const byte netstdopts[OPTSECTSIZE] = {
    7, /* PD_DTP 7=SOCKET */
    0, 0x01, 0, 0, 0x10,

    0, 0,    0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,    0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,    0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0,    0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,    0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,    0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/* get options from console */
os9err pNopt(_pid_, _spP_, byte *buffer)
{
    memcpy(buffer, netstdopts, OPTSECTSIZE);
    return 0;
}

os9err pNpos(_pid_, _spP_, uint32_t *posP)
/* get current file position */
{
    *posP = 0;
    return 0;
}

os9err pNready(_pid_, syspath_typ *spP, uint32_t *n)
{
    OSStatus err = 0;
    net_typ *net = &spP->u.net;

    //  #ifdef windows32
    //    WSANETWORKEVENTS ev;
    //  #endif

    err = ioctl(net->ep, FIONREAD, n);
    if (*n == 0) {
        //       err= WSAEnumNetworkEvents( net->ep, net->hEventObj, &ev );
        //  if (!err && (ev.lNetworkEvents & FD_CLOSE)!=0)
        //  if (ev.lNetworkEvents) upe_printf( "ready %d %x\n", err,
        //  ev.lNetworkEvents );
        err = -1;
    }

    if (err)
        *n = 0; /* nothing in buffer if error ! */

    *n += net->bsize; /* still something in buffer ? */
    if (*n > 0)
        return 0; /* return if not empty */

    return os9error(E_NOTRDY);
}

os9err pNask(ushort pid, syspath_typ *spP)
{
    net_typ *net = &spP->u.net;
    os9err   sig, err;
    uint32_t n;
    Boolean  ok;

#ifndef NET_SUPPORT
    return E_UNKSVC;
#endif

    if (net->check) {
        ok = true;
    }
    else {
        err = pNready(pid, spP, &n);
        ok  = (!err);
    }

    if (ok) {
        sig = spP->signal_to_send; /* must be stored before clear */
        spP->signal_to_send = 0;   /* remove the signal after used once */
        return sig;
    }

    return 0;
}

/* eof */
