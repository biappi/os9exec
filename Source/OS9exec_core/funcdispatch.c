// 
// Ê ÊOS9exec,   OS-9 emulator for Mac OS, Windows and Linux 
// Ê ÊCopyright (C) 2002 Ê[ Lukas Zeller / Beat Forster ]
//	  Available under http://www.synthesis.ch/os9exec
// 
// Ê ÊThis program is free software; you can redistribute it and/or 
// Ê Êmodify it under the terms of the GNU General Public License as 
// Ê Êpublished by the Free Software Foundation; either version 2 of 
// Ê Êthe License, or (at your option) any later version. 
// 
// Ê ÊThis program is distributed in the hope that it will be useful, 
// Ê Êbut WITHOUT ANY WARRANTY; without even the implied warranty of 
// Ê ÊMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// Ê ÊSee the GNU General Public License for more details. 
// 
// Ê ÊYou should have received a copy of the GNU General Public License 
// Ê Êalong with this program; if not, write to the Free Software 
// Ê ÊFoundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
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


/* OS9 Service Request Dispatch table */
/* ================================== */

#include "os9exec_incl.h"

#ifdef linux
  #include <time.h>
#endif


/* externally used */
ulong   rw__idleticks;            /* set by read and write of console */
ulong   slp_idleticks;            /* set by sleep in arbitration */

/* system call timing */
ulong   sum_rw__idleticks;        /* total number of ticks spent idle (console input) */
ulong   sum_slp_idleticks;        /* total number of ticks spent idle (arbitrate sleep) */
ulong   arbticks;                 /* total number of ticks spent in arbitration */
ulong   arbnum;                   /* number of arbitration calls */
ulong   last_complete;            /* time of last completion */
char    systime_prog[OS9NAMELEN]; /* one specific program will be logged */
Boolean systime_prog_ok;          /* chosen name has been called ? */

time_t  timerstart;               /* time in seconds when timing was started */
ulong   fcall_time[NUMFCALLS+1];  /* ticks spent in F$xxx calls */
ulong   icall_time[NUMICALLS+1];  /* ticks spent in I$xxx calls */
ulong   fcall_num [NUMFCALLS+1];  /* number of F$xxx calls */
ulong   icall_num [NUMICALLS+1];  /* number of I$xxx calls */
/* ----------------------------------------------------------------------------------- */




static os9err OS9_TCP_Select( regs_type *rp, ushort pid )
/* OS9TCP specific select */
/* currently not in use, because the only the ISP system is supported */
{
    #ifndef linux
    #pragma unused(pid)
    #endif
 
    upe_printf( "select\n" );
 
    rp->d[0]= 0;
    rp->d[1]= 0;
    rp->d[2]= 0;
 
    return 0;
} /* OS9_TCP_Select */



/* dispatch table */
const funcdispatch_entry startcall =
    { /* start   */ OS9_F_SDummy, "START",  0, 0 };

const funcdispatch_entry invalidcall =
    { /* invalid */ OS9_F_UnImp,  "---",    0, 0 };

const funcdispatch_entry nocall =
    { /* no call */ OS9_F_SDummy, "<none>", 0, 0 };

const funcdispatch_entry tcpselect =
    { /* $61     */ OS9_TCP_Select, "OS9_TCP_Select", 0, 0 }; /* OS9TCP specific call */



