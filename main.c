#include <stdio.h>
#include "types.h"
#include "elf_reader.h"

void main()
{
	char * filename = "hello.elf";
	load_code(filename);
}