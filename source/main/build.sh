#! /bin/bash

set -euo pipefail

#avr-gcc $GCC_COMPILE_FLAGS $GCC_LINK_FLAGS $INC_PATH -Wl,--section-start=.flagsection=0x3500 -o ctf.elf main.c $DEPS
avr-gcc $GCC_COMPILE_FLAGS $GCC_LINK_FLAGS $INC_PATH -Wl,--script=avr5.x -o ctf.elf main.c $DEPS
avr-objcopy $OBJCOPY_IHEX_FLAGS -j .flagsection ctf.elf ctf.hex
avr-objcopy $OBJCOPY_BIN_FLAGS -j .flagsection ctf.elf ctf.bin
