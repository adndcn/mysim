#pragma once

#include "types.h"

class BUS_DEVICE
{
public:
    BUS_DEVICE();
    ~BUS_DEVICE();

    virtual uint8_t Read8(oraddr_t phys_addr, oraddr_t memaddr){return 0;};
    virtual uint16_t Read16(oraddr_t phys_addr, oraddr_t memaddr){return 0;};
    virtual uint32_t Read32(oraddr_t phys_addr, oraddr_t memaddr){return 0;};

    virtual void Write8(oraddr_t memaddr, oraddr_t vaddr, uint8_t value){};
    virtual void Write16(oraddr_t memaddr, oraddr_t vaddr, uint16_t value){};
    virtual void Write32(oraddr_t memaddr, oraddr_t vaddr, uint32_t value){};
    
};