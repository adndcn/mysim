#include "types.h"
#include "spr-defs.h"
#include "cpu.h"
#include "except.h"
#include "mem.h"

struct cpu_state cpu_state;

/*---------------------------------------------------------------------------*/
/*!Set a specific register with value

Implementation specific. Abort if we are given a duff register.

@param[in] regno  The register of interest
@param[in] value  The value to be set                                     */
/*---------------------------------------------------------------------------*/
void setsim_reg(unsigned int regno,
				uorreg_t value)
{
	if (regno == 0) /* gpr0 is always zero */
	{
		value = 0;
	}

	if (regno < MAX_GPRS)
	{
		cpu_state.reg[regno] = value;
	}
	else
	{
		//PRINTF("\nABORT: write out of registers\n");
		//sim_done();
	}

} /* setsim_reg() */

/*---------------------------------------------------------------------------*/
/*!This code actually updates the PC value                                   */
/*---------------------------------------------------------------------------*/
void update_pc()
{
	cpu_state.delay_insn = next_delay_insn;
	cpu_state.sprs[SPR_PPC] = cpu_state.pc; /* Store value for later */
	cpu_state.pc = pcnext;
	pcnext = cpu_state.delay_insn ? cpu_state.pc_delay : pcnext + 4;
} /* update_pc() */

void cpu_reset()
{
	int i;
	//reset GPRS reg
	for (i = 0; i < MAX_GPRS; i++)
	{
		setsim_reg(i, 0);
	}

	memset(&cpu_state.iqueue, 0, sizeof(cpu_state.iqueue));

	pcnext = (cpu_state.sprs[SPR_SR] & SPR_SR_EPH ? 0xf0000000 : 0x00000000);

	cpu_state.pc = pcnext;
	pcnext += 4;

	/* MM1409: All programs should set their stack pointer!  */
	except_handle(EXCEPT_RESET, 0);
	update_pc();

	except_pending = 0;
	cpu_state.pc = cpu_state.sprs[SPR_SR] & SPR_SR_EPH ? 0xf0000000 + EXCEPT_RESET : EXCEPT_RESET;
}

unsigned int eval_insn(oraddr_t memaddr)
{
	unsigned int insn;
	insn = ((program_mem[memaddr] << 24) | (program_mem[memaddr + 1] << 16) | (program_mem[memaddr + 2] << 8) | (program_mem[memaddr + 3]));
	return insn;
}

int fetch()
{

	cpu_state.iqueue.insn_addr = cpu_state.pc;
	cpu_state.iqueue.insn = eval_insn(cpu_state.pc);

	/* update_pc will be called after execution */
	return 0;

} /* fetch() */

void cpu_clock()
{
	except_pending = 0;
	next_delay_insn = 0;

	fetch();

	decode_execute(&cpu_state.iqueue);
	update_pc();
}