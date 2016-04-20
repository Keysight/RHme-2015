#This script contains the hacks to recover the riscure challenge
#Usage:
#Install pyserial and pycrypto
#modify the serial initialisation line to connect to the challenge
#>> execfile("hack.py")
#>> hackAccounts()
#>> findAEAkeys()
#>> login(Nonce) -> gives the responses for user, admin and privileged user

import serial
import sys
import time
from Crypto.Cipher import AES

ser = serial.Serial("COM31", 1000000, timeout=2)					#Initialize serial at 1 MBaud

output = ser.read(195) 										#Read initial response

def hackAccounts(bytepos = 0x00, offset = 0x00):
	timeouts = [190, 240, 290, 340] 						#Set timeout values for different byte positions
	getNonce()
	for i in range(0xff):
		if (i%0xfd == 0):									#Get new nonce after 0xfd tries
			getNonce()
		response = "R%0.8x\r\n"%((i<<(8*bytepos)) + offset)	#Write response into string
		ser.write(response)
		output=ser.read(2) 									#Read cr+lf
		starttime = time.time()
		output = ser.read(1) 								#Read answer (is 'E' when response is invalid)
		endtime = time.time()
		totaltime = (endtime - starttime) *1000 			#The time in millis that it took between cr+lf and the final answer from the module
		if (output != 'E'):
			output = output + ser.read(1000)
			#print response[:-2] + " %d"%totaltime
			print "Found valid response: " + response[:-2]
			print output
			ser.write("X\r\n") 								#Logout
			ser.read(1000) 									#Read response
			break
		if (totaltime > timeouts[bytepos]): 				#Found next byte
			#print response[:-2] + " %d"%totaltime
			hackAccounts(bytepos + 1, ((i<<(8*bytepos)) + offset)) 	#Recurse into guessing the next byte

def findAESkeys():
	timings = [0, 0.01, 0.5]		#at these timings we find the different keys
	for timing in timings:
		loginTimingAttack(timing)
	
def loginTimingAttack(t = 1):
	ser.close()
	ser.open()
	ser.read(195)
	getNonce()
	ser.write("R498451d5\r\n")
	time.sleep(t)				#wait t seconds
	ser.close()					#reset the chip
	ser.open()	
	ser.read(195)
	ser.write('V\r\n')			#dump memory
	out = ser.read(49)
	ser.write("-1\r\n")
	dump = ser.read(1000000)
	ser.close()
	testdump(dump)				#check for valid keys in the memory
			
def testdump(dump):
	message = "\xe7\x21\x2f\x7d\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	dumplen = (len(dump)-16 + 1)
	for i in range(dumplen):
		key = dump[i:i + 16]
		if (encryptTest(message, key) == 1):
			print
			break
	
def encryptTest(message, key):
	obj = AES.new(key, AES.MODE_ECB)
	ciphertext = obj.encrypt(message)
	if (ciphertext[-2:] == "\x3b\x4c"):
		print "User key: \"%s\""%key
		print "Hex user key: " + ''.join("{:02x}".format(ord(c)) for c in key)
		return 1
	if (ciphertext[-3:] == "\x79\x80\x68"):
		print "Privileged user key: \"%s\""%key
		print "Hex privileged key: " + ''.join("{:02x}".format(ord(c)) for c in key)
		return 1
	if (ciphertext[-4:] == "\x49\x84\x51\xd5"):
		print "Admin key: \"%s\""%key
		print "Hex admin key: " + ''.join("{:02x}".format(ord(c)) for c in key)
		return 1
	return 0
	
def getNonce():
	ser.write("A\r\n")
	ser.read(92)
	
def login(nonce):
	keys= ["Riscure is cool!", "RHme+ C0n7ac7_u5", "Getting closer!!"]
	nonce = nonce + "000000000000000000000000"
	message = nonce.decode('hex')
	for key in keys:
		obj = AES.new(key, AES.MODE_ECB)
		ciphertext = obj.encrypt(message)
		print key + " : " + ''.join("{:02x}".format(ord(c)) for c in ciphertext[-4:])

