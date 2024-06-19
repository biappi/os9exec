#include <assert.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iterator>
#include <vector>

#include "musashi/m68k.h"

bool log_load   = false;
bool log_params = true;
bool log_cpu = false;
bool log_mem = false;
bool log_reg = false;

const auto AE_CPU_TYPE = M68K_CPU_TYPE_68030;

#include "os9-header.h"
#include "address-space.h"
#include "loader.h"
#include "disasm-utils.h"
#include "irq-handlers.h"

irq_handlers &irq_handlers::single() {
    static irq_handlers single_handlers(address_space::single());
    return single_handlers;
}

uint32_t args(address_space &space, uint32_t start, uint32_t end) {
    std::string program_name = "DXFOUT_OUT";

    std::vector<std::string> env = {
    };

    std::vector<std::string> arg = {
        "argument1",
        "ke",
    };

    std::vector<uint32_t> env_ptr;
    std::vector<uint32_t> arg_ptr;

    uint32_t ptr = start;                        assert(ptr < end);

    if (log_params) printf("args:\n");
    for (auto &i : arg) {
        if (log_params) printf("  args: %08x %s\n", ptr, i.c_str());
        arg_ptr.push_back(ptr - start);
        ptr = space.write_string(ptr, i);        assert(ptr < end);
        ptr = space.write8(ptr, 0);              assert(ptr < end);
    }

    ptr = space.write16(ptr, 0x000d);            assert(ptr < end);

    if (log_params) printf("envs:\n");

    for (auto &i : env) {
        if (log_params) printf("  envs: %08x %s\n", ptr, i.c_str());
        env_ptr.push_back(ptr - start);
        ptr = space.write_string(ptr, i);        assert(ptr < end);
        ptr = space.write8(ptr, 0);              assert(ptr < end);
    }

    ptr = space.write16(ptr, 0xfc01);            assert(ptr < end);

    if (log_params) printf("ptr1:     %08x\n", ptr + 4);
    ptr = space.write32(ptr, ptr + 4);           assert(ptr < end);

    if (log_params) printf("pgm name: %08x %s\n", ptr, program_name.c_str());
    ptr = space.write_string(ptr, program_name); assert(ptr < end);

    ptr = space.write16(ptr, 0x000d);            assert(ptr < end);

    ptr = space.write32(ptr, 0x00000000);        assert(ptr < end);

    for (auto &i : arg_ptr) {
        if (log_params) printf("argptr:   %08x %08x\n", ptr, i);
        ptr = space.write32(ptr, i);             assert(ptr < end);

    }

    ptr = space.write32(ptr, 0x00000000);        assert(ptr < end);

    for (auto &i : env_ptr) {
        ptr = space.write32(ptr, i);             assert(ptr < end);
        if (log_params) printf("envptr:     %08x %08x\n", ptr, i);
    }

    ptr = space.write32(ptr, 0x00000000);        assert(ptr < end);

    return ptr - start;
}

extern "C" void cpu_instr_callback(unsigned int pc) {
    if (log_reg)
        dump_regs();

    if (log_cpu) {
        char dasm[200];
        disassemble_into(dasm, sizeof(dasm), pc);
        printf("%s\n", dasm);
    }

    irq_handlers::single().check_address(pc);
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
    auto &irq   = irq_handlers::single();

    auto loaded = loader(data, space);

    auto args_size = args(space, loaded.param_start, loaded.param_end);

    if (log_load) {
        loaded.header.dump();
        printf(" -\n");

        loaded.dump();
        printf(" -\n");

        space.dump();
        printf(" -\n");

        // dump_disasm(loaded.exec(), 20);
        // printf(" -\n");

        printf(" === \n");
    }

    assert(loaded.header.exec != 0xcafebabe);

    auto stack_end = loaded.stack_start + loaded.header.stack - 1;

    space.write32(4 * 0, stack_end);
    space.write32(4 * 1, loaded.exec());

    m68k_init();
    m68k_set_cpu_type(AE_CPU_TYPE);
    m68k_pulse_reset();

    auto newpid = 1;
    auto group_user = 0x00020003;
    auto priority = 4;
    auto inherited_paths = 5;
    auto args = 6;

    uint32_t data_size = (uint32_t)data.size();

    auto top_of_stack = loaded.stack_start + loaded.header.stack;
    auto module_end = loaded.module_start + data_size;

    m68k_set_reg(M68K_REG_ISP, irq_handlers::stack_end);

    m68k_set_reg(M68K_REG_A1, loaded.param_start + args_size);
    m68k_set_reg(M68K_REG_A3, loaded.module_start);
    m68k_set_reg(M68K_REG_A5, top_of_stack);
    m68k_set_reg(M68K_REG_A6, loaded.data_start + 0x8000);
    m68k_set_reg(M68K_REG_A7, top_of_stack);

    m68k_set_reg(M68K_REG_D0, newpid);
    m68k_set_reg(M68K_REG_D1, group_user);
    m68k_set_reg(M68K_REG_D2, priority);
    m68k_set_reg(M68K_REG_D3, inherited_paths);
    m68k_set_reg(M68K_REG_D5, args_size);
    m68k_set_reg(M68K_REG_D6, data_size);

    while (true) {
        auto cycles = 100000;
        m68k_execute(cycles);
    }

    return 0;
}
