// 
// � �OS9exec,   OS-9 emulator for Mac OS, Windows and Linux 
// � �Copyright (C) 2002 �[ Lukas Zeller / Beat Forster ]
//	  Available under http://www.synthesis.ch/os9exec
// 
// � �This program is free software; you can redistribute it and/or 
// � �modify it under the terms of the GNU General Public License as 
// � �published by the Free Software Foundation; either version 2 of 
// � �the License, or (at your option) any later version. 
// 
// � �This program is distributed in the hope that it will be useful, 
// � �but WITHOUT ANY WARRANTY; without even the implied warranty of 
// � �MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// � �See the GNU General Public License for more details. 
// 
// � �You should have received a copy of the GNU General Public License 
// � �along with this program; if not, write to the Free Software 
// � �Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
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
/*        forsterb@dial.eunet.ch              */
/**********************************************/


/* This file contains the RBF Emulator */
#include "os9exec_incl.h"
#include "filescsi.h"

#ifdef macintosh
  #include <Scsi.h>
#elif defined(windows32)
  // NT DDK includes (extracted from NT4DDK)
  #include "devioctl.h"
  //#include <ntdddisk.h> // seems to be included in W2K windows.h
  #include "ntddscsi.h"
  #include "stddef.h"
  // spti struct with sense buffer included (as we need to specify offset...)
  typedef struct {
    SCSI_PASS_THROUGH_DIRECT sptd;
    ULONG             Filler;      // realign buffer to double word boundary
    UCHAR             ucSenseBuf[32];
  } SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER;
#endif


#define CmdRead  0x08
#define CmdWrite 0x0a
#define CmdSel   0x15
#define CmdSense 0x1a

#define CB_Size   6
#define SizeCmd  12



#ifdef windows32

#define MAXNUMSCSIADAPTORS 10

void scsiadaptor_help(void)
{
    int   n;
    char  w[30];
    char  v[5];
    short adaptno;
    char  adaptername[20] = "\\\\.\\scsix:";
    UCHAR buffer[2048];
    HANDLE fileHandle;
    ULONG returned,i;
    PSCSI_ADAPTER_BUS_INFO  adapterInfo;
    PSCSI_INQUIRY_DATA inquiryData;
        
    for(adaptno=0; adaptno<MAXNUMSCSIADAPTORS; adaptno++) {
      // - insert adaptor no in name
      adaptername[8]=0x30+adaptno;
      // - create file handle to use SCSI Pass Through with
      fileHandle = CreateFile((const char *)adaptername,
        GENERIC_READ,       // read only here
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
      );
      // - check error
      if (fileHandle == INVALID_HANDLE_VALUE) continue; // try next
      // - get inquiry data  
      if (!DeviceIoControl(
        fileHandle,
        IOCTL_SCSI_GET_INQUIRY_DATA,
        NULL,
        0,
        buffer,
        sizeof(buffer),
        &returned,
        FALSE
      )) {
        // failed
        CloseHandle(fileHandle);
        continue;
      }
      // - show inquiry data
      adapterInfo = (PSCSI_ADAPTER_BUS_INFO) buffer;
      n= adapterInfo->NumberOfBuses;
      upho_printf("Adaptor SCSI%d has %d %s:\n",adaptno,n, n==1 ? "bus":"buses");
      for (i=0; i<adapterInfo->NumberOfBuses; i++) {
        inquiryData = (PSCSI_INQUIRY_DATA) (buffer +
          adapterInfo->BusData[i].InquiryDataOffset);
        upho_printf("- Bus #%d, Initiator SCSI-ID = %d:\n",i, adapterInfo->BusData[i].InitiatorBusId);
        while (adapterInfo->BusData[i].InquiryDataOffset) {
            memcpy( w, &inquiryData->InquiryData[ 8], 24 );
                    w[24]= NUL; /* terminate it */
            memcpy( v, &inquiryData->InquiryData[32],  4 );
                    v[ 4]= NUL; /* terminate it */
           
            upho_printf("  - SCSI ID=%d, LUN=%d: %s %s%s\n",
               inquiryData->TargetId,
               inquiryData->Lun,
               w,v,
              (inquiryData->DeviceClaimed) ? " (NOT AVAILABLE)" : "" );
              
            if (inquiryData->NextInquiryDataOffset == 0) break;
            
            inquiryData= (PSCSI_INQUIRY_DATA) (buffer +
            inquiryData->NextInquiryDataOffset);
        } // while
      } // for each bus
    } // for each adaptor
} // scsiadaptor_help


