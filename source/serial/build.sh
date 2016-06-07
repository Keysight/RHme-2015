#! /bin/bash

set -euo pipefail

DEBUG=0

if [[ $BOARD_FAMILY == "ATX_MEGA" ]]; then
    avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c usartx.c
    avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c serial_io.c
    echo "$(pwd)/usartx.o $(pwd)/serial_io.o"
elif [[ $BOARD_FAMILY == "AT_MEGA" ]]; then
    avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c dbg_putchar.c
    avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c usart.c
    avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c serial_io.c
    echo "$(pwd)/dbg_putchar.o $(pwd)/usart.o $(pwd)/serial_io.o"
fi

if [[ $DEBUG == 1 ]]; then
    avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c serial_test.c

    >&2 echo "Building serial module in DEBUG mode"
    if [[ $BOARD_FAMILY == "ATX_MEGA" ]]; then
        avr-gcc $GCC_COMPILE_FLAGS $GCC_LINK_FLAGS -o serial_test.elf usartx.o serial_io.o serial_test.o $DEPS
        avr-objcopy $OBJCOPY_IHEX_FLAGS serial_test.elf serial_test.hex

    elif [[ $BOARD_FAMILY == "AT_MEGA" ]]; then
        avr-gcc $GCC_COMPILE_FLAGS $GCC_LINK_FLAGS -o serial_test.elf dbg_putchar.o usart.o serial_io.o serial_test.o $DEPS
        avr-objcopy $OBJCOPY_IHEX_FLAGS serial_test.elf serial_test.hex
    fi
fi