const funcdispatch_entry fcalltable[NUMFCALLS] =
{
    { /* 0x00 */ OS9_F_Link,   "F$Link",   d_w(0)+a_p(0),        d_w(0)+d_w(1)+a_l(0)+a_l(1)+a_l(2) }, /* Link to Module */
    { /* 0x01 */ OS9_F_Load,   "F$Load",   d_b(0)+a_p(0),        d_w(0)+d_w(1)+a_l(0)+a_l(1)+a_l(2) }, /* Load Module from File */
    { /* 0x02 */ OS9_F_UnLink, "F$UnLink", a_l(2),               0      }, /* Unlink Module */
    { /* 0x03 */ OS9_F_Fork,   "F$Fork",   d_w(0)+d_l(1)+d_l(2)+
                                           d_w(3)+d_w(4)+
                                           a_p(0)+a_l(1),        d_w(0)+a_l(0) }, /* Start New Process */
    { /* 0x04 */ OS9_F_Wait,   "F$Wait",   0,                    d_w(0)+d_w(1) }, /* Wait for Child Process to Die */
    { /* 0x05 */ OS9_F_Chain,  "F$Chain",  d_w(0)+d_l(1)+d_l(2)+
                                           d_w(3)+d_w(4)+
                                           a_p(0)+a_l(1),        0      }, /* Chain Process to New Module */
    { /* 0x06 */ OS9_F_Exit,   "F$Exit",   d_w(1),               0      }, /* Terminate Process */
    { /* 0x07 */ OS9_F_UnImp,  "F$Mem",    d_l(0),               d_l(0)+a_l(1) }, /* Set Memory Size */
    { /* 0x08 */ OS9_F_Send,   "F$Send",   d_w(0)+d_w(1),        0      }, /* Send Signal to Process */
    { /* 0x09 */ OS9_F_Icpt,   "F$Icpt",   a_l(0)+a_l(6),        0      }, /* Set Signal Intercept */
    { /* 0x0A */ OS9_F_Sleep,  "F$Sleep",  d_l(0),               d_l(0) }, /* Suspend Process */
    { /* 0x0B */ OS9_F_UnImp,  "F$SSpd",   d_w(0),               d_w(1) }, /* Suspend Process */
    { /* 0x0C */ OS9_F_ID,     "F$ID",     0,                    d_w(0)+d_l(1)+d_w(2) }, /* Return Process ID */
    { /* 0x0D */ OS9_F_SPrior, "F$SPrior", d_w(0)+d_w(1),        0      }, /* Set Process Priority */
    { /* 0x0E */ OS9_F_STrap,  "F$STrap",  a_l(0)+a_l(1),        0      }, /* Set Trap Intercept */
    { /* 0x0F */ OS9_F_PErr,   "F$PErr",   d_w(0)+d_w(1),        0      }, /* Print Error */
    { /* 0x10 */ OS9_F_PrsNam, "F$PrsNam", a_p(0),               d_b(0)+d_w(1)+a_l(0)+a_l(1) }, /* Parse Pathlist Name */
    { /* 0x11 */ OS9_F_CmpNam, "F$CmpNam", d_w(1)+a_p(0)+a_p(1), 0      }, /* Compare Two Name */
    { /* 0x12 */ OS9_F_UnImp,  "F$SchBit", d_w(0),               d_w(1) }, /* Search Bit Map */
    { /* 0x13 */ OS9_F_UnImp,  "F$AllBit", d_w(0),               d_w(1) }, /* Allocate in Bit Map */
    { /* 0x14 */ OS9_F_UnImp,  "F$DelBit", d_w(0),               d_w(1) }, /* Deallocate in Bit Map */
    { /* 0x15 */ OS9_F_Time,   "F$Time",   d_w(0),               d_l(0)+d_l(1)+d_w(2)+d_l(3) }, /* Get Current Time */
    { /* 0x16 */ OS9_F_STime,  "F$STime",  d_l(0)+d_l(1),        0      }, /* Set Current Time */
    { /* 0x17 */ OS9_F_CRC,    "F$CRC",    d_l(0)+d_l(1)+a_l(0), d_l(1) }, /* Generate CRC */
    { /* 0x18 */ OS9_F_GPrDsc, "F$GPrDsc", d_w(0)+d_w(1)+a_l(0), 0      }, /* get Process Descriptor copy */
    { /* 0x19 */ OS9_F_GBlkMp, "F$GBlkMp", d_w(0)+d_w(1)+a_l(0), d_l(0)+d_l(1)+d_l(2)+d_l(3)+a_l(0) }, /* get System Block Map copy */
    { /* 0x1A */ OS9_F_GModDr, "F$GModDr", d_l(1)+a_l(0),        d_l(1) }, /* get Module Directory copy */
    { /* 0x1B */ OS9_F_CpyMem, "F$CpyMem", d_w(0)+d_l(1)+a_l(0)+
                                           a_l(1),               0      }, /* Copy External Memory */
    { /* 0x1C */ OS9_F_SUser,  "F$SUser",  d_l(1),               0      }, /* Set User ID number */
    { /* 0x1D */ OS9_F_UnLoad, "F$UnLoad", d_w(0)+a_p(0),        a_l(0) }, /* Unlink Module by name */
    { /* 0x1E */ OS9_F_RTE,    "F$RTE",    0,                    0      }, /* Return from Intercept routine */
    { /* 0x1F */ OS9_F_GPrDBT, "F$GPrDBT", d_w(0),               d_w(1) }, /* Get system global data copy */
    { /* 0x20 */ OS9_F_Julian, "F$Julian", d_l(0)+d_l(1),        d_l(0)+d_l(1) }, /* Convert gregorian to Julian date */
    { /* 0x21 */ OS9_F_TLink,  "F$TLink",  d_w(0)+d_l(1)+a_p(0), a_l(0)+a_l(1)+a_l(2) }, /* Link trap subroutine package */
    { /* 0x22 */ OS9_F_UnImp,  "F$DFork",  d_w(0),               d_w(1) }, /* Debugging Fork call */
    { /* 0x23 */ OS9_F_UnImp,  "F$DExec",  d_w(0),               d_w(1) }, /* Debugging execution call (single step) */
    { /* 0x24 */ OS9_F_UnImp,  "F$DExit",  d_w(0),               d_w(1) }, /* Debugging exit call (kill child) */
    { /* 0x25 */ OS9_F_DatMod, "F$DatMod", d_l(0)+d_w(1)+d_w(2)+
                                           d_w(3)+a_p(0),        d_w(0)+d_w(1)+a_l(0)+a_l(1)+a_l(2) }, /* Create data module */
    { /* 0x26 */ OS9_F_SetCRC, "F$SetCRC", a_l(0),               0      }, /* Generate valid header and CRC in module */
    { /* 0x27 */ OS9_F_SetSys, "F$SetSys", d_w(0)+d_l(1)+d_l(2), d_l(2) }, /* Set/examine system global variable */
    { /* 0x28 */ OS9_F_SRqMem, "F$SRqMem", d_l(0),               d_l(0)+a_l(2) }, /* System Memory Request */
    { /* 0x29 */ OS9_F_SRtMem, "F$SRtMem", d_l(0)+a_l(2),        0      }, /* System Memory Return */
    { /* 0x2A */ OS9_F_UnImp,  "F$IRQ",    d_w(0),               d_w(1) }, /* Enter IRQ Polling Table */
    { /* 0x2B */ OS9_F_UnImp,  "F$IOQu",   d_w(0),               d_w(1) }, /* Enter I/O Queue */
    { /* 0x2C */ OS9_F_UnImp,  "F$AProc",  d_w(0),               d_w(1) }, /* Enter Active Process Queue */
    { /* 0x2D */ OS9_F_UnImp,  "F$NProc",  d_w(0),               d_w(1) }, /* Start Next Process */
    { /* 0x2E */ OS9_F_UnImp,  "F$VModul", d_w(0),               d_w(1) }, /* Validate Module */
    { /* 0x2F */ OS9_F_UnImp,  "F$FindPD", d_w(0),               d_w(1) }, /* Find Process/Path Descriptor */
    { /* 0x30 */ OS9_F_UnImp,  "F$AllPD",  d_w(0),               d_w(1) }, /* Allocate Process/Path Descriptor */
    { /* 0x31 */ OS9_F_UnImp,  "F$RetPD",  d_w(0),               d_w(1) }, /* Return Process/Path Descriptor */
    { /* 0x32 */ OS9_F_SSvc,   "F$SSvc",   a_l(1)+a_l(3),        0      }, /* Service Request Table Initialization */
    { /* 0x33 */ OS9_F_UnImp,  "F$IODel",  d_w(0),               d_w(1) }, /* Delete I/O Module */
    { /* 0x34 */ OS9_F_UnImp,  "F$???_34", d_w(0),               d_w(1) }, /* not existing */
    { /* 0x35 */ OS9_F_UnImp,  "F$???_35", d_w(0),               d_w(1) }, /* not existing */
    { /* 0x36 */ OS9_F_UnImp,  "F$???_36", d_w(0),               d_w(1) }, /* not existing */
    { /* 0x37 */ OS9_F_UnImp,  "F$GProcP", d_w(0),               d_w(1) }, /* Get Process ptr */
    { /* 0x38 */ OS9_F_UnImp,  "F$Move",   d_w(0),               d_w(1) }, /* Move Data */
    { /* 0x39 */ OS9_F_UnImp,  "F$AllRAM", d_w(0),               d_w(1) }, /* Allocate RAM blocks */
    { /* 0x3A */ OS9_F_Permit, "F$Permit", d_l(0)+d_b(1)+a_l(2), d_w(1) }, /* Protect access to RAM block */
    { /* 0x3B */ OS9_F_UnImp,  "F$Protect",d_w(0),               d_w(1) }, /* Protect access of RAM block */
    { /* 0x3C */ OS9_F_UnImp,  "F$SetImg", d_w(0),               d_w(1) }, /* Set Process DAT Image */
    { /* 0x3D */ OS9_F_UnImp,  "F$FreeLB", d_w(0),               d_w(1) }, /* Get Free Low Block */
    { /* 0x3E */ OS9_F_UnImp,  "F$FreeHB", d_w(0),               d_w(1) }, /* Get Free High Block */
    { /* 0x3F */ OS9_F_UnImp,  "F$AllTsk", d_w(0),               d_w(1) }, /* Allocate Process Task number */
    { /* 0x40 */ OS9_F_UnImp,  "F$DelTsk", d_w(0),               d_w(1) }, /* Deallocate Process Task number */
    { /* 0x41 */ OS9_F_UnImp,  "F$SetTsk", d_w(0),               d_w(1) }, /* Set Process Task DAT registers */
    { /* 0x42 */ OS9_F_UnImp,  "F$ResTsk", d_w(0),               d_w(1) }, /* Reserve Task number */
    { /* 0x43 */ OS9_F_UnImp,  "F$RelTsk", d_w(0),               d_w(1) }, /* Release Task number */
    { /* 0x44 */ OS9_F_UnImp,  "F$DATLog", d_w(0),               d_w(1) }, /* Convert DAT Block/Offset to Logical */
    { /* 0x45 */ OS9_F_UnImp,  "F$DATTmp", d_w(0),               d_w(1) }, /* Make temporary DAT image */
    { /* 0x46 */ OS9_F_UnImp,  "F$LDAXY",  d_w(0),               d_w(1) }, /* Load A [X,[Y]] */
    { /* 0x47 */ OS9_F_UnImp,  "F$LDAXYP", d_w(0),               d_w(1) }, /* Load A [X+,[Y]] */
    { /* 0x48 */ OS9_F_UnImp,  "F$LDDDXY", d_w(0),               d_w(1) }, /* Load D [D+X,[Y]] */
    { /* 0x49 */ OS9_F_UnImp,  "F$LDABX",  d_w(0),               d_w(1) }, /* Load A from 0,X in task B */
    { /* 0x4A */ OS9_F_UnImp,  "F$STABX",  d_w(0),               d_w(1) }, /* Store A at 0,X in task B */
    { /* 0x4B */ OS9_F_UnImp,  "F$AllPrc", d_w(0),               d_w(1) }, /* Allocate Process Descriptor */
    { /* 0x4C */ OS9_F_UnImp,  "F$DelPrc", d_w(0),               d_w(1) }, /* Deallocate Process Descriptor */
    { /* 0x4D */ OS9_F_UnImp,  "F$ELink",  d_w(0),               d_w(1) }, /* Link using Module Directory Entry */
    { /* 0x4E */ OS9_F_UnImp,  "F$FModul", d_w(0),               d_w(1) }, /* Find Module Directory Entry */
    { /* 0x4F */ OS9_F_UnImp,  "F$MapBlk", d_w(0),               d_w(1) }, /* Map Specific Block */
    { /* 0x50 */ OS9_F_UnImp,  "F$ClrBlk", d_w(0),               d_w(1) }, /* Clear Specific Block */
    { /* 0x51 */ OS9_F_UnImp,  "F$DelRAM", d_w(0),               d_w(1) }, /* Deallocate RAM blocks */
    { /* 0x52 */ OS9_F_SysDbg, "F$SysDbg", 0,                    0      }, /* Invoke system level debugger */
    { /* 0x53 */ OS9_F_Event,  "F$Event",  SEVENT_CALL+d_l(0)+
                                           d_w(1)+d_w(2)+
                                           d_w(3)+a_l(0),        d_l(0)+a_l(0) }, /* Create/Link to named event */
    { /* 0x54 */ OS9_F_Gregor, "F$Gregor", d_l(0)+d_l(1),        d_l(0)+d_l(1) }, /* Convert julian date to gregorian date */
    { /* 0x55 */ OS9_F_UnImp,  "F$SysID",  d_w(0),               d_w(1) }, /* return system identification */
    { /* 0x56 */ OS9_F_Alarm,  "F$Alarm",  d_l(0)+d_w(1)+
                                           d_l(2)+d_l(3)+d_l(4), d_l(0) }, /* send alarm signal */
    { /* 0x57 */ OS9_F_Sigmask,"F$Sigmask",d_l(0)+d_l(1),        0      }, /* set signal mask */
    { /* 0x58 */ OS9_F_UnImp,  "F$ChkMem", d_w(0),               d_w(1) }, /* determine if user process may access memory area */
    { /* 0x59 */ OS9_F_UnImp,  "F$UAcct",  d_w(0),               d_w(1) }, /* inform user accounting of process status */
    { /* 0x5A */ OS9_F_SDummy, "F$CCtl",   d_l(0),               0      }, /* cache control */
    { /* 0x5B */ OS9_F_SUnImp, "F$GSPUMp", d_w(0),               d_w(1) }, /* get SPU map information for a process */
    { /* 0x5C */ OS9_F_SRqMem, "F$SRqCMem",d_l(0)+d_l(1),        d_l(0)+a_l(2) }, /* System Colored Memory Request */
    { /* 0x5D */ OS9_F_UnImp,  "F$POSK",   d_w(0),               d_w(1) }, /* execute svc request */
    { /* 0x5E */ OS9_F_Panic,  "F$Panic",  d_w(0),               d_w(1) }, /* Panic warning */
    { /* 0x5F */ OS9_F_UnImp,  "F$MBuf",   d_w(0),               d_w(1) }, /* Memory buffer manager */
    { /* 0x60 */ OS9_F_UnImp,  "F$Trans",  d_w(0),               d_w(1) }  /* Translate memory address to/from external bus */
};

