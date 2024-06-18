#include <stdio.h>
#include <stdlib.h>

#include "address-space.h"
#include "musashi/m68k.h"

extern bool log_mem;

address_space &address_space::single() {
    static address_space single_space;
    return single_space;
}

void address_space::log_mem(write_size size, access_type ro, uint32_t address, uint32_t v) {
    static const char *access[] = {
        "writing",
        "reading",
    };

    static const char *arrow[] = {
        "<-",
        "->",
    };

    static const char *formats[] = {
        "%s ( 8) %08x %s %02x       %s\n",
        "%s (16) %08x %s %04x     %s\n",
        "%s (32) %08x %s %08x %s\n",
    };

    if (!::log_mem)
        return;

    auto zone = zone_for_addr(address);
    char addr[0x100];

    if (zone) {
        snprintf(addr, 
                 sizeof(addr), "(%s +%x)",
                 zone->name.c_str(),
                 address - zone->start());
    } else {
        addr[0] = 0;
    }

    printf(formats[(int)size],
           access[(int)ro],
           address,
           arrow[(int)ro],
           v,
           addr);
}

#define space address_space::single()

unsigned int m68k_read_disassembler_8(unsigned int address) {
    return space.read8(address);
}

unsigned int m68k_read_disassembler_16(unsigned int address) {
    return space.read16(address);
}

unsigned int m68k_read_disassembler_32(unsigned int address) {
    return space.read32(address);
}

unsigned int m68k_read_memory_8(unsigned int address) {
    auto v = space.read8(address);
    space.log_mem(write_size::write8, access_type::reading, address, v);
    return v;
}

unsigned int m68k_read_memory_16(unsigned int address) {
    auto v = space.read16(address);
    space.log_mem(write_size::write16, access_type::reading, address, v);
    return v;
}

unsigned int m68k_read_memory_32(unsigned int address) {
    auto v = space.read32(address);
    space.log_mem(write_size::write32, access_type::reading, address, v);
    return v;
}

void m68k_write_memory_8(unsigned int address, unsigned int value) {
    space.log_mem(write_size::write8, access_type::writing, address, value);
    space.write8(address, value);
}

void m68k_write_memory_16(unsigned int address, unsigned int value) {
    space.log_mem(write_size::write16, access_type::writing, address, value);
    space.write16(address, value);
}

void m68k_write_memory_32(unsigned int address, unsigned int value) {
    space.log_mem(write_size::write32, access_type::writing, address, value);
    space.write32(address, value);
}
