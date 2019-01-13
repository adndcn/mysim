#pragma once
#include "abstract.h"

uint8_t eval_mem8(oraddr_t memaddr, int *breakpoint);
uint16_t eval_mem16(oraddr_t memaddr, int *breakpoint);
uint32_t eval_mem32(oraddr_t memaddr, int *breakpoint);

void set_mem8(oraddr_t memaddr, uint8_t value, int *breakpoint);
void set_mem16(oraddr_t memaddr, uint16_t value, int *breakpoint);
void set_mem32(oraddr_t memaddr, uint32_t value, int *breakpoint);