const funcdispatch_entry icalltable[NUMICALLS] = {
    { /* 0x80 */ OS9_I_Attach, "I$Attach", d_b(0)+a_p(0),        a_l(2) }, /* Attach I/O Device */
    { /* 0x81 */ OS9_I_Detach, "I$Detach", a_l(2),               0      }, /* Detach I/O Device */
    { /* 0x82 */ OS9_I_Dup,    "I$Dup",    d_w(0),               d_w(0) }, /* Duplicate Path */
    { /* 0x83 */ OS9_I_Create, "I$Create", d_b(0)+d_w(1)+d_l(2)+
                                           a_p(0),               d_w(0)+a_l(0) }, /* Create New File */
    { /* 0x84 */ OS9_I_Open,   "I$Open",   d_b(0)+a_p(0),        d_w(0)+a_l(0) }, /* Open Existing File */
    { /* 0x85 */ OS9_I_MakDir, "I$MakDir", d_b(0)+d_w(1)+d_l(2)+
                                           a_p(0),               a_l(0) }, /* Make Directory File */
    { /* 0x86 */ OS9_I_ChgDir, "I$ChgDir", d_b(0)+a_p(0),        a_l(0) }, /* Change Default Directory */
    { /* 0x87 */ OS9_I_Delete, "I$Delete", d_b(0)+a_p(0),        a_l(0) }, /* Delete File */
    { /* 0x88 */ OS9_I_Seek,   "I$Seek",   d_w(0)+d_l(1),        0      }, /* Change Current Position */
    { /* 0x89 */ OS9_I_Read,   "I$Read",   d_w(0)+d_l(1)+a_l(0), d_l(1) }, /* Read Data */
    { /* 0x8A */ OS9_I_Write,  "I$Write",  d_w(0)+d_l(1)+a_l(0), d_l(1) }, /* Write Data */
    { /* 0x8B */ OS9_I_ReadLn, "I$ReadLn", d_w(0)+d_l(1)+a_l(0), d_l(1) }, /* Read Line of ASCII Data */
    { /* 0x8C */ OS9_I_WritLn, "I$WritLn", d_w(0)+d_l(1)+a_l(0), d_l(1) }, /* Write Line of ASCII Data */

    { /* 0x8D */ OS9_I_GetStt, "I$GetStt",SFUNC_STATCALL+d_w(0)+
                                           d_w(1)+d_l(2)+d_l(3)+
                                           a_l(0),               d_l(0)+d_l(1)+d_l(2)+d_l(3)+a_l(0) }, /* Get Path Status */
    { /* 0x8E */ OS9_I_SetStt, "I$SetStt",SFUNC_STATCALL+d_w(0)+
                                           d_w(1)+d_l(2)+d_l(3)+
                                           a_l(0),               d_l(0)+d_l(1)+d_l(2)+d_l(3)+a_l(0) }, /* Set Path Status */
                                           
    { /* 0x8F */ OS9_I_Close,  "I$Close",  d_w(0),               0      }, /* Close Path */
    { /* 0x90 */ OS9_F_UnImp,  "I$???_90", d_w(0),               d_w(1) }, /* not implemented */
    { /* 0x91 */ OS9_F_UnImp,  "I$???_91", d_w(0),               d_w(1) }, /* not implemented */

    { /* 0x92 */ OS9_I_SGetSt, "I$SGetSt",SFUNC_STATCALL+d_w(0)+
                                           d_w(1)+d_l(2)+d_l(3)+
                                           a_l(0),               d_l(0)+d_l(1)+d_l(2)+d_l(3)+a_l(0) }  /* Getstat using system path number */
};


