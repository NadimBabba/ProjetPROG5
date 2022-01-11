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
#include "elf32_dump.h"

int *tabindx;
int TextAdresse = 0x70 ;
int DataAdresse = 0x2000 ;
Elf32_Phdr ProgramHeader;

#define offset_inv(i) (((((i*256)<<12)>>8)|((i*256)>>16)<<4)<<12)
void FixeSymbolNdx (Elf32_Ehdr* elfhdr) {
    int indx = -1 ;
    for (int i=0; i<ELF32_R_SYM(elfhdr->e_shnum); i++) {
      Elf32_Shdr *shrd = ElfSection(elfhdr, i);
      if (ELF32_R_VAL(shrd->sh_type) == SHT_SYMTAB) {
          indx = i; 
         break;
      }
    }
    if (indx == -1) {
       return;
    }

    Elf32_Shdr *symtab = ElfSection(elfhdr, indx);
    char* symaddr = (char*)elfhdr + offset(symtab->sh_offset);


    Elf32_Sym *symbol;
    for(int i=0; i<offset(symtab->sh_size)/ELF32_R_VAL(symtab->sh_entsize); i++) {
      symbol = &((Elf32_Sym *)symaddr)[i];
      switch (symbol->st_shndx) {
        case SHN_ABS: break;
        case SHN_UNDEF : 
        default:
               {
		        if (ELF32_ST_BIND(symbol->st_shndx)) {
                   Elf32_Half newndx = (Elf32_Half) ( (tabindx[ELF32_ST_BIND(symbol->st_shndx)/16] << 4 ) * 16) ;
                   symbol->st_shndx = newndx;
                }
               } 
       }
     }
}

void FixeSectionsoffset (Elf32_Ehdr* elfhdr, int nbsections, int* sectionsize) {
    int indx = -1 ;
    for (int i=0; i<ELF32_R_SYM(elfhdr->e_shnum); i++) {
      Elf32_Shdr *shrd = ElfSection(elfhdr, i);
      if (ELF32_R_VAL(shrd->sh_type) == SHT_SYMTAB) {
          indx = i; 
         break;
      }
    }
    if (indx == -1) {
       return;
    }

    Elf32_Shdr *symtab = ElfSection(elfhdr, indx);
    int newsyntoffset =  (offset(elfhdr->e_shoff) + (nbsections * ELF32_R_SYM(elfhdr->e_shentsize)))  ;
     symtab->sh_offset = (Elf32_Off) offset_inv(newsyntoffset) ;
     int nbsectionsbytes = offset(symtab->sh_size);
     *sectionsize =  offset(symtab->sh_size);
    for (int i=indx+1; i<ELF32_R_SYM(elfhdr->e_shnum); i++) {
      Elf32_Shdr *shrd = ElfSection(elfhdr, i);
      newsyntoffset = newsyntoffset + nbsectionsbytes;
     shrd->sh_offset = (Elf32_Off) offset_inv(newsyntoffset) ;
     nbsectionsbytes = offset(shrd->sh_size);
     *sectionsize += offset(shrd->sh_size);
      
     }

}

int NBRelocationSections(Elf32_Ehdr* elfhdr) {
    int nb = 0;
    tabindx = (int *) malloc(ELF32_R_SYM(elfhdr->e_shnum)*sizeof(int));
    int new_ndx=0;
    tabindx[0] = 0;
    new_ndx++;
    for (int i=1; i<ELF32_R_SYM(elfhdr->e_shnum); i++) {
      Elf32_Shdr *shrd = ElfSection(elfhdr, i);
      if ((ELF32_R_VAL(shrd->sh_type) == SHT_RELA) ||  (ELF32_R_VAL(shrd->sh_type) == SHT_REL) 
         || (offset(shrd->sh_size) == 0) ) 
      {
         nb++;
        tabindx[i] = 0;
      } else {
        tabindx[i] = new_ndx;
        new_ndx++;
      }
    }
    return nb;
}

void RemoveRelSections(Elf32_Ehdr* elfhdr, char* fname) {
    unsigned int nbrels = NBRelocationSections(elfhdr);
    unsigned int initianbs = ELF32_R_SYM(elfhdr->e_shnum);
    FixeSymbolNdx(elfhdr) ; 
    int sectionssize;      
    FixeSectionsoffset(elfhdr, (initianbs - nbrels), &sectionssize) ; 
    //printf ("\n Nb section init %d remove %d new %d / %x   %x\n", initianbs, nbrels, (initianbs - nbrels) , elfhdr->e_shnum, elfhdr->e_shstrndx);
    elfhdr->e_shnum = (Elf32_Half) ((initianbs - nbrels)<<8);
    elfhdr->e_shstrndx = (Elf32_Half) ((tabindx[ELF32_R_SYM(elfhdr->e_shstrndx)])<<8); //TODO 9 get strtab
    //printf ("\n Nb section init %d remove %d new %d / %x   %x\n", initianbs, nbrels, (initianbs - nbrels) , elfhdr->e_shnum, elfhdr->e_shstrndx);
    char outfname[126];
    strcpy(outfname, fname);
    outfname[strlen(outfname)-2]='\0';
    strcat(outfname, "_E6.o");

    FILE* fd = fopen(outfname, "wb"); 
    fwrite(elfhdr, offset(elfhdr->e_shoff), 1, fd);

    //char *P = (char*) elfhdr + sizeof(Elf32_Ehdr);
    //fwrite(P, (offset(elfhdr->e_shoff) - sizeof(Elf32_Ehdr)), 1, fd); 
    int nbsections = 0;
    Elf32_Shdr *shrd = ElfSection(elfhdr, 0);
    fwrite(shrd, sizeof(Elf32_Shdr), 1, fd);         
    for (int i=1; i< initianbs; i++) {
      Elf32_Shdr *shrd = ElfSection(elfhdr, i);
      if ((ELF32_R_VAL(shrd->sh_type) != SHT_RELA) &&  (ELF32_R_VAL(shrd->sh_type) != SHT_REL)
          && (offset(shrd->sh_size)) ) 
          {
         if (ELF32_R_VAL(shrd->sh_link)) {
           shrd->sh_link = (Elf32_Word) (tabindx[ELF32_R_VAL(shrd->sh_link)]<<24);
         }
         fwrite(shrd, sizeof(Elf32_Shdr), 1, fd);         
         nbsections++;
      }
    }
    char *P = (char*) elfhdr + offset(elfhdr->e_shoff) + (initianbs * ELF32_R_SYM(elfhdr->e_shentsize)) ;
    fwrite(P, sectionssize, 1, fd); 

    printf ("write in file %s :  %d sections \n", outfname,   nbsections);
    fclose(fd);
}

char* ReadElf32(char* fname) {
    struct stat st;

    if (stat(fname, &st) != 0) {
       printf( " Error reading elf file %s \n", fname);
       return NULL;
    }

    int fd = open(fname, O_RDONLY); 

    if( fd < 0 ) {
        printf( " Error reading elf file %s \n", fname);
        return NULL;
    }

    char *allelf = malloc(st.st_size);

    int sz = read(fd, (char*) (allelf), st.st_size);

    if (sz == 0) {
        printf( " Error reading elf file\n");
        //free allelf;
        allelf = NULL;
    }
   close (fd);
   return allelf;
}
int main(int argc, char *argv[]) 
{
    char* allelf;
    Elf32_Ehdr* elfhdr;

    allelf = ReadElf32(argv[argc-1]);

    if (allelf == NULL)
          return 1;

    elfhdr = (Elf32_Ehdr*) allelf;    

 

   
   RemoveRelSections(elfhdr, argv[argc-1]);

   return 0;

}
