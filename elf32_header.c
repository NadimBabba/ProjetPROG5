#include<stdio.h> 
#include<fcntl.h> 
#include<errno.h> 
#include <unistd.h>

#include "elf32.h"

void write_elf32_hdr(Elf32_Ehdr* elfhdr) {
    printf("ELF Header:\n");
    printf("Magic:   7f 45 4c 46 01 02 01 00 00 00 00 00 00 00 00 00\n");
    printf("Class:                             %s\n", (elfhdr->e_ident[EI_CLASS] == ELFCLASS32) ? "ELF32": "ELF64");
    printf("Data:                              %s\n", "2's complement, big endian");
    printf("Version:                           %s\n", (elfhdr->e_version == EV_NONE)? "Invalid version" : "1 (current)");
    printf("OS/ABI:                            %s\n", "UNIX - System V");
    printf("ABI Version:                       %d\n", 0);
    printf("Type:                              ");
    switch (elfhdr->e_type) 
    {
        case ET_NONE_type : printf("NONE"); break;
        case ET_REL: printf("REL (Relocatable file)\n"); break;
        case ET_EXEC: printf("EXEC (Executable file)\n"); break;
        case ET_DYN: printf("DYN (Shared object file)\n"); break;
        case ET_CORE: printf("CORE (Core file)\n"); break;
        case ET_LOPROC: printf("LOPROC (Processor-specific)\n"); break;
        case ET_HIPROC: printf("HIPROC (Processor-specific)\n"); break;
        default: printf("%d Error Unknown type\n", elfhdr->e_type); break;
    }
    printf("Machine:                           ");
    switch (elfhdr->e_type) 
    {
        case ET_NONE_type : printf("NONE"); break;
        case EM_M32: printf("AT&T WE 32100\n"); break;
        case EM_SPARC: printf("SPARC\n"); break;
        case EM_386: printf("Intel Architecture\n"); break;
        case EM_68K: printf("Motorola 68000\n"); break;
        case EM_88K: printf("Motorola 88000\n"); break;
        case EM_860: printf("Intel 80860\n"); break;
        case EM_MIPS: printf("MIPS RS3000 Big-Endian\n"); break;
        case EM_MIPS_RS4_BE: printf("MIPS RS4000 Big-Endian\n"); break;
        case EM_ARM: printf("MIPS RS4000 Big-Endian\n"); break;
        default: printf("%d Error Machine\n", elfhdr->e_type); break;
    }
    printf("Version:                           %x\n", elfhdr->e_version);
    printf("Entry point address:               %x\n", elfhdr->e_entry);
    printf("Start of program headers:          %d %s\n", elfhdr->e_phoff , "(bytes into file)");
    printf("Start of section headers:          %d %s\n", elfhdr->e_shoff , "(bytes into file)");
    printf("Flags:                             ");
    switch (elfhdr->e_flags) 
    {
        //case ET_NONE_type : printf("NONE"); break;
        case EF_ARM_ABIMASK: printf("%x %s\n", elfhdr->e_flags, "Version5 EABI"); break;
        case EF_ARM_BE8: printf("BE_8 code v6 processor\n"); break;
        case EF_ARM_GCCMASK: printf("(EABI version 4 and earlier)\n"); break;
        case EF_ARM_ABI_FLOAT_HARD: printf("(ABI version 5 and later)\n"); break;
        case EF_ARM_ABI_FLOAT_SOFT: printf("(EABI version 5 and earlier)\n"); break;

        default: printf("%x Unknown Flags\n", elfhdr->e_flags); break;
    }
    
    printf("Size of this header:               %d (bytes)\n", elfhdr->e_ehsize);
    printf("Size of program headers:           %d (bytes)\n", elfhdr->e_phentsize);
    printf("Number of program headers:         %d\n", elfhdr->e_phnum);
    printf("Size of section headers:           %d (bytes)\n", elfhdr->e_shentsize);
    printf("Number of section headers:         %d\n", elfhdr->e_shnum);
    printf("Section header string table index: %d\n", elfhdr->e_shstrndx);

}
int read_elf32_hdr (char* elf_failename, Elf32_Ehdr *elfhdr) {
    int fd, sz;

    
    fd = open(elf_failename, O_RDONLY); 

    if( fd < 0 ) {
        printf( " Error reading elf file\n");
        return 0;
    }
    sz = read(fd, (char*) (elfhdr), sizeof(Elf32_Ehdr));
    close (fd);
    if (sz == 0) {
        printf( " Error reading elf header\n");
        return 0;
    }

    return 1;
}

int main(int argc, char *argv[]) 
{
    Elf32_Ehdr elfhdr;

    //printf ("Elf32_Addr %ld // 4 4 Unsigned program address\n", sizeof(Elf32_Addr)) ; 
    //printf ("Elf32_Half %ld //2 2 Unsigned medium intege\n", sizeof(Elf32_Half)) ; 
    //printf ("Elf32_Off %ld //4 4 Unsigned file offset\n", sizeof(Elf32_Off)) ; 
    //printf ("Elf32_Sword %ld //4 4 Signed large integer\n",  sizeof(Elf32_Sword)) ; 
    //printf ("Elf32_Word %ld // 4 4 Unsigned large integer\n", sizeof(Elf32_Word)); 

    if (argc < 2) {
        printf("Usage : elf_header <ELF File name>\n");
        return 1;
    }
    printf ("Reading ELF file %s \n", argv[1]);

    if ( read_elf32_hdr(argv[1], &elfhdr))
    write_elf32_hdr(&elfhdr);
    
    return 0;

}