/* quickly get correct entry for function call */
const funcdispatch_entry* getfuncentry( ushort func )
{
    if         (func==STARTCALL)             
                                             return &startcall;
    if         (func             <NUMFCALLS) return &fcalltable[func];
    else { 
        if     (func>=ICALLS_Offs) {
            if (func -ICALLS_Offs<NUMICALLS) return &icalltable[func-ICALLS_Offs];
            else                             return &invalidcall; /* any other: invalid call */
        }
        else {
            if (func==0x7F) return &nocall;            /* consider this as "no call" */
            if (func==0x61) return &tcpselect; /* this is used as selector in OS9TCP */
                            /* %%% in real OS-9 this will be installed with a F$SSvc */

            upe_printf( "function out of range: %x\n", func );
            return &invalidcall; /* any other: invalid call */
        }
    }
} /* getfuncentry */



static ulong DiffTick( void )
/* get the difference since last ticks */
{
    ulong          t= GetSystemTick(); /* time spent since last call except idle part */
    ulong       a= t - last_complete - rw__idleticks - slp_idleticks; 
    last_complete= t;
    
    procs[1]._iticks += rw__idleticks + slp_idleticks;
    sum_rw__idleticks+= rw__idleticks;  rw__idleticks= 0;
    sum_slp_idleticks+= slp_idleticks;  slp_idleticks= 0;
    return a;
} /* DiffTick */



