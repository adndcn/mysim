#pragma once

#include "types.h"
#include "spr-defs.h"
#include "bus_device.h"
#include "mmu.h"
#include "cache_model.h"
#include "except.h"
#include "bus.h"

#define MAX_GPRS 32
#define PAGE_SIZE 8192

/*! Index of the link register */
#define LINK_REGNO     9

/*! Instruction queue */
struct iqueue_entry
{
	int       insn_index;
	uint32_t  insn;
	oraddr_t  insn_addr;
};

struct config
{
	struct
	{
		int hardfloat;      /* whether hardfloat is enabled */
	}cpu;
};

class CPU
{
public:
	CPU(BUS* bus_pointer);
	~CPU();

	uorreg_t             reg[MAX_GPRS];	/*!< General purpose registers */
	uorreg_t             sprs[MAX_SPRS];	/*!< Special purpose registers */
	oraddr_t             insn_ea;		/*!< EA of instrs that have an EA */
	int                  delay_insn;	/*!< Is current instr in delay slot */
	int                  npc_not_valid;	/*!< NPC updated while stalled */
	oraddr_t             pc;		/*!< PC (and translated PC) */
	oraddr_t             pc_delay;	/*!< Delay instr EA register */
	struct iqueue_entry  iqueue;		/*!< Decode of just executed instr */
	int next_delay_insn;
	/*! Temporary program counter. Globally available */
	oraddr_t pcnext;

	int loadlock_active = 0;
	int loadlock_addr = 0;


	//debug
	struct config config;
	int do_stats;


	EXCEPT except;
	IMMU immu;
	DMMU dmmu;
	ICACHE icache;
	DCACHE dcache;

	BUS* bus_p;

	void cpu_reset();
	void setsim_reg(unsigned int regno, uorreg_t value);
	void decode_execute(struct iqueue_entry *current);
	int fetch();
	void update_pc();
	uint32_t get_insn(oraddr_t memaddr);
	void cpu_clock();
	void mtspr(uint16_t regno, const uorreg_t value);
	uorreg_t mfspr(const uint16_t regno);
	uint32_t get_mem32(oraddr_t memaddr);
	uint16_t get_mem16(oraddr_t memaddr);
	uint8_t get_mem8(oraddr_t memaddr);

	void set_mem32(oraddr_t memaddr, uint32_t value);
	void set_mem16(oraddr_t memaddr, uint16_t value);
	void set_mem8(oraddr_t memaddr, uint8_t value);

	uint32_t ffs(uint32_t reg);

	//debug function
	void analysis(struct iqueue_entry *current);
	void l_invalid();
	void btic_update(oraddr_t pc);
};


