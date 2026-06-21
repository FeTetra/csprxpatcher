#include "io_buf.h"

IoBuf IoBuf_ctor(size_t size, Endian endian) {
    IoBuf result = { Buf_ctor(size), 0, endian};
    return result;
}

void IoBuf_dtor(IoBuf *self) {
    Buf_dtor(&self->buf);
    self->pos = 0;
}

int IoBuf_has_remaining(IoBuf *self, size_t size) {
    return (self->buf.size - self->pos) >= size;
}

int IoBuf_seek_to(IoBuf *self, size_t pos) {
    if (pos <= self->buf.size) {
        self->pos = pos;
        return 1;
    }
    return 0;
}

int IoBuf_seek_from_cur(IoBuf *self, size_t nbytes) {
    if ((self->pos + nbytes) <= self->buf.size) {
        self->pos += nbytes;
        return 1;
    }
    return 0;
}

int IoBuf_pad(IoBuf *self, size_t count) {
    if (IoBuf_has_remaining(self, count)) {
        self->pos += count;
        for (size_t i = 0; i < count; i++) {
            self->buf.data[i] = 0;
        }
        return 1;
    }
    return 0;
}

int IoBuf_read_u8(IoBuf *self, uint8_t *out) {
    if (IoBuf_has_remaining(self, 1)) {
        *out = *(uint8_t *)(self->buf.data + self->pos);
        self->pos += 1;
        return 1;
    }
    return 0;
}

int IoBuf_read_u16(IoBuf *self, uint16_t *out) {
    if (IoBuf_has_remaining(self, 2)) {
        memcpy(out, self->buf.data + self->pos, sizeof(uint16_t));
        *out = u16_from_endian(*out, self->endian);
        self->pos += 2;
        return 1;
    }
    return 0;
}

int IoBuf_read_u32(IoBuf *self, uint32_t *out) {
    if (IoBuf_has_remaining(self, 4)) {
        memcpy(out, self->buf.data + self->pos, sizeof(uint32_t));
        *out = u32_from_endian(*out, self->endian);
        self->pos += 4;
        return 1;
    }
    return 0;
}

int IoBuf_read_u64(IoBuf *self, uint64_t *out) {
    if (IoBuf_has_remaining(self, 8)) {
        memcpy(out, self->buf.data + self->pos, sizeof(uint64_t));
        *out = u64_from_endian(*out, self->endian);
        self->pos += 8;
        return 1;
    }
    return 0;
}

int IoBuf_read_size(IoBuf *self, Buf *out, size_t size) {
    if (IoBuf_has_remaining(self, size) && out->size >= size) {
        memcpy(out->data, self->buf.data + self->pos, size);
        self->pos += size;
        return 1;
    }
    return 0;
}

int IoBuf_write_u8(IoBuf *self, uint8_t in) {
    if (IoBuf_has_remaining(self, 1)) {
        *(uint8_t *)(self->buf.data + self->pos) = in;
        self->pos += 1;
        return 1;
    }
    return 0;
}

int IoBuf_write_u16(IoBuf *self, uint16_t in) {
    if (IoBuf_has_remaining(self, 2)) {
        in = u16_to_endian(in, self->endian);
        memcpy(self->buf.data + self->pos, &in, sizeof(uint16_t));
        self->pos += 2;
        return 1;
    }
    return 0;
}

int IoBuf_write_u32(IoBuf *self, uint32_t in) {
    if (IoBuf_has_remaining(self, 4)) {
        in = u32_to_endian(in, self->endian);
        memcpy(self->buf.data + self->pos, &in, sizeof(uint32_t));
        self->pos += 4;
        return 1;
    }
    return 0;
}

int IoBuf_write_u64(IoBuf *self, uint64_t in) {
    if (IoBuf_has_remaining(self, 8)) {
        in = u64_to_endian(in, self->endian);
        memcpy(self->buf.data + self->pos, &in, sizeof(uint64_t));
        self->pos += 8;
        return 1;
    }
    return 0;
}

int IoBuf_write_size(IoBuf *self, Buf *in, size_t size) {
    if (IoBuf_has_remaining(self, size) && in->size >= size) {
        memcpy(self->buf.data + self->pos, in->data, size);
        self->pos += size;
        return 1;
    }
    return 0;
}

int IoBuf_write_size_from_mem(IoBuf *self, void *in, size_t size) {
    if(IoBuf_has_remaining(self, size)) {
        memcpy(self->buf.data + self->pos, in, size); // Cannot verify size, reccommend using Buf
        self->pos += size;
        return 1;
    }
    return 0;
}
