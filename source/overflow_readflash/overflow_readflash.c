#include <stdlib.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "serial_io.h"
#include "eeprom_rw.h"

#define CANARY 0xc7
#define READ_ADDR_MAX 0x3500

const char config_string[] PROGMEM =
    "MCU family\tATMega328P\r\n"
    "Firmware\t1.2.0-ALPHA\r\n"
    "Build date\t01-10-2015\r\n"
    "Build flags\t-O +x --eep rw --pgmem r --debug\r\n"
    "\r\n";

static uint8_t totp_validation(uint32_t);

void overflow_readflash(void) {
    /*
     * the order of the locals below is rather important because of
     * how they are ultimately placed in the compiled binary
     * the required placement at runtime needs(!!!) to be
     *      confstr_addr    high address
     *      buff            low address
     */
    volatile uint8_t continue_read = 1;
    volatile uint8_t protected_mem;
    volatile uint8_t str_byte;
    volatile char buff[20];             // 20 bytes
    volatile uint16_t confstr_addr;     // 2 bytes
                                        // 22 bytes in total
    volatile uint8_t nocrash;
    confstr_addr = (uint16_t)(&config_string);

    serial_printf("%S", PSTR("Please insert your TOTP user token\r\n"));
    serial_read((char *)buff, 23);   // 22 calculated above + 1 for \0
    serial_printf("%S", PSTR("\r\n"));

    serial_printf("%S", PSTR("Contacting TOTP validation server\r\n"));
    if ( totp_validation( atoi((char *)buff) ) ) {
        serial_printf("%S", PSTR("Token validated\r\n"));

        str_byte = pgm_read_byte_near(confstr_addr++);
        while (str_byte != 0) {
            protected_mem = eeprom_is_memory_protected();
            if ( protected_mem == 1 && confstr_addr > READ_ADDR_MAX )
                continue_read = 0;

            if (continue_read == 1) {
                serial_printf("%c", str_byte);
                str_byte = pgm_read_byte_near(confstr_addr++);
            } else {
                serial_printf("%S", PSTR("\r\n"));
                serial_printf_debug("%S", PSTR("instruction at untrusted address attempted to read from unauthorized memory address\r\n"));
                break;
            }
        }
    }
}

static uint8_t totp_validation(uint32_t token) {
    serial_printf_debug("%S", PSTR("function totp_validation not implemented\r\n"));
    serial_printf_debug("%S", PSTR("returning 1\r\n"));
    return 1;
}
