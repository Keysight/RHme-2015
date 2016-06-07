#include <stdio.h>

#include "dbg_putchar.h"
#include "serial_io.h"

char prbuff[PRBUFF_LEN];

#ifdef ATX_MEGA

void serial_printf(const char *format, ...) {
    va_list args;

    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void serial_read(char *buffer, uint8_t len) {
    getsedit(buffer, len, NULL);
}

#elif defined AT_MEGA

void serial_send_char(char c) {
    usart_send_byte(c);
}

void serial_printf(const char *format, ...) {
    va_list args;

    va_start(args, format);
    vsnprintf(prbuff, PRBUFF_LEN, format, args);
    usart_print(prbuff);
    va_end(args);
}

void serial_printf_debug(const char *format, ...) {
    va_list args;

    va_start(args, format);
    vsnprintf(prbuff, PRBUFF_LEN, format, args);
    dbg_puts(prbuff);
    va_end(args);
}

void serial_read(char *buffer, uint8_t len) {
    usart_read_str(buffer, len);
}

#endif
