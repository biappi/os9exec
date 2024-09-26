* System Definitions for Motorola VME110 system opt -l********************************* Edition History*   date    comments                                            by* -------- ---------------------------------------------------- ---* 11-01-84 Returned I/O port addresses to systype file.         rfd*          Added configuration module strings.                  rfd* 11-05-84 Changed names of A_Term, Term_Par to match boot.a    wgp* 07-10-85 Changed serial macros for new Type byte standards	res* 10-24-86 Changed configuration parameters for use with V2.0   djl* 05-22-87 Added CPUTyp definition                              wwb* 06-01-87 Added CPUType, ClocFreq (sc8x30) definition          wwb*          <<<---- OS-9/68000 V2.1 Release ---->>>* 08-28-87 Changed for Atari ST                                 weo* 10-10-87 Changes DiskH0 and DiskH1 macro (removed devcon*          table                                                weo* 10-15-87 Added flag for IEC IRQ                               weo* 10-28-87 Added variables for MIDI IRQ (in OEM area)           weo* 02-09-88 Added variable for VTD (in OEM area)                 weo* 02-10-88 Added VTERM0 - VTERM7 macros                         weo* 06-06-88 Added flag for 2nd harddisk interrupt                weo* 10-18-88 Added variables for mc68681 status regs (in OEM *          area)                                                weo* 10-19-88 Added T3, T4, T5 and T6 macros                       weo * 09-12-88 Added DiskH2 and DiskH3 macros                       weo* 03-21-90 Cleaned up a lot for 2.3.                            weo********************************* Hardware type definitionsAtariST equ 520 default value: Atari ST 520 ComputerClocFreq equ 0 (not used)SerType  equ MFP68901 (not used)IOType   equ 1 value from "Odd" in oskdefs.dIRQ_used equ 1 define the labelCPUTyp set 68000 mpu definitionCPUType set AtariST define cpu module in useClkPort equ $00FB0000 clock port addressClkVect set $45 clock vector numberClkPrior set 1 high polling table priority******************************** Configuration module constants*    used only by init moduleCONFIG_D0 macroMainFram dc.b "Atari ST",0SysStart dc.b "sysgo",0 name of initial module to executeSysParam dc.b C$CR,0 parameter to SysStartSysDev   dc.b "/d0",0 initial system disk pathlistConsolNm dc.b "/term",0  console terminal pathlistClockNm  dc.b "stclock",0 clock module name endmCONFIG_H0 macroMainFram dc.b "Atari ST",0SysStart dc.b "sysgo",0 name of initial module to executeSysParam dc.b C$CR,0 parameter to SysStartSysDev   dc.b "/h0",0 initial system disk pathlistConsolNm dc.b "/term",0  console terminal pathlistClockNm  dc.b "stclock",0 clock module name endm* (Other default values may be reset here)********************************* SCF device descriptor definitions*    used only by scf device descriptor modules* SCFDesc: Port,Vector,IRQlevel,Priority,Parity,BaudRate,DriverNameTERM macro* default descriptor values can be changed here SCFDesc $00000800,$46,6,1,0,0,stconpagpause equ ON endmTERM.M110 macro* default descriptor values can be changed here SCFDesc $c00000,$46,6,1,0,0,stm110pagpause equ ON endmTERM.194 macro* default descriptor values can be changed here SCFDesc $c00000,$46,6,1,0,0,st194pagpause equ ON endmVTERM macro* default descriptor values can be changed here SCFDesc $00000800+VtermNum,$46,6,1,0,0,stvirpagpause equ ON endmMOUSE macro* default descriptor values can be changed here SCFDesc $00000900,$46,6,1,0,0,stmouseDevCon set 0 endmT1 macro* default descriptor values can be changed here SCFDesc $00FFFA00,$4A,6,1,0,14,stserpagpause equ ONDevCon set 0 endmT2 macro* default descriptor values can be changed here SCFDesc $00FFFA00,$46,6,2,0,0,stmidipagpause equ ONDevCon set 0 endmT3 macro* default descriptor values can be changed here SCFDesc $00800001,$50,3,1,0,14,st681pagpause equ ONDevCon dc.w D_DUART1-D_DUART1 endmT4 macro* default descriptor values can be changed here SCFDesc $00800011,$50,3,2,0,14,st681pagpause equ ONDevCon dc.w D_DUART1-D_DUART1 endmT5 macro* default descriptor values can be changed here SCFDesc $00800021,$51,5,1,0,14,st681pagpause equ ONDevCon dc.w D_DUART2-D_DUART1 endmT6 macro* default descriptor values can be changed here SCFDesc $00800031,$51,5,2,0,14,st681pagpause equ ONDevCon dc.w D_DUART2-D_DUART1 endmP macro* default descriptor values can be changed here SCFDesc $00FF8800,$40,6,1,0,0,stparDevCon set 0 endmIEC macro* default descriptor values can be changed here IECDesc $00FF8600,$47,6,3,stiecDevCon set 0 endm***************************************************** PTY Pseudo TTY Desc macro*PTY macro* default descriptor values can be changed here SCFDesc $100+PtyType+(PtyNum*2),0,0,0,$00,$00,ptyDevCon dc.w PtyNum,PtyType endm***************************************************** Spooler Device Desc macro*SX macro* default descriptor values can be changed here PSSDesc pssdrivDevCon set 0 endm***************************************************** RBF device descriptor definitions** RBFDesc: Port,Vector,IRQLevel,Priority,DriverName*DiskD0 macro RBFDesc $00FF8600,$47,6,2,stfd,cum380* Default disk values may be changed hereBitDns set DoubleSectTrk set 18SectTrk0 set 18DnsTrk0 set DoubleTrkDns set DoubleSectOffs set 0TrkOffs set 0StepRate set 3DevCon dc.b FDCache  endmDiskD1 macro RBFDesc $00FF8600,$47,6,2,stfd,cum380* Default disk values may be changed hereBitDns set DoubleSectTrk set 18SectTrk0 set 18DnsTrk0 set DoubleSectOffs set 0TrkOffs set 0StepRate set 3DevCon set 0 endmDiskD0S macro RBFDesc $00FF8600,$47,6,2,stfd,std380* Default disk values may be changed hereBitDns set DoubleSectTrk set 16SectTrk0 set 16DnsTrk0 set DoubleTrkDns set DoubleSectOffs set 1TrkOffs set 0StepRate set 3DevCon dc.b FDCache  endmDiskD1S macro RBFDesc $00FF8600,$47,6,2,stfd,std380* Default disk values may be changed hereBitDns set DoubleSectTrk0 set 16SectTrk set 16DnsTrk0 set DoubleTrkDns set DoubleSectOffs set 1TrkOffs set 0StepRate set 3DevCon set 0 endmDiskD0U macro RBFDesc $00FF8600,$47,6,2,stfd,uv580* Default disk values may be changed hereStepRate set 3DevCon dc.b FDCache  endmDiskD1U macro RBFDesc $00FF8600,$47,6,2,stfd,std380* Default disk values may be changed hereStepRate set 3DevCon set 0 endmTOSDiskD0 macro RBFDesc $00FF8600,$47,6,2,stfd,std380* Default disk values may be changed hereSectSize set 512SectOffs set 1DevCon dc.b FDCache endmTOSDiskD1 macro RBFDesc $00FF8600,$47,6,2,stfd,std380* Default disk values may be changed hereSectSize set 512SectOffs set 1DevCon set 0 endm***************************************************** PFD Pseudo Floppy Disk desc macro*DiskPFD macro RBFDesc $00001000+PfdNum,$47,6,1,pfddriv,std380Control set MultEnabl endm***************************************************** HD Device desc macro*DiskHD macro RBFDesc $00FF8600,$47,6,1,sthd,autosizeDiskKind  set HardStepRate  set 0 allow enhanced seek speedNoVerify  set OFFControl   set MultEnabl+FmtDsabl+AutoEnablDevCon set 0* dc.w PrmSize* dc.b $01 # of cylinders (high byte)* dc.b $32 # of cylinders (low byte)* dc.b Heads # of heads* dc.b $01 cylinder # to begin reduced write (high byte)* dc.b $40 cylinder # to begin reduced write (low byte)* dc.b $00 cylinder to begin write precomp (high byte)* dc.b $C8 cylinder to begin write precomp (low byte)* dc.b $0B ecc burst length*PrmSize equ *-DevCon endm********************************* These definitions are only used by the bootstrap ROM source* This configuration is for a VME110 cpu and a VME400 serial card.*DEBUGGER equ 1 debugger is included with boot*ROMBUG equ 1 debugger is excluded RAMVects equ 1 exception vectors are RAM*PARITY equ 1 Memory with Parity may be usedConsType equ AtariConCommType equ MFP68901Mem.Beg  equ $8500 start of normal memory area to searchMem.End  equ $400000 end of normal memory area to searchSpc.Beg  equ 0 beginning of special area to searchSpc.End  equ 0 end of special area to searchCons_Adr equ $00000800 console device addressCons_Par equ $0 console device parity inizComm_Adr equ $00FFFA00 auxilliary device addressComm_Par equ 0 Parity unused in 7201 driverFDPort    equ  $00FF8600FD_Vct    equ $47SysDisk equ FDPort this equate is to make boot.a genericFDsk_Vct equ FD_VctModCntl  equ $0 address of module control register (not used)MemDefs macro   dc.l Mem.Beg,Mem.End the normal memory search list dc.l 0 dc.l Spc.Beg,Spc.End the special memory search list dc.l 0 dc.l 0,0,0,0,0,0,0,0,0,0,0,0,0,0 free bytes for patching endm opt l