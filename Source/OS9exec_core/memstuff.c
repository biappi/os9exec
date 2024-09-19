//
//    OS9exec,   OS-9 emulator for Mac OS, Windows and Linux
//    Copyright (C) 2002 Lukas Zeller / Beat Forster
//	  Available under http://www.synthesis.ch/os9exec
//
//    This program is free software; you can redistribute it and/or
//    modify it under the terms of the GNU General Public License as
//    published by the Free Software Foundation; either version 2 of
//    the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//    See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

/**********************************************/
/*             O S 9 E x e c / NT             */
/*  Cooperative-Multiprocess OS-9 emulation   */
/*         for Apple Macintosh and PC         */
/*                                            */
/* (c) 1993-2007 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        bfo@synthesis.ch                    */
/**********************************************/

/*
 *  CVS:
 *    $Author$
 *    $Date$
 *    $Revision$
 *    $Source$
 *    $State$
 *    $Name$ (Tag)
 *    $Locker$ (who has reserved checkout)
 *  Log:
 *    $Log$
 *    Revision 1.26  2007/01/07 14:01:04  bfo
 *    Fully rearranged
 *    No longer used as import file for PtoC system
 *    Parts moved to "c_access"
 *    New <pmem> structure introduced
 *
 *    Revision 1.25  2007/01/04 20:20:41  bfo
 *    Boolean return type added for 'IsZeroR' and 'IsZeroI'
 *
 *    Revision 1.24  2007/01/02 11:29:38  bfo
 *    Allow to release combined memory blocks (e.g. for "dir")
 *
 *    Revision 1.23  2006/08/06 22:42:43  bfo
 *    empty string causes <convErr>
 *
 *    Revision 1.22  2006/08/04 18:41:40  bfo
 *    MEM_SHIELD test environment added
 *
 *    Revision 1.21  2006/07/21 07:25:50  bfo
 *    UIntToStrN: Keep spaces for the remaining part of > 8
 *
 *    Revision 1.20  2006/07/14 11:43:30  bfo
 *    STRANGE/UNUSED BLOCK: Adapt it for easier debugging
 *
 *    Revision 1.19  2006/06/13 22:23:05  bfo
 *    StrToInt/StrToShort added
 *
 *    Revision 1.18  2006/03/12 19:30:24  bfo
 *    RealToStrN added
 *
 *    Revision 1.17  2006/02/19 16:33:21  bfo
 *    Some PtoC routines are implemented here now
 *
 *    Revision 1.16  2005/06/30 11:48:07  bfo
 *    Mach-O support
 *
 *    Revision 1.15  2004/11/20 11:44:07  bfo
 *    Changed to version V3.25 (titles adapted)
 *
 *    Revision 1.14  2004/09/15 19:57:13  bfo
 *    Up to date
 *
 *    Revision 1.13  2003/05/25 10:35:20  bfo
 *    <totalMem> can't be any more smaller than <memsz>
 *
 *    Revision 1.12  2003/05/21 20:32:01  bfo
 *    Allocate 512k blocks / Additional parameter <mem_fulldisp> for "show_mem"
 *
 *    Revision 1.11  2003/05/17 10:41:39  bfo
 *    'show_mem' with <mem_unused> parameter /
 *    better deallocation strategy for the unused blocks
 *
 *    Revision 1.10  2002/10/28 22:19:21  bfo
 *    slightly different checkpoint:  STRANGE/UNUSED
 *
 *    Revision 1.9  2002/10/27 23:52:28  bfo
 *    REUSE_MEM handling introduced
 *
 *    Revision 1.8  2002/10/15 18:24:19  bfo
 *    memtable handling prepared
 *
 *    Revision 1.7  2002/09/01 20:15:07  bfo
 *    some debug messages reduced
 *
 *    Revision 1.6  2002/08/13 21:24:17  bfo
 *    Some more variables defined at the real procid struct now.
 *
 *    Revision 1.5  2002/08/09 22:39:20  bfo
 *    New procedure set_os9_state introduced and adapted everywhere
 *
 *
 */

