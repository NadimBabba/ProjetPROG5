#include<stdio.h> 
#include<fcntl.h> 
#include<errno.h> 
#include<string.h> 
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>

#include <endian.h>

//#include <elf.h>

#include "elf32.h"

char* shstrtab;

// Phase1 , partie 1
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
// Phase1 , partie 2
Elf32_Shdr *ElfSheader(Elf32_Ehdr *hdr) {
	return (Elf32_Shdr *)((char*) hdr + offset(hdr->e_shoff) );
}
 
Elf32_Shdr *ElfSection(Elf32_Ehdr *hdr, int idx) {
	return &ElfSheader(hdr)[idx];
}
 


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
// Phase1 , partie 3
void printhexasectionindex(Elf32_Ehdr* elfhdr, int indx) {
    Elf32_Shdr *shrd = ElfSection(elfhdr, indx);
    char * strname = (char*) shstrtab + ELF32_R_VAL(shrd->sh_name);
    printf("\nHex dump of section '%s':\n", strname);
    char* shdata = (char*) elfhdr + offset(shrd->sh_offset) ; 
    for (int i=0; i< offset(shrd->sh_size); ) {

       printf("  0x%08x ", i);
       int k=0;
       for (int j=0; j<16;j++) {
         if (i+j > offset(shrd->sh_size)) {
            printf("  ");
         } else {
           printf("%02x",  (unsigned char) (shdata[i+j]));
         }
         k++;
         if (k ==4) { printf(" "); k=0;}
       }

       for (int j=0; j<16;j++) { 
         if (i+j > offset(shrd->sh_size)) break;       
         printf("%c",  (((unsigned char)(shdata[i+j]) >= 32) && ((unsigned char)(shdata[i+j]) <=127)) ? ((unsigned char)(shdata[i+j])) : '.');
       }

       printf("\n\n");
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
    printhexasectionindex(elfhdr, indx);
}

// Phase1 , partie 4
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
    printf("\nSymbol table '%s' contains %d entries:\n", strname, offset(symtab->sh_size)/ELF32_R_VAL(symtab->sh_entsize));
    printf("   Num:    Value  Size Type    Bind   Vis      Ndx Name\n");
    char* symaddr = (char*)elfhdr + offset(symtab->sh_offset);


    Elf32_Sym *symbol;
    for(int i=0; i<offset(symtab->sh_size)/ELF32_R_VAL(symtab->sh_entsize); i++) {
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
                printf("%6d: %08x %5x", i, be32tole(symbol->st_value), be32tole(symbol->st_size));
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
// Phase1 , partie 5
char * getsectionname(Elf32_Ehdr* elfhdr, int indx) {
      Elf32_Shdr *shrd = ElfSection(elfhdr, indx);
      return (char*) shstrtab + ELF32_R_VAL(shrd->sh_name);
}

void getsymboltab(Elf32_Ehdr* elfhdr, int isymb, char **SymbName,int *SymbVal) {
    int indx = -1 ;
    for (int i=0; i<ELF32_R_SYM(elfhdr->e_shnum); i++) {
      Elf32_Shdr *shrd = ElfSection(elfhdr, i);
       if (ELF32_R_VAL(shrd->sh_type) == SHT_SYMTAB) {
          indx = i; 
         break;
      }
    }
    if (indx == -1) {
      SymbName = NULL; // Error
      return;
    }

    Elf32_Shdr *symtab = ElfSection(elfhdr, indx);
    char* symaddr = (char*)elfhdr + offset(symtab->sh_offset);


    Elf32_Sym *symbol= &((Elf32_Sym *)symaddr)[isymb];

    *SymbVal = ELF32_R_VAL(symbol->st_value);
    if (ELF32_ST_TYPE(symbol->st_info) == STT_SECTION) {
      *SymbName = getsectionname(elfhdr, ELF32_ST_BIND(symbol->st_shndx)/16);
      return;
    }

    Elf32_Shdr *strtab = ElfSection(elfhdr, ELF32_R_VAL(symtab->sh_link));
    *SymbName =  (char *)elfhdr + offset(strtab->sh_offset) + ELF32_R_VAL(symbol->st_name);
}
void PrintRelocationSection(Elf32_Ehdr* elfhdr) {
    for (int i=0; i<ELF32_R_SYM(elfhdr->e_shnum); i++) {
      Elf32_Shdr *shrd = ElfSection(elfhdr, i);
      if ((ELF32_R_VAL(shrd->sh_type) == SHT_RELA) ||  (ELF32_R_VAL(shrd->sh_type) == SHT_REL)) {
         char * strname = (char*) shstrtab + ELF32_R_VAL(shrd->sh_name);
         printf("\nRelocation section '%s' at offset 0x%x contains %d entries:\n", strname, offset(shrd->sh_offset), 
			ELF32_R_VAL(shrd->sh_size)/ELF32_R_VAL(shrd->sh_entsize));
         printf(" Offset     Info    Type            Sym.Value  Sym. Name\n");
         char* reladdr = (char*)elfhdr + offset(shrd->sh_offset);
         for (int j=0; j<ELF32_R_VAL(shrd->sh_size)/ELF32_R_VAL(shrd->sh_entsize); j++) {
             Elf32_Rel* RelEntry = &((Elf32_Rel *)reladdr)[j];
	     printf("%08x  %08x", be32tole(RelEntry->r_offset), ELF32_MR_INFO(RelEntry->r_info)); 
             switch (ELF32_R_TYPE(RelEntry->r_info)) {
   		case R_ARM_NONE : printf(" %-17s",  "R_ARM_NONE");break;
   		case R_ARM_PC24 : printf(" %-17s",  "R_ARM_PC24");break;
   		case R_ARM_ABS32 : printf(" %-17s",  "R_ARM_ABS32");break;
   		case R_ARM_REL32 : printf(" %-17s",  "R_ARM_REL32");break;
   		case R_ARM_CALL : printf(" %-17s",  "R_ARM_CALL");break;
   		case R_ARM_JUMP24 : printf(" %-17s",  "R_ARM_JUMP24");break;
		default : printf("%x %-17s", ELF32_R_TYPE(RelEntry->r_info), "Type string TO DO");break;
             }
            char* SymbName = NULL;
            int   SymbVal;
            getsymboltab(elfhdr, ((RelEntry->r_info<<8)>>24), &SymbName, &SymbVal);	      
	    printf(" %08x", SymbVal);//TODO value ?
	    printf("   %s\n", SymbName);
         }
       }
    }
}
// Phase1 , Fin partie 5

