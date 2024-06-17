#include <assert.h>
#include <stdio.h>
#include <string>
#include <format>
#include <fstream>
#include <iterator>
#include <vector>

#include "musashi/m68k.h"

const auto AE_CPU_TYPE = M68K_CPU_TYPE_68030;

#include "os9-header.h"
#include "address-space.h"
#include "loader.h"
#include "disasm-utils.h"

extern "C" void cpu_instr_callback(unsigned int pc) {
    char dasm[200];
    disassemble_into(dasm, sizeof(dasm), pc);

    dump_regs();
    printf("%s\n", dasm);
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

    auto &space = address_space::single();
    auto loaded = loader(data, space);

    loaded.header.dump();
    printf(" -\n");

    loaded.dump();
    printf(" -\n");

    space.dump();
    printf(" -\n");

    // dump_disasm(loaded.exec(), 20);
    // printf(" -\n");

    assert(loaded.header.exec != 0xcafebabe);

    m68k_init();
    m68k_set_cpu_type(AE_CPU_TYPE);
    m68k_pulse_reset();

    printf(" === \n");

    auto newpid = 1;
    auto group_user = 0x00020003;
    auto priority = 4;
    auto inherited_paths = 5;
    auto args = 6;

    auto top_of_stack = loaded.stack_start + loaded.header.stack;
    auto module_end = loaded.module_start + (uint32_t)data.size();

    m68k_set_reg(M68K_REG_SR, 0);
    m68k_set_reg(M68K_REG_PC, loaded.exec());

    m68k_set_reg(M68K_REG_A1, module_end);
    m68k_set_reg(M68K_REG_A3, loaded.module_start);
    m68k_set_reg(M68K_REG_A5, top_of_stack);
    m68k_set_reg(M68K_REG_A6, loaded.data_start + 0x8000);
    m68k_set_reg(M68K_REG_A7, top_of_stack);

    m68k_set_reg(M68K_REG_D0, newpid);
    m68k_set_reg(M68K_REG_D1, group_user);
    m68k_set_reg(M68K_REG_D2, priority);
    m68k_set_reg(M68K_REG_D3, inherited_paths);

    auto cycles = 10000; // 100000;
    m68k_execute(cycles);

    return 0;
}
