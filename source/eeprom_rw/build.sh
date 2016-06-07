#! /bin/bash

set -euo pipefail

GCC_OPTIMIZATION_FLAG="-O0"

avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c eeprom_rw.c

echo "$(pwd)/eeprom_rw.o"
