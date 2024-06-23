#pragma once

#include <stdint.h>

/* register context:
   !!!!!!!! must be STRUCTURALLY EQUAL to regs_type in os9_uae.h */
/* register context:
   !!!!!!!! must be STRUCTURALLY EQUAL to regstruct in newcpu.h */
/* wil: unify those, and spoiler, they weren't.
   the `flags` field was only in regs_type, which was in os9_ll.h
   and not in os9_uae.h. */
typedef struct {
    uint32_t regs[16];

    uint32_t usp;
    uint32_t isp;
    uint32_t msp;

    uint16_t sr;
    uint16_t flags; /* wil: not in regstruct (?) */

    uint8_t t1;
    uint8_t t0;
    uint8_t s;
    uint8_t m;
    uint8_t x;
    uint8_t stopped;
    uint32_t intmask;

    uint32_t pc;
    uint8_t *pc_p;
    uint8_t *pc_oldp;

    uint32_t vbr;
    uint32_t sfc;
    uint32_t dfc;

    double fp[8];
    uint32_t fpcr;
    uint32_t fpsr;
    uint32_t fpiar;

    /* Fellow sources say this is 4 longwords. That's impossible. It needs
     * to be at least a longword. The HRM has some cryptic comment about two
     * instructions being on the same longword boundary.
     * The way this is implemented now seems like a good compromise.
     */
    uint32_t prefetch;

    /* --- link to traphandler entry of TRAP #1 */
    struct traphandler_typ *ttP;

    /* --- base address of process' static storage */
    void *membase; /* used to fool StackSniffer via modified ApplLimit */
} regs_type;

inline void regs_set_d(regs_type *r, int index, uint32_t value)
{
    r->regs[index] = value;
}

inline void regs_set_a(regs_type *r, int index, uint32_t value)
{
    r->regs[index + 8] = value;
}

inline uint32_t regs_get_d(regs_type *r, int index)
{
    return r->regs[index];
}

inline uint32_t regs_get_a(regs_type *r, int index)
{
    return r->regs[index + 8];
}

