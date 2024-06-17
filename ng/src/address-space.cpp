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
    return address_space::single().read16(address);
}

unsigned int m68k_read_disassembler_32(unsigned int address) {
    return address_space::single().read32(address);
}

unsigned int m68k_read_memory_8(unsigned int address) {
    auto v = address_space::single().read8(address);
    printf("reading ( 8) %08x -> %02x\n", address, v);
    return v;
}

unsigned int m68k_read_memory_16(unsigned int address) {
    auto v = address_space::single().read16(address);
    printf("reading (16) %08x -> %04x\n", address, v);
    return v;
}

unsigned int m68k_read_memory_32(unsigned int address) {
    auto v = address_space::single().read32(address);
    printf("reading (32) %08x -> %08x\n", address, v);
    return v;
}

void m68k_write_memory_8(unsigned int address, unsigned int value) {
    printf("writing ( 8) %08x <- %02x\n", address, value);
    address_space::single().write8(address, value);
}

void m68k_write_memory_16(unsigned int address, unsigned int value) {
    printf("writing (16) %08x <- %04x\n", address, value);
    address_space::single().write16(address, value);
}

void m68k_write_memory_32(unsigned int address, unsigned int value) {
    printf("writing (32) %08x <- %08x\n", address, value);
    address_space::single().write32(address, value);
}
