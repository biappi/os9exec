/*
 * UAE - The Un*x Amiga Emulator
 *
 * memory management
 *
 * Copyright 1995 Bernd Schmidt
 */

#pragma once

#include <stdint.h>

typedef uint32_t os9ptr;

typedef struct {
    void  *host;
    os9ptr guest;
} addrpair_typ;

#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wint-to-pointer-cast"

void* allocation_find(os9ptr addr);

static __inline__ void *get_pointer(os9ptr ptr) {
    return allocation_find(ptr);
}

static __inline__ uint32_t get_long(os9ptr addr)
{
    uint8_t *a = (uint8_t *)get_pointer(addr);
    uint8_t *b = (uint8_t *)a;

    return (*b << 24) | (*(b + 1) << 16) | (*(b + 2) << 8) | (*(b + 3));
}

static __inline__ uint32_t get_word(os9ptr addr)
{
    uint8_t *a = (uint8_t *)get_pointer(addr);
    uint8_t *b = (uint8_t *)a;

    return (*b << 8) | (*(b+1));
}

static __inline__ uint32_t get_byte(os9ptr addr)
{
    uint8_t *a = (uint8_t *)get_pointer(addr);
    return *a;
}

static __inline__ void put_long(os9ptr addr, uint32_t v)
{
    uint8_t *a = (uint8_t *)get_pointer(addr);
    uint8_t *b = (uint8_t *)a;

    *b = v >> 24;
    *(b+1) = v >> 16;    
    *(b+2) = v >> 8;
    *(b+3) = v;
}

static __inline__ void put_word(os9ptr addr, uint32_t v)
{
    uint8_t *a = (uint8_t *)get_pointer(addr);
    uint8_t *b = (uint8_t *)a;

    *b = v >> 8;
    *(b + 1) = (uint8_t)v;
}

static __inline__ void put_byte(os9ptr addr, uint32_t v)
{
    uint8_t *a = (uint8_t *)get_pointer(addr);
    *a = v;
}

#pragma clang diagnostic pop
