/*---------------------------------------------------------------------------,
|                                                                            |
|              Copyright 2001 by Microware Systems Corporation.              |
|              All Rights Reserved.   Reproduced Under License.              |
|                                                                            |
|  This software is confidential property of Microware Systems Corporation,  |
|  and is provided under license for internal development purposes only.     |
|  Reproduction, publication, modification, distribution, or creation of     |
|  derivative works in any form to any party other than the Licensee is      |
|  strictly prohibited, unless expressly provided for under the terms of a   |
|  mutually executed written license agreement for this software between     |
|  the end-user and Microware Systems Corporation, or otherwise expressly    |
|  authorized in writing by Microware Systems Corporation.  Licensee shall   |
|  reproduce this copyright notice exactly as provided herein on any copies  |
|  of the software that Licensee is authorized to distribute.                |
|                                                                            |
|----------------------------------------------------------------------------|
|                                                                            |
| Edition History:                                                           |
| #   Date      Comments                                                By   |
| --- --------- ------------------------------------------------------- ---- |
|   1 17-Mar-92 Created.                                                rcb  |
|   2 24-Jun-92 Lots of changes for _ANSI_EXT pre-processor symbol      rry  |
|   3 29-Jul-92 New error #'s for ANSI, POSIX, and exceptions           rry  |
|   4 01-Sep-92 New wide area network errors                            rcb  |
|   5 29-Sep-93 Added errors that were in sys.l, but not errno.h        rcb  |
|   6 13-Feb-95 New conditionals for C++                                rcb  |
|   7 03-Jul-96 Fixed octal error messages.                             rry  |
|   8 20-Feb-98 Added this header.                                      nan  |
|   9 08-Jan-00 Merged spf.h errors and changed WANBASE to SPFBASE.     rds  |
|  10 11-Jan-00 Merged MAUI error codes from MAUI/maui_com.h.           srm  |
|  11 11-Feb-00 Added Reserved Value comment for errors below $20.      mgh  |
|  12 21-Feb-00 Thread support added                                rry/ajk  |
|  13 09-Mar-00 Added IEEE1394 error codes, updated copyright.          cdh  |
|     31-Mar-00 Revised MIPS predefined name usage to fit new MIPS      jej  |
|               nomenclature.                                                |
|  14 25-May-00 Mapped EAGAIN to EWOULDBLOCK                            rds  |
|  15 13-Feb-01 Adding additional reserved group numbers				rry  |
|  16 05-Mar-01 Adding x86 specific										jhd  |
|  17 05-Jun-01 Changed CMX to SH-5M									rry  |
`---------------------------------------------------------------------------*/

/* System service error codes which may be returned from a call
 * or may be found in 'errno' after an error has occurred.
 *
 * Group allocation:
 *
 * 000  -  OS-9 / OS-9000 operating system
 * 001  -  C compiler
 * 006  -  RAVE
 * 007  -  IFF / Ethernet
 * 008  -  SPF
 * 009  -  ROM
 * 010  -  MAUI
 * 011	-  OS Thread
 * 012  -  Application FrameWork library
 * 020  -  POSIX
 * 100  -  PowerPC processor specific codes
 * 101  -  SH-5M   processor specific codes
 * 102  -  MIPS    processor specific codes
 * 103  -  ARM     processor specific codes
 * 104  -  SH      processor specific codes
 * 106  -  x86     processor specific codes
 * 107  -  SPARC   processor specific codes
 */

#if !defined(_ERRNO_ADAPTED_H)
#define _ERRNO_ADAPTED_H

/*
 * NOTE: Error numbers 0x0 through 0x1F in Group 0 (zero)
 *       are reserved by Microware.
 */

#define E_SIGABRT   0x20    /* abort signal                         */
#define E_SIGFPE    0x21    /* erroneous math operation signal      */
#define E_SIGILL    0x22    /* illegal function image signal        */
#define E_SIGSEGV   0x23    /* segment violation (bus error) signal */
#define E_SIGTERM   0x24    /* termination request signal           */
#define E_SIGALRM   0x25    /* alarm time elapsed signal            */
#define E_SIGPIPE   0x26    /* write to pipe with no readers signal */
#define E_SIGUSR1   0x27    /* user signal #1                       */
#define E_SIGUSR2   0x28    /* user signal #2                       */

