#pragma once

#define ENUMERATE_IMPLEMENTED_OS9_SYSCALLS() \
    X(0x06, F$Exit,     "Terminate Process") \
    X(0x28, F$SRqMem,   "System Memory Request") \
    X(0x84, I$Open,     "Open Existing File") \
    X(0x8c, I$WritLn,   "Write Line of ASCII Data") \
    X(0x8d, I$GetStt,   "Get Path Status") \
    X(0x8f, I$Close,    "Close Path")

#define ENUMERATE_OS9_SYSCALLS() \
    X(0x00, "F$Link",     "Link to Module") \
    X(0x01, "F$Load",     "Load Module from File") \
    X(0x02, "F$UnLink",   "Unlink Module") \
    X(0x03, "F$Fork",     "Start New Process") \
    X(0x04, "F$Wait",     "Wait for Child Process to Die") \
    X(0x05, "F$Chain",    "Chain Process to New Module") \
    X(0x06, "F$Exit",     "Terminate Process") \
    X(0x07, "F$Mem",      "Set Memory Size") \
    X(0x08, "F$Send",     "Send Signal to Process") \
    X(0x09, "F$Icpt",     "Set Signal Intercept") \
    X(0x0a, "F$Sleep",    "Suspend Process") \
    X(0x0b, "F$SSpd",     "Suspend Process") \
    X(0x0c, "F$ID",       "Return Process ID") \
    X(0x0d, "F$SPrior",   "Set Process Priority") \
    X(0x0e, "F$STrap",    "Set Trap Intercept") \
    X(0x0f, "F$PErr",     "Print Error") \
    X(0x10, "F$PrsNam",   "Parse Pathlist Name") \
    X(0x11, "F$CmpNam",   "Compare Two Names") \
    X(0x12, "F$SchBit",   "Search Bit Map") \
    X(0x13, "F$AllBit",   "Allocate in Bit Map") \
    X(0x14, "F$DelBit",   "Deallocate in Bit Map") \
    X(0x15, "F$Time",     "Get Current Time") \
    X(0x16, "F$STime",    "Set Current Time") \
    X(0x17, "F$CRC",      "Generate CRC") \
    X(0x18, "F$GPrDsc",   "get Process Descriptor copy") \
    X(0x19, "F$GBlkMp",   "get System Block Map copy") \
    X(0x1a, "F$GModDr",   "get Module Directory copy") \
    X(0x1b, "F$CpyMem",   "Copy External Memory") \
    X(0x1c, "F$SUser",    "Set User ID number") \
    X(0x1d, "F$UnLoad",   "Unlink Module by name") \
    X(0x1e, "F$RTE",      "Return from Intercept routine") \
    X(0x1f, "F$GPrDBT",   "Get system global data copy") \
    X(0x20, "F$Julian",   "Convert gregorian to Julian date") \
    X(0x21, "F$TLink",    "Link trap subroutine package") \
    X(0x22, "F$DFork",    "Debugging Fork call") \
    X(0x23, "F$DExec",    "Debugging execution call (single step)") \
    X(0x24, "F$DExit",    "Debugging exit call (kill child)") \
    X(0x25, "F$DatMod",   "Create data module") \
    X(0x26, "F$SetCRC",   "Generate valid header and CRC in module") \
    X(0x27, "F$SetSys",   "Set/examine system global variable") \
    X(0x28, "F$SRqMem",   "System Memory Request") \
    X(0x29, "F$SRtMem",   "System Memory Return") \
    X(0x2a, "F$IRQ",      "Enter IRQ Polling Table") \
    X(0x2b, "F$IOQu",     "Enter I/O Queue") \
    X(0x2c, "F$AProc",    "Enter Active Process Queue") \
    X(0x2d, "F$NProc",    "Start Next Process") \
    X(0x2e, "F$VModul",   "Validate Module") \
    X(0x2f, "F$FindPD",   "Find Process/Path Descriptor") \
    X(0x30, "F$AllPD",    "Allocate Process/Path Descriptor") \
    X(0x31, "F$RetPD",    "Return Process/Path Descriptor") \
    X(0x32, "F$SSvc",     "Service Request Table Initialization") \
    X(0x33, "F$IODel",    "Delete I/O Module") \
    X(0x37, "F$GProcP",   "Get Process ptr") \
    X(0x38, "F$Move",     "Move Data") \
    X(0x39, "F$AllRAM",   "Allocate RAM blocks") \
    X(0x3a, "F$Permit",   "(old F$AllImg) Allocate Image RAM blocks") \
    X(0x3b, "F$Protect",  "(old F$DelImg) Deallocate Image RAM blocks") \
    X(0x3f, "F$AllTsk",   "Allocate Process Task number") \
    X(0x40, "F$DelTsk",   "Deallocate Process Task number") \
    X(0x4b, "F$AllPrc",   "Allocate Process Descriptor") \
    X(0x4c, "F$DelPrc",   "Deallocate Process Descriptor") \
    X(0x4e, "F$FModul",   "Find Module Directory Entry") \
    X(0x52, "F$SysDbg",   "Invoke system level debugger") \
    X(0x53, "F$Event",    "Create/Link to named event") \
    X(0x54, "F$Gregor",   "Convert julian date to gregorian date") \
    X(0x55, "F$SysID",    "return system identification") \
    X(0x56, "F$Alarm",    "send alarm signal") \
    X(0x57, "F$SigMask",  "set signal mask") \
    X(0x58, "F$ChkMem",   "determine if user process may access memory area") \
    X(0x59, "F$UAcct",    "inform user accounting of process status") \
    X(0x5a, "F$CCtl",     "cache control") \
    X(0x5b, "F$GSPUMp",   "get SPU map information for a process") \
    X(0x5c, "F$SRqCMem",  "System Colored Memory Request") \
    X(0x5d, "F$POSK",     "execute svc request") \
    X(0x5e, "F$Panic",    "Panic warning") \
    X(0x5f, "F$MBuf",     "Memory buffer manager") \
    X(0x60, "F$Trans",    "Translate memory address to/from external bus") \
    X(0x61, "F$FIRQ",     "Add/Remove Fast IRQ service") \
    X(0x62, "F$Sema",     "Semphore P/V (reserve/release) service") \
    X(0x63, "F$SigReset", "Reset signal intercept context") \
    X(0x64, "F$DAttach",  "Debugger attach to running process") \
    X(0x65, "F$Flash",    "Manage FLASH device(s)") \
    X(0x66, "F$PwrMan",   "Perform Power Management functions") \
    X(0x67, "F$Crypt",    "Perform Cryptographic Functions") \
    X(0x70, "F$HLProto",  "High-Level Protocol manager request") \
    X(0x80, "I$Attach",   "Attach I/O Device") \
    X(0x81, "I$Detach",   "Detach I/O Device") \
    X(0x82, "I$Dup",      "Duplicate Path") \
    X(0x83, "I$Create",   "Create New File") \
    X(0x84, "I$Open",     "Open Existing File") \
    X(0x85, "I$MakDir",   "Make Directory File") \
    X(0x86, "I$ChgDir",   "Change Default Directory") \
    X(0x87, "I$Delete",   "Delete File") \
    X(0x88, "I$Seek",     "Change Current Position") \
    X(0x89, "I$Read",     "Read Data") \
    X(0x8a, "I$Write",    "Write Data") \
    X(0x8b, "I$ReadLn",   "Read Line of ASCII Data") \
    X(0x8c, "I$WritLn",   "Write Line of ASCII Data") \
    X(0x8d, "I$GetStt",   "Get Path Status") \
    X(0x8e, "I$SetStt",   "Set Path Status") \
    X(0x8f, "I$Close",    "Close Path") \
    X(0x92, "I$SGetSt",   "Getstat using system path number")

inline const char *os9_syscall_name(uint16_t call) {
    switch (call) {
#undef X
#define X(id, name, desc) case id: return name;
        ENUMERATE_OS9_SYSCALLS()
#undef X
    }

    return nullptr;
}

inline const char *os9_syscall_desc(uint16_t call) {
    switch (call) {
#undef X
#define X(id, name, desc) case id: return desc;
        ENUMERATE_OS9_SYSCALLS()
#undef X
    }

    return nullptr;
}

