cmake_minimum_required(VERSION 3.1...3.29)

project(os9exec VERSION 3.39 LANGUAGES C)

include_directories(
    Source/OS9exec_core
    Source/OS9exec_core/os9defs
    Source/OS9AppEmu/TARGET_STD
    Source/OS9AppEmu/UAE68emulator
    Source/OS9AppEmu
    Source/Platforms/LINUX
    Source/Platforms
    # Source/TARGET_STD
)

add_executable(os9exec 
    Source/OS9App/os9app.c
    Source/OS9exec_core/alarms.c
    Source/OS9exec_core/c_access.c
    Source/OS9exec_core/consio.c
    Source/OS9exec_core/debug.c
    Source/OS9exec_core/events.c
    Source/OS9exec_core/fcalls.c
    Source/OS9exec_core/fileaccess.c
    Source/OS9exec_core/filestuff.c
    Source/OS9exec_core/file_rbf.c
    Source/OS9exec_core/filescsi.c
    Source/OS9exec_core/filters.c
    Source/OS9exec_core/funcdispatch.c
    Source/OS9exec_core/icalls.c
    Source/OS9exec_core/intcommand.c
    Source/OS9exec_core/int_dir.c
    Source/OS9exec_core/int_move.c
    Source/OS9exec_core/int_rename.c
    Source/OS9exec_core/memstuff.c
    Source/OS9exec_core/modstuff.c
    Source/OS9exec_core/network.c
    Source/OS9exec_core/os9exec_nt.c
    Source/OS9exec_core/os9main.c
    Source/OS9exec_core/pipefiles.c
    Source/OS9exec_core/printer.c
    Source/OS9exec_core/procstuff.c
    Source/OS9exec_core/telnetaccess.c
    Source/OS9exec_core/utilstuff.c
    Source/OS9exec_core/vmod.c
    Source/OS9AppEmu/os9_uae.c
    Source/OS9AppEmu/luzstuff.c
    Source/OS9AppEmu/UAE68emulator/cpudefs.c
    Source/OS9AppEmu/UAE68emulator/cpustbl.c
    Source/OS9AppEmu/UAE68emulator/newcpu.c
    Source/OS9AppEmu/UAE68emulator/readcpu.c
    Source/OS9AppEmu/UAE68emulator/support.c
    Source/OS9AppEmu/UAE68emulator/cpuemu.c
    Source/OS9AppEmu/UAE68emulator/fpp.c
    Source/Platforms/LINUX/linuxfiles.c
    Source/Platforms/LINUX/net_linux.c

    Source/OS9AppEmu/UAE68emulator/compiler.h
    Source/OS9AppEmu/UAE68emulator/config.h
    Source/OS9AppEmu/UAE68emulator/cputbl.h
    Source/OS9AppEmu/UAE68emulator/m68k.h
    Source/OS9AppEmu/UAE68emulator/memory.h
    Source/OS9AppEmu/UAE68emulator/newcpu.h
    Source/OS9AppEmu/UAE68emulator/options.h
    Source/OS9AppEmu/UAE68emulator/readcpu.h
    Source/OS9AppEmu/UAE68emulator/sysdeps.h
    Source/OS9AppEmu/luzstuff.h
    Source/OS9exec_core/c_access.h
    Source/OS9exec_core/filescsi.h
    Source/OS9exec_core/os9defs/errno_adapted.h 
    Source/OS9exec_core/os9defs/module.h
    Source/OS9exec_core/os9defs/procid_adapted.h
    Source/OS9exec_core/os9defs/sgstat.h
    Source/OS9exec_core/os9defs/types.h
    Source/OS9exec_core/os9exec_incl.h
    Source/Platforms/LINUX/linuxfiles.h
    Source/Platforms/net_platform.h
    Source/TermConsole/serialaccess.h
)

target_compile_definitions(os9exec PUBLIC linux)
target_link_libraries(os9exec m dl)

set_target_properties(os9exec PROPERTIES XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED NO)
