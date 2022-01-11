


void write_elf32_hdr(Elf32_Ehdr* elfhdr) ;
Elf32_Shdr *ElfSheader(Elf32_Ehdr *hdr) ;
Elf32_Shdr *ElfSection(Elf32_Ehdr *hdr, int idx) ;
void printsection(Elf32_Ehdr* elfhdr, int indx) ;
void printhexasectionindex(Elf32_Ehdr* elfhdr, int indx) ;
void printhexasectionname(Elf32_Ehdr* elfhdr, char* sname) ;
void printsymboltab(Elf32_Ehdr* elfhdr) ;
char * getsectionname(Elf32_Ehdr* elfhdr, int indx) ;
void getsymboltab(Elf32_Ehdr* elfhdr, int isymb, char **SymbName,int *SymbVal) ;
void PrintRelocationSection(Elf32_Ehdr* elfhdr) ;


extern char* shstrtab;







