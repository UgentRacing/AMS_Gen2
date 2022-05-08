#include <Arduino.h>
#include <FlexCAN_T4.h>
#include "ams_slave.h"

/* Config */
#define NUM_SLAVES 1 /* Total number of slaves */
#define NUM_SEGMENTS 1 /* Total number of segments */

/* Pin Definitions */
#define PIN_DEBUG 14
const uint8_t PIN_CS_SLAVE[NUM_SLAVES] = {1};

/* Vars */
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can;
ams_slave* slaves[NUM_SLAVES];
uint16_t blink_delay = 1000; /* OK blink */

/* CAN */
void can_on_receive(const CAN_message_t &msg){

}
void can_init(){
	can.begin();
	can.setBaudRate(1000000);
	can.setMaxMB(16);
	can.enableFIFO();
	can.onReceive(can_on_receive);

	Serial.begin(115200);
}

/* SETUP */
void setup() {
	/* Setup IO */
	pinMode(PIN_DEBUG, OUTPUT);
	digitalWrite(PIN_DEBUG, LOW);

	/* Init CAN */
	can_init();

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

			/* Send CAN message */
			CAN_message_t m;
			m.id = 0x00;
			m.buf[0] = 0x46;
			m.buf[1] = slaves[i]->segment;
			can.write(m);

			continue;
		}

		/* Setup register map */
		// ams_slave_setup(slaves[i]);
	}
}

/* LOOP */
void loop() {
	/* Blink LED */
	digitalWrite(PIN_DEBUG, HIGH);
	delay(blink_delay);
	digitalWrite(PIN_DEBUG, LOW);
	delay(blink_delay);

	/* Dump register map */
	for(uint8_t i=0; i<NUM_SLAVES; i++){
		for(char r=0x00; r<0x8a; r++){
			/* Read values */
			char buff;
			ams_slave_read(slaves[i], r, &buff);

			/* Send CAN messages */
			CAN_message_t m;
			m.id = 0x06;
			m.buf[0] = slaves[i]->segment;
			m.buf[1] = i;
			m.buf[2] = r;
			m.buf[3] = buff;
			can.write(m);

			Serial.println(buff == 0b11110000 ? "correct" : "false");
			Serial.printf("(%d) [%d] %d\t\t- ", i, r, buff);
			for (uint8_t x = (1 << 7); x > 0; x = x >> 1)
				Serial.printf("%s", (buff & x) ? "1" : "0");
			Serial.println("");

			delayMicroseconds(10);
		}
	}

	/* Perform voltage scan */
	for(uint8_t i=0; i<NUM_SLAVES; i++){
		ams_slave_trigger_system_scan(slaves[i]);
	}
	for(uint8_t i=0; i<NUM_SLAVES; i++){
		/* Wait until scan complete */
		while(!ams_slave_is_idle(slaves[i])){
			delayMicroseconds(10);
		}

		/* Read values */
		uint16_t buff[16];
		ams_slave_read_voltages(slaves[i], buff);

		/* Send CAN messages */
		uint8_t num = slaves[i]->type == TYPE_13 ? 13 : 10;
		for(uint8_t j=0; j<num; j++){
			CAN_message_t m;
			m.id = 0x04;
			m.buf[0] = slaves[i]->segment;
			m.buf[1] = j;
			m.buf[2] = buff[j] >> 8;
			m.buf[3] = buff[j];
			can.write(m);
			delayMicroseconds(10);
		}
	}

	/* Update CAN */
	can.events();
}

