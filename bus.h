#pragma once

#include "ram.h"
#include "rom.h"
#include "types.h"

class BUS
{
public:
    RAM * ram_p;
    ROM * rom_p;

    RAM *GetRAM();
    BUS_DEVICE *get_device(oraddr_t phys_addr);
};