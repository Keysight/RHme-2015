#! /bin/bash

set -e

#./build.sh

# avrdude flag meanings:
#   -p  part no (avr chip type); use ? to get a list of values
#   -c  programmer; use ? to get a list of values
#   -P  serial port
#   -b  baud rate
#   -e  erase chip
#   -V  do not verify
#   -u  disable safemode
#   -U  memory operation

# upload via programmer
avrdude -p m328p -c avrispmkII -e -u -U flash:w:main/ctf.hex -P usb
avrdude -p m328p -c avrispmkII -u -U lock:w:0x3c:m -P usb

# upload via bootloader
#avrdude -p m328p -c arduino -P /dev/ttyUSB0 -b 57600 -e -V -u -U flash:w:main/ctf.hex

# check if board is alive
python check_board.py /dev/ttyUSB0
if [[ $? == 0 ]]; then
    echo 'Board is alive'
else
    echo 'Board is dead'
fi
