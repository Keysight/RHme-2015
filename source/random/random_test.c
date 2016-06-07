#include <avr/io.h>
#include <util/delay.h>

#include "serial_io.h"
#include "random.h"

static void byte_to_str(uint8_t, char *);

int main(void) {
    int i;
    uint16_t vals[256];
    char buff[9];

    serial_init();
    random_init();

    for (i = 0; i < 256; i++)
        vals[i] = 0;

    for (i = 0; i < 200; i++) {
        _delay_ms(5);
        vals[ get_random_hw() ]++;
    }

    serial_printf("\r\n");
    for (i = 0; i < 256; i++)
        if (vals[i] != 0) {
            byte_to_str(i, buff);
            serial_printf("%s %d\r\n", buff, vals[i]);
        }

    while(1);
    return 0;
}

static void byte_to_str(uint8_t byte, char *output) { // exptected to have at least 8 chars + '\0'
    int8_t i, j;

    i = 7;
    while(byte) {
        for (j = i; j < 7; j++)
            output[j] = output[j + 1];
        output[7] = '0' + byte % 2;
        i--;
        byte /= 2;
    }

    for (j = i; j >= 0; j--)
        output[j] = '0';

    output[8] = '\0';
}
