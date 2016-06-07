#ifndef SERIAL_IO_H
#define SERIAL_IO_H

#include <stdarg.h>

#include "dbg_putchar.h"

#ifdef ATX_MEGA

#include "usartx.h"

#elif defined AT_MEGA

#include "usart.h"

#endif

//#define ENABLE_DEBUG 1

#ifdef ENABLE_DEBUG
	#define DEBUG_PRINT(x, args...) serial_printf(x, args)
#else
	#define DEBUG_PRINT(x, args...)
#endif

void serial_send_char(char);
void serial_printf(const char *, ...);
void serial_printf_debug(const char *, ...);
void serial_read(char *, uint8_t);

#endif
