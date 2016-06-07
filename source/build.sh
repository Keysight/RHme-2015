#! /bin/bash

set -euo pipefail

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

#MMCU="atxmega64a3"
#F_CPU="32000000UL"
#export BOARD_FAMILY="ATX_MEGA"

MMCU="atmega328p"
F_CPU="16000000UL"
EEPROM_CHECK_ADDR="38"
export BOARD_FAMILY="AT_MEGA"

PRBUFF_LEN=120
INBUFF_LEN=20
export GCC_OPTIMIZATION_FLAG="-Os"
export GCC_COMPILE_FLAGS="-Wall -mmcu=$MMCU -DF_CPU=$F_CPU -D$BOARD_FAMILY -DPRBUFF_LEN=$PRBUFF_LEN -DINBUFF_LEN=$INBUFF_LEN -DEEPROM_CHECK_ADDR=$EEPROM_CHECK_ADDR -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fno-exceptions -Wundef"
export GCC_LINK_FLAGS="-Wl,--gc-sections"
export OBJCOPY_IHEX_FLAGS="-j .text -j .data -O ihex"
export OBJCOPY_BIN_FLAGS="-j .text -j .data -O binary"
export DEPS=""
export INC_PATH="-I $DIR"

MODULES=(
    serial
    random
    aes
    variables
    auth
    obfuscated_asm
    overflow_rop
    eeprom_rw
    overflow_readflash
    menu
    main
)

for M in ${MODULES[@]}; do
    echo ""
    echo ">>> Building $M"
    MOD_DIR="$DIR/$M"
    cd $MOD_DIR
    export DEPS="$DEPS $(./build.sh)"
    export INC_PATH="$INC_PATH -I $MOD_DIR"
done

#cp $DIR/main/ctf.elf /vmshared
#cp $DIR/main/ctf.hex /vmshared
