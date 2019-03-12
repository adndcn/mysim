#pragma once

#include "types.h"
// #include "cpu.h"

#define MAX_IC_SETS 1024
#define MAX_IC_WAYS 32
#define MIN_IC_BLOCK_SIZE 16
#define MAX_IC_BLOCK_SIZE 32

#define MAX_BLOCK_SIZE 32
#define MAX_SETS 1024
#define MAX_WAYS 32

class BUS;
class CPU;
class CACHE
{
public:
    int           enabled;	    /* Whether instruction cache is enabled */
    unsigned int  nways;		    /* Number of IC ways */
    unsigned int  nsets;		    /* Number of IC sets */
    unsigned int  blocksize;	    /* IC entry size */
    unsigned int  ustates;	    /* number of IC usage states */
    int           missdelay;	    /* How much cycles does the miss cost */
    int           hitdelay;	    /* How much cycles does the hit cost */
    unsigned int  blocksize_log2;	    /* log2(blocksize) */
    oraddr_t      set_mask;	    /* Mask to get set number */
    oraddr_t      tagaddr_mask;	    /* Mask to get tag address */
    oraddr_t      last_way;	    /* nways * nsets */
    oraddr_t      block_offset_mask;  /* mask to get offset into block */
    oraddr_t      block_mask;	    /* mask to get block number */
    unsigned int  ustates_reload;	    /* ustates - 1 */

    oraddr_t     *tags;
    uint8_t      *mem;
    unsigned int *lrus;

    CPU *cpu;
    BUS *bus_p;

    int log2_int (unsigned long x);

};

class ICACHE: public CACHE
{
public:
    ICACHE(CPU *cpu_pointer, BUS *bus_pointer);
    ~ICACHE();
    uint32_t ic_simulate_fetch(oraddr_t fetchaddr, oraddr_t virt_addr, int insn_ci);
};

class DCACHE:public CACHE
{
public:
    struct
    {
        struct
        {
            uint32_t line[MAX_BLOCK_SIZE / 4];
            oraddr_t tagaddr;
            int lru;
        }cache_set[MAX_SETS];
    }cache_mem[MAX_WAYS];

    DCACHE(CPU *cpu_pointer, BUS *bus_pointer);
    ~DCACHE();
    uint32_t dc_simulate_read(oraddr_t dataaddr, oraddr_t virt_addr, int width, int data_ci);
    void dc_simulate_write(oraddr_t dataaddr, oraddr_t virt_addr, uint32_t data, int width, int data_ci);
};