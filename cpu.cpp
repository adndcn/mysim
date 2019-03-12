#include "cpu.h"
#include <iostream>
#include <memory.h>

using namespace std;

CPU::CPU(BUS *bus_pointer) : icache(this, bus_pointer), dcache(this, bus_pointer), except(this), immu(this), dmmu(this)
{
	bus_p = bus_pointer;
	memset(reg, 0, sizeof(uorreg_t)*MAX_GPRS);
	memset(sprs, 0, sizeof(uorreg_t)*MAX_SPRS);
	insn_ea = 0;
	delay_insn = 0;
	npc_not_valid = 0;
	pc = 0;
	pc_delay = 0;
	memset(&iqueue, 0, sizeof(struct iqueue_entry));
	next_delay_insn = 0;
	pcnext = 0;

	memset(&config, 0, sizeof(struct config));
	do_stats = 0;
}

CPU::~CPU()
{
}

void CPU::setsim_reg(unsigned int regno, uorreg_t value)
{
	if (regno == 0) /* gpr0 is always zero */
	{
		value = 0;
	}

	if (regno < MAX_GPRS)
	{
		reg[regno] = value;
	}
	else
	{
		//PRINTF("\nABORT: write out of registers\n");
		//sim_done();
	}

} /* setsim_reg() */

void CPU::cpu_reset()
{
	int i;
	//reset GPRS reg
	for (i = 0; i < MAX_GPRS; i++)
	{
		setsim_reg(i, 0);
	}

	// memset(&cpu_state.iqueue, 0, sizeof(cpu_state.iqueue));

	pcnext = (sprs[SPR_SR] & SPR_SR_EPH ? 0xf0000000 : 0x00000000);

	pc = pcnext;
	pcnext += 4;

	/* MM1409: All programs should set their stack pointer!  */
	except.except_handle(EXCEPT_RESET, 0);
	update_pc();

	except.except_pending = 0;
	pc = sprs[SPR_SR] & SPR_SR_EPH ? 0xf0000000 + EXCEPT_RESET : EXCEPT_RESET;
}

void CPU::analysis(struct iqueue_entry *current)
{
	//debug;
}

uint32_t CPU::get_insn(oraddr_t memaddr)
{
	oraddr_t phys_addr;
	uint32_t instruction;

	phys_addr = immu.immu_translate(memaddr);

	if (except.except_pending == 1)
	{
		return 0;
	}

	if (icache.enabled)
	{
		instruction = icache.ic_simulate_fetch(phys_addr, memaddr, immu.insn_ci);
	}
	else
	{
		BUS_DEVICE *device = bus_p->get_device(phys_addr);
		instruction = device->Read32(phys_addr, memaddr);
	}

	return instruction;
}

int CPU::fetch()
{
	iqueue.insn_addr = pc;
	iqueue.insn = get_insn(pc);
	cout<<hex<<iqueue.insn<<endl;
	return 0;
}

void CPU::update_pc()
{
	delay_insn = next_delay_insn;
	sprs[SPR_PPC] = pc; /* Store value for later */
	pc = pcnext;
	pcnext = delay_insn ? pc_delay : pcnext + 4;
}

void CPU::cpu_clock()
{
	if (fetch())
	{
		//todo print error
	}

	if (except.except_pending == 1)
	{
		update_pc();
		except.except_pending = 0;
		return;
	}

	decode_execute(&iqueue);
	update_pc();

	return;
}

void CPU::l_invalid()
{
}

void CPU::btic_update(oraddr_t pc)
{
}

