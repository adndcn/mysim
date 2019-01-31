#include "config.h"
#include "cpu.h"
#include "tick.h"
#include "pm.h"

#define MIN_DC_BLOCK_SIZE 16

/* returns log2(x) */
/* Call this log2_int, because there is a library function named log2 */
int log2_int (unsigned long x)
{
    int c = 0;
    if (!x)
        return 0;			/* not by the book, but practical */
    while (x != 1)
        x >>= 1, c++;
    return c;
}

void init_config()
{
    unsigned char dc_enabled = 0;
    
    /* CPU */
    cpu_state.sprs[SPR_VR] = 0;
    cpu_state.sprs[SPR_UPR] = SPR_UPR_UP | SPR_UPR_TTP;
    cpu_state.sprs[SPR_SR] = SPR_SR_FO | SPR_SR_SM;
#ifndef OR32_NODELAY
    cpu_state.sprs[SPR_CPUCFGR] = SPR_CPUCFGR_OB32S;
#else
    cpu_state.sprs[SPR_CPUCFGR] = SPR_CPUCFGR_OB32S | SPR_CPUCFGR_ND;
#endif

    if (dc_enabled)
    {
        cpu_state.sprs[SPR_UPR] |= SPR_UPR_DCP;
    }
    else
    {
        cpu_state.sprs[SPR_UPR] &= ~SPR_UPR_DCP;
    }

    unsigned char set_bits;
    unsigned char dc_nsets = 1;
    unsigned char way_bits;
    unsigned char dc_nways = 1;

    set_bits = log2_int(dc_nsets);
    cpu_state.sprs[SPR_DCCFGR] &= ~SPR_DCCFGR_NCS;
    cpu_state.sprs[SPR_DCCFGR] |= set_bits << SPR_DCCFGR_NCS_OFF;

    way_bits = log2_int(dc_nways);
    cpu_state.sprs[SPR_DCCFGR] &= ~SPR_DCCFGR_NCW;
    cpu_state.sprs[SPR_DCCFGR] |= way_bits << SPR_DCCFGR_NCW_OFF;

    unsigned char dc_blocksize = 1;

    if (MIN_DC_BLOCK_SIZE == dc_blocksize)
    {
        cpu_state.sprs[SPR_DCCFGR] &= ~SPR_DCCFGR_CBS;
    }
    else
    {
        cpu_state.sprs[SPR_DCCFGR] |= SPR_DCCFGR_CBS;
    }

     /* Power management */
    unsigned char pm_enabled = 0;

    if (pm_enabled)
    {
        cpu_state.sprs[SPR_UPR] |= SPR_UPR_PMP;
    }
    else
    {
        cpu_state.sprs[SPR_UPR] &= ~SPR_UPR_PMP;
    }

    /* Programmable Interrupt Controller */
    unsigned char pic_enabled = 0;

    if (pic_enabled)
    {
        cpu_state.sprs[SPR_UPR] |= SPR_UPR_PICP;
    }
    else
    {
        cpu_state.sprs[SPR_UPR] &= ~SPR_UPR_PICP;
    }

    cpu_state.sprs[SPR_DCFGR] = SPR_DCFGR_WPCI |
                                MATCHPOINTS_TO_NDP(MAX_MATCHPOINTS);

    unsigned char debug_enabled = 0;
    if (debug_enabled)
    {
        cpu_state.sprs[SPR_UPR] |= SPR_UPR_DUP;
    }
    else
    {
        cpu_state.sprs[SPR_UPR] &= ~SPR_UPR_DUP;
    }

}