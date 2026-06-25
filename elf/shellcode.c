#include "shellcode.h"

ShellCode ShellCode_ctor(size_t count) {
    ShellCode result;
    result.count = count;
    result.opcodes = malloc(sizeof(uint32_t) * count);
    return result;
}

void ShellCode_dtor(ShellCode *self) {
    free(self->opcodes);
    memset(self, 0, sizeof(ShellCode));
}

void BuildJump(ShellCode *self, uint32_t address) {
    uint16_t upper = (uint16_t)(address >> 16);
    uint16_t lower = (uint16_t)(address & 0xffff);
    if (self->count >= 4) {
        self->opcodes[0] = 0x3d600000 | (uint32_t)upper; // lis r11, upper
        self->opcodes[1] = 0x616B0000 | (uint32_t)lower; // ori r11, r11, lower
        self->opcodes[2] = 0x7d6903a6;                   // mtctr r11
        self->opcodes[3] = 0x4e800420;                   // bctr
    }
}

int IoBuf_write_shellcode_with_count(IoBuf *self, ShellCode *in, size_t count) {
    if (IoBuf_ensure_remaining(self, count * 4)) {
        for (size_t i = 0; i < count; i++) {
            IoBuf_write_u32(self, in->opcodes[i]);
        }
        return 1;
    }
    return 0;
}

int IoBuf_write_shellcode(IoBuf *self, ShellCode *in) {
    return IoBuf_write_shellcode_with_count(self, in, in->count);
}

// TODO: How will we load prx_loader.bin (prolly a file)
Payload Payload_ctor(
    char *sprx_path, 
    ShellCode *entrypoint,
    uint32_t entrypoint_address,
    uint32_t payload_address,
    Buf *bin_code
) {
    Payload result;

    size_t bin_size = 4 * 128; // Test prx_loader.bin size
    size_t entrypoint_size = (4 * entrypoint->count);
    size_t code_size = bin_size + entrypoint_size;
    size_t sprx_path_size = strlen(sprx_path) + 1; // Include null terminator

    IoBuf payload = IoBuf_ctor(code_size + sprx_path_size, Big, Writer);

    IoBuf_write_from_buf(&payload, bin_code);
    IoBuf_seek_to(&payload, (bin_size - entrypoint_size)); // Seek back a bit to write old entrypoint
    IoBuf_write_shellcode(&payload, entrypoint);
    IoBuf_write_size(&payload, sprx_path, sprx_path_size);

    ShellCode jump = ShellCode_ctor(4); // I could do this on the stack but im lazy theres ctor
    BuildJump(&jump, (uint32_t)(entrypoint_address + entrypoint_size));
    IoBuf_write_shellcode(&payload, &jump);
    ShellCode_dtor(&jump);

    uint32_t sprx_path_address = payload_address + (uint64_t)code_size;
    uint16_t sprx_path_addr_upper = (sprx_path_address >> 16);
    uint16_t sprx_path_addr_lower = (sprx_path_address & 0xffff);

    static const size_t pos = 128; // Hate this
    IoBuf_seek_to(&payload, pos + 2);
    IoBuf_write_u16(&payload, sprx_path_addr_upper);
    IoBuf_seek_to(&payload, pos + 4 + 2);
    IoBuf_write_u16(&payload, sprx_path_addr_lower);

    result.payload = payload;
    result.jump = ShellCode_ctor(4);
    BuildJump(&result.jump, payload_address);
    result.address = payload_address;

    return result;
}

void Payload_dtor(Payload *self) {
    IoBuf_dtor(&self->payload);
}

