#include <Arduino.h>

/* Pin Definitions */
#define PIN_DEBUG 13

/* SETUP */
void setup() {
	/* Setup IO */
	pinMode(PIN_DEBUG, OUTPUT);
	digitalWrite(PIN_DEBUG, LOW);
}

/* LOOP */
void loop() {
	/* Blink LED */
	digitalWrite(PIN_DEBUG, HIGH);
	delay(500);
	digitalWrite(PIN_DEBUG, LOW);
	delay(500);
}

