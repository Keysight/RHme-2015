#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdint.h>

#include "random.h"
#include "serial_io.h"

#define LFSR_ROUNDS 10

static uint32_t gen_seed(void);
static uint32_t lfsr(uint32_t); // linear feedback shift register

#ifdef RAND_PB
static void random_pb_init(void);
static uint8_t get_pb_rand(void);

static void random_pb_init(void) {
    // set all pins of port B as inputs
    DDRB = 0x00;

    // deactivate pull-up registers; make the pins floating
    PORTB = 0x00;
}

static uint8_t get_pb_rand(void) {
    return PINB;
}

#elif defined RAND_ADC
static void random_adc_init(void);
static uint8_t get_adc_rand(void);

static void random_adc_init(void) {
    // AREF = AVcc
    ADMUX = (1 << REFS0);

    // ADC Enable and prescaler of 128
    // 16000000/128 = 125000
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

static uint8_t get_adc_rand(void) {
    /*
     * the ADC on the atmega has a resolution of 10 bits only
     * although this function return a uint16_t, the 6 MSBs will
     * always be the same
     */

    uint8_t ch = 1; // read ADC3

    // select the corresponding channel (0 to 5)
    ADMUX = (ADMUX & 0xF8) | ch;

    // start single conversion
    // write '1' to ADSC
    ADCSRA |= (1 << ADSC);

    // wait for conversion to complete
    // ADSC becomes '0' again
    // till then, run loop continuously
    while ( ADCSRA & (1 << ADSC) );

    return (uint8_t)(ADC);
}

#endif

void random_init(void) {
#ifdef RAND_PB
    random_pb_init();
#elif defined RAND_ADC
    random_adc_init();
#endif
}

uint8_t get_random_hw(void) {
    uint8_t r;
#ifdef RAND_PB
    r = get_pb_rand();
#elif defined RAND_ADC
    r = get_adc_rand();
#endif
    return r;
}

uint32_t get_random_uint32_t(void) {
    /*
     * we use srandom and random instead of srand and rand because the former
     * operate on unsigned long instead
     *
     * calling srandom every time is probably not a great idea (lol..)
     * especially since eeprom can be changed at runtime with a programmer
     * thus ensuring the same random numbers are returned each time
     */
    uint32_t rnd;

    srandom( gen_seed() );

    rnd = random() & 0xFFFF;
    rnd <<= 16;
    rnd |= random() & 0xFFFF;

    return lfsr(rnd);
}

static uint32_t gen_seed(void) {
    uint8_t i, aux;
    uint32_t seed;

    seed = 0;
    for (i = 0; i < 32; i++) {
        /*
         * for each bit, a modified von neumann corrector is applied
         * to the last two bits of the hardware random number generator
         *
         * this is modified because we want the RNG to be easily manipulated
         * when the ADC is connected to GND and returns 0 always
         */

        aux = get_random_hw() & 0b11;
        while (aux == 0b01 || aux == 0b11)
            aux = get_random_hw() & 0b11;

        if (aux == 0b10)
            seed |= 1;
        else if (aux == 0b00)
            seed |= 0;

        seed <<= 1;
    }

    return seed;
}

static uint32_t lfsr(uint32_t input) {
    uint32_t output;
    uint32_t bit;
    int i;

    for (i = 0; i < LFSR_ROUNDS; i++) {
        bit = (input >> 7) ^ (input >> 15) ^ (input >> 19) ^ (input >> 22) ^ (input >> 26) ^ (input >> 29);
        bit &= 1;
        output = (input >> 1) | (bit << 31);
    }

    return output;
}
