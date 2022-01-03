#include<stdio.h> 
#include<fcntl.h> 
#include<errno.h> 
#include<string.h> 
#include <malloc.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

//#include <elf.h>

#include "elf32.h"

void write_elf32_hdr(Elf32_Ehdr* elfhdr) {
    int i;
    printf("ELF Header:\n");
    printf("  Magic:  ");
    for (i=0; i<EI_NIDENT ; i++) printf(" %02x", elfhdr->e_ident[i]);
    printf(" \n");
    printf("  Class:                             %s\n", (elfhdr->e_ident[EI_CLASS] == ELFCLASS32) ? "ELF32": "ELF64");
    printf("  Data:                              ");
        switch (elfhdr->e_ident[5]) 
    {

        case ELFDATA2LSB : printf("2's complement, little endian\n"); break;        
        case ELFDATA2MSB : printf("2's complement, big endian\n"); break;
        default : printf("Invalid Dat encoding\n"); break;    
    }

    printf("  Version:                           %s\n", (elfhdr->e_version == EV_NONE)? "Invalid version" : "1 (current)");
    printf("  OS/ABI:                            ");
    switch (elfhdr->e_ident[EI_OSABI]) {
      case 0x00 : printf("UNIX - System V\n");break;
      case 0x01 : printf("HP_UX\n");break;
      default : printf("OS LIST TO COMPLETE\n");break;
    }
    printf("  ABI Version:                       %x\n", elfhdr->e_ident[EI_ABIVERSION]); 
    printf("  Type:                              ");
    switch (elfhdr->e_type) 
    {
        case ET_NONE : printf("NONE"); break;
        case ET_REL: printf("REL (Relocatable file)\n"); break;
        case ET_EXEC: printf("EXEC (Executable file)\n"); break;
        case ET_DYN: printf("DYN (Shared object file)\n"); break;
        case ET_CORE: printf("CORE (Core file)\n"); break;
        case ET_LOPROC: printf("LOPROC (Processor-specific)\n"); break;
        case ET_HIPROC: printf("HIPROC (Processor-specific)\n"); break;
        default: printf("%x Error Unknown type\n", elfhdr->e_type); break;
    }
    printf("  Machine:                           ");
    switch (elfhdr->e_machine) 
    {
        case ET_NONE : printf("NONE"); break;
        case EM_M32: printf("AT&T WE 32100\n"); break;
        case EM_SPARC: printf("SPARC\n"); break;
        case EM_386: printf("Intel Architecture\n"); break;
        case EM_68K: printf("Motorola 68000\n"); break;
        case EM_88K: printf("Motorola 88000\n"); break;
        case EM_860: printf("Intel 80860\n"); break;
        case EM_MIPS: printf("MIPS RS3000 Big-Endian\n"); break;
        case EM_ARM: printf("ARM\n"); break;
        default: printf("%x Error Machine !\n", elfhdr->e_machine); break;
    }
    printf("  Version:                           0x%0x\n", (elfhdr->e_version == EV_NONE)? 0:1);
    printf("  Entry point address:               0x%x\n", elfhdr->e_entry);
    printf("  Start of program headers:          %d %s\n", elfhdr->e_phoff , "(bytes into file)");
    printf("  Start of section headers:          %d %s\n",  offset(elfhdr->e_shoff), "(bytes into file)");
    printf("  Flags:                             ");
    switch (elfhdr->e_flags) 
    {
        case ET_NONE_type : printf("NONE"); break;
        case EF_ARM_ABIMASK: printf("0x%X000000, %s\n", (Elf32_Word) elfhdr->e_flags, "Version5 EABI"); break;
        case EF_ARM_BE8: printf("BE_8 code v6 processor\n"); break;
        case EF_ARM_GCCMASK: printf("(EABI version 4 and earlier)\n"); break;
        case EF_ARM_ABI_FLOAT_HARD: printf("(ABI version 5 and later)\n"); break;
        case EF_ARM_ABI_FLOAT_SOFT: printf("(EABI version 5 and earlier)\n"); break;

        default: printf("%x Unknown Flags\n", elfhdr->e_flags); break;
    }
    
    printf("  Size of this header:               %d (bytes)\n", ELF32_R_SYM(elfhdr->e_ehsize));
    printf("  Size of program headers:           %d (bytes)\n", ELF32_R_SYM(elfhdr->e_phentsize));
    printf("  Number of program headers:         %d\n", elfhdr->e_phnum);
    printf("  Size of section headers:           %d (bytes)\n", ELF32_R_SYM(elfhdr->e_shentsize));
    printf("  Number of section headers:         %d\n", ELF32_R_SYM(elfhdr->e_shnum));
    printf("  Section header string table index: %d\n", ELF32_R_SYM(elfhdr->e_shstrndx));

}
Elf32_Shdr *ElfSheader(Elf32_Ehdr *hdr) {
	return (Elf32_Shdr *)((char*) hdr + offset(hdr->e_shoff) );
}
 
Elf32_Shdr *ElfSection(Elf32_Ehdr *hdr, int idx) {
	return &ElfSheader(hdr)[idx];
}
 

char* shstrtab;


int main(int argc, char *argv[]) 
{
    char* allelf;
    Elf32_Ehdr* elfhdr;
    int fd;

    if (argc < 3) {
        printf("Usage : elf_header option <ELF File name>\nWhere Option is :\n");
        printf("\t--file-header       Display the ELF file header\n");
        printf("\t--section-headers   Display the sections' header\n");
        return 1;
    }

    struct stat st;
    char* fname = argv[argc-1];
    if (stat(fname, &st) != 0) {
       perror("stat");
       return 1;
    }

    fd = open(fname, O_RDONLY); 

    if( fd < 0 ) {
        printf( " Error reading elf file %s \n", fname);
        return 1;
    }

    allelf = malloc(st.st_size);
    elfhdr = (Elf32_Ehdr*) allelf;    

    int sz;

    
    sz = read(fd, (char*) (allelf), st.st_size);
     if (sz == 0) {
        printf( " Error reading elf file\n");
        return 1;
    }
     //printf("nb byte read %d\n",sz);

 
     Elf32_Shdr *shrd0 = ElfSection(elfhdr, 0);
     Elf32_Shdr* sh_strtab = &shrd0[ELF32_R_SYM(elfhdr->e_shstrndx)];
     shstrtab = (char*) allelf + ELF32_R_VAL(sh_strtab->sh_offset) + 256;

     if (!strcmp(argv[1], "--file-header") || !strcmp(argv[1], "-h")) {
          write_elf32_hdr(elfhdr);
      }



    
    close (fd);
   return 0;

}