#include "os9exec_incl.h"
#include <inttypes.h>

/* Memory management for OS9 processes */
/* =================================== */

// the mem alloc table
memblock_typ memtable[MAX_MEMALLOC];
pmem_typ     pmem[MAXPROCESSES];

const os9ptr memory_top = 0x00010000;

addrpair_typ allocation_add_absolute(void *host, os9ptr guest, uint32_t size);

/* prepare the memory handling for use */
void init_all_mem(void)
{
    totalMem = 0; /* initialize startup memory */

    for (int k = 0; k < MAX_MEMALLOC; k++) {
        memtable[k].base.host  = NULL;
        memtable[k].base.guest = 0;
        memtable[k].size       = 0;
    }

    // stupid hack to make fcontrol run somehow
    // since it writes on absolute $4afc0000
    void *buffer  = calloc(0x10000, 1); /* get memory block, cleared to 0 */
    allocation_add_absolute(buffer, 0x04fc0000, 0x10000);
}

/* show memory blocks */
void show_mem(ushort npid, Boolean mem_unused, Boolean mem_fulldisp)
{
    memblock_typ *m;
    int           k, pid;


    if (mem_unused) {
        if (mem_fulldisp) {
            upo_printf("  #     start       end      size   to next\n");
            upo_printf("---  --------  --------  --------  --------\n");


            return;
        }

        upo_printf("   size     #      total\n");
        upo_printf("-------  ----  ---------\n");


        return;
    }

    upo_printf("Pid  Block   Start      Size       End (+1)\n");
    upo_printf("---  -----  ---------  ---------  ---------\n");

    for (pid = 0; pid < MAXPROCESSES; pid++) {
        if (procs[pid].state != pUnused) {
            if (npid == MAXPROCESSES || npid == pid) {
                for (k = 0; k < MAXMEMBLOCKS; k++) {
                    //  m= &procs[pid].os9memblocks[k];
                    m = &pmem[pid].m[k];
                    if (m->base.host != NULL) {
                        upo_printf("%2d%c  %5d  $%08lX  $%08lX  $%08lX - %p\n",
                                   pid,
                                   pid == currentpid ? '*' : ' ',
                                   k,
                                   m->base.guest,
                                   m->size,
                                   m->base.guest + m->size,
                                   m->base.host);
                    }
                }
            }
        }
    }
}

void show_unused(void) {}

/* initialize process' memory block list */
void init_mem(ushort pid)
{
    for (int k = 0; k < MAXMEMBLOCKS; k++) {
        // no memory yet
        addrpair_typ *addr = &pmem[pid].m[k].base;
        addr->host         = NULL;
        addr->guest        = 0;
    }
}

/* the currently max available memory */
uint32_t max_mem() { return 0; }

// --------------------------------------------------------------------------------------
// install a memory area as memory block
static ushort install_memblock(ushort pid, addrpair_typ base, uint32_t size)
{
    for (int k = 0; k < MAXMEMBLOCKS; k++) {
        memblock_typ *m = &pmem[pid].m[k];
        if (m->base.host == NULL) {
            m->base = base; // save pointer
            m->size = size; // save block size
            return k;
        }
    }

    return MAXMEMBLOCKS;
}

void release_mem(addrpair_typ membase)
/* process independent part of memory deallocation */
{
    ulong memsz = 1;

    for (int k = 0; k < MAX_MEMALLOC; k++) {
        memblock_typ *m = &memtable[k];
        if (m->base.host == membase.host) {
            memsz = m->size; /* search for a segment to be released */

            totalMem -= memsz;

            m->base.host  = NULL; /* and release the segment */
            m->base.guest = 0;
            m->size       = 0;
            break;
        }
    }

    debugprintf(dbgMemory,
                dbgNorm,
                ("# release_mem: release block     at $%08lX (size=%5u) %8d\n",
                 membase,
                 memsz,
                 totalMem));

    free(membase.host);
}

