cmake_minimum_required(VERSION 3.1...3.29)

project(ae-test LANGUAGES C CXX)

include_directories(ae-test
    src/musashi
)

add_executable(ae-test 
    src/musashi/m68kconf.h
    src/musashi/m68k.h
    src/musashi/m68kmmu.h
    src/musashi/m68kcpu.c
    src/musashi/m68kops.c
    src/musashi/m68kdasm.c
    src/musashi/softfloat/softfloat.h
    src/musashi/softfloat/softfloat.c

    src/main.cpp
)

set_target_properties(ae-test PROPERTIES XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED NO)
