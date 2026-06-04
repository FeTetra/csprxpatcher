#include <stddef.h>
#include <stdint.h>

typedef enum ProgramType : uint32_t {
    Null = 0,
    Load = 1,
    Dynamic = 2,
    Interp = 3,
    Note = 4,
    Shlib = 5,
    Phdr = 6,
    Tls = 7,
    Loos = 0x60000000,
    Hios = 0x6fffffff,
    Loproc = 0x70000000,
    Hiproc = 0x7fffffff,
} ProgramType;

typedef enum ProgramFlags : uint32_t {
    X = 1,
    W = 2,
    R = 4,
} ProgramFlags;

typedef struct ElfProgramHeader {
    ProgramType type;
    ProgramFlags flags;
    uint64_t offset;
    uint64_t v_addr;
    uint64_t p_addr;
    uint64_t file_size;
    uint64_t mem_size;
    uint64_t alignment;
} ElfProgramHeader;