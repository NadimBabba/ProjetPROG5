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

void printsection(Elf32_Ehdr* elfhdr, int indx) {
    Elf32_Shdr *shrd = ElfSection(elfhdr, indx);
    if (indx==0) {
       printf("There are %d section headers, starting at offset 0x%x:\n\nSection Headers:\n",ELF32_R_SYM(elfhdr->e_shnum), offset(elfhdr->e_shoff) );
       printf("  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al\n");
     }
    printf ("  [%2d] ", indx);  
    char * strname = (char*) shstrtab + ELF32_R_VAL(shrd->sh_name);
    char strncopy[19];
    strncpy(strncopy, strname, 17); 
    strncopy[17]=0;
    printf ("%-17s", strncopy);   

    switch (ELF32_R_VAL(shrd->sh_type)) 
    {
      case SHT_NULL : printf (" %-15s", "NULL"); break;     
      case SHT_PROGBITS : printf (" %-15s", "PROGBITS"); break;      
      case SHT_SYMTAB : printf (" %-15s", "SYMTAB"); break;
      case SHT_STRTAB : if (!strcmp(strncopy, ".ARM.attributes"))
			    printf (" %-15s", "ARM_ATTRIBUTES"); 
                        else
			    printf (" %-15s", "STRTAB"); 
			break;
      case SHT_RELA : printf (" %-15s", "RELA"); break;
      case SHT_HASH : printf (" %-15s", "HASH"); break;
      case SHT_DYNAMIC : printf (" %-15s", "DYNAMIC"); break;
      case SHT_NOTE : printf (" %-15s", "NOTE"); break;
      case SHT_NOBITS : printf (" %-15s", "NOBITS"); break;
      case SHT_REL : printf (" %-15s", "REL"); break;
      default :printf (" 0x%x", shrd->sh_type); break;
    }
    printf (" %08x", shrd->sh_addr);
    printf (" %06x", offset(shrd->sh_offset));
    printf (" %06x", offset(shrd->sh_size));
    printf (" %02x", ELF32_R_VAL(shrd->sh_entsize));
    char FlagStr[4];
    int i =0;
    if (ELF32_R_VAL(shrd->sh_flags) & SHF_WRITE) FlagStr[i++] = 'W';
    if (ELF32_R_VAL(shrd->sh_flags) & SHF_ALLOC) FlagStr[i++] = 'A';
    if (ELF32_R_VAL(shrd->sh_flags) & SHF_EXECINSTR) FlagStr[i++] = 'X';
    FlagStr[i++] = '\0';
    printf ("  %-2s", FlagStr);
    printf (" %2d", ELF32_R_VAL(shrd->sh_link));
    printf ("  %2d", ELF32_R_VAL(shrd->sh_info));
    printf (" %2d\n", ELF32_R_VAL(shrd->sh_addralign));
}
void printhexasectionindex(Elf32_Ehdr* elfhdr, int indx) {
    Elf32_Shdr *shrd = ElfSection(elfhdr, indx);
    char * strname = (char*) shstrtab + ELF32_R_VAL(shrd->sh_name);
    printf("Hex dump of section '%s':\n", strname);
    char* shdata = (char*) elfhdr + ELF32_R_VAL(shrd->sh_offset) + 256;
    for (int i=0; i< offset(shrd->sh_size); ) {

       printf("  0x%08x ", i);
       int k=0;
       for (int j=0; j<16;j++) {
         if (i+j > offset(shrd->sh_size)) {
            printf("  ");
         } else {
           printf("%02x",  (shdata[i+j]));
         }
         k++;
         if (k ==4) { printf(" "); k=0;}
       }

       for (int j=0; j<16;j++) { 
         if (i+j > offset(shrd->sh_size)) break;       
         printf("%c",  (shdata[i+j]) ? (shdata[i+j]) : '.');
       }

       printf("\n");
       i=i+16;
    }
}
void printhexasectionname(Elf32_Ehdr* elfhdr, char* sname) {
    int indx = -1 ;
    for (int i=0; i<ELF32_R_SYM(elfhdr->e_shnum); i++) {
      Elf32_Shdr *shrd = ElfSection(elfhdr, i);
      char * strname = (char*) shstrtab + ELF32_R_VAL(shrd->sh_name);
      if (!strcmp(strname, sname)) {
         indx = i; 
         break;
       }
    }
    if (indx == -1) {
      printf("readelf: Warning: Section '%s' was not dumped because it does not exist!\n", sname);
      return;
    }
    printf("Hex dump of section %s %d\n", sname, indx);
}

