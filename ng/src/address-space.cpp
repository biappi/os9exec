#include <stdio.h>

#include "address-space.h"
#include "musashi/m68k.h"

address_space &address_space::single() {
    static address_space single_space;
    return single_space;
}

unsigned int m68k_read_disassembler_8(unsigned int address) {
    return address_space::single().read8(address);
}

unsigned int m68k_read_disassembler_16(unsigned int address) {
    auto h = m68k_read_disassembler_8(address + 0) << 8;
    auto l = m68k_read_disassembler_8(address + 1) << 0;
    return h + l;
}

unsigned int m68k_read_disassembler_32(unsigned int address)
{
    auto h = m68k_read_disassembler_16(address + 0) << 16;
    auto l = m68k_read_disassembler_16(address + 2) <<  0;
    return h + l;
}

unsigned int m68k_read_memory_8(unsigned int address) {
    printf("reading ( 8) %08x\n", address);
    return m68k_read_disassembler_8(address);
}

unsigned int m68k_read_memory_16(unsigned int address) {
    printf("reading (16) %08x\n", address);
    return m68k_read_disassembler_16(address);
}

unsigned int m68k_read_memory_32(unsigned int address) {
    printf("reading (32) %08x\n", address);
    return m68k_read_disassembler_32(address);
}

void m68k_write_memory_8(unsigned int address, unsigned int value) {
    address_space::single().write8(address, value);
}

void m68k_write_memory_16(unsigned int address, unsigned int value) {
    m68k_write_memory_8(address, (value & 0xff00) >> 8);
    m68k_write_memory_8(address, (value & 0x00ff) >> 0);
}

void m68k_write_memory_32(unsigned int address, unsigned int value) {
    m68k_write_memory_16(address, (value & 0xffff0000) >> 16);
    m68k_write_memory_16(address, (value & 0x0000ffff) >>  0);
}
