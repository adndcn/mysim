#include "cache_model.h"
#include "spr-defs.h"
#include "bus_device.h"
#include "cpu.h"

int CACHE::log2_int(unsigned long x)
{
    int c = 0;
    if (!x)
        return 0; /* not by the book, but practical */
    while (x != 1)
        x >>= 1, c++;
    return c;
}

uint32_t ICACHE::ic_simulate_fetch(oraddr_t fetchaddr, oraddr_t virt_addr, int insn_ci)
{
    oraddr_t set;
    oraddr_t way;
    oraddr_t lru_way;
    oraddr_t tagaddr;
    uint32_t tmp;
    oraddr_t reload_addr;
    oraddr_t reload_end;
    unsigned int minlru;

    /* ICache simulation enabled/disabled. */
    if (!(cpu->sprs[SPR_UPR] & SPR_UPR_ICP) ||
        !(cpu->sprs[SPR_SR] & SPR_SR_ICE) || insn_ci)
    {
        BUS_DEVICE * device = bus_p->get_device(fetchaddr);
        tmp = device->Read32(fetchaddr, virt_addr);
        // if (cur_area && cur_area->log)
        //     fprintf(cur_area->log, "[%" PRIxADDR "] -> read %08" PRIx32 "\n",
        //             fetchaddr, tmp);
        return tmp;
    }

    /* Which set to check out? */
    set = (fetchaddr & set_mask) >> blocksize_log2;
    tagaddr = fetchaddr & tagaddr_mask;

    /* Scan all ways and try to find a matching way. */
    for (way = set; way < last_way; way += nsets)
    {
        if (tags[way] == tagaddr)
        {
            // ic_stats.readhit++;

            for (lru_way = set; lru_way < last_way; lru_way += nsets)
                if (lrus[lru_way] > lrus[way])
                    lrus[lru_way]--;
            lrus[way] = ustates_reload;
            // runtime.sim.mem_cycles += ic->hitdelay;
            way <<= blocksize_log2;
            return *(uint32_t *)&mem[way | (fetchaddr & block_offset_mask)];
        }
    }

    minlru = ustates_reload;
    way = set;

    // ic_stats.readmiss++;

    for (lru_way = set; lru_way < last_way; lru_way += nsets)
    {
        if (lrus[lru_way] < minlru)
        {
            way = lru_way;
            minlru = lrus[lru_way];
        }
    }

    tags[way] = tagaddr;
    for (lru_way = set; lru_way < last_way; lru_way += nsets)
        if (lrus[lru_way])
            lrus[lru_way]--;
    lrus[way] = ustates_reload;

    reload_addr = fetchaddr & block_offset_mask;
    reload_end = reload_addr + blocksize;

    fetchaddr &= block_mask;

    way <<= blocksize_log2;
    for (; reload_addr < reload_end; reload_addr += 4)
    {
        BUS_DEVICE * device = bus_p->get_device(fetchaddr | (reload_addr & block_offset_mask));
        tmp = *(uint32_t *)&mem[way | (reload_addr & block_offset_mask)]
            = device->Read32(fetchaddr | (reload_addr & block_offset_mask), 0);

        // if (!cur_area)
        // {
        //     tags[way >> blocksize_log2] = -1;
        //     lrus[way >> blocksize_log2] = 0;
        //     return 0;
        // }
        // else if (cur_area->log)
        //     fprintf(cur_area->log, "[%" PRIxADDR "] -> read %08" PRIx32 "\n",
        //             fetchaddr, tmp);
    }

    // runtime.sim.mem_cycles += missdelay;

    // if (config.pcu.enabled)
    //     pcu_count_event(SPR_PCMR_ICM);

    return *(uint32_t *)&mem[way | (reload_addr & block_offset_mask)];
}

