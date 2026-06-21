#include <stdio.h>

#include "elf.h"
#include "elf_header.h"
#include "elf_s_header.h"
#include "shellcode.h"

uint32_t VirtualAddressToOffset(Elf *self, uint64_t v_addr) {
    uint32_t result = 0;
    for (int i = 0; i < self->header.sh_count; i++) {
        uint64_t s_v_addr = self->s_headers[i].v_addr;
        if ((s_v_addr <= v_addr) && (v_addr < (s_v_addr + self->s_headers[i].file_size))) {
            uint64_t offset = v_addr - s_v_addr;
            result = (uint32_t)(self->s_headers[i].p_addr + offset);
            break;
        }
    }
    return result;
}

uint64_t Elf_get_highest_v_addr(Elf *self) {
    uint64_t result = 0;
    for (int i = 0; i < self->header.sh_count; i++) {
        uint64_t v_addr = self->s_headers[i].v_addr + self->s_headers[i].file_size;
        result = (result < v_addr) ? v_addr : result;
    }

    return result;
}

Elf Elf_ctor(IoBuf *reader) {
    Elf result;

    ElfHeader header = ElfHeader_ctor(reader);

    ElfProgramHeader *p_headers = malloc(sizeof(ElfProgramHeader) * header.ph_count);
    IoBuf_seek_to(reader, header.ph_offset);
    for (int i = 0; i < header.ph_count; i++) {
        p_headers[i] = ElfProgramHeader_ctor(reader);
    }

    ElfSectionHeader *s_headers = malloc(sizeof(ElfSectionHeader) * header.sh_count);
    IoBuf_seek_to(reader, header.sh_offset);
    for (int i = 0; i < header.sh_count; i++) {
        s_headers[i] = ElfSectionHeader_ctor(reader);
    }

    size_t current = reader->pos;
    for (int i = 0; i < header.sh_count; i++) {
        IoBuf_seek_to(reader, s_headers[i].p_addr);
        IoBuf_read_size(reader, &s_headers[i].s_data, s_headers[i].file_size);
    }
    IoBuf_seek_to(reader, current);

    result.header = header;
    result.p_headers = p_headers;
    result.s_headers = s_headers;

    uint32_t toc_offset = VirtualAddressToOffset(&result, header.entry);
    IoBuf_seek_to(reader, toc_offset);
    IoBuf_read_u32(reader, &result.entrypoint_address);
    result.entrypoint_offset = VirtualAddressToOffset(&result, result.entrypoint_address);

    result.entrypoint = ShellCode_ctor(4);
    IoBuf_seek_to(reader, result.entrypoint_offset);
    for (int i = 0; i < 4; i++) {
        IoBuf_read_u32(reader, &result.entrypoint.opcodes[i]);
    }

    return result;
}

void Elf_dtor(Elf *self) {
    for (int i = 0; i < self->header.sh_count; i++) {
        Buf_dtor(&self->s_headers[i].s_data);
    }
    free(self->p_headers);
    free(self->s_headers);
    memset(self, 0, sizeof(Elf)); // Maybe bad since this makes nullptrs who cares
}

// These expect that you have malloc'd your headers
int Elf_add_program_header(Elf *self, ElfProgramHeader new_header) {
    self->header.ph_count++;
    size_t reallocated = (self->header.ph_count * sizeof(ElfProgramHeader));

    ElfProgramHeader *tmp_ptr = realloc(self->p_headers, reallocated);
    if (!tmp_ptr) {
        self->header.ph_count--;
        printf("Could not expand program headers\n");
        return 0;
    }
    
    self->p_headers = tmp_ptr;
    self->p_headers[self->header.ph_count - 1] = new_header;
    return 1;
}

int Elf_add_section_header(Elf *self, ElfSectionHeader new_header) {
    self->header.sh_count++;
    size_t reallocated = (self->header.sh_count * sizeof(ElfSectionHeader));

    ElfSectionHeader *tmp_ptr = realloc(self->s_headers, reallocated);
    if (!tmp_ptr) {
        self->header.ph_count--;
        printf("Could not expand section headers\n");
        return 0;
    }
    
    self->s_headers = tmp_ptr;
    self->s_headers[self->header.sh_count - 1] = new_header;
    return 1;
}

size_t Align(size_t value, size_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

void SeekToAlignment(IoBuf *writer, size_t alignment) {
    size_t current = writer->pos;
    size_t aligned = Align(current, alignment);

    if (aligned > current) {
        size_t zero_buf_size = aligned - current;
        IoBuf_pad(writer, zero_buf_size);
    } else if (aligned < current) {
        IoBuf_seek_to(writer, aligned);
    }
}

void Elf_write_prx_patched(Elf *self, IoBuf *writer, char *prx_path) {
    IoBuf_seek_to(writer, 0);
    IoBuf_pad(writer, sizeof(ElfHeader)); // Temporarily zero header

    uint64_t ph_offset = self->header.ph_offset;
    uint16_t ph_size = (self->header.ph_count * 0x38);
    uint64_t ph_end = (ph_offset + (uint64_t)ph_size);

    size_t current = writer->pos;
    uint64_t zero_buf_size = ph_end - (uint64_t)current;
    IoBuf_pad(writer, zero_buf_size);

    for (int i = 0; i < self->header.sh_count; i++) {
        ElfSectionHeader_write(&self->s_headers[i], writer);
    }

    IoBuf_seek_to(writer, writer->buf.size); // Seek to end
    SeekToAlignment(writer, 0x1000);
    uint64_t new_s_offset = writer->pos;

    uint64_t highest_v_addr = Elf_get_highest_v_addr(self);
    uint64_t new_s_v_addr = 0x13370000; // lol

    Payload entry_payload = Payload_ctor(
        prx_path, 
        &self->entrypoint, 
        self->entrypoint_address,
        new_s_v_addr
    );
    IoBuf_write_size(
        writer, 
        &entry_payload.payload.buf,
        entry_payload.payload.buf.size // Maybe i should make an IoBuf_write_entire(writer, buf)
    );

    IoBuf_write_shellcode(writer, &entry_payload.jump, entry_payload.jump.count); // Yea i should
    ShellCode_dtor(&entry_payload.jump);
    SeekToAlignment(writer, 0x1000);

    ElfSectionHeader new_s_header = {
        .name_offset = 0, 
        .type = Progbits, 
        .flags = Alloc | ExecInstr, 
        .v_addr = new_s_v_addr, 
        .p_addr = new_s_offset, 
        .file_size = entry_payload.payload.buf.size,
        .link = 0,
        .info = 0,
        .alignment = 1,
        .entry_size = 0,
    };

    // I need to rename some of these enum variants badly
    ElfProgramHeader new_p_header = {
        .type = Load,
        .flags = R | X,
        .offset = new_s_offset,
        .v_addr = new_s_header.v_addr,
        .p_addr = new_s_header.p_addr,
        .file_size = new_s_header.file_size,
        .mem_size = new_s_header.file_size,
        .alignment = 0x1000,
    };

    // TODO: error handling my beloathed
    Elf_add_section_header(self, new_s_header);
    Elf_add_program_header(self, new_p_header);
    self->header.sh_offset = writer->pos;
    for (size_t i = 0; i < self->header.sh_count; i++) {
        ElfSectionHeader_write(&self->s_headers[i], writer);
    }
    self->header.ph_offset = writer->pos;
    for (size_t i = 0; i < self->header.ph_count; i++) {
        ElfProgramHeader_write(&self->p_headers[i], writer);
    }

    // Write header with updated offsets
    IoBuf_seek_to(writer, 0);
    ElfHeader_write(&self->header, writer);
}

