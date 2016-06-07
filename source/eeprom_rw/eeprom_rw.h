#ifndef EEPROM_RC_H
#define EEPROM_RC_H

uint8_t eeprom_is_memory_protected(void);
void eeprom_set_memory_protected(void);
void eeprom_set_memory_unprotected(void) __attribute__((section(".set_memory_unprotected")));

#endif