/* free an allocated memory block */
static void release_memblock(ushort pid, ushort memblocknum)
{
    pmem_typ     *cm = &pmem[pid];
    memblock_typ *m  = &cm->m[memblocknum];

    if (m->base.host == NULL)
        return;

    debugprintf(
        dbgMemory,
        dbgNorm,
        ("# release_memblock:    block #%-2d at $%08lX (size=%5u) %8d pid=%d\n",
         memblocknum,
         m->base,
         m->size,
         totalMem,
         pid));

    release_mem(m->base);

    m->base.host  = NULL; // free block
    m->base.guest = 0;
    m->size       = 0;
}

/* free process' allocated memory blocks */
void free_mem(ushort pid)
{
    int k;
    for (k = 0; k < MAXMEMBLOCKS; k++)
        release_memblock(pid, k);
}

os9ptr TOP_OF_RAM = 0xface0000;

#define allocation_data_capacity 0xff

struct {
    addrpair_typ addr;
    uint32_t size;
} allocation_data[allocation_data_capacity];

int allocation_data_count;

addrpair_typ allocation_add_absolute(void *host, os9ptr guest, uint32_t size)
{
    addrpair_typ addr;
    addr.host = host;
    addr.guest = guest;

    allocation_data[allocation_data_count].addr = addr;
    allocation_data[allocation_data_count].size = size;
    allocation_data_count++;

    return addr;
}

addrpair_typ allocation_add(void *host, uint32_t size)
{
    addrpair_typ addr = allocation_add_absolute(host, TOP_OF_RAM, size);
    TOP_OF_RAM += size;
    return addr;
}

void allocation_print()
{
    printf("\nallocations data (%d)\n\n", allocation_data_count);
    printf("guest       host                size\n");
    printf("-------------------------------------\n");

    for (int i = 0; i < allocation_data_count; i++) {
        printf("%08x    %0" PRIxPTR "    %8x\n",
               allocation_data[i].addr.guest,
               (intptr_t)allocation_data[i].addr.host,
               allocation_data[i].size);
    }
}

void* allocation_find(os9ptr addr)
{
    for (int i = 0; i < allocation_data_count; i++) {
        if ((allocation_data[i].addr.guest <= addr) &&
            (addr < (allocation_data[i].addr.guest + allocation_data[i].size)))
        {
            return allocation_data[i].addr.host + (addr - allocation_data[i].addr.guest);
        }
    }

    printf("AIEE -- can't find allocation for guest ptr %08x\n", addr);
    allocation_print();

    _Exit(-1);
    return NULL;
}

void allocation_debug(regs_type *rp, uint32_t regmask)
{
    regmask &= REGMASK_REGBITS;
    for (int k = 0; k < 8; k++) {
        uint32_t a = rp->regs[REGS_A + k];
        uint32_t d = rp->regs[REGS_D + k];

        for (int i = 0; i < allocation_data_count; i++) {
            uint32_t truncated =
                (uint32_t)((size_t)allocation_data[i].addr.host & 0xffffffff);

            if ((truncated <= a) &&
                (a < truncated + allocation_data[i].size))
            {
                printf("warning!! register A%d (%08x) seems to contain a truncated host pointer (%p) instead of guest (%08x)\n", i, a, allocation_data[i].addr.host, allocation_data[i].addr.guest);
            }

            if ((truncated <= d) &&
                (d < truncated + allocation_data[i].size))
            {
                printf("warning!! register D%d (%08x) seems to contain a truncated host pointer (%p) instead of guest (%08x)\n", i, d, allocation_data[i].addr.host, allocation_data[i].addr.guest);
            }

        }

        regmask >>= 2;
    }
}


