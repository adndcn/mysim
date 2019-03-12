cmake_minimum_required(VERSION 2.8)

project(mysim)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# 指定生成目标
# add_executable(mysim main.c elf_reader.c mem.c abstract.c execgen.c cpu.c except.c dmmu.c config.c tick.c pm.c)
add_executable(mysim main.cpp elf_reader.cpp system.cpp bus_device.cpp 
                bus.cpp cache_model.cpp cpu.cpp except.cpp execgen.cpp mmu.cpp ram.cpp)

