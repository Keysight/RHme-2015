Riscure RHme+ Challenge Writeup
===============================

<https://www.riscure.com/challenge/>

About Us
--------

We are a team within Cisco that routinely deals with reverse engineering
tasks, mainly of embedded systems. The team itself deals mostly with
software, but we are "embedded" in a group that does the same for
hardware, and the two teams (HW and SW) often work jointly on various
projects. When time off of "real" projects permits, we try to particpate
in various challenges, CTFs, etc., in order to further our training. So
when the opportunity arose to particpate in this challenge, we were
happy to be able to spare the time for it.

The bulk of the work on this challenge was done in a two-day period last
week, by 6 members of the software team, and with support from 2 members
of the hardware team. A few individuals started working on certain
aspects of the challenge prior to this two-day period -- one, because he
was not able to attend during the joint effort; another, the team
leader, in order to evaluate how to divide up the team efforts during
the 2-day sprint; and one who spent a few hours getting acquainted with
the AVR instruction set and IDA's handling of it. All of the inputs from
those efforts were fed into the main efforts during the 2-day sprint.

We thank Riscure for setting up this challenge, and for providing us
with the necessary boards. We enjoyed the challenge a lot, and are happy
to present our results.

Communicating with the board
----------------------------

We connected the mini USB connector to the PC and noticed that the baud
rate for the UART is not any of the standard options. In order to
determine the correct baud rate, we connected a scope to the TX pin of
the Arduino and measured the time for a single bit to be transmitted.
The measured time was 1 microsecond, which implies a baud rate of
1000000. Indeed, setting that baud rate enabled us to communicate with
the board.

Disabling Random
----------------

The first problem we are presented with is a randomly changing nonce. We
looked at the hardware datasheet for the CPU, and noticed that it does
not have any hardware RNG. Thus, we assumed that its random source must
be one of the analog pins. We connected each of the analog pins to GND,
and found that A1 was the random source -- once A1 and GND were
connected, we were consistently presented with the constant nonce
`0xe7212f7d`.

Logging In as User
------------------

Exploring the menus available on the terminal, the 'V' command
("retrieve variables"), which is available even before logging in,
seemed interesting. Since it asked for a number between 0--5, we
naturally tried some out-of-range numbers. Inserting -1 gave a dump of
the RAM.

The dump did not prove that useful as long as no authentication attempt
had been made. However, a dump produced ''after'' an authentication
attempt proved, upon inspection, to contain (immediately after the
regular variables) the expected authentication response. This response
can then be used for logging in as the normal user. (The correct
response for the non-random nonce `0xe7212f7d` is `063b4c`.)

Timing Attack
-------------

We assumed the verification of the challenge response would be
susceptible to a timing attack, since the CPU is only 8-bit and a
typical check will check each byte one-at-a-time. We wrote a short
python script to time the responses we got from the device while trying
all possible byte values. For the constant nonce obtained by disabling
the random, we found 3 valid responses:

1.  User: `063b4c`
2.  Privileged User: `798068`
3.  Administrator: `498451d5`

Dumping the Flash
-----------------

By sending random inputs to the "read flash config" menu option, we
managed to produce what appeared to be dumps of portions of the flash.
Further exploration showed us that the values at positions 21 and 22 in
the input control the offset in the flash at which the dump begins. The
dump ends when a 0x00 byte is encountered. Using this information, we
implemented a simple script which dumps the entire contents of the flash
using this functionality.

However, the dump proved useful only up to address 0x3500 in the flash.
Beyond that, it was clear from the responses that our read attempts were
somehow failing. Thus, we realized that we were still missing part of
the flash.

Static Analysis of the Flash
----------------------------

We used IDA Pro to statically analyze the binary obtained by dumping the
flash.

### IDA Pro & AVR

