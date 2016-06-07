#! /bin/bash

set -euo pipefail

DEBUG=1
#RAND_SRC="RAND_PB"
RAND_SRC="RAND_ADC"

avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -D$RAND_SRC -c random.c

if [[ $DEBUG == 1 ]]; then
    avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c random_test.c
    avr-gcc $GCC_COMPILE_FLAGS $GCC_LINK_FLAGS -o random_test.elf random_test.o random.o $DEPS
    avr-objcopy $OBJCOPY_IHEX_FLAGS random_test.elf random_test.hex
fi

echo "$(pwd)/random.o"
