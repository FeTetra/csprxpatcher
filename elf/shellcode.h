#ifndef SHELLCODE_H
#define SHELLCODE_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

#include "../buffer/io_buf.h"

typedef struct ShellCode {
    uint32_t *opcodes;
    size_t count;
} ShellCode;

ShellCode ShellCode_ctor(size_t count);

void ShellCode_dtor(ShellCode *self);

typedef struct Payload {
    IoBuf payload;
    ShellCode jump;
} Payload;

void BuildJump(ShellCode *self, uint32_t address);

int IoBuf_write_shellcode_with_count(IoBuf *self, ShellCode *in, size_t count);
int IoBuf_write_shellcode(IoBuf *self, ShellCode *in);

Payload Payload_ctor(
    char *sprx_path, 
    ShellCode *entrypoint,
    uint32_t entrypoint_address,
    uint32_t payload_address
);

void Payload_dtor(Payload *self);

#endif // SHELLCODE_H
