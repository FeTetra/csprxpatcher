#ifndef ELF_H
#define ELF_H

#include <stddef.h>
#include <stdint.h>

#include "elf_header.h"
#include "elf_p_header.h"
#include "elf_s_header.h"
#include "../buffer.h"

typedef struct Elf {
    ElfHeader header;
    ElfProgramHeader *p_headers;
    ElfSectionHeader *s_headers;

    uint32_t entrypoint_offset;
    uint32_t entrypoint_address;
    uint32_t entrypoint_instructions[4];
} Elf;

#endif // ELF_H
