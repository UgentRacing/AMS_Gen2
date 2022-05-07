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
uint16_t blink_delay = 1000; /* OK blink */

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
	for(uint8_t i = 0; i < NUM_SLAVES; i++){
		char success = ams_slave_test_spi(slaves[i]);

		/* Check if successful */
		if(!success){
			blink_delay = 100; /* Error blink */

			/* TODO: Send CAN message */

			continue;
		}

		/* Setup register map */
		ams_slave_setup(slaves[i]);
	}
}

/* LOOP */
void loop() {
	/* Blink LED */
	digitalWrite(PIN_DEBUG, HIGH);
	delay(blink_delay);
	digitalWrite(PIN_DEBUG, LOW);
	delay(blink_delay);
}

