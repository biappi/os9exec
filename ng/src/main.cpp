#include <assert.h>
#include <stdio.h>
#include <string>
#include <format>
#include <fstream>
#include <iterator>
#include <vector>

#include "musashi/m68k.h"
#include "os9-header.h"
#include "address-space.h"

const auto AE_CPU_TYPE = M68K_CPU_TYPE_68020;

std::string make_hex(unsigned int pc, unsigned int length)
{
    std::string hex = "";

    for(; length > 0 ; length -= 2)
    {
        hex += std::format("{:04x}", m68k_read_disassembler_16(pc));
        pc += 2;
        if(length > 2) hex += " ";
    }

    return hex;
}

int disassemble_into(char *dasm, size_t len, uint32_t addr) {
    char buff[100];
    auto instr_size = m68k_disassemble(buff, addr, AE_CPU_TYPE);
    auto hex = make_hex(addr, instr_size);

    snprintf(dasm, len, "%08x:     %-20s: %s", addr, hex.c_str(), buff);
    return instr_size;
}

extern "C" void cpu_instr_callback(unsigned int pc) {
    char dasm[200];
    disassemble_into(dasm, sizeof(dasm), pc);
    printf("executing    %s\n", dasm);
}

void dump_disasm(uint32_t address, int howmany) {
    for (int i = 0; i < howmany; i++) {
        char dasm[200];
        address += disassemble_into(dasm, sizeof(dasm), address);
        printf("%s\n", dasm);
    }
}

int main(int argc, char **argv)
{
    const char *mydefault = "/Users/willy/Sources/os9exec-git_code/dd/CMDS/dxfout";
    const char *path = argc == 2 ? argv[1] : mydefault;

    auto file = std::ifstream(path, std::ios::binary);

    if (!file) {
        printf("Failed to open file: %s\n", path);
        return 1;
    }

    auto buf = std::istreambuf_iterator<char>(file);
    auto data = std::vector<uint8_t>(buf, {});
    auto header = os9_header(data);

    header.dump();
    printf(" -\n");

    char *namestring = (char *)&data[header.names];

    assert(header.stack != 0xcafebabe);

    const uint32_t data_start   = 0x1000;
    const uint32_t params_len   = 0x0100;
    const uint32_t module_align = 0x1000;

    uint32_t stack_start  = data_start + header.mem;
    uint32_t param_start  = stack_start + header.stack;
    uint32_t param_end    = param_start + params_len;
    uint32_t module_start = ((param_end / module_align) + 1) * module_align;

    auto &space = address_space::single();

    space.add_zone("data",   data_start,   header.mem);
    space.add_zone("stack",  stack_start,  header.stack);
    space.add_zone("params", param_start,  params_len);
    space.add_zone("code",   module_start, data);

    printf("name:         %s\n",   namestring);
    printf("stack_start:  %08x\n", stack_start);
    printf("param_start:  %08x\n", param_start);
    printf("param_end:    %08x\n", param_end);
    printf("module_start: %08x\n", module_start);
    printf(" -\n");

    space.dump();
    printf(" -\n");

    dump_disasm(module_start + header.exec, 20);

    return 0;

    auto r = be_reader(data);
    r.reset(header.irefs);

    printf(" - code refs\n");

    while (1) {
        auto msw    = r.read_16();
        auto n_lsw  = r.read_16();

        printf("msw:    %04x\n", msw);
        printf("n_lsw:  %04x\n", n_lsw);

        if (msw == 0 && n_lsw == 0) {
            break;
        }

        for (int i = 0; i < n_lsw; i++) {
            printf("  %03d:  %04x\n", i, r.read_16());
        }
    }

    printf(" - data refs\n");

    while (1) {
        auto msw    = r.read_16();
        auto n_lsw  = r.read_16();

        printf("msw:    %04x\n", msw);
        printf("n_lsw:  %04x\n", n_lsw);

        if (msw == 0 && n_lsw == 0) {
            break;
        }

        for (int i = 0; i < n_lsw; i++) {
            printf("  %03d:  %04x\n", i, r.read_16());
        }
    }

    return 0;

    m68k_init();
    m68k_set_cpu_type(AE_CPU_TYPE);
    m68k_pulse_reset();

    auto cycles = 10; // 100000;
    m68k_execute(cycles);

    return 0;
}