ICACHE::ICACHE(CPU * cpu_pointer, BUS *bus_pointer)
{
    int set_bits;
    int way_bits;
    unsigned int size = 0;

    cpu = cpu_pointer;
    bus_p = bus_pointer;
    enabled = 0;
    nsets = 512;
    nways = 1;
    blocksize = MIN_IC_BLOCK_SIZE;
    ustates = 2;
    hitdelay = 1;
    missdelay = 1;

    mem = NULL; /* Internal configuration */
    lrus = NULL;
    tags = NULL;

    size = nways * nsets * blocksize;
    if(size)
    {
        mem = new uint8_t[size]();
        lrus = new unsigned int[nsets*nways]();
        tags = new oraddr_t[nsets*nways]();

    }

    blocksize_log2 = log2_int(blocksize);
    set_mask = (nsets - 1) << blocksize_log2;
    tagaddr_mask = ~((nsets * blocksize) - 1);
    last_way = nsets * nways;
    block_offset_mask = blocksize - 1;
    block_mask = ~block_offset_mask;
    ustates_reload = ustates - 1;

    /* Set SPRs as appropriate */

    if (enabled)
    {
        cpu->sprs[SPR_UPR] |= SPR_UPR_ICP;
    }
    else
    {
        cpu->sprs[SPR_UPR] &= ~SPR_UPR_ICP;
    }

    set_bits = log2_int(nsets);
    cpu->sprs[SPR_ICCFGR] &= ~SPR_ICCFGR_NCS;
    cpu->sprs[SPR_ICCFGR] |= set_bits << SPR_ICCFGR_NCS_OFF;

    way_bits = log2_int(nways);
    cpu->sprs[SPR_ICCFGR] &= ~SPR_ICCFGR_NCW;
    cpu->sprs[SPR_ICCFGR] |= way_bits << SPR_ICCFGR_NCW_OFF;

    if (MIN_IC_BLOCK_SIZE == blocksize)
    {
        cpu->sprs[SPR_ICCFGR] &= ~SPR_ICCFGR_CBS;
    }
    else
    {
        cpu->sprs[SPR_ICCFGR] |= SPR_ICCFGR_CBS;
    }
}

ICACHE::~ICACHE()
{

}

DCACHE::DCACHE(CPU *cpu_pointer, BUS *bus_pointer)
{
    // cpu = cpu_pointer;
    // bus_p = bus_pointer;
}

DCACHE::~DCACHE()
{

}

uint32_t DCACHE::dc_simulate_read(oraddr_t dataaddr, oraddr_t virt_addr, int width, int data_ci)
{
    int set, way = -1;
    int i;
    oraddr_t tagaddr;
    uint32_t tmp = 0;
    BUS_DEVICE * device;
    device = bus_p->get_device(dataaddr);

    if (!(cpu->sprs[SPR_UPR] & SPR_UPR_DCP) ||
        !(cpu->sprs[SPR_SR] & SPR_SR_DCE) || data_ci)
    {
        if (width == 4)
        {
            tmp = device->Read32(dataaddr, virt_addr);
        }
        else if (width == 2)
        {
            tmp = device->Read16(dataaddr, virt_addr);
        }
        else if (width == 1)
        {
            tmp = device->Read8(dataaddr, virt_addr);
        }

        return tmp;
    }

    /* Which set to check out? */
    set = (dataaddr / blocksize) % nsets;
    tagaddr = (dataaddr / blocksize) / nsets;

    /* Scan all ways and try to find a matching way. */
    for (i = 0; i < nways; i++)
        if (cache_mem[i].cache_set[set].tagaddr == tagaddr)
            way = i;
    
    /* Did we find our cached data? */
    if (way >= 0)
    { /* Yes, we did. */
        // dc_stats.readhit++;

        for (i = 0; i < nways; i++)
            if (cache_mem[i].cache_set[set].lru > cache_mem[way].cache_set[set].lru)
                cache_mem[i].cache_set[set].lru--;
        cache_mem[way].cache_set[set].lru = ustates - 1;
        // runtime.sim.mem_cycles += config.dc.load_hitdelay;

        tmp =
            cache_mem[way].cache_set[set].line[(dataaddr & (blocksize - 1)) >> 2];
        if (width == 4)
            return tmp;
        else if (width == 2)
        {
            tmp = ((tmp >> ((dataaddr & 2) ? 0 : 16)) & 0xffff);
            return tmp;
        }
        else if (width == 1)
        {
            tmp = ((tmp >> (8 * (3 - (dataaddr & 3)))) & 0xff);
            return tmp;
        }
    }
    else
    { /* No, we didn't. */
        int minlru = ustates - 1;
        int minway = 0;

        // dc_stats.readmiss++;

        for (i = 0; i < nways; i++)
        {
            if (cache_mem[i].cache_set[set].lru < minlru)
            {
                minway = i;
                minlru = cache_mem[i].cache_set[set].lru;
            }
        }

        for (i = 0; i < (blocksize); i += 4)
        {
            /* FIXME: What is the virtual address meant to be? (ie. What happens if
	   * we read out of memory while refilling a cache line?) */
            oraddr_t head_addr = (dataaddr & ~(blocksize - 1));
            oraddr_t tail_addr = (((dataaddr & ~(3)) + i) & (blocksize - 1));
            device = bus_p->get_device(head_addr + tail_addr);
            tmp = device->Read32(head_addr + tail_addr, 0);

            cache_mem[minway].cache_set[set].line[((dataaddr + i) & (blocksize - 1)) >> 2] = tmp;

        }

        cache_mem[minway].cache_set[set].tagaddr = tagaddr;
        for (i = 0; i < nways; i++)
            if (cache_mem[i].cache_set[set].lru)
                cache_mem[i].cache_set[set].lru--;
        cache_mem[minway].cache_set[set].lru = ustates - 1;
        // runtime.sim.mem_cycles += config.dc.load_missdelay;

        // if (config.pcu.enabled)
        //     pcu_count_event(SPR_PCMR_DCM);

        tmp =
            cache_mem[minway].cache_set[set].line[(dataaddr & (blocksize - 1)) >> 2];
        if (width == 4)
            return tmp;
        else if (width == 2)
        {
            tmp = (tmp >> ((dataaddr & 2) ? 0 : 16)) & 0xffff;
            return tmp;
        }
        else if (width == 1)
        {
            tmp = (tmp >> (8 * (3 - (dataaddr & 3)))) & 0xff;
            return tmp;
        }
    }

    return 0;
}


