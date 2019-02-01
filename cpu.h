#pragma once

#include "types.h"
#include "spr-defs.h"

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

struct cpu_state {
	uorreg_t             reg[MAX_GPRS];	/*!< General purpose registers */
	uorreg_t             sprs[MAX_SPRS];	/*!< Special purpose registers */
	oraddr_t             insn_ea;		/*!< EA of instrs that have an EA */
	int                  delay_insn;	/*!< Is current instr in delay slot */
	int                  npc_not_valid;	/*!< NPC updated while stalled */
	oraddr_t             pc;		/*!< PC (and translated PC) */
	oraddr_t             pc_delay;	/*!< Delay instr EA register */
	struct iqueue_entry  iqueue;		/*!< Decode of just executed instr */
	//struct iqueue_entry  icomplet;        /*!< Decode of instr before this */
	int                  loadlock_active; /*!< A load lock is active */
	oraddr_t             loadlock_addr;   /*!< Address of the load lock */
};

struct config
{
	struct
	{
		int hardfloat;      /* whether hardfloat is enabled */
	}cpu;
};

extern struct cpu_state cpu_state;
extern oraddr_t          pcnext;
extern int next_delay_insn;

void setsim_reg(unsigned int regno, uorreg_t value);
