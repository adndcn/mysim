#pragma once

#include "types.h"

#define IADDR_PAGE(addr) ((addr) & page_mask)
#define DADDR_PAGE(addr) ((addr) & page_mask)

class CPU;
class MMU
{
public:
    MMU();
    ~MMU();

    int       enabled;		/* Whether IMMU is enabled */
    int       nways;		/* Number of ITLB ways */
    int       nsets;		/* Number of ITLB sets */
    oraddr_t  pagesize;		/* ITLB page size */
    int       pagesize_log2;	/* ITLB page size (log2(pagesize)) */
    oraddr_t  page_offset_mask;	/* Address mask to get page offset */
    oraddr_t  page_mask;		/* Page number mask (diff. from vpn) */
    oraddr_t  vpn_mask;		/* Address mask to get vpn */
    int       lru_reload;		/* What to reload the lru value to */
    oraddr_t  set_mask;		/* Mask to get set of an address */
    int       entrysize;		/* ITLB entry size */
    int       ustates;		/* number of ITLB usage states */
    int       missdelay;		/* How much cycles does the miss cost */
    int       hitdelay;		/* How much cycles does the hit cost */

    CPU* cpu;

    int log2_int(int nsets);

};


class IMMU: public MMU
{
public:
    IMMU(CPU *cpu_pointer);
    ~IMMU();

    int insn_ci;/*!< Global var: instr cache inhibit bit set */

    oraddr_t  immu_translate (oraddr_t virtaddr);
    oraddr_t  immu_simulate_tlb (oraddr_t virtaddr);
    // oraddr_t  peek_into_itlb (oraddr_t virtaddr);
    // void      reg_immu_sec ();

private:
    uorreg_t * immu_find_tlbmr(oraddr_t virtaddr, uorreg_t **itlbmr_lru);
};

class DMMU: public MMU
{
public:
    DMMU(CPU *cpu_pointer);
    ~DMMU();

    int data_ci; /*!< Global var: data cache inhibit bit set */

    oraddr_t  dmmu_translate (oraddr_t virtaddr, int write_access);
    oraddr_t  dmmu_simulate_tlb (oraddr_t  virtaddr, int write_access);

private:
    uorreg_t *dmmu_find_tlbmr(oraddr_t virtaddr, uorreg_t **dtlbmr_lru);
};