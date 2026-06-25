#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer/io_buf.h"
#include "elf/elf.h"
#include "elf/elf_header.h"
#include "elf/elf_p_header.h"
#include "elf/elf_s_header.h"

size_t GetFileSize(FILE *fp) {
    fseek(fp, 0L, SEEK_END);
    size_t result = ftell(fp);
    rewind(fp);
    return result;
}

// Most inefficient way possible to do this but its a test lol
void PrintHexLines(IoBuf *self, size_t per_line, size_t lines) {
    for (size_t i = 0; i < per_line * lines; i++) {
        if (i % per_line == 0 && i > 0) {
            printf("\n");
        }
        if (i % per_line == per_line / 2) {
            printf(" ");
        }
        if (IoBuf_has_remaining(self, 1)) {
            printf("%02x", self->buf.data[i]);
        }
    }
    printf("\n");
}

int ReadFileIntoBuf(FILE *fp, Buf *buf) {
    size_t size = GetFileSize(fp);

    if (size > 0 && size <= buf->size) {
        fread(buf->data, 1, size, fp);
        return 1;
    }

    return 0;
}

int WriteBufferIntoFile(FILE *fp, Buf *buf) {
    if (buf->size > 0) {
        fwrite(buf->data, 1, buf->size, fp);
        return 1;
    }

    return 0;
}

Buf CreateBufFromFile(char *path) {
    FILE *fp = fopen(path, "rb+");
    size_t file_size = GetFileSize(fp);
    Buf result = Buf_ctor(file_size);
    ReadFileIntoBuf(fp, &result);
    fclose(fp);

    return result;
}

void CreateFileFromBuf(Buf *buf, char *path) {
    FILE *fp = fopen(path, "wb+");
    WriteBufferIntoFile(fp, buf);
    fclose(fp);
}

void PrintElfHeader(ElfHeader *header) {
    printf("entry: %llx\n", header->entry);
    printf("program header offset %llx\n", header->ph_offset);
    printf("section header offset: %llx\n", header->sh_offset);
    printf("flags: %d\n", header->flags);

    printf("program header count: %d\n", header->ph_count);
    printf("section header count: %d\n", header->sh_count);

    printf("section header string index: %d\n", header->sh_str_idx);
}

void PrintElfProgramHeader(ElfProgramHeader *p_header) {
    printf("ph type: %d\n", p_header->type);
    printf("ph flags: %x\n", p_header->flags);
    printf("ph offset: %lld\n", p_header->offset);
    printf("ph virtual addr: %lld\n", p_header->v_addr);
    printf("ph physical addr: %lld\n", p_header->p_addr);
    printf("ph file size: %lld\n", p_header->file_size);
    printf("ph mem size: %lld\n", p_header->mem_size);
    printf("ph alignment: %lld\n", p_header->alignment);
}

void PrintElfSectionHeader(ElfSectionHeader *s_header) {
    printf("sh name offset: %d\n", s_header->name_offset);
    printf("sh type: %d\n", s_header->type);
    printf("sh flags: %lld\n", s_header->flags);

    printf("sh virtual addr: %lld\n", s_header->v_addr);
    printf("sh physical addr: %lld\n", s_header->p_addr);
    printf("sh file size: %lld\n", s_header->file_size);

    printf("sh link: %d\n", s_header->link);
    printf("sh info: %d\n", s_header->info);
    printf("sh alignment: %llx\n", s_header->alignment);
    printf("sh entry size: %lld\n", s_header->entry_size);
}

void PrintElfHeaders(Elf *self) {
    PrintElfHeader(&self->header);
    for (size_t i = 0; i < self->header.ph_count; i++) {
        printf("----------------\nProgram Header %ld\n----------------\n", i);
        PrintElfProgramHeader(&self->p_headers[i]);
    }
    for (size_t i = 0; i < self->header.sh_count; i++) {
        printf("----------------\nSection Header %ld\n----------------\n", i);
        PrintElfSectionHeader(&self->s_headers[i]);
    }
}

int main() {
    IoBuf elf_data = { CreateBufFromFile("eboot.elf"), 0, Big, Writer };
    Buf payload_bin = CreateBufFromFile("prx_load_payload.bin");

    Elf elf = Elf_ctor(&elf_data);

    Payload entry_payload = Payload_ctor(
        "/dev_hdd0/plugins/patchwork.sprx", 
        &elf.entrypoint, 
        elf.entrypoint_address,
        0x13370000,
        &payload_bin
    );

    Elf_write_prx_patched(&elf, &elf_data, &entry_payload);
    elf_data.pos = 0;
    PrintHexLines(&elf_data, 16,16);
    //PrintElfHeaders(&elf);

    CreateFileFromBuf(&elf_data.buf, "eboot.elf.patched");
    
    Elf_dtor(&elf);
    IoBuf_dtor(&elf_data);
    Buf_dtor(&payload_bin);

    return 0;
}    
