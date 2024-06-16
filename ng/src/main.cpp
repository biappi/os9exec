#include <stdio.h>
#include <string>
#include <format>

#include "musashi/m68k.h"

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

    auto instr_size = m68k_disassemble(buff, pc, M68K_CPU_TYPE_68000);
    auto hex = make_hex(pc, instr_size);

    printf("executing    %08x     %-20s: %s\n", pc, hex.c_str(), buff);
}

int main(int argc, char **argv)
{
    m68k_init();
    m68k_set_cpu_type(M68K_CPU_TYPE_68020);
    m68k_pulse_reset();

    auto cycles = 10; // 100000;
    m68k_execute(cycles);

    return 0;
}
