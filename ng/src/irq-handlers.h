#pragma once

#include "os9-syscalls-desc.h"
#include "os9-syscalls.h"
#include "musashi/m68kcpu.h"

class irq_handlers {
public:
    static irq_handlers &single();

    static const uint32_t handlers_count = 255;

    static const uint32_t handlers_base = 0xf0000000;
    static const uint32_t handlers_size = 255 << 4;
    static const uint32_t handlers_end = handlers_base + handlers_size;

    static const uint32_t stack_start = handlers_end;
    static const uint32_t stack_size = 0x1000;
    static const uint32_t stack_end = stack_start + stack_size;

    static bool address_is_irq_handler(uint32_t address) {
        return (handlers_base <= address) && (address < handlers_end);
    }

    void check_address(uint32_t pc) {
        if (!address_is_irq_handler(pc)) return;

        uint32_t sp = m68k_get_reg(NULL, M68K_REG_SP);
        auto vector = (pc - handlers_base) >> 4;

        switch (vector) {

            case  2: bus_error(); break;
            case 32: os9_trap();  break;

            default:
                printf("%08x  IRQ handler vector: %x\n", pc, vector);
        }
    }

    void bus_error() {
        uint32_t pc = m68k_get_reg(NULL, M68K_REG_PC);
        uint32_t sp = m68k_get_reg(NULL, M68K_REG_SP);

        printf("%08x  ======\n\n", pc);
        printf("bus error at pc: %08x\n\n", space.read32(sp +  2));

        m68k_pulse_halt();
        USE_ALL_CYCLES();
    }

    void os9_trap() {
        uint32_t pc = m68k_get_reg(NULL, M68K_REG_PC);
        uint32_t sp = m68k_get_reg(NULL, M68K_REG_SP);

        auto old_return = space.read32(sp + 2);
        auto func_code  = space.read16(old_return);
        space.write32(sp + 2, old_return + 2);

        auto name = os9_syscall_name(func_code) ?: "UNKNOWN";
        printf("%08x  ** OS9 trap func code: %x - %s\n", pc, func_code, name);

        dump_regs();
    }

private:
    irq_handlers(address_space &s)
        : space(s)
    {
        space.add_zone("irqvects", 0, 4 * handlers_count);
        space.add_zone("irqhandlers", handlers_base, handlers_size);
        space.add_zone("irqstack", stack_start, stack_size);

        // fill with addresses
        for (int i = 2; i < handlers_count; i++) {
            space.write32(4 * i, handlers_base + (i << 4));
        }

        // fill with rte
        for (int i = 0; i < handlers_count; i++) {
            const uint16_t rte_opcode = 0x4E73;
            auto irq_i_base = handlers_base + (i << 4);
            space.write16(irq_i_base, rte_opcode);
        }
    }

    address_space &space;
};
