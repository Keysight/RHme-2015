#include <avr/io.h>

#include "usart.h"

static uint8_t append_char(char *, uint8_t, uint8_t, uint8_t);

void serial_init(void) {
    // set baud rate
    uint16_t baud = BAUD_PRESCALLER;
    UBRR0H = (uint8_t)(baud >> 8 );
    UBRR0L = (uint8_t)baud;

    // enable received and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // set frame format (8N1)
    UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
}

void usart_send_byte(uint8_t byte) {
    // wait for empty transmit buffer
    while ( !(UCSR0A & (1 << UDRE0)) )
        ;
    // send byte
    UDR0 = byte;
}

// write a string to the usart
void usart_print(char *s) {
    while (*s != 0)
        usart_send_byte( *(s++) );
}

uint8_t usart_data_available(void) {
    if ( UCSR0A & (1 << RXC0) )
        return 1;
    return 0;
}

uint8_t usart_recv_byte(void) {
    // wait until data is available
    while ( !usart_data_available() )
        ;
    // read byte
    return UDR0;
}

void usart_read_str(char *str, uint8_t max) {
    uint8_t k, eos, ch, ch2;

    k = 0;          // index of first free position in the string
    eos = 0;        // end of string, used as boolean

    while (!eos) {
        ch = usart_recv_byte();

        //if (ch == '\r' || ch == '\n')               // user pressed enter
        if (ch == '\r') {
            ch2 = usart_recv_byte();

            if (ch2 == '\n')
                eos = 1;
            else {
                k = append_char(str, ch, k, max);
                k = append_char(str, ch2, k, max);
            }
        }
        //else if (ch == '\b') {                      // user pressed backspace
        //    if (k > 0) {
        //        usart_send_byte('\b');              // move cursor one char back
        //        usart_send_byte(' ');               // override char with a space
        //        usart_send_byte('\b');              // move cursor one char back again
        //        k--;
        //    }
        //}
        else {
            //if (0x20 <= ch && ch <= 0x7e)           // here we check for printable characters
            //    usart_send_byte(ch);
            //if (k < max - 1)                        // -1 becase \0 at end
            //    str[k++] = ch;
            k = append_char(str, ch, k, max);
        }
    }

    // terminate string accordingly
    str[k] = '\0';
}

static uint8_t append_char(char *str, uint8_t ch, uint8_t k, uint8_t max) {
    if (k < max - 1)
        str[k++] = ch;
    return k;
}