void os9_to_xxx( ushort pid, const char* name )
/* Get systime ticks on the way from OS-9 to XXX command */
{
    char      *mn, *p, *q, *systime_end;
    int       mid, ii, jj, kk;
    mod_exec* mod;
    st_typ    *s, *sj, *sj1;
    Boolean   eli= false; /* end of list */
    ulong     a= DiffTick();
    
    /* try to measure ticks */
    
    procs[pid]._uticks+= a; /* info for F$GPrDsc */

        mid= procs[pid].mid;    
        mod= os9mod   ( mid );
    if (mid==0 || mod==NULL) mn= (char*) name;
    else                     mn= Mod_Name( mod );

    /* question: must it be logged ? */
    systime_prog_ok= (*systime_prog==NUL)
          || (ustrcmp( systime_prog,mn )==0);

    /* search for "xxx,yyy" */
    if (!systime_prog_ok) {
        p= systime_prog;
        while (true) {
            p= strstr( p,mn ); if (p==NULL) break;
            
            systime_end= systime_prog+strlen(systime_prog)-1;
            q          = p           +strlen(mn)          -1;
                              
            if ((p==systime_prog || *(p-1)==',') &&
                (q==systime_end  || *(q+1)==',')) { systime_prog_ok= true; break; }
        } /* loop */
    } /* if */

    if (!systime_prog_ok) { mn= "      (other progs)"; eli= true; }
    
    /* search for the module's name in the list, if not there add name to list */
    for (ii=0; ii<MAX_OS9PROGS; ii++) {
                               s= &statistics[ii];
        if                  ( *s->name==NUL            /* not yet in the list */
         || (!eli && ustrncmp( s->name," ", 1 )==0)
         || (!eli && ustrcmp ( s->name,mn     ) >0)) { /* sort them alphabetically */

            kk= ii; /* search for the last item */
            while (kk<MAX_OS9PROGS && *statistics[kk].name!=NUL) kk++;

            for (jj= kk; jj>ii;  jj--) { /* shift up */
                sj = &statistics[jj  ];
                sj1= &statistics[jj-1];
                strcpy( sj->name,   sj1->name );
                        sj->intern= sj1->intern;
                        sj->ticks = sj1->ticks;
                        sj->num   = sj1->num;
            } /* inner for */
        
            strcpy( s->name,mn );
            s->intern= !eli && mid==0;
            s->ticks = 0;
            s->num   = 0;
        }
        
        if (ustrcmp( mn,s->name )==0) {
                        s->ticks+= a;
                        s->num  ++; break;
        }
    } /* outer for */
} /* os9_to_xxx */



