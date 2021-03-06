
#include "types.h"
#include "elf_reader.h"

unsigned char print_elf_hdr(Elf32_Ehdr * elf_hdr)
{
	int i = 0;
	unsigned char endian_flag = 0;
	printf("e_ident start:\n");
	printf("\tmagic number:\n");
	
	if ((elf_hdr->e_ident[0] != 0x7f) || (elf_hdr->e_ident[1] != 'E') ||
		(elf_hdr->e_ident[2] != 'L') || (elf_hdr->e_ident[3] != 'F'))
	{
		return false;
	}

	//print 0x7f E L F
	printf("\t0x%x ", elf_hdr->e_ident[0]);
	for (i = 1; i < 4; i++)
	{
		printf("%c ", elf_hdr->e_ident[i]);
	}
	printf("\n");

	//class
	printf("\tclass:\n");
	switch (elf_hdr->e_ident[4])
	{
	case 0:
		printf("\t0 illegal objective file\n");
		break;
	case 1:
		printf("\t1 32bit objective file\n");
		break;
	case 2:
		printf("\t2 64bit objective file\n");
		break;
	}
	
	printf("\tdata:\n");
	switch (elf_hdr->e_ident[5])
	{
	case 0:
		endian_flag = 0;
		printf("\t0 illegal coding format\n");
		break;
	case 1:
		endian_flag = 1;
		printf("\t1 little endian\n");
		break;
	case 2:
		endian_flag = 2;
		printf("\t2 big endian\n");
		break;
	}

	printf("\tversion: %d\n", elf_hdr->e_ident[6]);

	printf("\tunused:\n");
	printf("\t");
	for (i = 7; i < 16; i++)
	{
		printf("%d ", elf_hdr->e_ident[i]);
	}
	printf("\n");
	printf("e_ident end\n");

	printf("file type: ");
	switch (CONVERT_SHORT(elf_hdr->e_type))
	{
	case 0:
		printf("unknown file type\n");
		break;
	case 1:
		printf("relocate file type\n");
		break;
	case 2:
		printf("executable file type\n");
		break;
	case 3:
		printf("dll file type\n");
		break;
	case 4:
		printf("core file type\n");
		break;
	case 0xff00:
		printf("LOPROC\n");
		break;
	case 0xffff:
		printf("HIPROC\n");
		break;
	}

	printf("machine type: ");
	switch (CONVERT_SHORT(elf_hdr->e_machine))
	{
	case 0:
		printf("none type\n");
		break;
	case 1:
		printf("AT&T WE 3210\n");
		break;
	case 2:
		printf("SPARC\n");
		break;
	case 3:
		printf("intel\n");
		break;
	case 4:
		printf("motorola 6800\n");
		break;
	case 5:
		printf("motorola 8800\n");
		break;
	case 7:
		printf("intel 8086\n");
		break;
	case 8:
		printf("MIPS RS3000 Big-Endian\n");
		break;
	case 10:
		printf("MIPS RS3000 Big-Endian\n");
		break;
	default:
		printf("%d\n", CONVERT_SHORT(elf_hdr->e_machine));
		break;
	}

	printf("version: 0x%x\n", CONVERT_INT(elf_hdr->e_version));
	printf("e_entry: 0x%x\n", CONVERT_INT(elf_hdr->e_entry));
	printf("e_phoff: 0x%x\n", CONVERT_INT(elf_hdr->e_phoff));
	printf("e_shoff: 0x%x\n", CONVERT_INT(elf_hdr->e_shoff));
	printf("e_flags: 0x%x\n", CONVERT_INT(elf_hdr->e_flags));
	printf("e_ehsize: 0x%x\n", CONVERT_SHORT(elf_hdr->e_ehsize));
	printf("e_phentsize: 0x%x\n", CONVERT_SHORT(elf_hdr->e_phentsize));
	printf("e_phnum: 0x%x\n", CONVERT_SHORT(elf_hdr->e_phnum));
	printf("e_shentsize: 0x%x\n", CONVERT_SHORT(elf_hdr->e_shentsize));
	printf("e_shnum: 0x%x\n", CONVERT_SHORT(elf_hdr->e_shnum));
	printf("e_shstrndx: 0x%x\n", CONVERT_SHORT(elf_hdr->e_shstrndx));

	return true;
}

unsigned char print_section_name(FILE *fp, Elf32_Ehdr *elf_hdr)
{
	Elf32_Shdr *elf_shdr_array;
	unsigned short num_section_header = 0;
	unsigned short size_section_header = 0;

	fseek(fp, CONVERT_INT(elf_hdr->e_shoff), SEEK_SET);

	num_section_header = CONVERT_SHORT(elf_hdr->e_shnum);
	size_section_header = CONVERT_SHORT(elf_hdr->e_shentsize);
	elf_shdr_array = (Elf32_Shdr *)malloc(size_section_header*num_section_header);
	if (fread(elf_shdr_array, size_section_header*num_section_header, 1, fp) != 1)
	{
		printf("error!! read elf\n");
		return false;
	}

	//section name table
	unsigned short shstrndx = CONVERT_SHORT(elf_hdr->e_shstrndx);
	fseek(fp, CONVERT_INT(elf_shdr_array[shstrndx].sh_offset), SEEK_SET);
	char * shstrtab = (char *)malloc(CONVERT_INT(elf_shdr_array[shstrndx].sh_size));
	if (fread(shstrtab, CONVERT_INT(elf_shdr_array[shstrndx].sh_size), 1, fp) != 1)
	{
		printf("error!! read elf\n");
		return false;
	}

	//print all section name from section name table
	for(int i = 0; i < num_section_header; i++)
	{
		unsigned int name_index = CONVERT_INT(elf_shdr_array[i].sh_name);
		if(name_index != 0)
			printf("name = %s, addr = 0x%x\n", shstrtab + name_index, CONVERT_INT(elf_shdr_array[i].sh_addr));
		else
			printf("name = noname, addr = 0x%x\n", CONVERT_INT(elf_shdr_array[i].sh_addr));
	}

}

