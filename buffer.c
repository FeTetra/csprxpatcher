#include "buffer.h"
#include <stddef.h>

Buf Buf_ctor(size_t size) {
    void *data = malloc(size);
    Buf result = { data, size };
    return result;
}

void Buf_dtor(Buf *self) {
    free(self->data);
    self->data = NULL;
    self->size = 0;
}

BufReader BufReader_ctor(size_t size) {
    BufReader result = { Buf_ctor(size), 0 };
    return result;
}

void BufReader_dtor(BufReader *self) {
    Buf_dtor(&self->buf);
    self->pos = 0;
}

int BufReader_has_remaining(BufReader *self, size_t size) {
    return (self->buf.size - self->pos) >= size;
}

int BufReader_seek_to(BufReader *self, size_t pos) {
    if (pos <= self->buf.size) {
        self->pos = pos;
        return 1;
    }

    return 0;
}

int BufReader_seek_from_cur(BufReader *self, size_t nbytes) {
    if ((self->pos + nbytes) <= self->buf.size) {
        self->pos += nbytes;
        return 1;
    }

    return 0;
}

int BufReader_read_u8(BufReader *self, uint8_t *out) {
    if (BufReader_has_remaining(self, 1)) {
        *out = *(uint8_t *)(self->buf.data + self->pos);
        self->pos += 1;
        return 1;
    }

    return 0;
}

int BufReader_read_u16(BufReader *self, uint16_t *out) {
    if (BufReader_has_remaining(self, 2)) {
        *out = *(uint16_t *)(self->buf.data + self->pos);
        self->pos += 2;
        return 1;
    }

    return 0;
}

int BufReader_read_u32(BufReader *self, uint32_t *out) {
    if (BufReader_has_remaining(self, 4)) {
        *out = *(uint32_t *)(self->buf.data + self->pos);
        self->pos += 4;
        return 1;
    }

    return 0;
}

int BufReader_read_u64(BufReader *self, uint64_t *out) {
    if (BufReader_has_remaining(self, 8)) {
        *out = *(uint64_t *)(self->buf.data + self->pos);
        self->pos += 4;
        return 1;
    }

    return 0;
}

int BufReader_read_size(BufReader *self, Buf *out, size_t size) {
    if (BufReader_has_remaining(self, size) && out->size >= size) {
        memcpy(out->data, self->buf.data, size);
        self->pos += size;
        return 1;
    }

    return 0;
}