#include <stdio.h>
#include "types.h"
// #include "elf_reader.h"
// #include "mem.h"
// #include "config.h"
// #include "abstract.h"
#include "system.h"

int main()
{
	//init_config();

	//sim_init();

	char * filename = "F:/learn_field/mysim/build/simple_test.elf";
	//char * filename = "hello.elf";
	SYSTEM system;
	system.load_code(filename, system.ram_p->mem);

	system.init();
	system.run();
	// exec_main();
}