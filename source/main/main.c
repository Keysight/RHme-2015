#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <avr/pgmspace.h>

#include "serial_io.h"
#include "menu.h"
#include "random.h"
#include "eeprom_rw.h"
#include "examine_stack.h"

const char *inbuff;

//const char flag[] __attribute__((section(".flagsection"))) = "RHme+_Y0u_G0t_1t";

/*
 * these externs are declared in auth.c
 */
extern uint8_t is_authenticated;
extern uint8_t is_privileged;
extern uint8_t is_administrator;
extern uint8_t is_auth_nonce_valid;

int main(void) {
    /*
     * initialize the buffer which will be used storing input
     */
    inbuff = (char *)malloc( sizeof(char) * INBUFF_LEN );

    /*
     * just to be on the safe side,
     * make sure there is not auth session
     * enabled when the board boots
     */
    is_authenticated = 0;
    is_privileged = 0;
    is_administrator = 0;
    is_auth_nonce_valid = 0;

    /*
     * commence ctf! :D
     */
    serial_init();
    dbg_tx_init();
    random_init();
    eeprom_set_memory_protected();
    menu_main();

    return 0;
}
