#include "elf_s_header.h"
#include "../buffer.h"

ElfSectionHeader ElfSectionHeader_ctor(BufReader *reader) {
    ElfSectionHeader result;

    BufReader_read_u32(reader, &result.name_offset);
    BufReader_read_u32(reader, &result.type);
    BufReader_read_u64(reader, &result.flags);
    BufReader_read_u64(reader, &result.v_addr);
    BufReader_read_u64(reader, &result.p_addr);
    BufReader_read_u64(reader, &result.file_size);
    BufReader_read_u32(reader, &result.link);
    BufReader_read_u32(reader, &result.info);
    BufReader_read_u64(reader, &result.alignment);
    BufReader_read_u64(reader, &result.entry_size);

    return result;
}