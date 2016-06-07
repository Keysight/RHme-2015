#include <stdint.h>

#include "serial_io.h"

uint8_t lo;
uint8_t hi;

void examine_stack(void) {
    volatile uint8_t *ptr;  // two bytes
    volatile uint8_t i;     // one byte

    /*
     * here we get the current stack pointer
     * by the time this code gets executed, 11 bytes
     * are pushed (return address and 9 registers)
     *
     * after this code gets executed, the [hi;lo]
     * pointer will be 9 bytes(/registers) distance
     * from the return address of the current frame
     */

    asm volatile(
"in r18, 0x3d   \n\t"
"in r19, 0x3e   \n\t"
"sts hi, r18    \n\r"
"sts lo, r19    \n\t"
    :
    :
    : "r18", "r19"
    );

    ptr = lo;
    ptr = (uint8_t *)((uint16_t)ptr << 8);
    ptr = (uint8_t *)((uint16_t)ptr | hi);

    for (i = 0; i < 16 * 3; i++) {
        if ( i % 16 == 0 )
            serial_printf("\r\n");
        serial_printf("%02x ", ptr[i + 10]); // calculated above
    }
    serial_printf("\r\n");
}
