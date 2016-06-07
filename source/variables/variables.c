#include <stdio.h>
#include <ctype.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "serial_io.h"

char user_auth_response[9];

const char *variables[7][2] = {
    { "Terminal name",              "Erucsir007" },
    { "Accounts",                   "Enabled (2/2), Active (2/2)" },
    { "Operational mode",           "Enabled" },
    { "Debug mode",                 "Enabled" },
    { "Physical location",          "Delft, Netherlands" },
    { "User authentication token",  user_auth_response },
    { "Hello",                      "World!" } // this one is just to mess with people :)
};

void variables_menu(uint8_t k) {
    uint8_t i;

    for (i = 0; i < k; i++)
        if ( isalpha(variables[i][0][0]) )
            serial_printf("%-30s %s\r\n", variables[i][0], variables[i][1]);

    serial_printf("%S", PSTR("Finished printing\r\n"));
}