void scsi_finddefaults(void)
{
    short adaptno;
    char adaptername[20] = "\\\\.\\scsix:";
    UCHAR buffer[2048];
    HANDLE fileHandle;
    ULONG returned,i;
    PSCSI_ADAPTER_BUS_INFO  adapterInfo;
    PSCSI_INQUIRY_DATA inquiryData;
        
    for(adaptno=0; adaptno<MAXNUMSCSIADAPTORS; adaptno++) {
      // - insert adaptor no in name
      adaptername[8]=0x30+adaptno;
      // - create file handle to use SCSI Pass Through with
      fileHandle = CreateFile((const char *)adaptername,
        GENERIC_READ,       // read only here
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
      );
      // - check error
      if (fileHandle == INVALID_HANDLE_VALUE) continue; // try next
      // - get inquiry data  
      if (!DeviceIoControl(
        fileHandle,
        IOCTL_SCSI_GET_INQUIRY_DATA,
        NULL,
        0,
        buffer,
        sizeof(buffer),
        &returned,
        FALSE
      )) {
        // failed
        CloseHandle(fileHandle);
        continue;
      }
      // - analyze inquiry data
      adapterInfo = (PSCSI_ADAPTER_BUS_INFO) buffer;
      for (i=0; i<adapterInfo->NumberOfBuses; i++) {
        inquiryData = (PSCSI_INQUIRY_DATA) (buffer +
          adapterInfo->BusData[i].InquiryDataOffset);
        if ( adapterInfo->BusData[i].InitiatorBusId <=31) {
            // assume this is a SCSI bus (initiator of IDE is 255)
            defSCSIAdaptNo = adaptno;
            defSCSIBusNo = i;
            break; // done
        } // while
      } // for each bus
    } // for each adaptor
} // scsi_finddefaults
#endif



