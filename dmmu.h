#pragma once
#include "abstract.h"
#include "cpu.h"

oraddr_t dmmu_translate(oraddr_t virtaddr, int write_access);