#ifndef ELF_HEADER_H
#define ELF_HEADER_H

#include <stdint.h>
#include <stddef.h>

#include "../buffer.h"

typedef struct ElfHeader {
    uint64_t entry;
    uint64_t ph_offset;
    uint64_t sh_offset;
    uint32_t flags;
    uint16_t ph_count;
    uint16_t sh_count;
    uint16_t sh_str_idx;
} ElfHeader;

ElfHeader ElfHeader_ctor(BufReader *reader);

#endif // ELF_HEADER_H