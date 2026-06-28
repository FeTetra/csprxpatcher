# csprxpatcher
## Port of [SPRXPatcher](https://github.com/NotNite/SPRXPatcher) to c
This tool allows you to patch a PlayStation 3 ELF binary to load and run a PRX module at startup.

## Usage
No cli tool currently. `test.c` can be compiled as-is via `build.sh`. The build script requires `gcc`, `llvm-mc`, and `llvm-objcopy`. Though the compiler can be replaced with any other c compiler, and the assembler can be replaced with any PowerPC-64 assembler as long as you can output the raw opcodes in some fashion. The code comes pre-assembled as expected in `prx_load_payload.bin` if you cannot assemble it.

## Credits
- [NotNite](https://github.com/NotNite) for the original code
