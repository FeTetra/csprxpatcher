#include "elf_header.h"

ElfHeader ElfHeader_ctor(IoBuf *reader) {
    ElfHeader result;

    uint32_t magic;
    IoBuf_read_u32(reader, &magic);

    uint8_t ei_class;
    IoBuf_read_u8(reader, &ei_class);
    uint8_t ei_data;
    IoBuf_read_u8(reader, &ei_data);
    uint8_t ver;
    IoBuf_read_u8(reader, &ver);
    uint8_t os_abi;
    IoBuf_read_u8(reader, &os_abi);
    uint8_t abi_ver;
    IoBuf_read_u8(reader, &abi_ver);

    Buf padding = Buf_ctor(7);
    IoBuf_read_size(reader, &padding, 7);

    uint16_t type;
    IoBuf_read_u16(reader, &type);
    uint16_t machine;
    IoBuf_read_u16(reader, &machine);
    uint32_t version;
    IoBuf_read_u32(reader, &version);

    IoBuf_read_u64(reader, &result.entry);
    IoBuf_read_u64(reader, &result.ph_offset);
    IoBuf_read_u64(reader, &result.sh_offset);
    IoBuf_read_u32(reader, &result.flags);
    
    uint16_t size;
    IoBuf_read_u16(reader, &size);
    uint16_t ph_ent_size;
    IoBuf_read_u16(reader, &ph_ent_size);
    IoBuf_read_u16(reader, &result.ph_count);

    uint16_t sh_ent_size;
    IoBuf_read_u16(reader, &sh_ent_size);
    IoBuf_read_u16(reader, &result.sh_count);

    IoBuf_read_u16(reader, &result.sh_str_idx);

    return result;
}


void ElfHeader_write(ElfHeader *self, IoBuf *writer) {
    unsigned char magic_num[] = {0x7F, 'E', 'L', 'F'};
    IoBuf_write_size_from_mem(writer, magic_num, 4);
    IoBuf_write_u8(writer, 2);
    IoBuf_write_u8(writer, 2);
    IoBuf_write_u8(writer, 1);
    IoBuf_write_u8(writer, 0x66);
    IoBuf_write_u8(writer, 0);
    IoBuf_pad(writer, 7);
    IoBuf_write_u16(writer, 2);
    IoBuf_write_u16(writer, 0x15);
    IoBuf_write_u32(writer, 1);
    uint64_t entry_swap = u64_to_endian(self->entry, Big);
    PrintMemBytes(&entry_swap, sizeof(entry_swap));
    IoBuf_write_u64(writer, self->entry);
    IoBuf_write_u64(writer, self->ph_offset);
    IoBuf_write_u64(writer, self->sh_offset);
    IoBuf_write_u32(writer, self->flags);
    IoBuf_write_u16(writer, 0x40);
    IoBuf_write_u16(writer, 0x38);
    IoBuf_write_u16(writer, self->ph_count);
    IoBuf_write_u16(writer, 0x40);
    IoBuf_write_u16(writer, self->sh_count);
    IoBuf_write_u16(writer, self->sh_str_idx);
}
