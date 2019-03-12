#include "bus.h"

BUS_DEVICE *BUS::get_device(oraddr_t phys_addr)
{
    if(phys_addr < RAM_SIZE)
    {
        return ram_p;
    }
    else
    {
        //todo return error device
        return NULL;
    }
    
}