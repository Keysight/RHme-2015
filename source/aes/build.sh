#! /bin/bash

set -euo pipefail

avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c aes128_dec.c
avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c aes128_enc.c
avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c aes_dec.c
avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c aes_enc.c
avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c aes_invsbox.c
avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c aes_keyschedule.c
avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c aes_sbox.c
avr-gcc $GCC_COMPILE_FLAGS $GCC_OPTIMIZATION_FLAG $INC_PATH -c gf256mul.S

echo "$(pwd)/aes128_dec.o $(pwd)/aes128_enc.o $(pwd)/aes_dec.o $(pwd)/aes_enc.o $(pwd)/aes_invsbox.o $(pwd)/aes_keyschedule.o $(pwd)/aes_sbox.o $(pwd)/gf256mul.o"
