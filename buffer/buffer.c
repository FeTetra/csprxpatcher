#include "buffer.h"

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

