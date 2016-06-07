#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <avr/pgmspace.h>

#include "serial_io.h"
#include "examine_stack.h"

#define CANARY 0xc8
#define READ_ADDR_MAX 0x2500

extern char *inbuff;

static uint8_t validate_len(char *, uint8_t);
void send_ics_command(void);

void overflow_rop(void) {
    /*
     * the order of the locals below is rather important because of
     * how they are ultimately placed in the compiled binary
     * the required placement at runtime needs(!!!) to be
     *      canary          highest address
     *      buff            lowest address
     */
    uint32_t len;
    int8_t i;
    volatile char buff[6];
    volatile uint8_t canary = CANARY;

    /*
     * to check that the locals are placed correctly in memory, decomment
     * the following lines and make sure the addresses are printed from
     * lowest to highest
     */
    serial_printf("%p\r\n", buff);
    serial_printf("%p\r\n", &canary);

    serial_printf("%S", PSTR("Insert instruction length (min 0, max 4)\r\n"));
    serial_read(inbuff, INBUFF_LEN);
    serial_printf("%S", PSTR("\r\n"));
    if ( validate_len(inbuff, INBUFF_LEN) ) {
        len = strtoul(inbuff, NULL, 10);
        serial_printf("Length %x\r\n", len);

        serial_printf("%S", PSTR("Insert instruction opcode\r\n"));
        serial_printf("%S", PSTR(""));
        serial_read((char *)buff, len + 1);
        serial_printf("%S", PSTR("\r\n"));

        serial_printf("%S", PSTR("The inserted command was\r\n"));
        for (i = 0; i < len; i++)
            serial_send_char(buff[i]);
        serial_printf("\r\n");

        if (canary != CANARY) {
            serial_printf_debug("%S", PSTR("stack buffer overflow detected\r\n"));
            serial_printf_debug("%S", PSTR("disabling terminal\r\n"));
            while(1);
        } else
            send_ics_command();
    } else
        serial_printf_debug("%S", PSTR("invalid length\r\n"));
}

static uint8_t validate_len(char *buff, uint8_t len) {
    int k = 0;

    if (buff[0] == '\0')
        return 0;
    while (buff[k] != '\0') {
        if ( !isdigit(buff[k]) )
            return 0;
        k++;
    }
    return 1;
}

void send_ics_command(void) {
    serial_printf_debug("%S", PSTR("function send_ics_command not implemented\r\n"));
}
