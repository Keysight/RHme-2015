#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "dbg_putchar.h"
#include "serial_io.h"

int main(void) {
    char buff[20];

    //serial_init();

    dbg_tx_init();
    serial_printf_debug("\r\nlolz");

    //serial_printf("Testing output\r\n");
    //serial_printf("Testing input... type something and press enter\r\n");
    //serial_read(buff, 20);
    //serial_printf("\r\n");
    //serial_printf("You typed \"%s\"\r\n", buff);

    while(1);
    return 0;
}
