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
 *    Revision 1.4  2003/05/17 11:22:11  bfo
 *    (CVS header included)
 *
 *
 */



/* OS9 Service Request Code Definitions */
/* ==================================== */
#define F_Link      0x00  /* Link to Module */
#define F_Load      0x01  /* Load Module from File */
#define F_UnLink    0x02  /* Unlink Module */
#define F_Fork      0x03  /* Start New Process */
#define F_Wait      0x04  /* Wait for Child Process to Die */
#define F_Chain     0x05  /* Chain Process to New Module */
#define F_Exit      0x06  /* Terminate Process */
#define F_Mem       0x07  /* Set Memory Size */
#define F_Send      0x08  /* Send Signal to Process */
#define F_Icpt      0x09  /* Set Signal Intercept */
#define F_Sleep     0x0A  /* Suspend Process */
#define F_SSpd      0x0B  /* Suspend Process */
#define F_ID        0x0C  /* Return Process ID */
#define F_SPrior    0x0D  /* Set Process Priority */
#define F_STrap     0x0E  /* Set Trap Intercept */
#define F_PErr      0x0F  /* Print Error */
#define F_PrsNam    0x10  /* Parse Pathlist Name */
#define F_CmpNam    0x11  /* Compare Two Name */
#define F_SchBit    0x12  /* Search Bit Map */
#define F_AllBit    0x13  /* Allocate in Bit Map */
#define F_DelBit    0x14  /* Deallocate in Bit Map */
#define F_Time      0x15  /* Get Current Time */
#define F_STime     0x16  /* Set Current Time */
#define F_CRC       0x17 /* Generate CRC */
#define F_GPrDsc    0x18 /* get Process Descriptor copy */
#define F_GBlkMp    0x19 /* get System Block Map copy */
#define F_GModDr    0x1A /* get Module Directory copy */
#define F_CpyMem    0x1B /* Copy External Memory */
#define F_SUser     0x1C /* Set User ID number */
#define F_UnLoad    0x1D /* Unlink Module by name */
#define F_RTE       0x1E  /* Return from Intercept routine */
#define F_GPrDBT    0x1F /* Get system global data copy */
#define F_Julian    0x20  /* Convert gregorian to Julian date */
#define F_TLink     0x21 /* Link trap subroutine package */
#define F_DFork     0x22 /* Debugging Fork call */
#define F_DExec     0x23 /* Debugging execution call (single step) */
#define F_DExit     0x24 /* Debugging exit call (kill child) */
#define F_DatMod    0x25 /* Create data module */
#define F_SetCRC    0x26 /* Generate valid header and CRC in module */
#define F_SetSys    0x27 /* Set/examine system global variable */
#define F_SRqMem    0x28 /* System Memory Request */
#define F_SRtMem    0x29 /* System Memory Return */
#define F_IRQ       0x2A /* Enter IRQ Polling Table */
#define F_IOQu      0x2B /* Enter I/O Queue */
#define F_AProc     0x2C /* Enter Active Process Queue */
#define F_NProc     0x2D /* Start Next Process */
#define F_VModul    0x2E /* Validate Module */
#define F_FindPD    0x2F /* Find Process/Path Descriptor */
#define F_AllPD     0x30 /* Allocate Process/Path Descriptor */
#define F_RetPD     0x31 /* Return Process/Path Descriptor */
#define F_SSvc      0x32 /* Service Request Table Initialization */
#define F_IODel     0x33 /* Delete I/O Module */

#define F_GProcP    0x37  /* Get Process ptr */
#define F_Move      0x38  /* Move Data */
#define F_AllRAM    0x39  /* Allocate RAM blocks */

/* semi-obsolete DAT requests */
#define F_Permit    0x3A /* (old F_AllImg) Allocate Image RAM blocks */
#define F_Protect   0x3B /* (old F_DelImg) Deallocate Image RAM blocks */
#define F_SetImg    0x3C /* Set Process DAT Image */
#define F_FreeLB    0x3D /* Get Free Low Block */
#define F_FreeHB    0x3E /* Get Free High Block */
#define F_AllTsk    0x3F /* Allocate Process Task number */
#define F_DelTsk    0x40 /* Deallocate Process Task number */
#define F_SetTsk    0x41 /* Set Process Task DAT registers */
#define F_ResTsk    0x42 /* Reserve Task number */
#define F_RelTsk    0x43 /* Release Task number */
#define F_DATLog    0x44 /* Convert DAT Block/Offset to Logical */
#define F_DATTmp    0x45 /* Make temporary DAT image */
#define F_LDAXY     0x46 /* Load A [X,[Y]] */
#define F_LDAXYP    0x47 /* Load A [X+,[Y]] */
#define F_LDDDXY    0x48 /* Load D [D+X,[Y]] */
#define F_LDABX     0x49 /* Load A from 0,X in task B */
#define F_STABX     0x4A /* Store A at 0,X in task B */

