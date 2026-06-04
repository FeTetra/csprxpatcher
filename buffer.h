#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct Buf {
    uint8_t *data;
    size_t size;
} Buf;

Buf Buf_ctor(size_t size);
void Buf_dtor(Buf *self);

typedef struct BufReader {
    Buf buf;
    size_t pos;
} BufReader;

BufReader BufReader_ctor(void *buf, size_t size);
void BufReader_dtor(BufReader *self);

int BufReader_has_remaining(BufReader *self, size_t size);

int BufReader_read_u8(BufReader *self, uint8_t *out);
int BufReader_read_u16(BufReader *self, uint16_t *out);
int BufReader_read_u32(BufReader *self, uint32_t *out);
int BufReader_read_u64(BufReader *self, uint64_t *out);
int BufReader_read_size(BufReader *self, Buf *out, size_t size);