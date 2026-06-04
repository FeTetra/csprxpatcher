#include "elf_p_header.h"
#include "../buffer.h"

ElfProgramHeader ElfProgramHeader_ctor(BufReader *reader) {
    ElfProgramHeader result;

    BufReader_read_u32(reader, &result.type);
    BufReader_read_u32(reader, &result.flags);
    BufReader_read_u64(reader, &result.offset);
    BufReader_read_u64(reader, &result.v_addr);
    BufReader_read_u64(reader, &result.p_addr);
    BufReader_read_u64(reader, &result.file_size);
    BufReader_read_u64(reader, &result.mem_size);
    BufReader_read_u64(reader, &result.alignment);

    return result;
}