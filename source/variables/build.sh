#! /bin/bash

set -euo pipefail

avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c variables.c

echo "$(pwd)/variables.o"