void xxx_to_arb( ushort func, ushort pid )
/* Get systime ticks on the way from XXX command to arbitration */
{
    /* calc and save time spent */
    ulong f;
    ulong a= DiffTick();
            
    /* func was within range, register */
    if (func<=NUMFCALLS) {
        if (systime_prog_ok) f= func;
        else                 f= NUMFCALLS;
        
        fcall_time[f]     += a;
        fcall_num [f]     ++;
        procs[pid]._fticks+= a; /* info for F$GPrDsc */
    }
    else {
        if (systime_prog_ok) f= func-0x80;
        else                 f= NUMICALLS;

        icall_time[f]     += a;
        icall_num [f]     ++;
        procs[pid]._iticks+= a; /* info for F$GPrDsc */
    }               
} /* xxx_to_arb */



void arb_to_os9( Boolean last_arbitrate )
/* Get systime ticks on the way from arbitration to OS-9 */
{
    ulong a= DiffTick();  /* must be done after <b> calculation */
    
    arbticks        += a;
    procs[1]._fticks+= a; /* info for F$GPrDsc */
    
    if (last_arbitrate) arbnum++;
} /* arb_to_os9 */




/* perform system call */
os9err exec_syscall( ushort func, ushort pid, regs_type *rp )
{
    os9err  err;
    const   funcdispatch_entry* fdeP= getfuncentry(func);
    char*   fName= fdeP->name; /* allows much easier debugging, because function name is visible */
    char*   fSS  = "";

    if (logtiming) {
        os9_to_xxx( pid, "" );
        
        if (fdeP->inregs & SFUNC_STATCALL) /* get the getstat/setstat code as name for debugging */
            fSS= get_stat_name(loword(rp->d[1]));

        debugprintf(dbgSysCall,dbgDetail,("# %s %s\n", fName,fSS )); /* fName+fSS would be eliminated by compiler */
    } /* if (logtiming) */


    /* execute syscall */
    err= (fdeP->func)(rp,pid);


    if (logtiming) {
        xxx_to_arb( func,pid );
    } /* if (logtiming) */


    if (fdeP!=&invalidcall) {
        if     (func<NUMFCALLS) procs[pid]._fcalls++; /* "procs -a" uses it */
        else  { func-=0x80;
            if (func<NUMICALLS) procs[pid]._icalls++; 
        }
    }
    
    return err;
} /* exec_syscall */



void init_L2(void)
/* support for the /L2 led blinking */
{
	l2.hw_location= 0;
	
	l2.col1  = L2_Off;
	l2.ratio1= L2_InitRatio;
	l2.col2  = L2_Off;
	l2.ratio2= L2_InitRatio;
			
	l2.lcount= 0;
	l2.phase = 1; /* blink phases: either 1 or 2 */
} /* init_L2 */


static void update_L2( ulong t )
{
    byte ll;
    ulong diff;
    
    /* make the /L2 interrupt handling */
    	diff= t-lastTick; lastTick= t;
    if (diff==0 || l2.lcount>diff)
    		       l2.lcount-= diff;
    else {
    	if (l2.phase==1) {
    		l2.phase      = 2;
    		l2.lcount     = l2.ratio2;
    		ll            = l2.col2;
    	}
    	else {
    		l2.phase      = 1;
    		l2.lcount     = l2.ratio1;
     		ll            = l2.col1;
    	}
    		
    	if (ll>L2_Off) ll= L2_Off;
    	l2.hw_location= os9_word(ll); 
    } /* if */
} /* update_L2 */



/* get system tick count and subtract the start tick */
ulong GetSystemTick(void)
{
    ulong t;
    
    #ifdef macintosh
      t=    TickCount();

    #elif defined(windows32)
      t= GetTickCount()/10; /* take 100 instead of 1000 on PC */

    #elif defined linux
      struct timeval  tv;
      struct timezone tz;
      
      gettimeofday( &tv, &tz );
      if (sec0==0) sec0= tv.tv_sec;
      t=         tv.tv_sec - sec0;
      t= 100*t + tv.tv_usec/10000;

    #else
      #error Not yet implemented GetSystemTick()
    #endif

    
    /* make the /L2 interrupt handling */
    update_L2( t );
    
    return t-startTick;
} /* GetSystemTick */



