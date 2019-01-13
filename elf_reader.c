#include <stdio.h>
#include <stdlib.h>
#include "elf_general.h"
#include "types.h"

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

unsigned char copy_to_mem(FILE *fp, unsigned int *rom_mem, Elf32_Ehdr *elf_hdr)
{
    Elf32_Shdr *elf_shdr_array;
	unsigned short num_section_header = 0;
	unsigned short size_section_header = 0;
    struct elf32_phdr *elf_phdata = NULL;

    num_section_header = CONVERT_SHORT(elf_hdr->e_shnum);
	size_section_header = CONVERT_SHORT(elf_hdr->e_shentsize);

	elf_shdr_array = (Elf32_Shdr *)malloc(size_section_header*num_section_header);
    if (fread(elf_shdr_array, size_section_header*num_section_header, 1, fp) != 1)
	{
		printf("error!! read elf\n");
		return false;
	}

    unsigned short phnum = 0;
	unsigned short phentsize = 0;

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

unsigned char load_code(char* filename, unsigned int *rom_mem)
{
    FILE *fp = NULL;
    Elf32_Ehdr *elf_hdr;
    
    fp = fopen(filename, "r+b");
    if(fp != NULL)
    {
        elf_hdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
        if (fread(elf_hdr, sizeof(Elf32_Ehdr), 1, fp) != 1)
        {
            printf("error!! read elf header error\n");
        }

        if(!print_elf_hdr(elf_hdr))
        {
            fclose(fp);
            return false;
        }
    }
    else
    {
        fclose(fp);
        return false;
    }
}