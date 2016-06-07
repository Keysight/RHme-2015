#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <stdint.h>

#include "serial_io.h"

uint8_t eeprom_is_memory_protected(void) {
    return eeprom_read_byte((uint8_t *)EEPROM_CHECK_ADDR);
}

void eeprom_set_memory_protected(void) {
    serial_printf("%S", PSTR("Calling eeprom_set_memory_protected\r\n"));
    eeprom_write_byte((uint8_t *)EEPROM_CHECK_ADDR, 1);
}

void eeprom_set_memory_unprotected(void) {
    serial_printf("%S", PSTR("Calling eeprom_set_memory_unprotected\r\n"));
    eeprom_write_byte((uint8_t *)EEPROM_CHECK_ADDR, 0);
}
