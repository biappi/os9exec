#include <assert.h>
#include <stdio.h>
#include <string>
#include <format>
#include <fstream>
#include <iterator>
#include <vector>

#include "musashi/m68k.h"

const auto AE_CPU_TYPE = M68K_CPU_TYPE_68020;

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

    dump_disasm(loaded.exec(), 20);
    printf(" -\n");

    // dump_relocations(data, loaded.header);
    // printf(" -\n");

    m68k_init();
    m68k_set_cpu_type(AE_CPU_TYPE);
    m68k_pulse_reset();

    auto cycles = 10; // 100000;
    m68k_execute(cycles);

    return 0;
}