void printsymboltab(Elf32_Ehdr* elfhdr) {
    int indx = -1 ;
    char * strname;
    for (int i=0; i<ELF32_R_SYM(elfhdr->e_shnum); i++) {
      Elf32_Shdr *shrd = ElfSection(elfhdr, i);
      strname = (char*) shstrtab + ELF32_R_VAL(shrd->sh_name);
      if (ELF32_R_VAL(shrd->sh_type) == SHT_SYMTAB) {
          indx = i; 
         break;
      }
    }
    if (indx == -1) {
       return;
    }

    Elf32_Shdr *symtab = ElfSection(elfhdr, indx);
    printf("\nSymbol table '%s' contains %d entries:\n", strname, ELF32_R_VAL(symtab->sh_size)/ELF32_R_VAL(symtab->sh_entsize));
    printf("   Num:    Value  Size Type    Bind   Vis      Ndx Name\n");
    char* symaddr = (char*)elfhdr + offset(symtab->sh_offset);


    Elf32_Sym *symbol;
    for(int i=0; i<ELF32_R_VAL(symtab->sh_size)/ELF32_R_VAL(symtab->sh_entsize); i++) {
      symbol = &((Elf32_Sym *)symaddr)[i];
      switch (symbol->st_shndx) {
        case SHN_ABS: {
		printf ("Absolute symbol\n");
                break;
	} 
        case SHN_UNDEF : 
        default:
               {
		Elf32_Shdr *strtab = ElfSection(elfhdr, ELF32_R_VAL(symtab->sh_link));
 		const char *name = (const char *)elfhdr + offset(strtab->sh_offset) + ELF32_R_VAL(symbol->st_name);
                printf("%6d: %08x %5x", i, ELF32_R_VAL(symbol->st_value), ELF32_R_VAL(symbol->st_size));
                switch(ELF32_ST_TYPE(symbol->st_info)) {
		    case STT_NOTYPE: printf (" NOTYPE "); break;
 		    case STT_OBJECT: printf (" OBJECT "); break;
 		    case STT_FUNC: printf   (" FUN    "); break;
  		    case STT_SECTION: printf(" SECTION"); break;
  		    case STT_FILE: printf   (" FILE   "); break;
 		    default: printf(" TODO OTHER BIND"); break;
                }
                switch(ELF32_ST_BIND(symbol->st_info)) {
		    case STB_LOCAL: printf (" LOCAL "); break;
 		    case STB_GLOBAL: printf(" GLOBAL"); break;
 		    case STB_WEAK: printf  (" WEAK  "); break;
 		    default: printf(" TODO OTHER BIND"); break;
                }
 
                printf(" DEFAULT");
		if (ELF32_ST_BIND(symbol->st_shndx))
                   printf(" %4d", ELF32_ST_BIND(symbol->st_shndx)/16);
		else
                   printf(" %4s", "UND");
                printf(" %s\n", name);

              } 
       }
     }
}

void usage()
{
  printf("Usage : elf_header option <ELF File name>\nWhere Option is :\n");
  printf("\t--file-header       Display the ELF file header\n");
  printf("\t--section-headers   Display the sections' header\n");
  printf("\t-s --syms           Display the symbol table\n");
}

int main(int argc, char *argv[]) 
{
    char* allelf;
    Elf32_Ehdr* elfhdr;
    int fd, i;

    if (argc < 3) {
        usage();
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


      if ( !strcmp(argv[1], "--section-headers") || !strcmp(argv[1], "-S") || !strcmp(argv[1], "--sections")) {
        for (i=0; i<ELF32_R_SYM(elfhdr->e_shnum); i++) {
          printsection((Elf32_Ehdr*) allelf, i);
        }
        printf("Key to Flags:\n");
        printf("  W (write), A (alloc), X (execute), M (merge), S (strings)\n");
        printf("  I (info), L (link order), G (group), T (TLS), E (exclude), x (unknown)\n");
        printf("  O (extra OS processing required) o (OS specific), p (processor specific)\n");
       }
       
      if ( !strcmp(argv[1], "-x")) {
         int nums = atoi(argv[2]);        
         if (nums && strcmp(argv[2], "0")) {
            printhexasectionindex((Elf32_Ehdr*) allelf, nums);
         } else {
            printhexasectionname((Elf32_Ehdr*) allelf, argv[2]);
         }

      }
    
      if ( !strcmp(argv[1], "-s")) {
         printsymboltab((Elf32_Ehdr*) allelf);
      }

    
    close (fd);
   return 0;

}
