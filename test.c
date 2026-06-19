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

int ReadFileIntoBufReader(FILE *fp, IoBuf *reader) {
    size_t size = GetFileSize(fp);

    if (size > 0) {
        fread(reader->buf.data, 1, size, fp);
        return 1;
    }

    return 0;
}

void PrintElfHeader(ElfHeader *header) {
    printf("entry: %lld\n", header->entry);
    printf("flags: %d\n", header->flags);

    printf("program header count: %d\n", header->ph_count);
    printf("program header offset %llx\n", header->ph_offset);

    printf("section header count: %d\n", header->sh_count);
    printf("section header offset: %llx\n", header->sh_offset);
    printf("section header string index: %d\n", header->sh_str_idx);
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

    Elf elf = Elf_ctor(&file_buf);

    IoBuf_dtor(&file_buf);

    PrintElfHeader(&elf.header);
    
    Elf_dtor(&elf);

    return 0;
}    
