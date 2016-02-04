# RHme+ 2015

## License
This challenge has been published under the terms of the Reciprocal Public License 1.5.
The terms of the license can be checked here: [http://opensource.org/licenses/RPL-1.5](http://opensource.org/licenses/RPL-1.5)

## What is RHme+

The RHme+ (Riscure Hack me ) is a low level hardware challenge that comes in the form of an Arduino board. It was launched during BlackHat Amsterdam in 2015. The winners of the first edition were announced on 18th of January 2015. The writeups together with the interview of the winners can be found from March 1 at the [official challenge website](www.riscure.com/challenge). Source of the challenge will be also released in March.
 
Use your weapon of choice to extract the flags. We have no preference and we are curious to see where your creativity and skill will take you! Just be sure to have fun! ;-)
We estimate the difficulty level to be moderate. If you like these challenges and you would like more, let us know. Get in touch with us via twitter (#riscure #rhme+) or send us an email at challenge. at. riscure.com


###### Choosing the target
You need to install the RHme+ firmware into a target. The "official" target for the challenge is an [Arduino nano 3.0](https://www.arduino.cc/en/Main/ArduinoBoardNano). If you don't have one, you can buy a clone in ebay for few euros. Alternatively you can use any other development board based in the Atmel ATmega328. Many other Arduino and Arduino-based boards should work. The following is a list of Arduino targets:

**Tested devices**
- Arduino Nano 3.0

**Untested devices that should work**
- Arduino Uno
- Arduino Pro
- Arduino Pro Mini (5V and 16 MHz version)
- Arduino Duemilanove (ATmega328P version)
- Arduino Mini (Pro ATmega328P version)

**Untested devices that might work**
- LilyPad Arduino: It runs at 8MHz instead of 16MHz, so the timing-dependent attacks could be affected

For other Arduino-based targets, you can check the [Wikipedia](https://en.wikipedia.org/wiki/List_of_Arduino_boards_and_compatible_systems). Any board based in ATmega328P is potentially able to run the firmware unless that the UARTs or analog inputs of the ATmega are connected to another chip (e.g. a Bluetooth or Zigbee module).

###### Burning the firmware into the target
You need to burn the file binary\ctf.hex into the ATmega overwriting Arduino bootloader. For that you need an external AVR-ISP programmer. Is up to you to find a suitable programmer and the instructions to use it. You can even use another Arduino and use it as an AVR-ISP programmer. Follow [this instructions](https://www.arduino.cc/en/Tutorial/ArduinoISP) to prepare another Arduino as a ISP programmer and follow [this](https://learn.adafruit.com/arduino-tips-tricks-and-techniques/arduinoisp#bonus-using-with-avrdude) to burn the RHme+ firmware in your target.

###### Checking that your target is working
Connect the target to your computer through the USB (or the UART0 is you are not using a target with USB). Open the serial port with any terminal tool and press the reset bottom in the target. Do you see garbage being send? Congrats! It is very likely that you programmed correctly the firmware. Unfortunately the first challenge you have to solve is to find how to properly communicate with the target so that garbage you see now becomes something meaningful.# RHme+ 2015

## What is RHme+

## Preparing the target

###### Choosing a board
You need to install the RHme+ firmware into a target. The "official" target for the challenge is an [Arduino nano 3.0](https://www.arduino.cc/en/Main/ArduinoBoardNano). If you don't have one, you can buy a clone in Ebay for few euros. Alternatively you can use any other development board based in the Atmel ATmega328. Many other Arduino and Arduino-based boards should work. The following is a list of Arduino targets:

**Tested devices**
- Arduino Nano 3.0

**Untested devices that should work**
- Arduino Uno
- Arduino Pro
- Arduino Pro Mini (5V and 16 MHz version)
- Arduino Duemilanove (ATmega328P version)
- Arduino Mini (Pro ATmega328P version)

**Untested devices that might work**
- LilyPad Arduino: It runs at 8MHz instead of 16MHz, so the timing-dependent attacks could be affected

For other Arduino-based targets, you can check the [Wikipedia](https://en.wikipedia.org/wiki/List_of_Arduino_boards_and_compatible_systems). Any board based in ATmega328P is potentially able to run the firmware unless that the UARTs or analog inputs of the ATmega are connected to another chip (e.g. a Bluetooth or Zigbee module).

###### Burning the firmware into the target
You need to burn the file binary\ctf.hex into the ATmega overwriting Arduino bootloader. For that you need an external AVR-ISP programmer. Is up to you to find a suitable programmer and the instructions to use it. You can even use another Arduino and use it as an AVR-ISP programmer. Follow [this instructions](https://www.arduino.cc/en/Tutorial/ArduinoISP) to prepare another Arduino as a ISP programmer and follow [this](https://learn.adafruit.com/arduino-tips-tricks-and-techniques/arduinoisp#bonus-using-with-avrdude) to burn the RHme+ firmware in your target.

###### Checking that your target is working
Connect the target to your computer through the USB (or the UART0 is you are not using a target with USB). Open the serial port with any terminal tool and press the reset bottom in the target. Do you see garbage being send? Congrats! It is very likely that you programmed correctly the firmware. Unfortunately the first challenge you have to solve is to find how to properly communicate with the target so that garbage you see now becomes something meaningful.

## Tips and tricks

During the Blackhat Europe 2015, Riscure gave away more than 150 Arduino boards prepared with the RHme+ challenge. The original challenge was a "black box" evaluation, so the participants had no access to the source code, binary or such. Today we are releasing the source and binary of the challenge so everybody can play and try it at home. We recommend you to try to solve the challenge without reading the source or reversing the binary.

The ultimate goal of the challenge is to recover the Admin Key stored in the hardware. In order to ease the challenge, we provide some information that could help you:

* All hardware and software attacks are in the scope. There are many ways to solve the challenge, so if you get stuck at any point, just go back and try something different. Please, consider techniques like side-channel attacks, fault injection and other fancy hardware attacks as well as classic software explotation attacks like buffer overflows or unvalidated inputs.
* If you have no experience with hardware attacks, don’t worry! You can solve the challenge using exclusively software exploitation techniques. However, you can also solve the challenge using only hardware attacks or a combination of both.
* Read about the CPU architecture and learn about its limitations and try to figure out what the I/Os do;
* There are three types of users that can login, the normal user, the privileged user and the admin. All of them have a 128 bit private key, the User, Prividleged and Admin Key respectively.
* The login mechanism is a challenge-response protocol. The target generates a 32 bit random number called nonce using an external source of noise. The nonce is sent to the user and the target waits for the correct response.
* The response is calculated as follows: the nonce is padded with zeros up to 128 bits to form the plaintext P, which is then encrypted using AES-128 with the Key. The output is AND-masked with a 32 bit mask M. The result of the masking is the 32 bit response R to the challenge.
* The target calculates three possible responses using the User Key, Privileged and the Admin key with the masks 0x000FFFFF, 0x00FFFFFF and 0xFFFFFFFF respectively. If one response matches the input provided by the user, the access is allowed with the corresponding privilege level (normal user, privileged user or admin). Here is an example of a response calculation:

```
Admin Key = 000102030405060708090A0B0C0D0E0F

User Key = F0E0D0C0B0A090807060504030201000

Madmin= FFFFFFFF

Muser = 000FFFFF

Nonce = 0AEEB964

P = 0AEEB964000000000000000000000000

Radmin = AES(P,Admin Key) & Madmin = 2496faad

Ruser= AES(P,User Key) &Muser = 00033695
```

* There are many secondary "assets" you can try to obtain: login as User, login as Admin, recovering the User key, dumping the binary, gaining runtime control, affecting the RNG, etc. You don't need to get all these assets in order to extract the Admin Key, but you will have fun doing it!
* Write us to let us know your impressions about the challenge!!!

