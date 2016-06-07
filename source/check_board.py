#! env python

import sys
import avr_serial

ser = avr_serial.AVRSerial( sys.argv[1], verbose = False )

ser.w('H')
response = ser.r(100)

if response:
    exit(0)
else:
    exit(1)
