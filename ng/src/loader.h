#pragma once

void read_relocations(be_reader &r, std::vector<uint32_t> &rels);

struct loader {
    const uint32_t data_start   = 0x1000;
    const uint32_t params_len   = 0x0100;
    const uint32_t module_align = 0x1000;

    os9_header  header;
    std::string name;

    std::vector<uint32_t> code_rel;
    std::vector<uint32_t> data_rel;

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
        assert(header.mem   != 0xcafebabe);
        assert(header.stack != 0xcafebabe);
        assert(header.idata != 0xcafebabe);

        space.add_zone("data",   data_start,   header.mem);
        space.add_zone("stack",  stack_start,  header.stack);
        space.add_zone("params", param_start,  params_len);
        space.add_zone("code",   module_start, data);

        {
            auto r = be_reader(data, header.idata);
            auto idata_dst = r.read_32();
            auto idata_len = r.read_32();

            for (int i = 0; i < idata_len; i++) {
                space.write8(data_start + idata_dst + i, r.read_8());
            }
        }

        {
            auto r = be_reader(data, header.irefs);
            read_relocations(r, code_rel);
            read_relocations(r, data_rel);

            printf("code relocs\n");
            for (auto addr : code_rel) {
                auto address  = data_start + addr;
                auto original = space.read32(address);
                auto rebased  = module_start + original;
                space.write32(address, rebased);
                auto control = space.read32(address);
                printf("    %08x: was %08x is %08x (%08x) \n", address, original, rebased, control);
            }

            printf("data relocs\n");
            for (auto addr : data_rel) {
                auto address  = data_start + addr;
                auto original = space.read32(address);
                auto rebased  = data_start + original;
                space.write32(address, rebased);
                auto control = space.read32(address);
                printf("    %08x: was %08x is %08x (%08x) \n", address, original, rebased, control);
            }
        }
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

void read_relocations(be_reader &r, std::vector<uint32_t> &rels) {
    while (1) {
        uint32_t msw    = r.read_16() << 16;
        uint32_t n_lsw  = r.read_16();

        if (msw == 0 && n_lsw == 0) {
            return;
        }

        for (int i = 0; i < n_lsw; i++) {
            rels.push_back(msw + r.read_16());
        }
    }
}
