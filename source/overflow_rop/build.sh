#! /bin/bash

set -euo pipefail

avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c overflow_rop.c
avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c examine_stack.c

echo "$(pwd)/overflow_rop.o $(pwd)/examine_stack.o"
