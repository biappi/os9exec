#include <assert.h>
#include <stdio.h>
#include <string>
#include <format>
#include <fstream>
#include <iterator>
#include <vector>

#include "musashi/m68k.h"
#include "os9-header.h"

const auto AE_CPU_TYPE = M68K_CPU_TYPE_68020;

unsigned int m68k_read_memory_8(unsigned int address)
{
    printf("reading ( 8) %08x\n", address);
    return 0;
}

unsigned int m68k_read_memory_16(unsigned int address)
{
    printf("reading (16) %08x\n", address);
    return 0;
}

unsigned int m68k_read_memory_32(unsigned int address)
{
    printf("reading (32) %08x\n", address);
    return 0;
}

unsigned int m68k_read_disassembler_16(unsigned int address) 
{
    return m68k_read_memory_16(address);
}

unsigned int m68k_read_disassembler_32(unsigned int address)
{
    return m68k_read_memory_32(address);
}

void m68k_write_memory_8(unsigned int address, unsigned int value)
{
    return;
}

void m68k_write_memory_16(unsigned int address, unsigned int value)
{
    return;
}

void m68k_write_memory_32(unsigned int address, unsigned int value)
{
    return;
}

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

extern "C" void cpu_instr_callback(unsigned int pc)
{
    char buff[100];

    auto instr_size = m68k_disassemble(buff, pc, AE_CPU_TYPE);
    auto hex = make_hex(pc, instr_size);

    printf("executing    %08x     %-20s: %s\n", pc, hex.c_str(), buff);
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
    char *namestring = (char *)&data[header.names];

    printf(" -\n");

    assert(header.stack != 0xcafebabe);

    const uint32_t data_start   = 0x1000;
    const uint32_t params_len   = 0x0100;
    const uint32_t module_align = 0x1000;

    uint32_t stack_start  = data_start + header.mem;
    uint32_t param_start  = stack_start + header.stack;
    uint32_t param_end    = param_start + params_len;
    uint32_t module_start = ((param_end / module_align) + 1) * module_align;

    printf("name:         %s\n",   namestring);
    printf("stack_start:  %08x\n", stack_start);
    printf("param_start:  %08x\n", param_start);
    printf("param_end:    %08x\n", param_end);
    printf("module_start: %08x\n", module_start);

    printf(" - code refs\n");

    auto r = be_reader(data);
    r.reset(header.irefs);


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
