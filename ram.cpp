#include "ram.h"

uint8_t RAM::Read8(oraddr_t phys_addr, oraddr_t memaddr)
{
    uint8_t data = 0;
    data = mem[phys_addr];

    return data;
}


uint16_t RAM::Read16(oraddr_t phys_addr, oraddr_t memaddr)
{
    uint16_t data = 0;
    int i = 0;
    for(i=0; i<2; i++)
    {
        data |= mem[phys_addr + i]<<(8*(1-i));
    }
    
    return data;
}


uint32_t RAM::Read32(oraddr_t phys_addr, oraddr_t memaddr)
{
    uint32_t data = 0;
    int i = 0;
    for(i=0; i<4; i++)
    {
        data |= mem[phys_addr + i]<<(8*(3-i));
    }
    
    return data;

}

void RAM::Write8(oraddr_t memaddr, oraddr_t vaddr, uint8_t value)
{

}
void RAM::Write16(oraddr_t memaddr, oraddr_t vaddr, uint16_t value)
{

}
void RAM::Write32(oraddr_t memaddr, oraddr_t vaddr, uint32_t value)
{
    
}

