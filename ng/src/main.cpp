#include "musashi/m68k.h"

unsigned int m68k_read_memory_8(unsigned int address)
{
    return 0;
}

unsigned int m68k_read_memory_16(unsigned int address)
{
    return 0;
}

unsigned int m68k_read_memory_32(unsigned int address)
{
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

int main(int argc, char **argv)
{
    return 0;
}