/* the SCSI lowlevel call */
static os9err SCSIcall( short scsiAdapt, ushort scsiBus, ushort scsiID, ushort scsiLUN,
                        byte* cb, byte*  buffer, ulong len, Boolean doWrite )
{
    os9err err;
    char*  s;

    #ifdef macintosh
      #pragma unused(scsiAdapt,scsiBus,scsiLUN)
      
      OSErr  oserr, ocerr;
      ushort tib[10];
      ulong  *l;
      short  stat,message;
      int    wait= 200;

          tib[0]=     os9_word( scInc  ); /* fill the tibPtr structure */
      l= &tib[1]; *l= os9_long( (ulong)buffer );
      l= &tib[3]; *l= os9_long( len           );
          tib[5]=     os9_word( scStop );
      l= &tib[6]; *l= os9_long( NULL   );
      l= &tib[8]; *l= os9_long( NULL   );

      do {            oserr= SCSIGet(); /* go through the SCSI commands */
          if (!oserr) oserr= SCSISelect( scsiID );
          if  (oserr) { err= host2os9err(oserr,E_UNIT); break; }

               oserr= SCSICmd( cb,CB_Size );
          if (!oserr) {
              if (doWrite) oserr= SCSIWrite( tib );
              else         oserr= SCSIRead ( tib );
          }
                      /* it's done */
                      ocerr= SCSIComplete( &stat,&message, wait );
          if (!oserr) oserr= ocerr;
          
	      err= host2os9err(oserr,E_NOTRDY);
	  } while (false);

    #elif defined(windows32)      
      HANDLE fileHandle;

      DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;  // default
      DWORD accessMode = GENERIC_WRITE | GENERIC_READ;       // default
      // number will be substituted
      char adaptername[12] = "\\\\.\\scsix:";
      BOOL repeated;
      
      // more vars
      byte *dataBuffer = NULL; // pointer to extra data buffer if passed buffer is ill-aligned
      byte *bufptr; // buffer pointer used for chunked transfers
      ULONG returned,length;
      ULONG align;
      IO_SCSI_CAPABILITIES capabilities;
      SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER spti_desc;
      ushort k;
      ushort sensekey;
    
      // test address first
      if (scsiAdapt<0) {
        debugprintf(dbgFiles,dbgNorm,(
          "# SCSI subsystem: bad Adaptor No (%d>) or bad Bus No (%d)\n",
          scsiAdapt,
          scsiBus
        ));
        err=E_UNIT;
        goto exitroutine;
      }
      // default error
      err=E_UNIT;
      // set correct adapter no
      adaptername[8]=0x30+scsiAdapt;
      // - create file handle to use SCSI Pass Through with
      fileHandle = CreateFile((const char *)adaptername,
        accessMode,
        shareMode,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
      );
      // - check error
      if (fileHandle == INVALID_HANDLE_VALUE) {
        debugprintf(dbgFiles,dbgNorm,(
          "# SCSI subsystem: failed to open adapter handle for '%s', winError=%d\n",
          adaptername,
          GetLastError()
        ));
        err=E_UNIT;
        goto exitroutine;
      }
      // - get capabilities
      if (!DeviceIoControl(
        fileHandle,
        IOCTL_SCSI_GET_CAPABILITIES,
        NULL,
        0,
        &capabilities,
        sizeof(IO_SCSI_CAPABILITIES),
        &returned,
        FALSE
      )) {
      // failed
        debugprintf(dbgFiles,dbgNorm,(
          "# SCSI subsystem: IOCTL_SCSI_GET_CAPABILITIES failed, winerr=%ld\n",
          GetLastError()
        ));
        err=E_UNIT;
        goto closeandexit;        
      }
      // check capabilities
      bufptr=buffer; // assume we can use the passed buffer directly
      // - check alignment
      align=capabilities.AlignmentMask;
      if ((align & (ULONG)buffer)!=0) {
          debugprintf(dbgFiles,dbgNorm,(
              "# SCSI subsystem: Passed buffer (@ $%0lX) does not meet alignment requirement of SCSI hardware: $%0lX\n",
              (ULONG)buffer,
              align
          ));
          // we need an aligned temp buffer
          // - get buffer that is long enough to contain aligned data
          dataBuffer = get_mem(len+align,false);
          if (!dataBuffer) {
              debugprintf(dbgFiles,dbgNorm,("# SCSI subsystem: Could not allocate aligned buffer\n"));
              err=E_NORAM;
              goto closeandexit;
          }
          // - determine aligned starting point in new buffer
          bufptr = (byte *)(((ULONG)dataBuffer + align) & ~align);
          // - copy data to buffer if writing
          if (doWrite) CopyMemory(bufptr,buffer,len);
      }   
      // - check max transfer size
      if (len>capabilities.MaximumTransferLength) {
          debugprintf(dbgFiles,dbgNorm,(
              "# SCSI subsystem: Request exceeds ($%0lX) max transfer size of $%0lX bytes\n",
              len,
              capabilities.MaximumTransferLength
          ));
          err=E_PARAM;        
          goto closeandexit;
      }
      // Now issue SCSI command
      repeated=false;      
    doItAgain: // label for retry after UNIT ATTENTION
      // - create passthrough structure for request
      ZeroMemory(&spti_desc, sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER));
      spti_desc.sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
      // - addressing
      spti_desc.sptd.PathId = scsiBus;
      spti_desc.sptd.TargetId = scsiID;
      spti_desc.sptd.Lun = scsiLUN;
      // - set up data buffers
      spti_desc.sptd.DataIn = doWrite ? SCSI_IOCTL_DATA_OUT : SCSI_IOCTL_DATA_IN; // in or out
      spti_desc.sptd.SenseInfoLength = 24; // how much sense info we'd like to get max
      spti_desc.sptd.DataTransferLength = len; // how many bytes in(to) the buffer
      spti_desc.sptd.TimeOutValue = 2;
      spti_desc.sptd.DataBuffer = bufptr;
      spti_desc.sptd.SenseInfoOffset =
         offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER,ucSenseBuf);
      // - set up command itself       
      spti_desc.sptd.CdbLength = CB_Size; // size of command block
      // - copy command bytes
      for (k=0; k<CB_Size; k++) spti_desc.sptd.Cdb[k]=cb[k];
      // - issue now
      length = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
      if (!DeviceIoControl(
          fileHandle,
          IOCTL_SCSI_PASS_THROUGH_DIRECT,
          &spti_desc,
          length,
          &spti_desc,
          length,
          &returned,
          FALSE
      )) {
          // failed
          debugprintf(dbgFiles,dbgNorm,(
            "# SCSI subsystem: IOCTL_SCSI_PASS_THROUGH_DIRECT failed, winerr=%ld\n",
            GetLastError()
          ));
          err=E_UNIT;
          goto closeandexit;
      }
      // - call succeeded, but check for SCSI errors
      err=0; // assume ok
      if (spti_desc.sptd.ScsiStatus) {
          // non-zero SCSI status
          debugprintf(dbgFiles,dbgNorm,(
              "# SCSI subsystem: SCSI Status = 0x%02X\n",
              spti_desc.sptd.ScsiStatus
          ));
          // show sense info if available
          sensekey=0;
          if (spti_desc.sptd.SenseInfoLength>=3) {
              // at least 3 bytes, including sense key
              sensekey = spti_desc.ucSenseBuf[2]; // sense key 
              #ifndef NODEBUG    
              debugprintf(dbgFiles,dbgDetail,("# - Sense info : "));
              for (k=0; k<spti_desc.sptd.SenseInfoLength; k++) {
                debugprintf(dbgFiles,dbgDetail,("%02X ", spti_desc.ucSenseBuf[k]));
              }
              debugprintf(dbgFiles,dbgDetail,("\n"));
              #endif
          }
          // convert SCSI error to os9error
          switch (sensekey) {
            case 2: // not ready
              err=E_NOTRDY;
              break;          
            case 3: // medium error
              err=E_BTYP;
              break;
            case 4: // hardware failure
              err=E_SEEK;
              break;
            case 5: // illegal request
              err=E_SECT;
              break;
            case 6: // Unit attention
              if (!repeated) {
                // repeat once after unit attention
                repeated=true;
                goto doItAgain;
              }
              err=E_UNIT;
              break;
            case 7: // data protect
              err=E_WP;
              break;
            default:
              err=E_UNIT;
              break;          
          }  
      }    
      // if aligned buffer was allocated, copy back (on read) and deallocate it
      if (dataBuffer) {
          // copy back to original buffer on read
          if (!doWrite) CopyMemory(buffer,bufptr,len);
          // anyway, get rid of buffer
          release_mem(dataBuffer,false);
      }
      // - close file
    closeandexit:
      CloseHandle(fileHandle);
    exitroutine:
    #else
      // linux or others???
      err=E_UNIT; // just fail
    #endif

    if (debugcheck(dbgFiles,dbgDetail)) {
        switch (cb[0]) {
            case CmdSel  : s= "CmdSel";   break;
            case CmdSense: s= "CmdSense"; break;
            case CmdRead : s= "CmdRead";  break;
            case CmdWrite: s= "CmdWrite"; break;
            default      : s= "<none>";
        }

        upe_printf("# SCSIcall: %s (err=%d)\n", s, err );
    } /* if */

    return err;
} /* SCSIcall */



