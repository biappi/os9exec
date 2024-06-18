#pragma once

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

    snprintf(dasm, len, "%08x  :  %-24s :  %s", addr, hex.c_str(), buff);
    return instr_size;
}

void dump_disasm(uint32_t address, int howmany) {
    for (int i = 0; i < howmany; i++) {
        char dasm[200];
        address += disassemble_into(dasm, sizeof(dasm), address);
        printf("%s\n", dasm);
    }
}

void dump_regs() {
    printf("          D%d  ", 0);

    for (int i = 0; i < 8; i++) {
        auto reg = m68k_register_t(M68K_REG_D0 + i);
        printf("%08x  ", m68k_get_reg(nullptr, reg));
    }
    printf("\n");

    printf("          A%d  ", 0);
    for (int i = 0; i < 8; i++) {
        auto reg = m68k_register_t(M68K_REG_A0 + i);
        printf("%08x  ", m68k_get_reg(nullptr, reg));
    }

    printf("\n");
}
