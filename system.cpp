#include "system.h"
#include "elf_reader.h"
#include <stdio.h>
#include <stdlib.h>

SYSTEM::SYSTEM()
{
    ram_p = new RAM;
    rom_p = new ROM;

    bus_p = new BUS;
    bus_p->ram_p = ram_p;
    bus_p->rom_p = rom_p;

    cpu_p = new CPU(bus_p);
    
    
}

SYSTEM::~SYSTEM()
{
    delete cpu_p;
    delete bus_p;
    delete ram_p;
    delete rom_p;
}

unsigned char SYSTEM::load_code(char* filename, unsigned char *rom_mem)
{
    FILE *fp = NULL;
    Elf32_Ehdr *elf_hdr;
    
    fp = fopen(filename, "r+b");
    if(fp != NULL)
    {
        elf_hdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
        if (fread(elf_hdr, sizeof(Elf32_Ehdr), 1, fp) != 1)
        {
            printf("error!! read elf header error\n");
        }

        if(!print_elf_hdr(elf_hdr))
        {
            fclose(fp);
            return false;
        }

		print_section_name(fp, elf_hdr);
		copy_to_mem(fp, elf_hdr, rom_mem);
    }
    else
    {
        fclose(fp);
        return false;
    }
}

void SYSTEM::run()
{
    while(1)
    {
        cpu_p->cpu_clock();
    }
}

void SYSTEM::init()
{
    cpu_p->cpu_reset();
}