unsigned char copy_to_mem(FILE *fp, Elf32_Ehdr *elf_hdr, unsigned char *rom_mem)
{
    Elf32_Shdr *elf_shdr_array;
	unsigned short num_section_header = 0;
	unsigned short size_section_header = 0;
    struct elf32_phdr *elf_phdata = NULL;
	unsigned int padd = 0;
	unsigned int freemem_addr = 0;
	unsigned int sectsize = 0;
	unsigned int insn;

    num_section_header = CONVERT_SHORT(elf_hdr->e_shnum);
	size_section_header = CONVERT_SHORT(elf_hdr->e_shentsize);
	fseek(fp, CONVERT_INT(elf_hdr->e_shoff), SEEK_SET);
	elf_shdr_array = (Elf32_Shdr *)malloc(size_section_header*num_section_header);

    if (fread(elf_shdr_array, size_section_header*num_section_header, 1, fp) != 1)
	{
		printf("error!! read elf\n");
		return false;
	}

	unsigned short phnum = 0;
	unsigned short phentsize = 0;
	//load segment
	if(CONVERT_INT(elf_hdr->e_phoff))
	{
		phnum = CONVERT_SHORT(elf_hdr->e_phnum);
		phentsize = CONVERT_SHORT(elf_hdr->e_phentsize);

		elf_phdata = (struct elf32_phdr *)malloc(phnum * phentsize);

		fseek(fp, CONVERT_INT(elf_hdr->e_phoff), SEEK_SET);

		if (fread(elf_phdata, 1, phnum*phentsize, fp) != phnum*phentsize)
		{
			printf("error!! read elf error\n");
			return false;
		}
	}

	//section name table
	unsigned short shstrndx = CONVERT_SHORT(elf_hdr->e_shstrndx);
	fseek(fp, CONVERT_INT(elf_shdr_array[shstrndx].sh_offset), SEEK_SET);
	char * shstrtab = (char *)malloc(CONVERT_INT(elf_shdr_array[shstrndx].sh_size));
	if (fread(shstrtab, CONVERT_INT(elf_shdr_array[shstrndx].sh_size), 1, fp) != 1)
	{
		printf("error!! read elf\n");
		return false;
	}

	for(int i = 0; i<num_section_header; i++)
	{
#ifdef __PRINT_DEBUG__
		unsigned int name_index = CONVERT_INT(elf_shdr_array[i].sh_name);
		printf("name = %s\n", shstrtab + name_index);
#endif
		if( (CONVERT_INT(elf_shdr_array[i].sh_type) & SHT_PROGBITS) && (CONVERT_INT(elf_shdr_array[i].sh_flags) & SHF_ALLOC) )
		{
			//segment covers section
			padd = CONVERT_INT(elf_shdr_array[i].sh_addr);
			for(int j = 0; j<phnum; j++)
			{
				if(CONVERT_INT(elf_phdata[j].p_offset) && 
					(CONVERT_INT(elf_phdata[j].p_offset) <= CONVERT_INT(elf_shdr_array[i].sh_offset)) &&
					((CONVERT_INT(elf_phdata[j].p_offset) + CONVERT_INT(elf_phdata[j].p_memsz)) > CONVERT_INT(elf_shdr_array[i].sh_offset)) )
				padd = CONVERT_INT(elf_phdata[j].p_paddr) + CONVERT_INT(elf_shdr_array[i].sh_offset) - CONVERT_INT(elf_phdata[j].p_offset);
			}

			freemem_addr = padd;
			sectsize = CONVERT_INT(elf_shdr_array[i].sh_size);

			fseek(fp, CONVERT_INT(elf_shdr_array[i].sh_offset), SEEK_SET);

			while(sectsize>0 && fread(&insn, sizeof(insn), 1, fp))
			{
				insn = CONVERT_INT(insn);
				//add program to program mem
				//based on section
				add_program(freemem_addr, insn, rom_mem);
#ifdef __PRINT_DEBUG__
				printf("0x%02x%02x%02x%02x\n", program_mem[freemem+0], program_mem[freemem+1], program_mem[freemem+2], program_mem[freemem+3]);
#endif
				sectsize-=4;
				freemem_addr+=4;
			}

		}
	}
    
}

void set_program8(unsigned char *rom_mem, oraddr_t memaddr, uint8_t value)
{
	rom_mem[memaddr] = value;
}

void add_program(oraddr_t address, uint32_t insn, unsigned char *rom_mem)
{
	set_program8(rom_mem, address, (insn >> 24) & 0xff);
	set_program8(rom_mem, address + 1, (insn >> 16) & 0xff);
	set_program8(rom_mem, address + 2, (insn >> 8) & 0xff);
	set_program8(rom_mem, address + 3, insn & 0xff);
}