os9err Set_SSize( short scsiAdapt, ushort scsiBus, ushort scsiID, ushort scsiLUN,
                  ulong sctSize )
/* Set the SCSI sector size */
{
    byte   cb     [CB_Size];
    ulong  dat_buf[3];

    cb[0]= CmdSel;
    cb[1]= ((scsiLUN & 0x07)<<5);
    cb[2]= 0;
    cb[3]= 0;
    cb[4]= SizeCmd;
    cb[5]= 0;

    dat_buf[0]= os9_long( 8 ); /* block descriptor */
    dat_buf[1]= os9_long( 0 );
    dat_buf[2]= os9_long( sctSize );

    return SCSIcall( scsiAdapt, scsiBus, scsiID, scsiLUN, (byte*)&cb, (byte*)&dat_buf,sizeof(dat_buf), true );
} /* Set_SSize */


os9err Get_SSize( short scsiAdapt, ushort scsiBus, ushort scsiID, ushort scsiLUN,
                  ulong *sctSize )
/* Get the SCSI sector size */
{
    os9err err;
    byte   cb     [CB_Size];
    ulong  dat_buf[3];

    cb[0]= CmdSense;
    cb[1]= ((scsiLUN & 0x07)<<5);
    cb[2]= 0;
    cb[3]= 0;
    cb[4]= SizeCmd;
    cb[5]= 0;

    err= SCSIcall( scsiAdapt, scsiBus, scsiID, scsiLUN, (byte*)&cb, (byte*)&dat_buf,sizeof(dat_buf), false );
    *sctSize = os9_long( dat_buf[2] );
    return err;
} /* Set_SSize */


