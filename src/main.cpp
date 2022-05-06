#include <Arduino.h>
#include "ams_slave.h"

/* Config */
#define NUM_SLAVES 1 /* Total number of slaves */
#define NUM_SEGMENTS 1 /* Total number of segments */

/* Pin Definitions */
#define PIN_DEBUG 14
const uint8_t PIN_CS_SLAVE[NUM_SLAVES] = {1};


/* Vars */
ams_slave* slaves[NUM_SLAVES];
uint8_t blink_delay;

/* SETUP */
void setup() {
	/* Setup IO */
	pinMode(PIN_DEBUG, OUTPUT);
	digitalWrite(PIN_DEBUG, LOW);

	/* Init SPI */
	spi_init();

	/* Init slaves */
	for(uint8_t i = 0; i < NUM_SLAVES; i++){
		slaves[i] = ams_slave_init(
			i, /* ID */
			i / NUM_SEGMENTS, /* Segment ID */
			TYPE_13, /* Type */
			PIN_CS_SLAVE[i] /* Chip Select pin */
		);
	}

	/* Check is slaves are connected */
	char success = 0b1;
	for(uint8_t i = 0; i < NUM_SLAVES; i++){
		success &= ams_slave_test_spi(slaves[i]);
	}
	blink_delay = success ? 1000 : 200;
}

/* LOOP */
void loop() {
	/* Blink LED */
	digitalWrite(PIN_DEBUG, HIGH);
	delay(blink_delay);
	digitalWrite(PIN_DEBUG, LOW);
	delay(blink_delay);
}