#define E_SIGADDR   0x29    /* address error signal                 */
#define E_SIGCHK    0x2a    /* chk instruction signal               */
#define E_SIGTRAPV  0x2b    /* trapv instruction signal             */
#define E_SIGPRIV   0x2c    /* priveledge violation signal          */
#define E_SIGTRACE  0x2d    /* trace exception signal               */
#define E_SIG1010   0x2e    /* 1010 line-A exception signal         */
#define E_SIG1111   0x2f    /* 1111 line-F exception signal         */

#define E_ILLFNC    0x40    /* illegal function code                */
#define E_FMTERR    0x41    /* format error                         */
#define E_NOTNUM    0x42    /* number not found/not a number        */
#define E_ILLARG    0x43    /* illegal argument                     */


#define E_BUSERR    0x66    /* bus error TRAP 2 occurred            */
#define E_ADRERR    0x67    /* address error TRAP 3 occurred        */
#define E_ILLINS    0x68    /* illegal instruction TRAP 4 occurred  */
#define E_ZERDIV    0x69    /* zero divide TRAP 5 occurred          */
#define E_CHK       0x6a    /* CHK instruction TRAP 6 occurred      */
#define E_TRAPV     0x6b    /* TrapV instruction TRAP 7 occurred    */
#define E_VIOLAT    0x6c    /* privelage violation TRAP 8 occurred  */
#define E_TRACE     0x6d    /* Uninitialized Trace TRAP 9 occurred  */
#define E_1010      0x6e    /* Uninitialized 1010 TRAP 10 occurred  */
#define E_1111      0x6f    /* Uninitialized 1111 TRAP 11 occurred  */
#define E_RESRVD    0x70    /* (Motorola reserved)                  */
#define E_CPROTO    0x71    /* Coprocessor protocol error           */
#define E_STKFMT    0x72    /* Stack frame format error             */
#define E_UNIRQ     0x73    /* Uninitialized interrupt              */
#define E_TRAP      0x85    /* user traps 1-15 E_TRAP+n             */
#define E_FPUNORDC  0x94    /* FP unordered condition error         */
#define E_FPINXACT  0x95    /* FP inexact result                    */
#define E_FPDIVZER  0x96    /* FP zero divisor                      */
#define E_FPUNDRFL  0x97    /* FP underflow                         */
#define E_FPOPRERR  0x98    /* FP operand error                     */
#define E_FPOVERFL  0x99    /* FP overflow                          */
#define E_FPNOTNUM  0x9a    /* FP not a number                      */
#define E_FPUNDATA  0x9b    /* FP Unsupported data type             */
#define E_MMUCONF   0x9c    /* MMU configuration error              */
#define E_MMUILLEG  0x9d    /* MMU illegal operation                */
#define E_MMUACCES  0x9e    /* MMU access level violation           */

#define E_PERMIT    0xa4    /* You must be super user to do that    */
#define E_DIFFER    0xa5    /* Arguments to F$ChkNam are different  */
#define E_STKOVF    0xa6    /* Stack overflow (ChkNam)              */
#define E_EVNTID    0xa7    /* Invalid or Illegal event ID number   */
#define E_EVNF      0xa8    /* Event (name) not found               */
#define E_EVBUSY    0xa9    /* The event is busy (can't be deleted) */
#define E_EVPARM    0xaa    /* Impossible Event parameters supplied */
#define E_DAMAGE    0xab    /* System data structures are damaged   */
#define E_BADREV    0xac    /* Incompatible revision                */
#define E_PTHLOST   0xad    /* Path Lost (net node was down)        */
#define E_BADPART   0xae    /* Bad partition data or no active      */
                            /* partition                            */
#define E_HARDWARE  0xaf    /* Hardware is damaged                  */

