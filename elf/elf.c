#include "elf.h"
#include "elf_header.h"

uint32_t VirtualAddressToOffset(Elf *self, uint64_t v_addr) {
    uint32_t result = 0;
    for (int i = 0; i < self->header.sh_count; i++) {
        uint64_t s_v_addr = self->s_headers[i].v_addr;
        if ((s_v_addr <= v_addr) && (v_addr < (s_v_addr + self->s_headers[i].file_size))) {
            uint64_t offset = v_addr - s_v_addr;
            result = (uint32_t)(self->s_headers[i].p_addr + offset);
            break;
        }
    }
    return result;
}

Elf Elf_ctor(IoBuf *reader) {
    Elf result;

    ElfHeader header = ElfHeader_ctor(reader);

    IoBuf_seek_to(reader, header.ph_offset);
    ElfProgramHeader *p_headers = malloc(sizeof(ElfProgramHeader) * header.ph_count);
    for (int i = 0; i < header.ph_count; i++) {
        p_headers[i] = ElfProgramHeader_ctor(reader);
    }

    ElfSectionHeader *s_headers = malloc(sizeof(ElfSectionHeader) * header.sh_count);
    IoBuf_seek_to(reader, header.sh_offset);
    for (int i = 0; i < header.sh_count; i++) {
        s_headers[i] = ElfSectionHeader_ctor(reader);
    }

    size_t current = reader->pos;
    for (int i = 0; i < header.sh_count; i++) {
        IoBuf_seek_to(reader, s_headers[i].p_addr);
        IoBuf_read_size(reader, &s_headers[i].s_data, s_headers[i].file_size);
    }
    IoBuf_seek_to(reader, current);

    uint32_t toc_offset = VirtualAddressToOffset(&result, header.entry);
    IoBuf_seek_to(reader, toc_offset);
    IoBuf_read_u32(reader, &result.entrypoint_address);
    result.entrypoint_offset = VirtualAddressToOffset(&result, result.entrypoint_address);

    IoBuf_seek_to(reader, result.entrypoint_offset);
    for (int i = 0; i < 4; i++) {
        IoBuf_read_u32(reader, &result.entrypoint_instructions[i]);
    }

    result.header = header;
    result.p_headers = p_headers;
    result.s_headers = s_headers;

    return result;
}

void Elf_dtor(Elf *self) {
    for (int i = 0; i < self->header.sh_count; i++) {
        Buf_dtor(&self->s_headers[i].s_data);
    }
    free(self->p_headers);
    free(self->s_headers);
    memset(self, 0, sizeof(Elf)); // Maybe bad since this makes nullptrs who cares
}

void Elf_write(Elf *self, IoBuf *writer) {
    IoBuf_seek_to(writer, 0);
    ElfHeader_write(&self->header, writer);
    
}