#define F_AllPrc    0x4B /* Allocate Process Descriptor */
#define F_DelPrc    0x4C /* Deallocate Process Descriptor */
#define F_ELink     0x4D /* Link using Module Directory Entry */
#define F_FModul    0x4E /* Find Module Directory Entry */
#define F_MapBlk    0x4F /* Map Specific Block */
#define F_ClrBlk    0x50 /* Clear Specific Block */
#define F_DelRAM    0x51 /* Deallocate RAM blocks */
#define F_SysDbg    0x52 /* Invoke system level debugger */
#define F_Event     0x53 /* Create/Link to named event */
#define F_Gregor    0x54 /* Convert julian date to gregorian date */
#define F_SysID     0x55 /* return system identification */
#define F_Alarm     0x56 /* send alarm signal */
#define F_SigMask   0x57 /* set signal mask */
#define F_ChkMem    0x58 /* determine if user process may access memory area */
#define F_UAcct     0x59 /* inform user accounting of process status */
#define F_CCtl      0x5A /* cache control */
#define F_GSPUMp    0x5B /* get SPU map information for a process */
#define F_SRqCMem   0x5C /* System Colored Memory Request */
#define F_POSK      0x5D /* execute svc request */
#define F_Panic     0x5E /* Panic warning */
#define F_MBuf      0x5F /* Memory buffer manager */
#define F_Trans     0x60 /* Translate memory address to/from external bus */

/* I/O Service Request Code Definitions */
/* ==================================== */

#define I_Attach    0x80 /* Attach I/O Device */
#define I_Detach    0x81 /* Detach I/O Device */
#define I_Dup       0x82 /* Duplicate Path */
#define I_Create    0x83 /* Create New File */
#define I_Open      0x84 /* Open Existing File */
#define I_MakDir    0x85 /* Make Directory File */
#define I_ChgDir    0x86 /* Change Default Directory */
#define I_Delete    0x87 /* Delete File */
#define I_Seek      0x88 /* Change Current Position */
#define I_Read      0x89 /* Read Data */
#define I_Write     0x8A /* Write Data */
#define I_ReadLn    0x8B /* Read Line of ASCII Data */
#define I_WritLn    0x8C /* Write Line of ASCII Data */
#define I_GetStt    0x8D /* Get Path Status */
#define I_SetStt    0x8E /* Set Path Status */
#define I_Close     0x8F /* Close Path */

#define I_SGetSt    0x92 /* Getstat using system path number */



/*
    Get/SetStat function codes
*/
#define SS_Opt      0x00    /* read/write PD options */
#define SS_Ready    0x01    /* check for device ready */
#define SS_Size     0x02    /* read/write file size */
#define SS_Reset    0x03    /* device restore */
#define SS_WTrk     0x04    /* device write track */
#define SS_Pos      0x05    /* get file current position */
#define SS_EOF      0x06    /* test for end of file */
#define SS_Link     0x07    /* link to status routines */
#define SS_ULink    0x08    /* unlink status routines */
#define SS_Feed     0x09    /* destructive forward skip (form feed) */
#define SS_Frz      0x0a    /* freeze DD_ information */
#define SS_SPT      0x0b    /* set DD_TKS to given value */
#define SS_SQD      0x0c    /* sequence down hard disk */
#define SS_DCmd     0x0d    /* send direct command to device */
#define SS_DevNm    0x0e    /* return device name */
#define SS_FD       0x0f    /* return file descriptor */
#define SS_Ticks    0x10    /* set lockout honor duration */
#define SS_Lock     0x11    /* lock/release record */
#define SS_DStat    0x12    /* return display status */
#define SS_Joy      0x13    /* return joystick value */
#define SS_BlkRd    0x14    /* block read */
#define SS_BlkWr    0x15    /* block write */
#define SS_Reten    0x16    /* retention cycle */
#define SS_WFM      0x17    /* write file mark */
#define SS_RFM      0x18    /* read past file mark */
#define SS_ELog     0x19    /* read error log */
#define SS_SSig     0x1a    /* send signal on data ready */
#define SS_Relea    0x1b    /* release device */
#define SS_Attr     0x1c    /* set file attributes */
#define SS_Break    0x1d    /* send break out serial device */
#define SS_RsBit    0x1e    /* reserve bitmap sector (for disk reorganization) */
#define SS_RMS      0x1f    /* get/set Motorola RMS status */
#define SS_FDInf    0x20    /* get FD info for specified FD sector */
#define SS_ACRTC    0x21    /* get/set Hitachi ACRTC status */
#define SS_IFC      0x22    /* serial input flow control */
#define SS_OFC      0x23    /* serial output flow control */
#define SS_EnRTS    0x24    /* enable RTS (modem control) */
#define SS_DsRTS    0x25    /* disable RTS (modem control) */
#define SS_DCOn     0x26    /* send signal DCD TRUE */
#define SS_DCOff    0x27    /* send signal DCD FALSE */
#define SS_Skip     0x28    /* skip block(s) */
#define SS_Mode     0x29    /* set RBF access mode */
#define SS_Open     0x2a    /* notification of new path opened */
#define SS_Close    0x2b    /* notification of path being closed */

