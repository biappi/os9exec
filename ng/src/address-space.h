#pragma once

#include <string>
#include <vector>

#include "musashi/m68k.h"

struct memory_zone {
    std::string name;
    uint32_t address;
    std::vector<uint8_t> data;

    uint32_t size() { return (uint32_t)data.size(); }
    uint32_t start() { return address; }
    uint32_t end() { return address + size(); }

    bool contains(uint32_t address) {
        return (start() <= address) && (address < end());
    }
};

enum class access_type: int {
    writing = 0,
    reading = 1,
};

enum class write_size: int {
    write8  = 0,
    write16 = 1,
    write32 = 2,
};

class address_space {
public:
    static address_space &single();

    void log_mem(write_size size,
                 access_type ro,
                 uint32_t address,
                 uint32_t v);

    void add_zone(std::string name,
                  uint32_t address,
                  std::vector<uint8_t> &data)
    {
        zones.emplace_back(name, address, data);
    }

    void add_zone(std::string name, 
                  uint32_t address,
                  size_t size)
    {
        auto data = std::vector<uint8_t>();
        data.resize(size, 0);
        zones.emplace_back(name, address, data);
    }

    memory_zone *zone_for_addr(uint32_t addr) {
        for (auto &zone : zones) {
            if (zone.contains(addr))
                return &zone;
        }

        return nullptr;
    }

    void unmapped_access(uint32_t addr, bool write) {
        const char * what = write ? "write" : "read";
        printf("unmapped access (%s) at %08x\n", what, addr);
        m68k_pulse_bus_error();
    }

    void write8(uint32_t addr, uint8_t data) {
        auto zone = zone_for_addr(addr);
        if (!zone) { unmapped_access(addr, true); return; }
        zone->data[addr - zone->address] = data;
    }

    void write16(uint32_t address, uint16_t value) {
        write8(address + 0, (value & 0xff00) >> 8);
        write8(address + 1, (value & 0x00ff) >> 0);
    }

    void write32(uint32_t address, uint32_t value) {
        write16(address + 0, (value & 0xffff0000) >> 16);
        write16(address + 2, (value & 0x0000ffff) >>  0);
    }

    uint8_t read8(uint32_t addr) {
        auto zone = zone_for_addr(addr);
        if (!zone) { unmapped_access(addr, false); return 0; }
        return zone->data[addr - zone->address];
    }

    uint16_t read16(uint32_t addr) {
        auto h = read8(addr + 0) << 8;
        auto l = read8(addr + 1) << 0;
        return h + l;
    }

    uint32_t read32(uint32_t addr) {
        auto h = read16(addr + 0) << 16;
        auto l = read16(addr + 2) <<  0;
        return h + l;
    }

    std::string read_string(uint32_t addr) {
        std::string path;
        char x = read8(addr);

        while (x != 0) {
            path += x;
            x = read8(addr++);
        }

        return path;
    }

    std::string read_string(uint32_t addr, uint32_t max_len) {
        std::string path;
        uint32_t end = addr + max_len;
        char x = read8(addr);

        while ((x != 0) && (addr < end)) {
            path += x;
            x = read8(addr++);
        }

        return path;
    }

    void dump() {
        printf("%-10s %-8s %-8s %-8s\n"
               "---------------------------------------\n",
               "name",
               "addr",
               "size",
               "len");

        for (auto &zone : zones) {
            printf("%-10s %08x %08x (%08x)\n",
                   zone.name.c_str(),
                   zone.start(),
                   zone.end(),
                   zone.size());
        }
    }

private:
    std::vector<memory_zone> zones;

protected:
    address_space() { }
};