/* process independent part of memory allocation */
addrpair_typ get_mem(uint32_t memsz)
{
#define MBlk 64
    /* round up to next boundary */
    uint32_t sz = memsz = (memsz + MBlk - 1) & 0xFFFFFFC0;

    addrpair_typ addr;
    addr.host  = calloc(sz, 1); /* get memory block, cleared to 0 */
    addr.guest = TOP_OF_RAM;
    TOP_OF_RAM += sz;

    if (allocation_data_count == allocation_data_capacity) {
        printf("AIEE -- ran out of allocation data entries\n");
        exit(-1);
    }

    allocation_data[allocation_data_count].addr = addr;
    allocation_data[allocation_data_count].size = sz;
    allocation_data_count++;
    
    if (addr.host != NULL) {
        for (int k = 0; k < MAX_MEMALLOC; k++) {
            if (memtable[k].base.host == NULL) { /* search for a free segment */
                totalMem += memsz;

                memtable[k].base = addr;
                memtable[k].size = memsz;

                debugprintf(dbgMemory,
                            dbgNorm,
                            ("# get_mem:    allocate block     at (os9: $%08X "
                             "host: %p) "
                             "(size=%5u) %8d\n",
                             addr.guest,
                             addr.host,
                             memsz,
                             totalMem));
                return addr;
            }
        }
    }

    //#ifndef PLUGIN_DLL
    upe_printf("No more memory !!!\n");
    //#endif

    return addr;
}

/* memory allocation for OS-9 */
addrpair_typ os9malloc(ushort pid, uint32_t memsz)
{
    addrpair_typ pp = get_mem(memsz);
    if (pp.host == NULL)
        return pp; /* no memory */

    int k = install_memblock(pid, pp, memsz);
    if (k >= MAXMEMBLOCKS) {
        /* memory block list is full */
        release_mem(pp);
        pp.host  = NULL;
        pp.guest = 0;

        //#ifndef PLUGIN_DLL
        upe_printf("No more memory (MAXMEMBLOCKS) !!!\n");
        //#endif
    }

    debugprintf(dbgMemory,
                dbgNorm,
                ("# os9malloc:  allocate block #%-2d at (os9: $%08X host: %p) "
                 "(size=%5u) %8d pid=%d\n",
                 k,
                 pp.guest,
                 pp.host,
                 memsz,
                 totalMem,
                 pid));
    return pp;
}

/* memory deallocation for OS-9 */
os9err os9free(ushort pid, os9ptr membase, uint32_t memsz)
{
    os9err    err;
    pmem_typ *cm = &pmem[pid];

    debugprintf(dbgMemory,
                dbgDetail,
                ("# os9free:      free request   at $%08lX (size=%lu) pid=%d\n",
                 (ulong)membase,
                 memsz,
                 pid));

    if (membase != 0) {
        for (int k = 0; k < MAXMEMBLOCKS; k++) {
            memblock_typ *m = &cm->m[k];
            if (m->base.guest == membase) {
                if (m->size == memsz) {
                    release_memblock(pid, k);
                    return 0; /* freed ok */
                }

                // try to free it in smaller pieces ...
                // NOTE: e.g. OS-9 "dir" is doing it this way !
                if (memsz > m->size) { // recursive call
                    err = os9free(pid, membase + m->size, memsz - m->size);
                    if (!err) {
                        release_memblock(
                            pid,
                            k);   // only release it, if all of them are fitting
                        return 0; /* freed ok */
                    }

                    debugprintf(
                        dbgMemory,
                        dbgNorm,
                        ("# os9free:     release block #%-2d at $%08lX, but "
                         "wrong size=%lu (specified=%4u) %8d\n",
                         k,
                         membase,
                         m->size,
                         memsz,
                         totalMem));
                    break;
                }

                return 0; /* freed ok */
            }
        }
    }

    // upe_printf( "bad block %08X size=%d\n", membase, memsz );
    debugprintf(dbgMemory + dbgAnomaly,
                dbgNorm,
                ("# os9free: Block at $%08lX (size=%lu) not found in pid=%d's "
                 "memory list\n",
                 membase,
                 memsz,
                 pid));
    return os9error(E_BPADDR); /* no memory was allocated here */
}

/* eof */
