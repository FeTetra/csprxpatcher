.set PRX_OPT_STACK, 0x40
.set PRX_NAME_STRING, 0x12345678
.set REGISTER_STACK_OFFSET, 0x50

// Recursive macro to store and estore egister ange
.macro PRESERVE reg
    std \reg, REGISTER_STACK_OFFSET + (8 * \reg)(1)
.endm

.macro RESTORE reg
    ld \reg, REGISTER_STACK_OFFSET + (8 * \reg)(1)
.endm

preserve_registers:
    PRESERVE 0
    PRESERVE 1
    PRESERVE 2
    PRESERVE 3
    PRESERVE 4
    PRESERVE 5
    PRESERVE 6
    PRESERVE 7
    PRESERVE 8
    PRESERVE 9
    PRESERVE 10
    PRESERVE 11
    PRESERVE 12
    PRESERVE 13
    PRESERVE 14
    PRESERVE 15
    PRESERVE 16
    PRESERVE 17
    PRESERVE 18
    PRESERVE 19
    PRESERVE 20
    PRESERVE 21
    PRESERVE 22
    PRESERVE 23
    PRESERVE 24
    PRESERVE 25
    PRESERVE 26
    PRESERVE 27
    PRESERVE 28
    PRESERVE 29
    PRESERVE 30
    PRESERVE 31

load_prx_module:
    // sys_prx_load_module(PRX_NAME_STRING, 0, NULL)
    lis 3, PRX_NAME_STRING@h // Load upper half of string pointer
    ori 3, 3, PRX_NAME_STRING@l // Load lower half
    li 4, 0
    li 5, 0
    li 11, 0x1e0
    sc

start_prx_module:
    li 11, 0x28
    std 11, PRX_OPT_STACK(1)
    li 11, 0x1
    std 11, PRX_OPT_STACK + 0x8(1)
    li 11, -1
    std 11, PRX_OPT_STACK + 0x10(1)
    extsw 3, 3
    li 4, 0
    addi 5, 1, 0x40
    li 11, 0x1e1
    sc

run_prx_entry:
    ld 11, 0x50(1)
    cmpdi 11, -1
    beq restore_registers // ouu shii
    clrldi 9, 11, 0x20
    lwz 0, 0x0(9)
    lwz 2, 0x4(9)
    mtctr 0
    bctrl

restore_registers:
    RESTORE 0
    RESTORE 1
    RESTORE 2
    RESTORE 3
    RESTORE 4
    RESTORE 5
    RESTORE 6
    RESTORE 7
    RESTORE 8
    RESTORE 9
    RESTORE 10
    RESTORE 11
    RESTORE 12
    RESTORE 13
    RESTORE 14
    RESTORE 15
    RESTORE 16
    RESTORE 17
    RESTORE 18
    RESTORE 19
    RESTORE 20
    RESTORE 21
    RESTORE 22
    RESTORE 23
    RESTORE 24
    RESTORE 25
    RESTORE 26
    RESTORE 27
    RESTORE 28
    RESTORE 29
    RESTORE 30
    RESTORE 31
    addi 1, 1, 0x150

return:
    // Blank instructions to eplace with jump
    nop
    nop
    nop
    nop