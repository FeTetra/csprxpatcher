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

int ReadFileIntoBufReader(FILE *fp, IoBuf *reader) {
    size_t size = GetFileSize(fp);

    if (size > 0) {
        fread(reader->buf.data, 1, size, fp);
        return 1;
    }

    return 0;
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
    FILE *fp = fopen("eboot.elf", "rb+");
    size_t file_size = GetFileSize(fp);
    printf("elf file size: %lu\n", file_size);

    if (file_size <= 0) {
        printf("invalid file size\n");
        return 1;
    }

    IoBuf file_buf = IoBuf_ctor(file_size, Big);
    ReadFileIntoBufReader(fp, &file_buf);
    //PrintHexLines(&file_buf, 16, 16);

    Elf elf = Elf_ctor(&file_buf);

    IoBuf_dtor(&file_buf);

    PrintElfHeaders(&elf);
    
    Elf_dtor(&elf);

    return 0;
}    
