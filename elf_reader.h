#pragma once
#include "elf_general.h"
#include <stdio.h>
#include <stdlib.h>


unsigned char print_elf_hdr(Elf32_Ehdr * elf_hdr);
unsigned char print_section_name(FILE *fp, Elf32_Ehdr *elf_hdr);
unsigned char copy_to_mem(FILE *fp, Elf32_Ehdr *elf_hdr, unsigned char *rom_mem);
void add_program(oraddr_t address, uint32_t insn, unsigned char *rom_mem);
void set_program8(unsigned char *rom_mem, oraddr_t memaddr, uint8_t value);