void DCACHE::dc_simulate_write(oraddr_t dataaddr, oraddr_t virt_addr, uint32_t data, int width, int data_ci)
{
    int set, way = -1;
    int i;
    oraddr_t tagaddr;
    uint32_t tmp;
    BUS_DEVICE * device;
    device = bus_p->get_device(dataaddr);

    if (width == 4)
        device->Write32(dataaddr, virt_addr, data);
    else if (width == 2)
        device->Write16(dataaddr, virt_addr, data);
    else if (width == 1)
        device->Write8(dataaddr, virt_addr, data);

    if (!(cpu->sprs[SPR_UPR] & SPR_UPR_DCP) ||
        !(cpu->sprs[SPR_SR] & SPR_SR_DCE) || data_ci)
        return;

    /* Which set to check out? */
    set = (dataaddr / blocksize) % nsets;
    tagaddr = (dataaddr / blocksize) / nsets;

    /* Scan all ways and try to find a matching way. */
    for (i = 0; i < nways; i++)
        if (cache_mem[i].cache_set[set].tagaddr == tagaddr)
            way = i;
    
    /* Did we find our cached data? */
    if (way >= 0)
    { /* Yes, we did. */
        // dc_stats.writehit++;

        for (i = 0; i < nways; i++)
            if (cache_mem[i].cache_set[set].lru > cache_mem[way].cache_set[set].lru)
                cache_mem[i].cache_set[set].lru--;
        cache_mem[way].cache_set[set].lru = ustates - 1;
        // runtime.sim.mem_cycles += store_hitdelay;

        tmp =
            cache_mem[way].cache_set[set].line[(dataaddr & (blocksize - 1)) >> 2];
        if (width == 4)
            tmp = data;
        else if (width == 2)
        {
            tmp &= 0xffff << ((dataaddr & 2) ? 16 : 0);
            tmp |= (data & 0xffff) << ((dataaddr & 2) ? 0 : 16);
        }
        else if (width == 1)
        {
            tmp &= ~(0xff << (8 * (3 - (dataaddr & 3))));
            tmp |= (data & 0xff) << (8 * (3 - (dataaddr & 3)));
        }
        cache_mem[way].cache_set[set].line[(dataaddr & (blocksize - 1)) >> 2] =
            tmp;
    }
    else
    { /* No, we didn't. */
        int minlru = ustates - 1;
        int minway = 0;

        // dc_stats.writemiss++;

        for (i = 0; i < nways; i++)
            if (cache_mem[i].cache_set[set].lru < minlru)
                minway = i;

        for (i = 0; i < (blocksize); i += 4)
        {
            device = bus_p->get_device((dataaddr & ~(blocksize - 1)) + (((dataaddr & ~3ul) + i) & (blocksize -1)));
            cache_mem[minway].cache_set[set].line[((dataaddr + i) & (blocksize - 1)) >> 2] =
                /* FIXME: Same comment as in dc_simulate_read */
                device->Read32((dataaddr & ~(blocksize - 1)) + (((dataaddr & ~3ul) + i) & (blocksize -1)), 0);

        }

        cache_mem[minway].cache_set[set].tagaddr = tagaddr;
        for (i = 0; i < nways; i++)
            if (cache_mem[i].cache_set[set].lru)
                cache_mem[i].cache_set[set].lru--;
        cache_mem[minway].cache_set[set].lru = ustates - 1;
        // runtime.sim.mem_cycles += store_missdelay;

        // if (config.pcu.enabled)
        //     pcu_count_event(SPR_PCMR_DCM);
    }
}