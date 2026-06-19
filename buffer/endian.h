#ifndef ENDIAN_H
#define ENDIAN_H

#include <stdint.h>
#include <stddef.h>

typedef enum Endian {
    Little = 0,
    Big = 1,
} Endian;

static const int SYSTEM_ENDIAN = (uint8_t)0xff00 != 0; // Compile-time endian test using cast

uint16_t u16_swap(uint16_t n);
uint32_t u32_swap(uint32_t n);
uint64_t u64_swap(uint64_t n);

uint16_t u16_to_system_endian(uint16_t n, Endian e);
uint32_t u32_to_system_endian(uint16_t n, Endian e);
uint64_t u64_to_system_endian(uint16_t n, Endian e);

#endif // ENDIAN_H
