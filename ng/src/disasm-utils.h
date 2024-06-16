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
