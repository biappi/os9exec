#pragma once

#include "os9-syscalls-desc.h"

class os9_calls {
public:
    os9_calls(address_space &s)
        : space(s)
    {
    }

    void F$Exit() {
        uint32_t code = m68k_get_reg(NULL, M68K_REG_D1) & 0x0000ffff;
        printf("\n\nemulation complete. exit code %d\n\n", code);
        exit(0);
    }

    void F$SRqMem() {
        uint32_t requested_block_size = m68k_get_reg(NULL, M68K_REG_D0);
        printf("blocks size: %08x\n",
               requested_block_size);

        uint32_t actual_size = 0;
        uint32_t out_buffer = 0;

        m68k_set_reg(M68K_REG_D0, actual_size);
        m68k_set_reg(M68K_REG_A2, out_buffer);
    }

    void I$Open() {
        uint32_t pathname_ptr = m68k_get_reg(NULL, M68K_REG_A0);
        uint32_t access_mode  = m68k_get_reg(NULL, M68K_REG_D0);

        auto path = space.read_string(pathname_ptr);

        auto r = access_mode & 0x01;
        auto w = access_mode & 0x02;
        auto e = access_mode & 0x04;
        auto s = access_mode & 0x40;
        auto d = access_mode & 0x80;

        printf("path: %s - access mode: %c%c...%c%c%c\n",
               path.c_str(),
               d ? 'd' : '-',
               s ? 's' : '-',
               e ? 'e' : '-',
               w ? 'w' : '-',
               r ? 'r' : '-');
    }

    void I$WritLn() {
        uint16_t path       = m68k_get_reg(NULL, M68K_REG_D0) & 0x0000ffff;
        uint32_t max_len    = m68k_get_reg(NULL, M68K_REG_D1);
        uint32_t string_ptr = m68k_get_reg(NULL, M68K_REG_A0);

        auto string = space.read_string(string_ptr);

        printf("path: %02x - max_len: %08x: %s\n",
               path,
               max_len,
               string.c_str());
    }

    void I$GetStt() {
#define SS_Opt   0x00 // read/write PD options
#define SS_Ready 0x01 // check for device ready
#define SS_Size  0x02 // read/write file size
#define SS_Pos   0x05 // get file current position
#define SS_EOF   0x06 // test for end of file

#define SS_DevNm 0x0e // return device name
#define SS_FD    0x0f // return file descriptor
#define SS_FDInf 0x20 // get FD info for specified FD sector

#define SS_LBlink 0x080 + 32
#define SS_201    0x201
#define SS_Etc    0x3EE

        uint16_t path = m68k_get_reg(NULL, M68K_REG_D0) & 0x0000ffff;
        uint16_t func = m68k_get_reg(NULL, M68K_REG_D1) & 0x0000ffff;

        const char *m;
        switch (func) {
            case SS_Opt:   m = "SS_Opt: read/write PD options";                 break;
            case SS_Ready: m = "SS_Ready: check for device ready";              break;
            case SS_Size:  m = "SS_Size: read/write file size";                 break;
            case SS_Pos:   m = "SS_Pos: get file current position";             break;
            case SS_EOF:   m = "SS_EOF: test for end of file";                  break;
            case SS_DevNm: m = "SS_DevNm: return device name";                  break;
            case SS_FD:    m = "SS_FD: return file descriptor";                 break;
            case SS_FDInf: m = "SS_FDInf: get FD info for specified FD sector"; break;
            default:       m = "unknown"; break;
        }

        printf("path_nr: %04x %02x: %s\n", path, func, m);
    }

    void I$Close() {
        uint16_t path = m68k_get_reg(NULL, M68K_REG_D0) & 0x0000ffff;
        printf("path: %02x\n", path);
    }

    void dispatch_call() {
        uint32_t pc = m68k_get_reg(NULL, M68K_REG_PC);
        uint32_t sp = m68k_get_reg(NULL, M68K_REG_SP);

        auto old_return = space.read32(sp + 2);
        auto func_code  = space.read16(old_return);
        space.write32(sp + 2, old_return + 2);

        auto name = os9_syscall_name(func_code) ?: "UNKNOWN";
        //auto desc = os9_syscall_desc(func_code) ?: "";

        printf("%08x  ** OS9 syscall: %2x - %-8s    ",
               pc, func_code, name);

        switch (func_code) {
#undef X
#define X(id, name, desc) case id: name (); return;
            ENUMERATE_IMPLEMENTED_OS9_SYSCALLS()
#undef X
            default:
                printf("\n");
                dump_regs();
        }
    }

private:
    address_space &space;
};
