#pragma once

#include "types.h"

/*! Instruction queue */
struct iqueue_entry
{
	int       insn_index;
	uint32_t  insn;
	oraddr_t  insn_addr;
};

void analysis(struct iqueue_entry *current);
void btic_update(oraddr_t targetaddr);
void mtspr(uint16_t regno, const uorreg_t value);
uorreg_t mfspr(const uint16_t regno);
void l_invalid();
int ffs(uorreg_t reg);
void add_program(oraddr_t address, uint32_t insn);
