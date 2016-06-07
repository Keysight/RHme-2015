#ifndef USART_H
#define USART_H

#include <stdint.h>

# define USART_BAUDRATE 1000000
# define BAUD_PRESCALLER ((( F_CPU / ( USART_BAUDRATE * 16UL))) - 1)

void serial_init(void);

void usart_send_byte(uint8_t);
void usart_print(char *);
uint8_t usart_data_available(void);
uint8_t usart_recv_byte(void);
void usart_read_str(char *, uint8_t);

#endif