It appears that IDA's support for the AVR instruction set is not as
developed as its support of more common architectures such as x86 and
ARM. One of the more annoying issues is that although IDA automatically
annotates the meaning of various values and memory xrefs, it does so
only for immediate values that are directly used for accessing memory or
I/O ports. However, indirect accesses (e.g., loading values into
registers, then accessing memory using those registers) are not
annotated. So we wrote an IDA Python script that identifies consecutive
loads of two bytes into paired registers (AVR registers are 8-bit
registers, but addresses are 16-bits; so pairs of registers --r24:r25,
for example -- are used in tandem for accessing memory locations) and
adds a comment with a hyperlink to the relevant address (in both RAM and
ROM, as the address spaces overlap, and it is not always possible to
know upfront which is being accessed), as well as the text of the string
beginning at that address, if that is the case.

This greatly eased the subsequent analysis of the binary.

### Analyzing the Flash

Once acquainted with the AVR instruction set and IDA's quirks in
analyzing it, we were able to very quickly locate the main command loop,
which led us to the authentication function. By reverse engineering this
function we determined the keys of the normal and privileged users, and
we learned that the admin key was stored at location 0x3500 in the
flash. Unfortunately, as explained above, our dump ended at exactly that
address.

-   Normal User Key:

        Riscure is cool!

-   Privileged User Key:

        Getting closer!!

We reverse engineered the "read flash config" function, which had been
used to dump the flash, and noted that there is a check to see if the
upper byte of the area being dumped is 0x35. If so, the actual flash
contents are not returned. We also learned that this check was gated by
a value in the EEPROM: if the byte at offset 0x26 in the EEPROM is 1,
the read is blocked; for any other value, the read would proceed as
normal, enabling memory addresses beyond 0x3500 to be dumped as well.

Debug UART
----------

During the static analysis of the binary, we noted that strings were
being passed to a function that looks similar to `printf`, and which
outputs to the UART. This function does a `sprintf` to a buffer and then
bit-bangs the results over a pin in PORTB. We attached an FTDI cable to
the appropriate output pin (D8) in order to read this debug info.

Stack Buffer Overflow Exploit
-----------------------------

While further exploring the menus, we noted what appeared to be a stack
buffer overflow in the "Send command to backend ICS network" menu
functionality. This functionality requests an "instruction length" from
the user, then an "instruction opcode", and then prints "the inserted
command was" followed by the actual input ("opcode") provided by the
user. By interactively playing around with these inputs, as well as
statically analyzing the related code, we learned that in fact there
were two buffer overflows here:

1.  A read overflow: the ultimate output ("the inserted command was") is
    of length "instruction length" provided by the user; thus, by
    providing a length longer than the actual "opcode", we get a glimpse
    of the memory beyond the command buffer -- presumably, the stack;
2.  A write overflow: by inserting a command longer than the capacity of
    the stack buffer into which it is placed, we can overwrite portions
    of the stack.

Combining these two overflows should allow us to construct a payload
that will overwrite the return address stored on the stack, thus gaining
control over the program counter.

### Stack Canary

Confirmation that indeed a stack buffer overflow was occurring was
provided by printouts to that effect made to the Debug UART discovered
earlier. However, the fact that the overflows are being detected implies
a detection method --which presumably could also prevent any code
execution. And indeed, the static analysis showed that a stack canary is
used in this function to detect buffer overflows. However, in this case
the canary consists of single-byte constant value ('0xc8'), so it is not
difficult to overcome.

### No Execution from RAM

The AVR is not capable of running code from RAM, but only from flash.
This means that we cannot provide our own code to be executed, but
rather we must find existing code that will be useful to us to run.
Luckily, such a function had already been found: Besides the
`eeprom_set_memory_protected` function, which is called every time the
board boots up, before entering the main command loop, we also found a
function `eeprom_set_memory_unprotected`, which exists in the code, but
it is never called. This function simply writes 0 to offset 0x26 in the
EEPROM (which, recall, would allow flash beyond address 0x3500 to be
dumped).

### Constructing the Exploit

By combining all of the above information, we are able to craft our
exploit payload:

-   we need to bypass the stack canary;
-   we need to replace the return address with the address of
    `eeprom_set_memory_unprotected` (0xbca);
-   after that function is executed, we need to continue running as
    normal, in order to enable us to dump the flash again; so we need to
    call the main command loop (at 0xd01).

This leads to the following exploit payload:

    c8c8c8c8c8c8c8c8c8c8c8c8c8c8c8c8c80bca0d01

