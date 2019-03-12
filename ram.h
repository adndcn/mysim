#pragma once

#include "bus_device.h"

//1MB
#define RAM_SIZE 0x100000

class RAM: public BUS_DEVICE
{
public:
    unsigned int size;
    unsigned char mem[RAM_SIZE];

    uint8_t Read8(oraddr_t phys_addr, oraddr_t memaddr);
    uint16_t Read16(oraddr_t phys_addr, oraddr_t memaddr);
    uint32_t Read32(oraddr_t phys_addr, oraddr_t memaddr);

    void Write8(oraddr_t memaddr, oraddr_t vaddr, uint8_t value);
    void Write16(oraddr_t memaddr, oraddr_t vaddr, uint16_t value);
    void Write32(oraddr_t memaddr, oraddr_t vaddr, uint32_t value);
};