/* Set a specific SPR with a value. */
void CPU::mtspr(uint16_t regno, const uorreg_t value)
{
	uorreg_t prev_val;

	prev_val = sprs[regno];
	sprs[regno] = value;

	/* MM: Register hooks.  */
	switch (regno)
	{
	case SPR_TTCR:
		// spr_write_ttcr(value);
		break;
	case SPR_TTMR:
		// spr_write_ttmr(prev_val);
		break;
	/* Data cache simulateing stuff */
	case SPR_DCBPR:
		/* FIXME: This is not correct.  The arch. manual states: "Memory accesses
     * are not recorded (Unlike load or store instructions) and cannot invoke
     * any exception".  If the physical address is invalid a bus error will be
     * generated.  Also if the effective address is not resident in the mmu
     * the read will happen from address 0, which is naturally not correct. */
		// dc_simulate_read(peek_into_dtlb(value, 0, 1), value, 4);
		// sprs[SPR_DCBPR] = 0;
		break;
	case SPR_DCBFR:
		// dc_inv(value);
		// sprs[SPR_DCBFR] = -1;
		break;
	case SPR_DCBIR:
		// dc_inv(value);
		// sprs[SPR_DCBIR] = 0;
		break;
	case SPR_DCBWR:
		// sprs[SPR_DCBWR] = 0;
		break;
	case SPR_DCBLR:
		// sprs[SPR_DCBLR] = 0;
		break;
	/* Instruction cache simulateing stuff */
	case SPR_ICBPR:
		/* FIXME: The arch manual does not say what happens when an invalid memory
     * location is specified.  I guess the same as for the DCBPR register */
		// ic_simulate_fetch(peek_into_itlb(value), value);
		// sprs[SPR_ICBPR] = 0;
		break;
	case SPR_ICBIR:
		// ic_inv(value);
		// sprs[SPR_ICBIR] = 0;
		break;
	case SPR_ICBLR:
		// sprs[SPR_ICBLR] = 0;
		break;
	case SPR_SR:
		// sprs[regno] |= SPR_SR_FO;
		// if ((value & SPR_SR_IEE) && !(prev_val & SPR_SR_IEE))
			// pic_ints_en();
		break;
	case SPR_NPC:
	{
		/* The debugger has redirected us to a new address */
		/* This is usually done to reissue an instruction
         which just caused a breakpoint exception. */

		/* JPB patch. When GDB stepi, this may be used to set the PC to the
         value it is already at. If this is the case, then we do nothing (in
         particular we do not trash a delayed branch) */

		// if (value != pc)
		// {
		// 	pc = value;

		// 	if (!value && config.sim.verbose)
		// 		PRINTF("WARNING: PC just set to 0!\n");

			/* Clear any pending delay slot jumps also */
			// delay_insn = 0;
			// pcnext = value + 4;

			/* Further JPB patch. If the processor is stalled, then subsequent
	     reads of the NPC should return 0 until the processor is
	     unstalled. If the processor is stalled, note that the NPC has
	     been updated while the processor was stalled. */

			// if (runtime.cpu.stalled)
			// {
			// 	npc_not_valid = 1;
			// }
		// }
	}
	break;
	case SPR_PICSR:
		// if (!config.pic.edge_trigger)
			/* When configured with level triggered interrupts we clear PICSR in PIC
	 peripheral model when incoming IRQ goes low */
			// sprs[SPR_PICSR] = prev_val;
		break;
	case SPR_PICMR:
		/* If we have non-maskable interrupts, then the bottom two bits are always
       one. */
		// if (config.pic.use_nmi)
		// {
		// 	sprs[SPR_PICMR] |= 0x00000003;
		// }

		// if (sprs[SPR_SR] & SPR_SR_IEE)
		// 	pic_ints_en();
		break;
	case SPR_PMR:
		/* PMR[SDF] and PMR[DCGE] are ignored completely. */
		// if (config.pm.enabled && (value & SPR_PMR_SUME))
		// {
		// 	PRINTF("SUSPEND: PMR[SUME] bit was set.\n");
		// 	sim_done();
		// }
		break;
	default:
		/* Mask reserved bits in DTLBMR and DTLBMR registers */
		// if ((regno >= SPR_DTLBMR_BASE(0)) && (regno < SPR_DTLBTR_LAST(3)))
		// {
		// 	if ((regno & 0xff) < 0x80)
		// 		sprs[regno] = DADDR_PAGE(value) |
		// 								(value & (SPR_DTLBMR_V | SPR_DTLBMR_PL1 | SPR_DTLBMR_CID | SPR_DTLBMR_LRU));
		// 	else
		// 		sprs[regno] = DADDR_PAGE(value) |
		// 								(value & (SPR_DTLBTR_CC | SPR_DTLBTR_CI | SPR_DTLBTR_WBC | SPR_DTLBTR_WOM |
		// 										  SPR_DTLBTR_A | SPR_DTLBTR_D | SPR_DTLBTR_URE | SPR_DTLBTR_UWE | SPR_DTLBTR_SRE |
		// 										  SPR_DTLBTR_SWE));
		// }

		/* Mask reseved bits in ITLBMR and ITLBMR registers */
		// if ((regno >= SPR_ITLBMR_BASE(0)) && (regno < SPR_ITLBTR_LAST(3)))
		// {
		// 	if ((regno & 0xff) < 0x80)
		// 		sprs[regno] = IADDR_PAGE(value) |
		// 								(value & (SPR_ITLBMR_V | SPR_ITLBMR_PL1 | SPR_ITLBMR_CID | SPR_ITLBMR_LRU));
		// 	else
		// 		sprs[regno] = IADDR_PAGE(value) |
		// 								(value & (SPR_ITLBTR_CC | SPR_ITLBTR_CI | SPR_ITLBTR_WBC | SPR_ITLBTR_WOM |
		// 										  SPR_ITLBTR_A | SPR_ITLBTR_D | SPR_ITLBTR_SXE | SPR_ITLBTR_UXE));
		// }

		/* Links to GPRS */
		// if (regno >= 0x0400 && regno < 0x0420)
		// {
		// 	reg[regno - 0x0400] = value;
		// }
		break;
	}
}

/* Get a specific SPR. */
uorreg_t CPU::mfspr(const uint16_t regno)
{
	return 0;
}

uint32_t CPU::get_mem32(oraddr_t memaddr)
{
	return 0;
}

uint16_t CPU::get_mem16(oraddr_t memaddr)
{
	return 0;
}

uint8_t CPU::get_mem8(oraddr_t memaddr)
{
	return 0;
}

void CPU::set_mem32(oraddr_t memaddr, uint32_t value)
{

}

void CPU::set_mem16(oraddr_t memaddr, uint16_t value)
{
	
}

void CPU::set_mem8(oraddr_t memaddr, uint8_t value)
{
	
}

uint32_t CPU::ffs(uint32_t reg)
{
	return 0;
}