#include "elf_s_header.h"

ElfSectionHeader ElfSectionHeader_ctor(IoBuf *reader) {
    ElfSectionHeader result;

    IoBuf_read_u32(reader, &result.name_offset);
    IoBuf_read_u32(reader, &result.type);
    IoBuf_read_u64(reader, &result.flags);

    IoBuf_read_u64(reader, &result.v_addr);
    IoBuf_read_u64(reader, &result.p_addr);
    IoBuf_read_u64(reader, &result.file_size);
    result.s_data = Buf_ctor(result.file_size);

    IoBuf_read_u32(reader, &result.link);
    IoBuf_read_u32(reader, &result.info);
    IoBuf_read_u64(reader, &result.alignment);
    IoBuf_read_u64(reader, &result.entry_size);

    return result;
}

void ElfSectionHeader_dtor(ElfSectionHeader *self) {
    Buf_dtor(&self->s_data);
    memset(self, 0, sizeof(Buf));
}

void ElfSectionHeader_write(ElfSectionHeader *self, IoBuf *writer) {
    IoBuf_write_u32(writer, self->name_offset);
    IoBuf_write_u32(writer, self->type);
    IoBuf_write_u64(writer, self->flags);

    IoBuf_write_u64(writer, self->v_addr);
    IoBuf_write_u64(writer, self->p_addr);
    IoBuf_write_u64(writer, self->file_size);

    IoBuf_write_u32(writer, self->link);
    IoBuf_write_u32(writer, self->info);
    IoBuf_write_u64(writer, self->alignment);
    IoBuf_write_u64(writer, self->entry_size);
}