#define E_SECTSIZE  0xb0    /* Invalid sector size                  */
#define E_BSIG      0xb1    /* process recieved signal fatal signal or no intercept */

#define E_PTHFUL    0xc8    /* Path Table full                      */
#define E_BPNUM     0xc9    /* Bad Path Number                      */
#define E_POLL      0xca    /* Polling Table Full                   */
#define E_BMODE     0xcb    /* Bad Mode                             */
#define E_DEVOVF    0xcc    /* Device Table Overflow                */
#define E_BMID      0xcd    /* Bad Module ID                        */
#define E_DIRFUL    0xce    /* Module Directory Full                */
#define E_MEMFUL    0xcf    /* Process Memory Full                  */
#define E_UNKSVC    0xd0    /* Unknown Service Code                 */
#define E_MODBSY    0xd1    /* Module Busy                          */
#define E_BPADDR    0xd2    /* Bad Page Address                     */
#define E_EOF       0xd3    /* End of File                          */
#define E_VCTBSY    0xd4    /* IRQ Vector is already used           */
#define E_NES       0xd5    /* Non-Existing Segment                 */
#define E_FNA       0xd6    /* File Not Accesible                   */
#define E_BPNAM     0xd7    /* Bad Path Name                        */
#define E_PNNF      0xd8    /* Path Name Not Found                  */
#define E_SLF       0xd9    /* Segment List Full                    */
#define E_CEF       0xda    /* Creating Existing File               */
#define E_IBA       0xdb    /* Illegal Block Address                */
#define E_HANGUP    0xdc    /* Phone hangup occurred (modem)        */
#define E_MNF       0xdd    /* Module Not Found                     */
#define E_NOCLK     0xde    /* System has no clock running          */
#define E_DELSP     0xdf    /* Deleting Stack Pointer memory        */
#define E_IPRCID    0xe0    /* Illegal Process ID                   */
#define E_PARAM     0xe1    /* Impossible polling parameter specified*/
#define E_NOCHLD    0xe2    /* No Children                          */
#define E_ITRAP     0xe3    /* Invalid or unavailable Trap code     */
#define E_PRCABT    0xe4    /* Process Aborted                      */
#define E_PRCFUL    0xe5    /* Process Table Full                   */
#define E_IFORKP    0xe6    /* Illegal Fork Parameter               */
#define E_KWNMOD    0xe7    /* Known Module                         */
#define E_BMCRC     0xe8    /* Bad Module CRC                       */

#define E_USIGP     0xe9    /* Unprocessed Signal Pending (obsolescent)*/

#define E_SIGNAL    0xe9    /* Signal Error (replaces E_USIGP)      */
#define E_NEMOD     0xea    /* Not Executable Module                */
#define E_BNAM      0xeb    /* Bad Name                             */
#define E_BMHP      0xec    /* bad module header parity             */
#define E_NORAM     0xed    /* No Ram Available                     */
#define E_DNE       0xee    /* Directory not empty                  */
#define E_NOTASK    0xef    /* No available Task number             */
#define E_UNIT      0xf0    /* Illegal Unit (drive)                 */
#define E_SECT      0xf1    /* Bad SECTor number                    */
#define E_WP        0xf2    /* Write Protect                        */
#define E_CRC       0xf3    /* Bad Check Sum                        */
#define E_READ      0xf4    /* Read Error                           */
#define E_WRITE     0xf5    /* Write Error                          */
#define E_NOTRDY    0xf6    /* Device Not Ready                     */
#define E_SEEK      0xf7    /* Seek Error                           */
#define E_FULL      0xf8    /* Media Full                           */
#define E_BTYP      0xf9    /* Bad Type (incompatible) media        */
#define E_DEVBSY    0xfa    /* Device Busy                          */
#define E_DIDC      0xfb    /* Disk ID Change                       */
#define E_LOCK      0xfc    /* Record is busy (locked out)          */
#define E_SHARE     0xfd    /* Non-sharable file busy               */
#define E_DEADLK    0xfe    /* I/O Deadlock error                   */
#define E_FORMAT    0xff    /* Device is format protected           */

#endif
