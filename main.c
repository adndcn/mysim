#include <stdio.h>
#include "types.h"
#include "elf_reader.h"
#include "mem.h"
#include "config.h"
#include "abstract.h"

void main()
{
	init_config();

	sim_init();

	//char * filename = "F:/learn_field/mysim/build/hello.elf";
	char * filename = "hello.elf";
	load_code(filename);


}