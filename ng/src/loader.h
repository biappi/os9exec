#pragma once

struct loader {
    const uint32_t data_start   = 0x1000;
    const uint32_t params_len   = 0x0100;
    const uint32_t module_align = 0x1000;

    os9_header header;
    std::string name;

    uint32_t stack_start;
    uint32_t param_start;
    uint32_t param_end;
    uint32_t module_start;

    loader(std::vector<uint8_t> data, address_space &space)
        : header       (os9_header(data))
        , name         ((char *)&data[header.names])
        , stack_start  (data_start + header.mem)
        , param_start  (stack_start + header.stack)
        , param_end    (param_start + params_len)
        , module_start (((param_end / module_align) + 1) * module_align)
    {
        assert(header.stack != 0xcafebabe);

        space.add_zone("data",   data_start,   header.mem);
        space.add_zone("stack",  stack_start,  header.stack);
        space.add_zone("params", param_start,  params_len);
        space.add_zone("code",   module_start, data);
    }

    void dump() {
        printf("name:         %s\n",   name.c_str());
        printf("stack_start:  %08x\n", stack_start);
        printf("param_start:  %08x\n", param_start);
        printf("param_end:    %08x\n", param_end);
        printf("module_start: %08x\n", module_start);
    }

    uint32_t exec()  { return module_start + header.exec;  }
    uint32_t excpt() { return module_start + header.excpt; }
};

void dump_relocations(std::vector<uint8_t> &data, const os9_header &header) {
    auto r = be_reader(data);
    r.reset(header.irefs);

    printf(" - code refs\n");

    while (1) {
        auto msw    = r.read_16();
        auto n_lsw  = r.read_16();

        printf("msw:    %04x\n", msw);
        printf("n_lsw:  %04x\n", n_lsw);

        if (msw == 0 && n_lsw == 0) {
            break;
        }

        for (int i = 0; i < n_lsw; i++) {
            printf("  %03d:  %04x\n", i, r.read_16());
        }
    }

    printf(" - data refs\n");

    while (1) {
        auto msw    = r.read_16();
        auto n_lsw  = r.read_16();

        printf("msw:    %04x\n", msw);
        printf("n_lsw:  %04x\n", n_lsw);

        if (msw == 0 && n_lsw == 0) {
            break;
        }

        for (int i = 0; i < n_lsw; i++) {
            printf("  %03d:  %04x\n", i, r.read_16());
        }
    }
}
