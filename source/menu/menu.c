#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#include "serial_io.h"
#include "variables.h"
#include "auth.h"
#include "overflow_readflash.h"
#include "overflow_rop.h"

static void menu_loop(void);
static uint8_t verify_int(char *);
static void trollTheHacker();

extern uint8_t is_authenticated;
extern uint8_t is_administrator;
extern uint8_t is_privileged;
extern char *inbuff;

uint8_t valve = 0;

const char commands_text[] PROGMEM =
    "Possible commands:\r\n";

const char menu_message_start[] PROGMEM =
    "\tA\tauthenticate\r\n"
    "\tRxxxx\tsubmit response (xxxx = response in hex)\r\n"
    "\tV\tretrieve variables\r\n";

const char menu_message_auth_user[] PROGMEM =
    "\tR\tread flash config\r\n"
    "\tP\tprint the valve state\r\n";

const char menu_message_privileged_user[] PROGMEM =
    "\tV\tchange the valve state\r\n"
    "\tS\tsend command to backend ICS network\r\n";

const char menu_message_auth_admin[] PROGMEM =
    "\tK\tread the Admin key\r\n";



const char menu_message_logout[] PROGMEM =
    "\tX\tlog out\r\n";

const char help_end[] PROGMEM =
    "\tH\tshow help\r\n"
    "\r\n";

const char troll[13][26] __attribute__((section(".flagsection"))) = {"\r\nCalculating index",
                                    "\r\nChecking Key CRC",
                                    "\r\nShuffling bits",
                                    "\r\nMore shuffling",
                                    "\r\nEven more shuffling!",
                                    "\r\nDeshuffling bits",
                                    "\r\nError! Dizzy bit!",
                                    "\r\nApplying Dimenhydrinate",
                                    "\r\nThe bit doesnt respond!",
                                    "\r\nAny doctor in room?",
                                    "\r\nBit recovered.Printing",
                                    "\r\nThe key is",
                                    "\r\nMUAHAHAAH!! TROLLED!\r\n"
                                };



void menu_main(void) {
    serial_printf("%s", "Welcome to Erucsir ICS terminal\r\n");
    serial_printf("%S", commands_text);
    serial_printf("%S", menu_message_start);
    serial_printf("%S", help_end);
    while(1)
        menu_loop();
}

static void menu_loop(void) {
    int8_t aux;
    uint8_t res;

    serial_read(inbuff, INBUFF_LEN);
    serial_printf("%S", PSTR("\r\n"));

    if (is_authenticated == 0) {
        if (inbuff[0] == 'A') {
            generate_challenge();

        } else if (inbuff[0] == 'R') {
            verify_response();

        } else if (inbuff[0] == 'V') {
            /*
             * we will be using strtol() to convert the string to a long which
             * gets downcasted to int, but this is not an issue as we verify the
             * length of the string in verify_int
             *
             * the variables_menu function though excepts an unsigned int
             *
             * a regular int for this board is 16 bits, meaning that an unsigned
             * int takes values from 0 to 65535, thus -1 as signed is 65535 unsigned
             */
            serial_printf("%S", PSTR("How many variables should be printed? (max 5)\r\n"));
            serial_read(inbuff, INBUFF_LEN);
            serial_printf("%S", PSTR("\r\n"));

            res = verify_int(inbuff);
            if (res == 1) {
                aux = strtol(inbuff, NULL, 10);
                if (aux > 5) {
                    res = 0;
                } else {
                    serial_printf("Printing %d variables\r\n", aux);
                    variables_menu(aux);
                }
            }
            if (res == 0) {
                serial_printf("%S", PSTR("Input is not a valid number \r\n"));
                serial_printf("%S", PSTR("Please enter a number smaller than 5\r\n"));
            }

        } else if (inbuff[0] == 'H') {
            serial_printf("%S", commands_text);
            serial_printf("%S", menu_message_start);
            serial_printf("%S", help_end);
        } else
            serial_printf("Command %s is invalid\r\n", inbuff);

    } else { // is_authenticated == 1

        switch (inbuff[0]) {

            case 'R':
                overflow_readflash();
                break;

            case 'X':
                is_privileged=is_administrator=is_authenticated=0;
                serial_printf("%s", "Welcome to Erucsir ICS terminal\r\n");
                serial_printf("%S", commands_text);
                serial_printf("%S", menu_message_start);
                serial_printf("%S", help_end);
                break;

            case 'H':
                serial_printf("%S", commands_text);
                serial_printf("%S", menu_message_auth_user);
                if (is_privileged==1 || is_administrator==1)
                    serial_printf("%S", menu_message_privileged_user);
                if (is_administrator)
                    serial_printf("%S", menu_message_auth_admin);
                serial_printf("%S", menu_message_logout);
                serial_printf("%S", help_end);
                break;


            case 'P':
                if (valve==0)
                    serial_printf("Valve closed\r\n");
                else
                    serial_printf("Valve open\r\n");
                break;

            case 'V':
                if (is_privileged==1 || is_administrator == 1) {
                    if (valve==0)
                        valve=1;
                    else
                        valve=0;
                    serial_printf("Valve changed\r\n");
                    break;
                }
                goto switch_default;

            case 'S':
                if (is_privileged==1 || is_administrator == 1) {
                    overflow_rop();
                    break;
                }
                goto switch_default;

            case 'K':
                if (is_administrator == 1) {
                    trollTheHacker();
                    break;
                }
                // Dont break here!

            default:
                switch_default:
                serial_printf("Command %s is invalid\r\n", inbuff);

        }

    }
}

static void returnTrollString (char * buffer, uint8_t index) {

    uint8_t i;
    //char key[] = "\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa";

    strncpy_P(buffer,((const PROGMEM char *)(troll[index])),26);

    /*for (i=0; i<26; i++) {
        buffer[i] ^= 0x01;
        //buffer[i] ++;
    }*/

}

static void trollTheHacker() {

    volatile uint32_t j;
    uint8_t i;

    char buffer[26];

    serial_printf("Retriving the Admin Key");

    for (i=1; i<131; i++) {
        for (j=0; j<900000; j++) {

        }
        serial_printf(".");
        if (i%10 == 0) {
            returnTrollString(buffer,i/10);

            serial_printf("%s",buffer);
        }
    }
}

static uint8_t verify_int(char *buff) {
    int i;
    int len;

    len = strlen(buff);
    for (i = 0; i < len; i++)
        if ( !(isdigit(buff[i]) || buff[i] == '-') ) {
            serial_printf_debug("%S", PSTR("int parsing routine detected invalid characters\r\n"));
            return 0;
        }

    if ( !((buff[0] == '-' && len == 2) || len == 1) ) {
        serial_printf_debug("%S", PSTR("int parsing routine detected invalid input size\r\n"));
        return 0;
    }

    return 1;
}
