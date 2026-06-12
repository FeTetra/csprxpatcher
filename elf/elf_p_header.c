#include "elf_p_header.h"

ElfProgramHeader ElfProgramHeader_ctor(IoBuf *reader) {
    ElfProgramHeader result;

    IoBuf_read_u32(reader, &result.type);
    IoBuf_read_u32(reader, &result.flags);
    IoBuf_read_u64(reader, &result.offset);
    IoBuf_read_u64(reader, &result.v_addr);
    IoBuf_read_u64(reader, &result.p_addr);
    IoBuf_read_u64(reader, &result.file_size);
    IoBuf_read_u64(reader, &result.mem_size);
    IoBuf_read_u64(reader, &result.alignment);

    return result;
}