/* init syscall timing */
void init_syscalltimers(void)
{
    ushort k;

    time( &timerstart );
    logtiming        = true;
    arbticks         = 0;
    arbnum           = 0;
    last_complete    = GetSystemTick();
    
        rw__idleticks= 0;   /* must be initialized too */
    sum_rw__idleticks= 0;
        slp_idleticks= 0;
    sum_slp_idleticks= 0;
    strcpy( systime_prog,"" );    /* invalidate it */
            systime_prog_ok= true;

    for (k=0; k<=NUMFCALLS;   k++) { fcall_time[k]= 0;    fcall_num[k]= 0; }
    for (k=0; k<=NUMICALLS;   k++) { icall_time[k]= 0;    icall_num[k]= 0; }

    for (k=0; k<MAXPROCESSES; k++) { procs[k]._uticks= 0;
                                     procs[k]._fticks= 0; procs[k]._fcalls= 0; 
                                     procs[k]._iticks= 0; procs[k]._icalls= 0; }

    for (k=0; k<MAX_OS9PROGS; k++) { strcpy( statistics[k].name,"" );
                                             statistics[k].ticks= 0;
                                             statistics[k].num  = 0; }
} /* init_syscalltimers */



/* time display */
static char *time_disp(ulong t)
{
    static char tbuf[13];
    ulong ms, secs, mins;
    
    
    /* calc milliseconds */
    ms  =(ulong)((double)t/(double)TICKS_PER_SEC*1000.0);   
    secs= ms/1000;
    mins= secs/60;
    
    if (ms==0) sprintf( tbuf, "" );
    else {
        if (mins==0) sprintf( tbuf,      "%2d.%03d\"",       secs % 60, ms % 1000 );
        else         sprintf( tbuf, "%0d'%02d.%03d\"", mins, secs % 60, ms % 1000 );
    }

    return tbuf;
} /* time_disp */



static void show_title( Boolean show, char* name )
{
    char* n= name; 
    char* u= "-------------------";
    
    if (show) {
        if (*n==NUL) n= "Name";
        
        upho_printf("\n");
        upho_printf(" %-19s       ticks  # of calls       %%        time \n", n );
        upho_printf(" %-19s  ----------  ----------  ------  -----------\n", u );
    }
} /* show_title */



static void show_line( Boolean show, ushort mode, 
                       char c, char* name, ulong t, int n, ulong active )
{
    #define Lim  0.051
    #define GLim 1
    char    nnnn[20], perc[20];
    float   f;
    
    if (!show || (mode!=0 && n==0)) return;

    if (active==0) f= 0; /* no division by zero */
    else           f= (float)100*t/active;

    if ((mode & STIM_TICKAVAIL) &&   t==0) return;
    if ((mode & STIM_PERCENT  ) && f<GLim) return;
    
    if (n==-1) strcpy ( nnnn,"" );
    else       sprintf( nnnn,"%d", n );
    if (f<Lim) strcpy ( perc,"-" );
    else       sprintf( perc,"%c%1.1f%%", ustrcmp( "TOTAL idle",name )==0 ? '+':' ', f );
        
    upo_printf("  %c%-19s  %10ld  %10s %7s %12s\n", c,name, t,nnnn, perc, time_disp(t) );
} /* show_line */


static void show_sline( char* name,  ulong t, int n, ulong active )
{           show_line ( true,0, ' ', name, t,n, active );
} /* show_sline */



/* show system call timers */
static void show_syscalltimers( Boolean icalls, ushort mode, ulong active, 
                                                ulong *call, ulong *num )
{
    ushort   k;
    int      t, n;
    char*    name;
    Boolean  show=    icalls ? mode & STIM_ICALLS : mode & STIM_FCALLS;
    ushort   max =    icalls ?          NUMICALLS :          NUMFCALLS;
    show_title( show, icalls ?            "I$XXX" :            "F$XXX" );

    *call= 0;
    *num = 0;

    for ( k=0; k<=max; k++ ) {
        t= icalls ? icall_time[k] : fcall_time[k];
        n= icalls ? icall_num [k] : fcall_num [k];
        
        if (k==max) name= icalls ? "I$xxx (other progs)":"F$xxx (other progs)";
        else        name= icalls ?  icalltable[k].name  : fcalltable[k].name;
        
        show_line( show,mode, ' ',name, t,n, active );
        if (n>0) { *call+= t; *num+= n; }
    } /* for */
} /* show_syscalltimers */



/* show system call timers */
static void show_os9timers( ushort mode, ulong active, ulong *call, ulong *num )
{
    ushort  k;
    st_typ* s;
    int     t, n;
    Boolean show= mode & STIM_OS9;
    
    show_title( show,"OS-9" );
    *call= 0;
    *num = 0;

    for ( k=0; k<MAX_OS9PROGS; k++ ) {
           s= &statistics[k];
        t= s->ticks;
        n= s->num;
        
        show_line( show,mode, s->intern ? '>':' ', s->name, t,n, active );
        if (n>0) { *call+= t; *num+= n; }
    } /* for */
} /* show_os9timers */



