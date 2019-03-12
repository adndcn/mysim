#include "mmu.h"
#include "spr-defs.h"
#include "cpu.h"

MMU::MMU()
{
    enabled = 0;
    nsets = 1;
    nways = 1;
    pagesize = 8192;
    entrysize = 1; /* Not currently used */
    ustates = 2;
    hitdelay = 1;
    missdelay = 1;
}

MMU::~MMU()
{
}

int MMU::log2_int(int nsets)
{
    int log_num = 0;
    if (!nsets)
    {
        return 0;
    }

    while (nsets != 1)
    {
        nsets >>= 1;
        log_num++;
    }
    return log_num;
}

IMMU::IMMU(CPU *cpu_pointer)
{
    int set_bits;
    int way_bits;

    cpu = cpu_pointer;
    enabled = 0;
    nsets = 1;
    nways = 1;
    pagesize = 8192;
    entrysize = 1; /* Not currently used */
    ustates = 2;
    hitdelay = 1;
    missdelay = 1;

    if (enabled)
    {
        cpu->sprs[SPR_UPR] |= SPR_UPR_IMP;
    }
    else
    {
        cpu->sprs[SPR_UPR] &= ~SPR_UPR_IMP;
    }

    set_bits = log2_int(nsets);
    cpu->sprs[SPR_IMMUCFGR] &= ~SPR_IMMUCFGR_NTS;
    cpu->sprs[SPR_IMMUCFGR] |= set_bits << SPR_IMMUCFGR_NTS_OFF;

    way_bits = nways - 1;
    cpu->sprs[SPR_IMMUCFGR] &= ~SPR_IMMUCFGR_NTW;
    cpu->sprs[SPR_IMMUCFGR] |= way_bits << SPR_IMMUCFGR_NTW_OFF;
}

IMMU::~IMMU()
{
}

uorreg_t *IMMU::immu_find_tlbmr(oraddr_t virtaddr, uorreg_t **itlbmr_lru)
{
    int set;
    int i;
    oraddr_t vpn;
    uorreg_t *itlbmr;

    /* Which set to check out? */
    set = IADDR_PAGE(virtaddr) >> pagesize_log2;
    set &= set_mask;
    vpn = virtaddr & vpn_mask;

    itlbmr = &cpu->sprs[SPR_ITLBMR_BASE(0) + set];
    *itlbmr_lru = itlbmr;

    /*find match via vpn*/
    for (i = nways; i; i--, itlbmr += (128 * 2))
    {
        if (((*itlbmr & vpn_mask) == vpn) && (*itlbmr & SPR_ITLBMR_V))
            return itlbmr;
    }

    return NULL;
}

oraddr_t IMMU::immu_translate(oraddr_t virtaddr)
{
    int i;
    uorreg_t *itlbtr;
    uorreg_t *itlbmr;
    uorreg_t *itlbmr_lru;

    //disable mmu
    if (!(cpu->sprs[SPR_SR] & SPR_SR_IME) ||
        !(cpu->sprs[SPR_UPR] & SPR_UPR_IMP))
    {
        insn_ci = (virtaddr >= 0x80000000);
        return virtaddr;
    }

    itlbmr = immu_find_tlbmr(virtaddr, &itlbmr_lru);

    if (itlbmr)
    {
        itlbtr = itlbmr + 128;

        /* Set LRUs */
        /*
         * every itlbmr have distance of 256.
         * every itlbmr have 128 in maximum.
        */
        for (i = 0; i < nways; i++, itlbmr_lru += (128 * 2))
        {
            //LRU 7-6
            if (*itlbmr_lru & SPR_ITLBMR_LRU)
                *itlbmr_lru = (*itlbmr_lru & ~SPR_ITLBMR_LRU) |
                              ((*itlbmr_lru & SPR_ITLBMR_LRU) - 0x40);
        }

        *itlbmr |= lru_reload;

        /* Check if page is cache inhibited */
        insn_ci = *itlbtr & SPR_ITLBTR_CI;

        //itlbtr's PPM + (virtaddr & page_offset_mask)
        return (*itlbtr & SPR_ITLBTR_PPN) | (virtaddr & page_offset_mask);
    }

    //TODO:
    //except_handle(EXCEPT_ITLBMISS, virtaddr);

    return 0;
}

