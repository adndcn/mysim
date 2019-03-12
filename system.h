#pragma once

#include "types.h"
#include "cpu.h"
#include "bus.h"
#include "ram.h"
#include "rom.h"

class SYSTEM
{
public:
    SYSTEM();
    ~SYSTEM();

    CPU* cpu_p;
    BUS* bus_p;
    RAM* ram_p;
    ROM* rom_p;
    unsigned char load_code(char* filename, unsigned char *rom_mem);
    void run();
    void init();
};


