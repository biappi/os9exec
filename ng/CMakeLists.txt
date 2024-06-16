cmake_minimum_required(VERSION 3.1...3.29)

project(ae-test LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories(ae-test
    src/musashi
)

add_library(musashi STATIC
    src/musashi/m68kconf.h
    src/musashi/m68k.h
    src/musashi/m68kmmu.h
    src/musashi/m68kcpu.c
    src/musashi/m68kops.c
    src/musashi/m68kdasm.c
    src/musashi/softfloat/softfloat.h
    src/musashi/softfloat/softfloat.c
)

target_compile_definitions(musashi PRIVATE MUSASHI_CNF="../musashi-conf.h")

add_executable(ae-test
    src/musashi-conf.h
    src/be-reader.h
    src/os9-header.h
    src/address-space.h
    src/address-space.cpp
    src/main.cpp
)

target_link_libraries(ae-test musashi)

set_target_properties(ae-test PROPERTIES XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED NO)
