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
/* (c) 1993-2002 by Lukas Zeller, CH-Zuerich  */
/*                  Beat Forster, CH-Maur     */
/*                                            */
/* email: luz@synthesis.ch                    */
/*        forsterb@dial.eunet.ch              */
/**********************************************/

#include "os9exec_incl.h"

/* Memory management for OS9 processes */
/* =================================== */


/* show memory blocks */
void show_mem(ushort npid)
{
    memblock_typ *m;
    int k,pid;
    
    #ifdef macintosh
    upo_printf("Macintosh Heap: MemFree=%ld, MaxBlock=%ld\n\n",FreeMem(),MaxBlock());
    #endif
    
    upo_printf("Pid  Block   Start      Size       End (+1)\n");
    upo_printf("---  -----  ---------  ---------  ---------\n");
    
    for (pid=0; pid<MAXPROCESSES; pid++) {
        if (procs[pid].state!=pUnused) {
            if (npid==MAXPROCESSES || npid==pid) {
                for (k=0; k<MAXMEMBLOCKS; k++) {
                        m= &procs[pid].os9memblocks[k];
                    if (m->base!=NULL) {
                        upo_printf("%c%02d  %05d  $%08lX  $%08lX  $%08lX\n",
                                    pid==currentpid ? '*' : ' ',
                                    pid,
                                    k,      m->base,
                                                      m->size,
                                    (ulong) m->base + m->size);
                    }
                }
            }
        }
    }
} /* show_mem */
    

/* initialize process' memory block list */
void init_mem(ushort pid)
{
   int k;
   for(k=0; k<MAXMEMBLOCKS; k++) procs[pid].os9memblocks[k].base=NULL; /* no memory yet */
} /* init_mem */


/* install a memory area as memory block */
ushort install_memblock(ushort pid, void *base, ulong size)
{
    memblock_typ *m;
    int k;
    
    for(k=0;k<MAXMEMBLOCKS;k++) {
        m= &procs[pid].os9memblocks[k];
        if (m->base==NULL) {
            m->base=base;            /* save pointer */
            m->size=size;            /* save block size */
            LockMemRange(base,size); /* keep always paged in !! */ 
            return k;
        }
    }
   
    return MAXMEMBLOCKS;
} /* install_memblock */


void release_mem( void* membase, Boolean mac_asHandle )
/* process independent part of memory deallocation */
{
    #ifdef MACMEM
      if (mac_asHandle) DisposeHandle(membase);
      else              DisposePtr   (membase);

      if (MemError()!=noErr) {
          debugprintf(dbgMemory+dbgAnomaly,dbgDeep,
                     ("# release_memblock: DisposePtr returned Mac OS9 MemError=%d\n",MemError()));
      }
    #else
      #ifndef linux
      #pragma unused(mac_asHandle)
      #endif
      
      free(membase);
    #endif
} /* release_mem */



/* free an allocated memory block */
void release_memblock(ushort pid, ushort memblocknum)
{
    memblock_typ *m;
    
        m= &procs[pid].os9memblocks[memblocknum];
    if (m->base!=NULL) {
        debugprintf(dbgMemory,dbgNorm,
              ("# release_memblock: block #%d @ $%lX size=$%lX\n",
              memblocknum,m->base, m->size ));
        UnlockMemRange  ( m->base, m->size );
        release_mem(      m->base, false );
                          m->base=NULL; /* free block */
   }
} /* release_memblock */



/* free process' allocated memory blocks */
void free_mem(ushort pid)
{
   int k;
   for(k=0; k<MAXMEMBLOCKS; k++) release_memblock(pid,k);   
} /* free_mem */


ulong max_mem()
/* the currently max available memory */
{
    ulong memsz;
    
    #ifdef macintosh
      memsz= MaxBlock();
    #elif defined win_linux
      memsz= 0x00800000; /* 8 megs is MUUUUCH for an OS-9 system */
    #else
      #error MaxBlock size must be defined here
    #endif
    
    return memsz;
} /* max_mem */



void *get_mem( ulong memsz, Boolean mac_asHandle )
/* process independent part of memory allocation */
{
    void *pp;
    
    #ifdef MACMEM
      char *n;
      
      if (mac_asHandle) { pp= NewHandle  (memsz); n= "NewHandle";   }
      else              { pp= NewPtrClear(memsz); n= "NewPtrClear"; }
      
      if (pp==NULL) {
        debugprintf(dbgMemory,dbgNorm,("# get_mem: %s returned MacOS MemError=%d\n",
                                        n, MemError()));
        upe_printf( "No more memory !!!\n" );
      }
    #else
      #ifndef linux
      #pragma unused(mac_asHandle)
      #endif
      
      pp= calloc((size_t)memsz, (size_t)1); /* get memory block, cleared to 0 */
    #endif
    
    return pp;
} /* get_mem */



/* memory allocation for OS-9 */
void *os9malloc(ushort pid, ulong memsz)
{
    void *pp;
    int   k;

        pp= get_mem(memsz,false);
    if (pp==NULL) return NULL; /* no memory */
     
        k= install_memblock( pid,pp,memsz );
    if (k>=MAXMEMBLOCKS) {
    /* memory block list is full */
        #ifdef MACMEM
          DisposePtr(pp);
          if (MemError()!=noErr) {
            debugprintf(dbgMemory+dbgAnomaly,dbgDeep,
               ("# os9malloc: DisposePtr returned Mac OS9 MemError=%d\n",MemError()));
          }
        #else
          free(pp);
        #endif
        
        pp=NULL; /* bfo */
        upe_printf( "No more memory (MAXMEMBLOCKS) !!!\n" );
    }
   
    debugprintf(dbgMemory,dbgNorm,("# os9malloc: Allocated block #%d at $%08lX (size=%lu)\n",
                                      k,(ulong) pp,memsz));
    return pp;
} /* os9malloc */


/* memory deallocation for OS-9 */
os9err os9free(ushort pid, void* membase, ulong memsz)
{
    memblock_typ *m;
    int k;

    debugprintf(dbgMemory,dbgNorm,("# os9free: Free request of PID=%d, at $%08lX (size=%lu)\n",
                                    pid,(ulong) membase,memsz));
    if (membase==NULL) return os9error(E_BPADDR); /* no memory was allocated here */

    for(k=0;k<MAXMEMBLOCKS;k++) {
        m= &procs[pid].os9memblocks[k];
    
        if     (m->base==membase) {
            if (m->size==memsz  ) {
                release_memblock(pid,k);
                debugprintf(dbgMemory,dbgNorm,("# os9free: Found block #%d to free at $%08lX (size=%lu)\n",
                                                k,membase,m->size));
            }
            else {
                debugprintf(dbgMemory,dbgNorm,("# os9free: Found block #%d at $%08lX, but wrong size=%lu (specified=%lu)\n",
                                                k,membase,m->size,memsz));
                return os9error(E_BPADDR);
            }
            
            return 0; /* freed ok */
        }
    } /* for */
    
    debugprintf(dbgMemory+dbgAnomaly,dbgDeep,("# os9free: Block at $%08lX (size=%lu) not found in pid=%d's memory list\n",membase,memsz,pid));
    return os9error(E_BPADDR); /* no memory was allocated here */
} /* os9free */


/* eof */

