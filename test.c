#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "elf/elf_header.h"
#include "elf/elf_p_header.h"
#include "elf/elf_s_header.h"

size_t GetFileSize(FILE *fp) {
    fseek(fp, 0L, SEEK_END);
    size_t result = ftell(fp);
    rewind(fp);
    return result;
}

int ReadFileIntoBufReader(FILE *fp, BufReader *reader) {
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

    BufReader file_buf = BufReader_ctor(file_size);
    ReadFileIntoBufReader(fp, &file_buf);

    ElfHeader header = ElfHeader_ctor(&file_buf);
    PrintElfHeader(&header);

    BufReader_seek_to(&file_buf, header.ph_offset);
    ElfProgramHeader p_headers[header.ph_count];
    for (int i = 0; i < header.ph_count; i++) {
        p_headers[i] = ElfProgramHeader_ctor(&file_buf);
    }

    BufReader_seek_to(&file_buf, header.sh_offset);
    ElfSectionHeader s_headers[header.sh_count];
    for (int i = 0; i < header.sh_count; i++) {
        s_headers[i] = ElfSectionHeader_ctor(&file_buf);
    }

    return 0;
}    
