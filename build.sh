CSRCS="buffer/buffer.c buffer/io_buf.c buffer/endian.c"
CSRCS="$CSRCS elf/elf_header.c elf/elf_p_header.c elf/elf_s_header.c elf/elf.c"
CSRCS="$CSRCS shellcode/shellcode.c"
CFLAGS=""
DEBUG=1

if [ $[DEBUG] == 1 ]; then
    CSRCS="$CSRCS test.c"
    CFLAGS="$CFLAGS -g"
fi

gcc $CFLAGS $CSRCS

llvm-mc -triple=ppc64 -filetype=obj shellcode/prx_load_payload.s | llvm-objcopy -O binary - prx_load_payload.bin
