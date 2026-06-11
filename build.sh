CSRCS="buffer.c elf/elf_header.c elf/elf_p_header.c elf/elf_s_header.c"
CFLAGS=""
DEBUG=1

if [ $[DEBUG] == 1 ]; then
    CSRCS="$CSRCS test.c"
    CFLAGS="$CFLAGS -g"
fi

gcc $CFLAGS $CSRCS
