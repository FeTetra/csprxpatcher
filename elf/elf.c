#include "elf.h"
#include "elf_header.h"
#include "elf_s_header.h"

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

uint64_t Elf_get_highest_v_addr(Elf *self) {
    uint64_t result = 0;
    for (int i = 0; i < self->header.sh_count; i++) {
        uint64_t v_addr = self->s_headers[i].v_addr;
        result = (result < v_addr) ? v_addr : result;
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

size_t Align(size_t value, size_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

void SeekToAlignment(IoBuf *writer, size_t alignment) {
    size_t current = writer->pos;
    size_t aligned = Align(current, alignment);

    if (aligned > current) {
        size_t zero_buf_size = aligned - current;
        uint8_t zeroes[zero_buf_size];
        memset(zeroes, 0, zero_buf_size);
        IoBuf_write_size_from_mem(writer, zeroes, zero_buf_size);
    } else if (aligned < current) {
        IoBuf_seek_to(writer, aligned);
    }
}

void Elf_write(Elf *self, IoBuf *writer) {
    IoBuf_seek_to(writer, 0);
    ElfHeader_write(&self->header, writer);

    uint64_t ph_offset = self->header.ph_offset;
    uint16_t ph_size = (self->header.ph_count * 0x38);
    uint64_t ph_end = (ph_offset + (uint64_t)ph_size);

    size_t current = writer->pos;
    uint64_t zero_buf_size = ph_end - (uint64_t)current;
    uint8_t zeroes[zero_buf_size];
    memset(zeroes, 0, zero_buf_size);
    IoBuf_write_size_from_mem(writer, zeroes, zero_buf_size);

    for (int i = 0; i < self->header.sh_count; i++) {
        ElfSectionHeader_write(&self->s_headers[i], writer);
    }

    IoBuf_seek_to(writer, writer->buf.size); // Seek to end
    SeekToAlignment(writer, 0x1000);
    uint64_t new_s_offset = writer->pos;

    uint64_t highest_v_addr = Elf_get_highest_v_addr(self);
    uint64_t new_s_v_addr = 0x13370000; // lol
}