/* show timing summary */
ulong show_timing( ushort mode )
{
    ulong  tfcall, ticall, oscall;
    ulong  tfnum,  tinum,  osnum;
    ulong  t_call, active;
    ulong  realtime;
    time_t now;


    time(&now);
    upho_printf( "%s timing (in 1/%d sec)\n", OS9exec_Name(),TICKS_PER_SEC );
    if (!logtiming) {
        upo_printf("**** WARNING: timing log is currently DISABLED!\n");
    }

    show_syscalltimers( false, 0,1, &tfcall,&tfnum );
    show_syscalltimers( true,  0,1, &ticall,&tinum );
    show_os9timers    (        0,1, &oscall,&osnum );

    t_call= tfcall+ticall;
    active= t_call+oscall + arbticks + sum_rw__idleticks + sum_slp_idleticks;
    
    if (active==0) active=1; /* prevent division by zero */
    
    show_syscalltimers( false, mode, active, &tfcall,&tfnum );
    show_syscalltimers( true,  mode, active, &ticall,&tinum );
    show_os9timers    (        mode, active, &oscall,&osnum );
        
    if (!logtiming) timerstart= now+1; /* prevent uninitialized <timerstart> */
    if (now<timerstart) now= timerstart;
    
    realtime= (now-timerstart)*TICKS_PER_SEC;            /* avoid lost ticks */
    if (realtime<active) realtime= active;       /* avoid negative lost ticks */


    show_title( true,"Description" );
    show_sline( "TOTAL F$xxx",      tfcall,         tfnum, active );
    show_sline( "TOTAL I$xxx",      ticall,         tinum, active );
    show_sline( "TOTAL SysCall",    t_call,   tfnum+tinum, active );
    show_sline( "TOTAL OS-9",       oscall,         osnum, active );
    show_sline( "TOTAL arbitration",arbticks,      arbnum, active );
    show_sline( "TOTAL idle sleep", sum_slp_idleticks, -1, active );
    show_sline( "TOTAL idle (R/W)", sum_rw__idleticks, -1, active );
    show_sline( "TOTAL active",     active,         osnum, active );
    show_sline( "TOTAL realtime",   realtime,          -1,      0 );

    if (realtime>active) {
        show_sline( "lost  ticks",  realtime-active,   -1,      0 );
    }
    
    return ticall+tfcall;
} /* show_timing */



/* internal command for timing management */

static void usage( char *name,ushort pid )
{
    #ifndef linux
    #pragma unused(pid)
    #endif

    uphe_printf("Usage:    %s [options]\n",name);
    uphe_printf("Function: OS9exec emulator timing utility\n");
    uphe_printf("Options:  -r          switch on timing measurement & reset counters\n");
    uphe_printf("          -d          disable   timing measurement (slightly better performance)\n");
    uphe_printf("          -e          re-enable timing measurement (but no reset)\n");
    uphe_printf("          -n=[<prog>] display info of OS-9 <prog> only\n");
    uphe_printf("          -f          show F$xxx call timing list\n");
    uphe_printf("          -i          show I$xxx call timing list\n");
    uphe_printf("          -o          show OS-9 progs timing list\n");
    uphe_printf("          -s          show I$xxx + F$xxx calls + OS-9 progs (same as -i -f -o)\n");
    copyright ();
} /* usage */


os9err int_systime(ushort pid, int argc, char **argv)
{
    ushort mode= STIM_NONE; /* set default options */
    char   *p;              /* command line scanning */
    char   opt;
    ushort h;
    
    /* get arguments and options, multiple options allowed (bfo) */
    for ( h=1; h<argc; h++ ) {
        p= argv[h];    
        if (*p=='-') { 
            p++;
            
            /* multiple options allowed now */
            while (*p!=NUL) {
                        opt= p[0];
                switch (opt) { /* no longer case sensitive */
                    case '?': usage(argv[0],pid);                              break;
                    case 'r': init_syscalltimers();
     						  upho_printf("Activated and resetted timing measurement\n"); break;
                    case 'd': logtiming= false;
                              upho_printf("Disabled timing measurement\n");    break;
                    case 'e': logtiming=  true;
                              upho_printf("Re-enabled timing measurement\n");  break;
                               
                    case 'n': switch (p[1]) {
                                  case '=': strcpy( systime_prog,&p[2] );
                                             p= p+strlen(p)-1; /* skip rest */ break;
                                  default : strcpy( systime_prog,"" );         break;
                              }
                              break;
                              
                    case 'f': mode |= STIM_FCALLS;                             break;
                    case 'i': mode |= STIM_ICALLS;                             break;
                    case 'o': mode |= STIM_OS9;                                break;
                    case 's': mode  = STIM_ICALLS | STIM_FCALLS | STIM_OS9;    break;

                    case 't': mode |= STIM_TICKAVAIL; /* >=1 ticks   */        break;
                    case 'p': mode |= STIM_PERCENT;   /* >=1 percent */        break;
                    
                    default : uphe_printf("Error: unknown option '%c'!\n",*p); 
                              usage(argv[0],pid); return 1;
                }
                
                p++;
            }
        }
        else {
            uphe_printf("Error: no arguments allowed\n"); return 1;
        }
    }   
    if (mode!=STIM_NONE) show_timing(mode);
    return 0;
}
    
/* eof */