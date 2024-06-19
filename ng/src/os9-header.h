#pragma once

#include "be-reader.h"

enum class module_type: uint8_t {
    any = 0,
    program = 1,
    subrout = 2,
    multi = 3,
    data = 4,
    csddata = 5,
    traplib = 11,
    system = 12,
    fileman = 13,
    devdrvr = 14,
    devdesc = 15,
};

enum class language_type: uint8_t {
    any      = 0,
    object   = 1,
    icode    = 2,
    pcode    = 3,
    ccode    = 4,
    cblcode  = 5,
    frtncode = 6,
    javacode = 7,
};

const char *module_type_string(module_type type)
{
    switch (type) {
        case module_type::any:     return "any";
        case module_type::program: return "program";
        case module_type::subrout: return "subrout";
        case module_type::multi:   return "multi";
        case module_type::data:    return "data";
        case module_type::csddata: return "csddata";
        case module_type::traplib: return "traplib";
        case module_type::system:  return "system";
        case module_type::fileman: return "fileman";
        case module_type::devdrvr: return "devdrvr";
        case module_type::devdesc: return "devdesc";
    }

    return "";
}

const char *language_type_string(language_type type)
{
    switch (type) {
        case language_type::any:      return "any";
        case language_type::object:   return "object";
        case language_type::icode:    return "icode";
        case language_type::pcode:    return "pcode";
        case language_type::ccode:    return "ccode";
        case language_type::cblcode:  return "cblcode";
        case language_type::frtncode: return "frtncode";
        case language_type::javacode: return "javacode";
    }

    return "";
}

struct os9_header {
    uint16_t      magic;
    uint16_t      sysrev;
    uint32_t      size;
    uint32_t      owner;
    uint32_t      names;
    uint16_t      accs;
    module_type   type;
    language_type lang;
    uint8_t       attr;
    uint8_t       revs;
    uint16_t      edit;
    uint32_t      usage;
    uint32_t      symbol;
    uint16_t      ident;
    uint32_t      exts_off;
    uint16_t      exts_size;
    uint16_t      parity;

    uint32_t      exec;
    uint32_t      excpt;
    uint32_t      mem;
    uint32_t      stack;
    uint32_t      idata;
    uint32_t      irefs;
    uint32_t      init;
    uint32_t      term;

    os9_header(std::vector<uint8_t> &data) {
        auto reader = be_reader(data);

        magic  = reader.read_16();
        sysrev = reader.read_16();
        size   = reader.read_32();
        owner  = reader.read_32();
        names  = reader.read_32();
        accs   = reader.read_16();
        type   = module_type(reader.read_8());
        lang   = language_type(reader.read_8());
        attr   = reader.read_8();
        revs   = reader.read_8();
        edit   = reader.read_16();
        usage  = reader.read_32();
        symbol = reader.read_32();
        ident  = reader.read_16();

        for (int i = 0; i < 6; i++)
            reader.read_8();

        exts_off  = reader.read_32();
        exts_size = reader.read_16();
        parity    = reader.read_16();

        exec  = 0xcafebabe;
        excpt = 0xcafebabe;
        mem   = 0xcafebabe;
        stack = 0xcafebabe;
        idata = 0xcafebabe;
        irefs = 0xcafebabe;
        init  = 0xcafebabe;
        term  = 0xcafebabe;

        if (type == module_type::fileman ||
            type == module_type::system  ||
            type == module_type::devdrvr ||
            type == module_type::program ||
            type == module_type::traplib)
        {
            exec = reader.read_32();
            excpt = reader.read_32();
        }

        if (type == module_type::devdrvr ||
            type == module_type::program ||
            type == module_type::traplib)
        {
            mem = reader.read_32();
        }

        if (type == module_type::program ||
            type == module_type::traplib)
        {
            stack = reader.read_32();
            idata = reader.read_32();
            irefs = reader.read_32();
        }

        if (type == module_type::traplib) {
            init = reader.read_32();
            term = reader.read_32();
        }
    }

    void dump() {
        printf("magic:     %04x\n", magic);
        printf("sysrev:    %04x\n", sysrev);
        printf("size:      %08x\n", size);
        printf("owner:     %08x\n", owner);
        printf("names:     %08x\n", names);
        printf("accs:      %04x\n", accs);
        printf("type:      %02hhx -- %s\n", type, module_type_string(type));
        printf("lang:      %02hhx -- %s\n", lang, language_type_string(lang));
        printf("attr:      %02x\n", attr);
        printf("revs:      %02x\n", revs);
        printf("edit:      %04x\n", edit);
        printf("usage:     %08x\n", usage);
        printf("symbol:    %08x\n", symbol);
        printf("ident:     %04x\n", ident);
        printf("exts_off:  %08x\n", exts_off);
        printf("exts_size: %04x\n", exts_size);
        printf("parity:    %04x\n", parity);
        printf(" - \n");
        printf("exec:      %08x\n", exec);
        printf("excpt:     %08x\n", excpt);
        printf("mem:       %08x\n", mem);
        printf("stack:     %08x\n", stack);
        printf("idata:     %08x\n", idata);
        printf("irefs:     %08x\n", irefs);
        printf("init:      %08x\n", init);
        printf("term:      %08x\n", term);
    }

};