os9err Get_DSize( short scsiAdapt, ushort scsiBus, ushort scsiID, ushort scsiLUN,
                  ulong *totScts )
/* Get the SCSI device size */
{
    os9err err;
    byte   cb     [CB_Size];
    ulong  dat_buf[3];
    
    cb[0]= CmdSense;
    cb[1]= ((scsiLUN & 0x07)<<5);
    cb[2]= 0;
    cb[3]= 0;
    cb[4]= SizeCmd;
    cb[5]= 0;

    err= SCSIcall( scsiAdapt, scsiBus, scsiID, scsiLUN, (byte*)&cb, (byte*)&dat_buf,sizeof(dat_buf), false );
    *totScts= os9_long( dat_buf[1] );
    return err;
} /* Get_DSize */



os9err ReadFromSCSI( short scsiAdapt, ushort scsiBus, ushort scsiID, ushort scsiLUN,
                     ulong sectorNr, ulong nSectors,
                     ulong len, byte* buffer )
{
    byte   cb[CB_Size];
    ulong* l;

    l= (ulong*)&cb[0]; *l= os9_long( sectorNr); /* cb0,cb1,cb2,cb3 */
                cb[0]= CmdRead;    /* and overwrite cb0 field */
                cb[1]= (cb[1] & 0x1F) | ((scsiLUN & 0x07)<<5); // upper 3 bits are LUN
                cb[4]= nSectors;
                cb[5]= 0;

    return SCSIcall( scsiAdapt, scsiBus, scsiID, scsiLUN, (byte*)&cb, buffer,len, false );
} /* ReadFromSCSI */



os9err WriteToSCSI( short scsiAdapt, ushort scsiBus, ushort scsiID, ushort scsiLUN,
                    ulong sectorNr, ulong nSectors,
                    ulong len,      byte* buffer )
{
    byte   cb[CB_Size];
    ulong* l;

    l= (ulong*)&cb[0]; *l= os9_long( sectorNr ); /* cb0,cb1,cb2,cb3 */
                cb[0]= CmdWrite;   /* and overwrite cb0 field */
                cb[1]= (cb[1] & 0x1F) | ((scsiLUN & 0x07)<<5); // upper 3 bits are LUN
                cb[4]= nSectors;
                cb[5]= 0;

    return SCSIcall( scsiAdapt, scsiBus, scsiID, scsiLUN, (byte*)&cb, buffer,len, true );
} /* WriteToSCSI */


/* eof */