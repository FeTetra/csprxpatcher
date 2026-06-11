#ifndef ELF_S_HEADER_H
#define ELF_S_HEADER_H

#include <stdint.h>
#include <stddef.h>

#include "../buffer.h"

typedef enum SectionType : uint32_t {
    SNull = 0,
    Progbits = 1,
    Symtab = 2,
    Strtab = 3,
    Rela = 4,
    Hash = 5,
    SDynamic = 6,
    SNote = 7,
    Nobits = 8,
    Rel = 9,
    SShlib = 10,
    Dynsym = 11,
    InitArray = 14,
    FiniArray = 15,
    PreinitArray = 16,
    Group_st = 17,
    SymtabShndx = 18,
    SLoos = 0x60000000,
} SectionType;

typedef enum SectionFlags : uint64_t {
    Write = 1,
    Alloc = 2,
    ExecInstr = 4,
    Merge = 16,
    Strings = 32,
    InfoLink = 64,
    LinkOrder = 128,
    OsNonConforming = 256,
    Group_sf = 512,
    STls = 1024,
    MaskOs = 0x0FF00000,
    MaskProc = 0xF0000000,
    Ordered = 0x40000000,
    Exclude = 0x80000000
} SectionFlags;

typedef struct ElfSectionHeader {
    uint32_t name_offset;
    SectionType type;
    SectionFlags flags;
    uint64_t v_addr;
    uint64_t p_addr;
    uint64_t file_size;
    Buf s_data;
    uint32_t link;
    uint32_t info;
    uint64_t alignment;
    uint64_t entry_size;
} ElfSectionHeader;

ElfSectionHeader ElfSectionHeader_ctor(BufReader *reader);

#endif // ELF_S_HEADER_H