#pragma once

#include "types.h"
#include "cpu.h"

void analysis(struct iqueue_entry *current);
void btic_update(oraddr_t targetaddr);
void mtspr(uint16_t regno, const uorreg_t value);
uorreg_t mfspr(const uint16_t regno);
void l_invalid();
int ffs(uorreg_t reg);
void add_program(oraddr_t address, uint32_t insn);

void sim_init();
void exec_main();
