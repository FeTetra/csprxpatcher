#ifndef ELF_P_HEADER_H
#define ELF_P_HEADER_H

#include <stddef.h>
#include <stdint.h>

#include "../buffer/io_buf.h"

typedef enum ProgramType : uint32_t {
    PNull = 0,
    Load = 1,
    PDynamic = 2,
    Interp = 3,
    PNote = 4,
    PShlib = 5,
    Phdr = 6,
    PTls = 7,
    PLoos = 0x60000000,
    Hios = 0x6fffffff,
    Loproc = 0x70000000,
    Hiproc = 0x7fffffff,
} ProgramType;

typedef enum ProgramFlags : uint32_t {
    X = 1,
    W = 2,
    R = 4,
} ProgramFlags;

typedef struct ElfProgramHeader {
    ProgramType type;
    ProgramFlags flags;
    uint64_t offset;
    uint64_t v_addr;
    uint64_t p_addr;
    uint64_t file_size;
    uint64_t mem_size;
    uint64_t alignment;
} ElfProgramHeader;

ElfProgramHeader ElfProgramHeader_ctor(IoBuf *reader);
void ElfProgramHeader_write(ElfProgramHeader *self, IoBuf *writer);

#endif // ELF_P_HEADER_H