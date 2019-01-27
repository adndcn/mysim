#include "config.h"
#include "cpu.h"

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

    set_bits = log2_int(config.dc.nsets);
    cpu_state.sprs[SPR_DCCFGR] &= ~SPR_DCCFGR_NCS;
    cpu_state.sprs[SPR_DCCFGR] |= set_bits << SPR_DCCFGR_NCS_OFF;

    way_bits = log2_int(config.dc.nways);
    cpu_state.sprs[SPR_DCCFGR] &= ~SPR_DCCFGR_NCW;
    cpu_state.sprs[SPR_DCCFGR] |= way_bits << SPR_DCCFGR_NCW_OFF;

    if (MIN_DC_BLOCK_SIZE == config.dc.blocksize)
    {
        cpu_state.sprs[SPR_DCCFGR] &= ~SPR_DCCFGR_CBS;
    }
    else
    {
        cpu_state.sprs[SPR_DCCFGR] |= SPR_DCCFGR_CBS;
    }

     /* Power management */
    config.pm.enabled = 0;

    if (config.pm.enabled)
    {
        cpu_state.sprs[SPR_UPR] |= SPR_UPR_PMP;
    }
    else
    {
        cpu_state.sprs[SPR_UPR] &= ~SPR_UPR_PMP;
    }

    /* Programmable Interrupt Controller */
    config.pic.enabled = 0;
    config.pic.edge_trigger = 1;
    config.pic.use_nmi = 1;

    if (config.pic.enabled)
    {
        cpu_state.sprs[SPR_UPR] |= SPR_UPR_PICP;
    }
    else
    {
        cpu_state.sprs[SPR_UPR] &= ~SPR_UPR_PICP;
    }

    cpu_state.sprs[SPR_DCFGR] = SPR_DCFGR_WPCI |
                                MATCHPOINTS_TO_NDP(MAX_MATCHPOINTS);

    if (config.debug.enabled)
    {
        cpu_state.sprs[SPR_UPR] |= SPR_UPR_DUP;
    }
    else
    {
        cpu_state.sprs[SPR_UPR] &= ~SPR_UPR_DUP;
    }

}