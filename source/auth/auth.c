#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "auth.h"
#include "serial_io.h"
#include "random.h"
#include "aes.h"

uint8_t is_authenticated;
uint8_t is_privileged;
uint8_t is_administrator;
uint8_t is_auth_nonce_valid;
uint8_t k_attempts;
uint32_t auth_nonce;
extern char user_auth_response[9];

const uint8_t flash_aes_key_User[] PROGMEM =
    { 'R','i','s','c','u','r','e',' ','i','s',' ','c','o','o','l','!'};

const uint8_t flash_aes_key_Privileged[] PROGMEM =
    { 'G','e','t','t','i','n','g',' ','c','l','o','s','e','r','!','!'};

const uint8_t flash_aes_key_Admin[] __attribute__((section(".flagsection"))) =
    { 'R','H','m','e','+',' ','C','0','n','7','a','c','7','_','u','5'};

extern char *inbuff; // declared and initialized in main.c

static void uint32_to_uint8_array(uint32_t, uint8_t *);
static void uint32_to_uint8_array_bigEndian(uint32_t, uint8_t *);
static uint32_t uint8_array_to_uint32_bigEndian(uint8_t *);
static uint8_t timing_vulnerable_verification(uint32_t, uint8_t);
static void calculate_response(uint32_t, uint32_t *, uint8_t user, uint8_t calculate_response);

void generate_challenge(void) {
    uint32_t responses[3];

    is_auth_nonce_valid = 1;
    k_attempts = 0;

    auth_nonce = get_random_uint32_t();
    calculate_response(auth_nonce, &responses[0], USER_USER,0);
    calculate_response(auth_nonce, &responses[1], USER_ADMIN,1);    
    calculate_response(auth_nonce, &responses[2], USER_PRIVILEGED,0);
    snprintf(user_auth_response, 9, "%lx", responses[0]);

    serial_printf("Your nonce is %08lx\r\n", auth_nonce);
    serial_printf("%S", PSTR("Please submit your response to finish the authentication process\r\n"));
    DEBUG_PRINT("DEVDEBUG: valid responses are %08lx  %08lx %08lx\r\n", responses[0], responses[1], responses[2]);
    DEBUG_PRINT("DEVDEBUG: auth priv adm %d %d %d\r\n", is_authenticated, is_privileged, is_administrator);
}

void verify_response(void) {
    uint32_t user_input;
    uint8_t i;

    if (is_auth_nonce_valid == 1) {
        user_input = strtoul(inbuff + 1, NULL, 16);
        DEBUG_PRINT("DEBUG: input was %lx\r\n", user_input);

        // commence glitch chaining
        for (i = 0; i < 3; i++) {
            if( timing_vulnerable_verification(user_input, i) == 1 ) { // glitch 1
                is_authenticated = 1;
                if (i == USER_USER) { // glitch 2
                    serial_printf("%S", PSTR("Authentication successful as user\r\n"));
                } else if (i == USER_PRIVILEGED) { // glitch 2
                    serial_printf("%S", PSTR("Authentication successful as privileged user\r\n"));
                    is_privileged = 1;
                } else {
                    serial_printf("%S", PSTR("Authentication successful as administrator\r\n"));
                    is_administrator = 1;
                }
            }
        }

        if (is_authenticated == 0) { // another glitch here, maybe useful if no RNG manipulation and no glitching above
            k_attempts++;
            if ( k_attempts == 255) {
                is_auth_nonce_valid = 0;
                serial_printf("%S", PSTR("Your nonce is now invalid, please request a new one to perform authentication\r\n"));
            } else
                serial_printf("E");
        }
    } else {
        serial_printf("%S", PSTR("You have not requested an authentication nonce\r\n"));
        serial_printf("%S", PSTR("Please start the authentication process with that step\r\n"));
    }
}

static uint8_t timing_vulnerable_verification(uint32_t user_input, uint8_t idx_resp) {
    int i;
    uint32_t responses[3];
    uint8_t buff_resp[4], buff_input[4];

    i = 0;
    uint32_to_uint8_array(user_input, buff_input);
    do {
        calculate_response(auth_nonce, &responses[0], USER_USER, 0);
        calculate_response(auth_nonce, &responses[1], USER_ADMIN, 1);        
        calculate_response(auth_nonce, &responses[2], USER_PRIVILEGED, 2);
        uint32_to_uint8_array( responses[idx_resp], buff_resp );

        if (buff_input[i] != buff_resp[i])
            return 0;

        i++;
    } while(i < 4);

    return 1;
}


static void calculate_response(uint32_t nonce, uint32_t *response, uint8_t user, uint8_t countermeasures) {
    uint8_t i;
    uint8_t aes_data[16], ram_aes_key[16];
    aes128_ctx_t aes_ctx;
    const uint8_t *key;

    if (user == USER_USER)
        key = flash_aes_key_User;
    else if (user == USER_PRIVILEGED) 
        key = flash_aes_key_Privileged;
    else
        key = flash_aes_key_Admin;

    for (i = 0; i < 16; i++)
        ram_aes_key[i] = pgm_read_byte(key + i);

    uint32_to_uint8_array_bigEndian(nonce, aes_data);
    for (i = 4; i < 16; i++)
        aes_data[i] = 0;

    aes128_init(ram_aes_key, &aes_ctx);
    aes128_enc(aes_data, &aes_ctx, countermeasures);

    for (i = 0; i < 16; i++)
        ram_aes_key[i] = 0; // Cleaning the key.

    *response = uint8_array_to_uint32_bigEndian(&aes_data[12]);

    if (user == USER_USER)
        *response = *response & 0x000FFFFF;
    else if (user == USER_PRIVILEGED)
        *response = *response & 0x00FFFFFF;
    else
        *response = *response & 0xFFFFFFFF;
}

static void uint32_to_uint8_array(uint32_t n, uint8_t *buffer) {
    uint8_t *ptr;
    int i;

    ptr = (uint8_t *)&n;
    for (i = 3; i >= 0; i--)
        buffer[i] = ptr[i];
}

static void uint32_to_uint8_array_bigEndian(uint32_t n, uint8_t *buffer) {
    uint8_t *ptr;
    int i;

    ptr = (uint8_t *)&n;
    for (i = 3; i >= 0; i--)
        buffer[i] = *ptr++;
}

static uint32_t uint8_array_to_uint32_bigEndian(uint8_t *buffer) {
    uint32_t val;
    int8_t i;

    val = 0;
    for (i = 0; i <= 3; i++) {
        val <<= 8;
        val |= buffer[i];
    }

    return val;
}
