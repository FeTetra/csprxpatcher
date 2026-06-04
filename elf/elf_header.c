#include "../buffer.h"
#include <stdint.h>
#include "elf_header.h"

ElfHeader ElfHeader_ctor(BufReader *reader) {
    ElfHeader result;

    uint32_t magic;
    BufReader_read_u32(reader, &magic);

    uint8_t ei_class;
    BufReader_read_u8(reader, &ei_class);
    uint8_t ei_data;
    BufReader_read_u8(reader, &ei_data);
    uint8_t ver;
    BufReader_read_u8(reader, &ver);
    uint8_t os_abi;
    BufReader_read_u8(reader, &os_abi);

    Buf padding = Buf_ctor(7);
    BufReader_read_size(reader, &padding, 7);

    uint16_t type;
    BufReader_read_u16(reader, &type);
    uint16_t machine;
    BufReader_read_u16(reader, &machine);
    uint32_t version;
    BufReader_read_u32(reader, &version);

    BufReader_read_u64(reader, &result.entry);
    BufReader_read_u64(reader, &result.ph_offset);
    BufReader_read_u64(reader, &result.sh_offset);
    BufReader_read_u32(reader, &result.flags);
    
    uint16_t size;
    BufReader_read_u16(reader, &size);
    uint16_t ph_ent_size;
    BufReader_read_u16(reader, &ph_ent_size);
    
    BufReader_read_u16(reader, &result.ph_count);
    BufReader_read_u16(reader, &result.sh_count);
    BufReader_read_u16(reader, &result.sh_str_idx);

    return result;
}