#include "mem.h"


/* Returns 8-bit values from mem array.
*
* STATISTICS OK (only used for cpu_access, that is architectural access)
*/
uint8_t eval_mem8(oraddr_t memaddr, int *breakpoint)
{
	uint8_t temp;
	temp = 0;

	return temp;
}

/* Returns 16-bit values from mem array. Big endian version.
*
* STATISTICS OK (only used for cpu_access, that is architectural access)
*/
uint16_t eval_mem16(oraddr_t memaddr, int *breakpoint)
{
	uint16_t temp;
	temp = 0;


	return temp;
}

/* Returns 32-bit values from mem array. Big endian version.
*
* STATISTICS OK (only used for cpu_access, that is architectural access)
*/
uint32_t eval_mem32(oraddr_t memaddr, int *breakpoint)
{
	uint32_t temp;
	oraddr_t phys_memaddr;
	temp = 0;

	return temp;
}

void set_mem8(oraddr_t memaddr, uint8_t value, int *breakpoint)
{
	//struct dev_memarea *mem;

	//if ((mem = verify_memoryarea(memaddr)))
	//{
	//	cur_vadd = vaddr;
	//	runtime.sim.mem_cycles += mem->ops.delayw;
	//	mem->ops.writefunc8(memaddr & mem->size_mask, value,
	//		mem->ops.write_dat8);
	//}
	//else
	//{
	//	if (config.sim.report_mem_errs)
	//	{
	//		PRINTF("EXCEPTION: write out of memory (8-bit access to %" PRIxADDR
	//			")\n",
	//			memaddr);
	//	}

	//	except_handle(EXCEPT_BUSERR, vaddr);
	//}
}


/* Set mem, 16-bit. Big endian version. */

void set_mem16(oraddr_t memaddr, uint16_t value, int *breakpoint)
{

}


/* Set mem, 32-bit. Big endian version.
*
* STATISTICS OK. (the only suspicious usage is in sim-cmd.c,
*                 where this instruction is used for patching memory,
*                 wether this is cpu or architectual access is yet to
*                 be decided)
*/
void set_mem32(oraddr_t memaddr, uint32_t value, int *breakpoint)
{
	oraddr_t phys_memaddr;

	//if (config.sim.mprofile)
	//	mprofile(memaddr, MPROF_32 | MPROF_WRITE);

	//if (memaddr & 3)
	//{
	//	except_handle(EXCEPT_ALIGN, memaddr);
	//	return;
	//}

	//phys_memaddr = dmmu_translate(memaddr, 1);
	//;

	//if ((phys_memaddr & ~3) == cpu_state.loadlock_addr)
	//	cpu_state.loadlock_active = 0;

	///* If we produced exception don't set anything */
	//if (except_pending)
	//	return;

	//if (config.pcu.enabled)
	//	pcu_count_event(SPR_PCMR_SA);

	//if (config.debug.enabled)
	//{
	//	*breakpoint += check_debug_unit(DebugStoreAddress, memaddr); /* 28/05/01 CZ */
	//	*breakpoint += check_debug_unit(DebugStoreData, value);
	//}

	//if (config.dc.enabled)
	//	dc_simulate_write(phys_memaddr, memaddr, value, 4);
	//else
	//	setsim_mem32(phys_memaddr, memaddr, value);

	//if (cur_area && cur_area->log)
	//	fprintf(cur_area->log, "[%" PRIxADDR "] -> write %08" PRIx32 "\n",
	//		memaddr, value);
}