#ifndef ELF_H
#define ELF_H

#include <stddef.h>
#include <stdint.h>

#include "elf_header.h"
#include "elf_p_header.h"
#include "elf_s_header.h"
#include "../buffer/io_buf.h"
#include "../shellcode/shellcode.h"

typedef struct Elf {
    ElfHeader header;
    ElfProgramHeader *p_headers;
    ElfSectionHeader *s_headers;

    uint32_t entrypoint_offset;
    uint32_t entrypoint_address;
    ShellCode entrypoint;
} Elf;

Elf Elf_ctor(IoBuf *reader);
void Elf_dtor(Elf *self);

void Elf_write_prx_patched(Elf *self, IoBuf *writer, Payload *entry_payload);

#endif // ELF_H
