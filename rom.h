#pragma once

#include "bus_device.h"

#define ROM_SIZE 0x100000

class ROM: public BUS_DEVICE
{
public:
    unsigned int size;
    unsigned char mem[ROM_SIZE];
};