#define SS_Path     0x2c    /* CDFM return pathlist for open path */
#define SS_Play     0x2d    /* CDFM play (CD-I) file */
#define SS_HEADER   0x2e    /* CDFM return header of last sector read */
#define SS_Raw      0x2f    /* CDFM read raw sectors */
#define SS_Seek     0x30    /* CDFM issue physical seek command */
#define SS_Abort    0x31    /* CDFM abort asynchronous operation in progress */
#define SS_CDDA     0x32    /* CDFM play CD digital audio */
#define SS_Pause    0x33    /* CDFM pause the disc driver */
#define SS_Eject    0x34    /* CDFM open the drive door */
#define SS_Mount    0x35    /* CDFM mount disc by disc number */
#define SS_Stop     0x36    /* CDFM stop the disc drive */
#define SS_Cont     0x37    /* CDFM start the disc after pause */
#define SS_Disable  0x38    /* CDFM disable hardware controls */
#define SS_Enable   0x39    /* CDFM enable hardware controls */
#define SS_ReadToc  0x3a    /* CDFM read TOC (on red discs) */
#define SS_SM       0x3b    /* CDFM's soundmap control status code */
#define SS_SD       0x3c    /* CDFM's sound data manipulation status code */
#define SS_SC       0x3d    /* CDFM's sound control status code */

#define SS_SEvent   0x3e    /* set event on data ready */
#define SS_Sound    0x3f    /* produce audible sound */
#define SS_DSize    0x40    /* get drive size (in sectors) */
#define SS_Net      0x41    /* NFM wild card getstat/setstat, with subcode */
#define SS_Rename   0x42    /* rename file */
#define SS_Free     0x43    /* return free statistics (NRF) */
#define SS_VarSect  0x44    /* variable sector size supported query */
#define SS_VolStore 0x45    /* return volume storage statistics */
#define SS_MIDI     0x46    /* MIDI wild card getstat/setstat, with subcode */
#define SS_ISDN     0x47    /* ISDN getstat/setstat, with subcode */
#define SS_PMOD     0x48    /* Protocol module get/setstat, with subcode */
#define SS_SPF      0x48    /* SPF get/setstat, with subcode: SAME as SS_PMOD */
#define SS_LUOPT    0x49    /* DPIO: get/set logical unit options */
#define SS_RTNFM    0x4a    /* RTNFM set/getstat with sub-code */

#define SS_UCM      0x4c    /* UCM reserved */
#define SS_DM       0x51    /* UCM's drawmap control status code */
#define SS_GC       0x52    /* UCM's graphics cursor status code */
#define SS_RG       0x53    /* UCM's region status code */
#define SS_DP       0x54    /* UCM's drawing parameters status code */
#define SS_DR       0x55    /* UCM's graphics drawing status code */
#define SS_DC       0x56    /* UCM's display control status code */
#define SS_CO       0x57    /* UCM's character output status code */
#define SS_VIQ      0x58    /* UCM's video inquiry status code */
#define SS_PT       0x59    /* UCM's pointer status code */
#define SS_SLink    0x5a    /* UCM link external subroutine module to UCM */
#define SS_KB       0x5b    /* keyboard status code */

/*
    Get/SetStat function codes for sockets
*/

#define SS_Bind     0x6c    /* bind a socket name */
#define SS_Listen   0x6d    /* listen for connections */
#define SS_Connect  0x6e    /* initiate a connection */
#define SS_Resv     0x6f    /* socket characteristics specification */
#define SS_Accept   0x70    /* accept socket connections */
#define SS_Recv     0x71    /* receive data */
#define SS_Send     0x72    /* send data */
#define SS_GNam     0x73    /* get socket name */
#define SS_SOpt     0x74    /* set socket option */
#define SS_GOpt     0x75    /* get socket option */
#define SS_Shut     0x76    /* shut down socket connection */
#define SS_SendTo   0x77    /* send to address */
#define SS_RecvFr   0x78    /* receive from address */
#define SS_Install  0x79    /* install upper level protocol (ULP) */
                            /* handler on ifman list */
#define SS_PCmd     0x7a    /* protocol direct command */

#define SS_SN       0x8c    /* DSM's screen functions */
#define SS_AR       0x8d    /* DSM's action region functions */
#define SS_MS       0x8e    /* DSM's message functions */
#define SS_AC       0x8f    /* DSM's action cursor functions */
#define SS_CDFD     0x90    /* CDFM return file descriptor information */
#define SS_CChan    0x91    /* CDFM change channel request */
#define SS_FG       0x92    /* GFM frame grabber set/getstat functions */
#define SS_XWin     0x93    /* X windows set/getstat functions */

#define SS_SOCKMAN  0x94    /* Get/Set Stat code for SOCKMAN (with subcodes)*/
#define SS_MAUI     0x95    /* Get/Set Stat code for MAUI (with subcodes) */




/* Signal Codes */
/* ============ */

#define S_Kill      0   /* system abort, unconditional */
#define S_Wake      1   /* wake up process */
#define S_Abort     2   /* keyboard abort */
#define S_Intrpt    3   /* keyboard interrupt */
#define S_HangUp    4   /* modem hangup */
#define S_Activate  5   /* activation signal */

