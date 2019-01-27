#include "abstract.h"
#include "cpu.h"
#include "mem.h"
#include <stdio.h>

/*---------------------------------------------------------------------------*/
/*!Perform analysis of the instruction being executed

This could be static for SIMPLE_EXECUTION, but made global for general use.

@param[in] current  The instruction being executed                        */
/*---------------------------------------------------------------------------*/
void analysis(struct iqueue_entry *current)
{
	printf("%d\n", current->insn_index);

} /* analysis() */


void btic_update(oraddr_t targetaddr)
{
	;
}

/* Set a specific SPR with a value. */
void mtspr(uint16_t regno, const uorreg_t value)
{
	uorreg_t prev_val;

	prev_val = cpu_state.sprs[regno];
	cpu_state.sprs[regno] = value;

	
}


/* Get a specific SPR. */
uorreg_t mfspr(const uint16_t regno)
{
	uorreg_t ret;

	ret = cpu_state.sprs[regno];

	//switch (regno) {
	//case SPR_NPC:

	//	/* The NPC is the program counter UNLESS the NPC has been changed and we
	//	are stalled, which will have flushed the pipeline, so the value is
	//	zero. Currently this is optional behavior, since it breaks GDB.
	//	*/

	//	if (config.sim.strict_npc && cpu_state.npc_not_valid)
	//	{
	//		ret = 0;
	//	}
	//	else
	//	{
	//		ret = cpu_state.pc;
	//	}
	//	break;

	//case SPR_TTCR:
	//	ret = spr_read_ttcr();
	//	break;
	//case SPR_FPCSR:
	//	// If hard floating point is disabled - return 0
	//	if (!config.cpu.hardfloat)
	//		ret = 0;
	//	break;
	//default:
	//	/* Links to GPRS */
	//	if (regno >= 0x0400 && regno < 0x0420)
	//		ret = cpu_state.reg[regno - 0x0400];
	//}

	return ret;
}

void l_invalid()
{
	printf("invalid\n");
	//except_handle(EXCEPT_ILLEGAL, cpu_state.iqueue.insn_addr);

} /* l_invalid() */

int ffs(uorreg_t reg)
{
	return 0;
}

void add_program(oraddr_t address, uint32_t insn)
{
	set_program8(address, (insn >> 24) & 0xff);
	set_program8(address + 1, (insn >> 16) & 0xff);
	set_program8(address + 2, (insn >> 8) & 0xff);
	set_program8(address + 3, insn & 0xff);
}

void sim_init()
{
	
}