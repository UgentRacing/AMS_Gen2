#include <Arduino.h>
#include <FlexCAN_T4.h>
#include "ams_slave.h"
#include <cmath>

/* Config */
#define NUM_SLAVES 1   /* Total number of slaves */
#define NUM_SEGMENTS 1 /* Total number of segments */

/* Pin Definitions */
#define PIN_DEBUG 14
const uint8_t PIN_CS_SLAVE[NUM_SLAVES] = {1};

/* Vars */
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can;
ams_slave *slaves[NUM_SLAVES];
uint16_t blink_delay = 1000; /* OK blink */

/* CAN */
void can_on_receive(const CAN_message_t &msg)
{
}
void can_init()
{
	can.begin();
	can.setBaudRate(1000000);
	can.setMaxMB(16);
	can.enableFIFO();
	can.onReceive(can_on_receive);

	Serial.begin(115200);

	delay(5000);

	Serial.println("Start");
}

/* SETUP */
void setup()
{
	/* Setup IO */
	pinMode(PIN_DEBUG, OUTPUT);
	digitalWrite(PIN_DEBUG, LOW);

	/* Init CAN */
	can_init();

	/* Init SPI */
	spi_init();

	/* Init slaves */
	for (uint8_t i = 0; i < NUM_SLAVES; i++)
	{
		slaves[i] = ams_slave_init(
			i,				  /* ID */
			i / NUM_SEGMENTS, /* Segment ID */
			TYPE_13,		  /* Type */
			PIN_CS_SLAVE[i]	  /* Chip Select pin */
		);
	}

	Serial.println("wtf1");

	char buff;
	ams_slave_read(slaves[0], 0x00, &buff);

	for (uint8_t x = (1 << 7); x > 0; x = x >> 1)
		Serial.printf("%s", (buff & x) ? "1" : "0");

	/* Check is slaves are connected */
	for (uint8_t i = 0; i < NUM_SLAVES; i++)
	{
		char success = ams_slave_test_spi(slaves[i]);

		Serial.println(success);

		/* Check if successful */
		if (!success)
		{
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
		ams_slave_setup(slaves[i]);
		char buff;
		ams_slave_read(slaves[0], 0x00, &buff);

		for (uint8_t x = (1 << 7); x > 0; x = x >> 1)
			Serial.printf("%s", (buff & x) ? "1" : "0");
	}

	Serial.println("wtf2");

	delay(2000);
	Serial.println("wtf3");

	while (ams_slave_is_idle(slaves[0]))
	{
	}
	Serial.println("wtf4");
}

void dump_register_map()
{
	for (uint8_t i = 0; i < NUM_SLAVES; i++)
	{
		for (char r = 0x00; r < 0x8a; r++)
		{
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

			Serial.printf("(%d) [", i);
			Serial.print(r, HEX);
			Serial.print("] ");
			for (uint8_t x = (1 << 7); x > 0; x = x >> 1)
				Serial.printf("%s", (buff & x) ? "1" : "0");
			Serial.println("");

			delayMicroseconds(10);
		}
	}
}

/* LOOP */
void loop()
{

	/* Dump register map */
	dump_register_map();

	// Now try a voltage measurement (manual)
	Serial.println("Voltage scan");
	ams_slave_write(slaves[0], 0x02, 0b10000001);
	Serial.println("Voltage scan init");
	while (ams_slave_is_idle(slaves[0]))
	{
		Serial.println("Idle");
		delay(100);
	}
	dump_register_map();

	ams_slave_write(slaves[0], 0x02, 0b10000010);

	while (ams_slave_is_idle(slaves[0]))
	{
		Serial.println("Idle");
		delay(100);
	}

	dump_register_map();

	// 	/* Read values */
	uint16_t buff[16];
	ams_slave_read_voltages(slaves[0], buff);

	/* Send CAN messages */
	uint8_t num = slaves[0]->type == TYPE_13 ? 13 : 10;

	const double vcell_step = 73.55 * pow(10, -3); // This is in mV

	for (uint8_t j = 0; j < num; j++)
	{
		CAN_message_t m;
		m.id = 0x04;
		m.buf[0] = slaves[0]->segment;
		m.buf[1] = j;
		m.buf[2] = buff[j] >> 8;
		m.buf[3] = buff[j];
		can.write(m);

		Serial.printf("VOL (%d) [%d] ", 0, j);
		Serial.print(vcell_step * buff[j] + 0.5 * vcell_step);
		Serial.println(" mV");

		delayMicroseconds(10);
	}

	delay(1000);

	// while (true)
	// {
	// }

	// Serial.println("Start scan");

	/* Perform voltage scan */
	// for (uint8_t i = 0; i < NUM_SLAVES; i++)
	// {
	// 	ams_slave_write(slaves[i], 0x2e, 0b01011011);
	// 	char buff;
	// 	ams_slave_read(slaves[i], 0x01, &buff);
	// 	Serial.printf("scan - 0x01: ");
	// 	for (uint8_t x = (1 << 7); x > 0; x = x >> 1)
	// 		Serial.printf("%s", (buff & x) ? "1" : "0");
	// 	Serial.println("");

	// 	ams_slave_read(slaves[i], 0x2e, &buff);
	// 	Serial.printf("scan - 0x2e: ");
	// 	for (uint8_t x = (1 << 7); x > 0; x = x >> 1)
	// 		Serial.printf("%s", (buff & x) ? "1" : "0");
	// 	Serial.println("");

	// 	ams_slave_write(slaves[i], 0x2e, 0b01011011);

	// 	ams_slave_trigger_system_scan(slaves[i]);
	// }

	// Serial.println("Wait scan to end");

	// for (uint8_t i = 0; i < NUM_SLAVES; i++)
	// {
	// 	/* Wait until scan complete */
	// 	while (!ams_slave_is_idle(slaves[i]))
	// 	{
	// 		delayMicroseconds(10);

	// 		char buff;
	// 		ams_slave_read(slaves[i], 0x01, &buff);
	// 		Serial.printf("results - 0x01: ");
	// 		for (uint8_t x = (1 << 7); x > 0; x = x >> 1)
	// 			Serial.printf("%s", (buff & x) ? "1" : "0");
	// 		Serial.println("");
	// 	}
	// 	Serial.println("Scan ended, readout values");

	// 	/* Read values */
	// 	uint16_t buff[16];
	// 	ams_slave_read_voltages(slaves[i], buff);

	// 	// for (uint8_t y = 0; y < 15; y++) {
	// 	// 	for (uint16_t x = (1 << 15); x > 0; x = x >> 1) {
	// 	// 		Serial.printf("%s", (buff[y] & x) ? "1" : "0");
	// 	// 	}

	// 	// 	Serial.println("");
	// 	// }

	// 	/* Send CAN messages */
	// 	uint8_t num = slaves[i]->type == TYPE_13 ? 13 : 10;

	// 	const double vcell_step = 73.55 * pow(10, -3); // This is in mV

	// 	for (uint8_t j = 0; j < num; j++)
	// 	{
	// 		CAN_message_t m;
	// 		m.id = 0x04;
	// 		m.buf[0] = slaves[i]->segment;
	// 		m.buf[1] = j;
	// 		m.buf[2] = buff[j] >> 8;
	// 		m.buf[3] = buff[j];
	// 		can.write(m);

	// 		Serial.printf("VOL (%d) [%d] ", i, j);
	// 		Serial.print(vcell_step * buff[j] + 0.5 * vcell_step);
	// 		Serial.println(" mV");

	// 		delayMicroseconds(10);
	// 	}
	// }

	/* Update CAN */
	can.events();
}