DMMU::DMMU(CPU *cpu_pointer)
{
    int set_bits;
    int way_bits;

    cpu = cpu_pointer;
    enabled = 0;
    nsets = 1;
    nways = 1;
    pagesize = 8192;
    entrysize = 1; /* Not currently used */
    ustates = 2;
    hitdelay = 1;
    missdelay = 1;

    if (enabled)
    {
        cpu->sprs[SPR_UPR] |= SPR_UPR_DMP;
    }
    else
    {
        cpu->sprs[SPR_UPR] &= ~SPR_UPR_DMP;
    }

    set_bits = log2_int(nsets);
    cpu->sprs[SPR_DMMUCFGR] &= ~SPR_DMMUCFGR_NTS;
    cpu->sprs[SPR_DMMUCFGR] |= set_bits << SPR_DMMUCFGR_NTS_OFF;

    way_bits = nways - 1;
    cpu->sprs[SPR_DMMUCFGR] &= ~SPR_DMMUCFGR_NTW;
    cpu->sprs[SPR_DMMUCFGR] |= way_bits << SPR_DMMUCFGR_NTW_OFF;
}

DMMU::~DMMU()
{
}

uorreg_t *DMMU::dmmu_find_tlbmr(oraddr_t virtaddr, uorreg_t **dtlbmr_lru)
{
    int set;
    int i;
    oraddr_t vpn;
    uorreg_t *dtlbmr;

    /* Which set to check out? */
    set = DADDR_PAGE(virtaddr) >> pagesize_log2;
    set &= set_mask;
    vpn = virtaddr & vpn_mask;

    dtlbmr = &cpu->sprs[SPR_DTLBMR_BASE(0) + set];
    *dtlbmr_lru = dtlbmr;

    /* FIXME: Should this be reversed? */
    for (i = nways; i; i--, dtlbmr += (128 * 2))
    {
        if (((*dtlbmr & vpn_mask) == vpn) && (*dtlbmr & SPR_DTLBMR_V))
            return dtlbmr;
    }

    return NULL;
}

oraddr_t DMMU::dmmu_translate(oraddr_t virtaddr, int write_access)
{
    int i;
    uorreg_t *dtlbmr;
    uorreg_t *dtlbtr;
    uorreg_t *dtlbmr_lru;
    if (!(cpu->sprs[SPR_SR] & SPR_SR_DME) ||
        !(cpu->sprs[SPR_UPR] & SPR_UPR_DMP))
    {
        data_ci = (virtaddr >= 0x80000000);
        return virtaddr;
    }

    dtlbmr = dmmu_find_tlbmr(virtaddr, &dtlbmr_lru);

    /* Did we find our tlb entry? */
    if (dtlbmr)
    {
        dtlbtr = dtlbmr + 128;

        /* Set LRUs */
        for (i = 0; i < nways; i++, dtlbmr_lru += (128 * 2))
        {
            if (*dtlbmr_lru & SPR_DTLBMR_LRU)
                *dtlbmr_lru = (*dtlbmr_lru & ~SPR_DTLBMR_LRU) |
                              ((*dtlbmr_lru & SPR_DTLBMR_LRU) - 0x40);
        }

        *dtlbmr |= lru_reload;

        /* Check if page is cache inhibited */
        data_ci = *dtlbtr & SPR_DTLBTR_CI;

        // /* Test for page fault */
        // if (cpu->sprs[SPR_SR] & SPR_SR_SM)
        // {
        //     if ((write_access && !(*dtlbtr & SPR_DTLBTR_SWE)) || (!write_access && !(*dtlbtr & SPR_DTLBTR_SRE)))
        //         except_handle(EXCEPT_DPF, virtaddr);
        // }
        // else
        // {
        //     if ((write_access && !(*dtlbtr & SPR_DTLBTR_UWE)) || (!write_access && !(*dtlbtr & SPR_DTLBTR_URE)))
        //         except_handle(EXCEPT_DPF, virtaddr);
        // }

        return (*dtlbtr & SPR_DTLBTR_PPN) | (virtaddr & (page_offset_mask));
    }

    //TODO
    //except_handle(EXCEPT_DTLBMISS, virtaddr);

    return 0;
}