Now that the memory is unprotected, dumping the flash beyond 0x3500
becomes possible, using the "read flash config" functionality described
above. We dumped the flash, and thus the admin key was retrieved:

    RHme+ C0n7ac7_u5

Dumping the EEPROM (just for kicks)
-----------------------------------

Using the code execution described above, we decided to try to read the
entire contents of the EEPROM. This requires a ROP chain, since, as
explained above, we must make do with code already existing in flash, as
we cannot execute from RAM, and cannot write to flash.

The gadgets we used were:

  Address   Gadget
  --------- -----------------------------
  0x0434    r25:r24 = 0; pop Y
  0x048d    r24 = uart\_recv\_byte()
  0x1573    r24 = eeprom\_read(r25:r24)
  0x0474    uart\_send\_byte(r24)
  0x1787    r25:r24 += 1

This is the crafted payload:

    payload  = 'c8c8c8c8c8c8c8c8c8c8c8c8c8c8c8c8c8' 
    payload += '0434' # r25:r24 = 0; pop Y 
    payload += '0000' # -- dummy.                  ## This value will be popped in to Y
    payload += '048d' # r24 = uart_recv_byte()     ## Read the low eeprom address from the serial port
    payload += '1573' # r24 = eeprom_read(r25:r24) ## Read an eeprom byte from the address that came in from the serial port
    payload += '0474' # uart_set_byte(r24)         ## Send the eeprom byte read, to the serial port

The above payload dumps a single value from the EEPROM, from the offset
provided over the UART. This was used in a loop to dump offsets
0x00-0xFF.

However, since only a single byte "offset" is read from the UART, we
cannot access offsets beyond 0xFF. So in order to dump the 0x100 - 0x3FF
range, we needed to set r25 to 1,2 or 3. This was achieved by inserting
the following payload after zeroing r25:r24:

    payload += '1578' # r24 = uart_recv_byte()    ## We will send FF. so r24 = 0xFF
    payload += '1787' # r25:r24 += 1              ## Now r25 = 0x01

Inserting the above payload N times will set r25 = N

As expected, we found that EEPROM offset 0x26 contained `0x01`. All
other offsets had the value `0xFF`, except for offset 0x9e, where the
value `0xa8` was observed. We do not know whether this is intentional,
or perhaps was written somehow by accident during our experiments.
However, we found no use being made of it (or any other offset other
than 0x26) by the flashed binary.

EM Glitch
---------

Having obtained the key using software-only techniques, we decided to
also attempt some hardware attacks.

Since only the value 0x1 obtained from offset 0x26 in the EEPROM will
block reads beyond 0x3500, and any other value will allow such reads, we
surmised that another way of gaining access to the flash contents at
0x3500 would be to cause a glitch that would corrupt that value as it
was read from the EEPROM. In order to determine the exact point in time
at which to cause the glitch, we compared the power consumption of the
device when asked to get address 0x3400 and address 0x3500. By noting
the difference between the two traces, we were able to determine the
exact time at which to effect the EM glitch. After a few attempts, we
succeeded in reading the flash contents at 0x3500:

    00000000: 52 48 6D 65 2B 20 43 30  6E 37 61 63 37 5F 75 35  RHme+ C0n7ac7_u5
    00000010: 0D 0A 43 61 6C 63 75 6C  61 74 69 6E 67 20 69 6E  ..Calculating in
    00000020: 64 65 78                                          dex

Differential Power Analysis
---------------------------

Another thought was to use DPA to discover the AES key. However,
standard DPA methods on the first/last round of AES can reveal only the
first 4 bytes of the first/last round key. Therefore, we did not use
such techniques.

More sophisticated attacks on internal rounds where not considered due
to timing limitations.

Similarly, Differential Fault Attack on the 8th and 9th round were also
not considered.

Reading Flash / EEPROM with Atmel Programmer
--------------------------------------------

We connected the Atmel ISP programmer to the board. When we tried to
read the flash we got all FF's. This is consistent with the reported
state of the fuses -- namely, they were set to not allow the program to
be read. We tried reading in High Voltage Mode, but did not succeed.

