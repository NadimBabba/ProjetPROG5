#ifndef ELF32_DEF
#define ELF32_DEF


typedef int  Elf32_Addr ; // 4 4 Unsigned program address
typedef unsigned short Elf32_Half ; //2 2 Unsigned medium integer
typedef unsigned int Elf32_Off ; //4 4 Unsigned file offset
typedef int  Elf32_Sword ; //4 4 Signed large integer
typedef unsigned int Elf32_Word; // 4 4 Unsigned large integer
//unsigned char 1 1 Unsigned small integer

//#define EI_NIDENT 16 

enum Elf32_Ehdr_type {
    ET_NONE_type = 0 ,       //No file type
    ET_REL = 0x0100 ,            //Relocatable file
    ET_EXEC = 0x0200 ,       //Executable file
    ET_DYN = 0x0300 ,            //Shared object file
    ET_CORE = 0x0400 ,       //Core file
    ET_LOPROC = 0xff00 ,    //Processor-specific
    ET_HIPROC = 0xffff      //Processor-specific
};

enum Elf32_Ehdr_machine { 
    ET_NONE = 0 , //No machine
    EM_M32 = 1 , //AT&T WE 32100
    EM_SPARC = 2 , //SPARC
    EM_386 = 3 , //Intel Architecture
    EM_68K = 4 , //Motorola 68000
    EM_88K = 5 , //Motorola 88000
    EM_860 = 7 , //Intel 80860
    EM_MIPS = 8 , //MIPS RS3000 Big-Endian
    EM_MIPS_RS4_BE = 10 , //MIPS RS4000 Big-Endian
    EM_ARM = 0x2800 , //ARM Specification
    RESERVED = 11  //11-16 Reserved for future use
};

enum Elf32_Ehdr_version {
    EV_NONE = 0 , // Invalid versionn
    EV_CURRENT = 1 // Current version
};

enum Elf32_Ehdr_ident_index {
    EI_MAG0 = 0 , // File identification
    EI_MAG1 = 1 , // File identification
    EI_MAG2 = 2 , // File identification
    EI_MAG3 = 3 , // File identification
    EI_CLASS = 4 , // File class
    EI_DATA = 5 , // Data encoding
    EI_VERSION = 6 , // File version
    EI_OSABI = 0x07,
    EI_ABIVERSION = 0x08 ,
    EI_PAD = 7 , // Start of padding bytes
    EI_NIDENT = 16 // Size of e_ident[]
};

enum Elf32_Ehdr_ei_class_type {
    ELFCLASSNONE = 0 , // Invalid class
    ELFCLASS32 = 1 , // 32-bit objects
    ELFCLASS64 = 2 // 64-bit objects
};

enum Elf32_Ehdr_ei_data_type {
    ELFDATANONE = 0 , // Invalid data encoding
    ELFDATA2LSB = 1 , // See below
    ELFDATA2MSB = 2  // See below
};

enum special_section_index {
    SHN_UNDEF = 0 ,
    SHN_LORESERVE = 0xff00 ,
    SHN_LOPROC = 0xff00 ,
    SHN_HIPROC = 0xff1f ,
    SHN_ABS = 0xfff1 ,
    SHN_COMMON = 0xfff2 ,
    SHN_HIRESERVE = 0xffff
};

enum elf32_flags {
    EF_ARM_ABIMASK = 5 ,
    EF_ARM_BE8 = 0x00800000 ,
    EF_ARM_GCCMASK = 0x00400FFF , 
    EF_ARM_ABI_FLOAT_HARD = 0x00000400 ,
    EF_ARM_ABI_FLOAT_SOFT = 0x00000200
};



typedef struct {
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;

enum Elf32_Shdr_type { 
    SHT_NULL = 0x0 ,
    SHT_PROGBITS = 0x1 ,
    SHT_SYMTAB = 0x2 ,
    SHT_STRTAB = 0x3 ,
    SHT_RELA = 0x4 ,
    SHT_HASH = 0x5 ,
    SHT_DYNAMIC = 0x6 ,
    SHT_NOTE = 0x7 ,
    SHT_NOBITS = 0x8 ,
    SHT_REL = 0x9 ,
};

enum Elf32_Shdr_flags { 
    SHF_WRITE = 0x1 ,
    SHF_ALLOC = 0x2 ,
    SHF_EXECINSTR = 0x4 
    //SHF_MASKPROC = 0xf0000000
};

typedef struct {
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct {
    Elf32_Word st_name;
    Elf32_Addr st_value;
    Elf32_Word st_size;
    unsigned char st_info;
    unsigned char st_other;
    Elf32_Half st_shndx;
} Elf32_Sym;

#define ELF32_ST_BIND(i) ((i)>>4)
#define ELF32_ST_TYPE(i) ((i)&0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))
enum ELF32_ST_BIND_enum {
    STB_LOCAL = 0 ,
    STB_GLOBAL = 1 ,
    STB_WEAK = 2 ,
    STB_LOPROC = 13 ,
    STB_HIPROC = 15
};

enum ELF32_ST_type_enum {
    STT_NOTYPE = 0 ,
    STT_OBJECT = 1 ,
    STT_FUNC = 2 ,
    STT_SECTION = 3 ,
    STT_FILE = 4 ,
    STT_LOPROC = 13 ,
    STT_HIPROC = 15
};

typedef struct {
    Elf32_Addr r_offset;
    Elf32_Word r_info;
} Elf32_Rel;

typedef struct {
    Elf32_Addr r_offset;
    Elf32_Word r_info;
    Elf32_Sword r_addend;
} Elf32_Rela;


#define ELF32_R_SYM(i) ((i)>>8)
#define ELF32_R_TYPE(i) ((i)>>24)
#define ELF32_R_INFO(s,t) (((s)<<8)+(unsigned char)(t))
#define ELF32_R_VAL(i) ((i) >> 24)
#define be32tole(i)  ((unsigned int) __builtin_bswap32(i))
enum Arm_Rel_Type {
   R_ARM_NONE = 0 ,
   R_ARM_PC24 = 1 ,
   R_ARM_ABS32 = 2 ,
   R_ARM_REL32 = 3 ,
   R_ARM_CALL = 28 ,
   R_ARM_JUMP24 = 29 
};
   
// Program Header
typedef struct {
    Elf32_Word p_type;
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;

enum Elf32_Phdr_type {
    PT_NULL = 0 ,
            PT_LOAD = 1 ,
            PT_DYNAMIC = 2 ,
            PT_INTERP = 3 ,
            PT_NOTE = 4 ,
            PT_SHLIB = 5 ,
            PT_PHDR = 6 ,
            PT_LOPROC = 0x70000000 ,
            PT_HIPROC = 0x7fffffff
};

#define offset(i) (((((i/256)>>12)<<8)|((i/256)<<16)>>4)>>12)
#define ELF32_MR_INFO(i) (unsigned short) (ELF32_R_SYM(i)>>16) + (unsigned short) (ELF32_R_SYM(i))

#endif
