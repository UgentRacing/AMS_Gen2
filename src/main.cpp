#include <Arduino.h>
#include <FlexCAN_T4.h>
#include "ams_slave.h"
#include <cmath>

/* Config */
#define NUM_SLAVES 10  /* Total number of slaves */
#define NUM_SEGMENTS 1 /* Total number of segments */
#define DEBUG 1
#define SHUTDOWN_CIRCUIT_PIN 20
#define OK_BLINK 2000
#define ERROR_BLINK 30
#define INIT_BLINK 500
#define VOLTAGE_PUB_TIME_PERIOD 1000
#define INIT_TIMEOUT 15000

#define SETUP_TIME 5000

#define VOLTAGE_LIMIT_LAG 1

/* Voltage threshold */
#define HIGH_VOLTAGE_LIMIT 4200
#define LOW_VOLTAGE_LIMIT 2500

#define ERROR_COUNTER_LAG 0

/* Pin Definitions */
#define PIN_DEBUG 14
const uint8_t PIN_CS_SLAVE[NUM_SLAVES] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

/* Vars */
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can;
ams_slave *slaves[NUM_SLAVES];
uint16_t blink_delay = OK_BLINK; /* OK blink */
unsigned long blink_time = millis();
bool blink_state = false;
const double vcell_step = 73.55 * pow(10, -3); // This is in mV

unsigned long voltage_pub_time = millis();

unsigned long voltage_limit_lag = 0;

uint16_t temperature_error_counter = 0;

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

void handle_error(uint16_t error_code, uint8_t *data_buff, uint8_t len)
{
	blink_delay = ERROR_BLINK; /* Error blink */
	pinMode(SHUTDOWN_CIRCUIT_PIN, 0);

	CAN_message_t m;
	m.id = error_code;
	memcpy(m.buf, data_buff, len);
	can.write(m);
}

bool wait_for_idle_or_fail(ams_slave *s, uint16_t timeout = 1000)
{

	for (uint16_t i = 0; i < timeout / 10; i++)
	{
		if (!ams_slave_is_idle(s))
		{
			return true;
		}
		delay(10);
	}
	return false;
}

/* SETUP */
void setup()
{
	if (DEBUG)
	{

		Serial.begin(115200);
		Serial.println("[AMS_MASTER]> Setup started");
	}

	/* Setup IO */
	pinMode(PIN_DEBUG, OUTPUT);
	digitalWrite(PIN_DEBUG, LOW);

	pinMode(SHUTDOWN_CIRCUIT_PIN, 20);
	digitalWrite(SHUTDOWN_CIRCUIT_PIN, 1); // Zero means not ok.

	pinMode(21, INPUT_PULLDOWN);
	pinMode(19, INPUT_PULLDOWN);
	pinMode(18, INPUT_PULLDOWN);
	pinMode(17, INPUT_PULLDOWN);

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

	slaves[0]->type = TYPE_13;
	slaves[1]->type = TYPE_10;
	slaves[2]->type = TYPE_13;
	slaves[3]->type = TYPE_10;
	slaves[4]->type = TYPE_13;
	slaves[5]->type = TYPE_10;
	slaves[6]->type = TYPE_13;
	slaves[7]->type = TYPE_10;
	slaves[8]->type = TYPE_13;
	slaves[9]->type = TYPE_2;

	/* Check if slaves are connected */
	bool aggregatedSuccess = true;
	for (uint8_t i = 0; i < NUM_SLAVES; i++)
	{
		/* Setup register map */
		ams_slave_setup(slaves[i]);
	}

	if (aggregatedSuccess)
	{
		digitalWrite(SHUTDOWN_CIRCUIT_PIN, 1);
	}
	voltage_limit_lag = millis();
}

bool quit = false;

/* LOOP */
void loop()
{

	/* Blink LED for debugging */
	if (millis() - blink_time > blink_delay)
	{
		blink_time = millis();
		blink_state = !blink_state;
		digitalWrite(PIN_DEBUG, blink_state);
	}

	bool publish_voltages = true; // millis() - voltage_pub_time > VOLTAGE_PUB_TIME_PERIOD;

	bool areAllInitd = true;
	bool isError = false;

	if (quit) {
		blink_delay = ERROR_BLINK;
		return;
	}
	for (uint8_t i = 0; i < NUM_SLAVES; i++)
	{

		areAllInitd = areAllInitd && slaves[i]->state != INIT;
		isError = isError || slaves[i]->state == ERROR;

		// if (slaves[i]->state == INIT && millis() - voltage_limit_lag > INIT_TIMEOUT)
		// {
		// 	Serial.printf("[ERROR]> Slave %d did not init in time!\n", 0);
		// 	uint8_t error_buff[] = {i};
		// 	handle_error(0x48, error_buff, 1);
		// 	slaves[i]->state = ERROR;
		// 	break;
		// }

		// Start a manual voltage measurement
		ams_slave_write(slaves[i], 0x02, 0b10000011);

		uint16_t buff[16];
		ams_slave_read_voltages(slaves[i], buff);

		//! Change this to the actual amount of cells
		uint8_t num = slaves[i]->type == TYPE_13 ? 13 : TYPE_10 ? 10
																: 2;
		for (uint8_t j = 0; j < num; j++)
		{
			double voltage_level = (vcell_step * buff[j] + 0.5 * vcell_step); // + 65535 * vcell_step) / 2;

			if (buff[j] != 0 && slaves[i]->state == INIT)
			{
				slaves[i]->state = NORMAL;
			}

			if (slaves[i]->state == INIT)
			{
				Serial.printf("[WARNING]> Slave %d is in init mode\n", i);
			}

			if (slaves[i]->state == NORMAL && (voltage_level < LOW_VOLTAGE_LIMIT || voltage_level > HIGH_VOLTAGE_LIMIT))
			{
				slaves[i]->error_counter++;
			}

			if (voltage_level < LOW_VOLTAGE_LIMIT && slaves[i]->state == NORMAL && slaves[i]->error_counter > ERROR_COUNTER_LAG)
			{
				Serial.printf("ERROR (%d) [%d] : LOW_VOLTAGE_LIMIT exceeded\n", i, j);
				uint8_t error_buff[] = {slaves[i]->segment, j, (buff[j]) >> 8, buff[j]};
				handle_error(0x43, error_buff, 4);
				slaves[i]->state = ERROR;
				quit = true;
				break;
			}
			if (voltage_level > HIGH_VOLTAGE_LIMIT && slaves[i]->state == NORMAL && slaves[i]->error_counter > ERROR_COUNTER_LAG)
			{
				Serial.printf("ERROR (%d) [%d] : HIGH_VOLTAGE_LIMIT exceeded\n", i, j);
				uint8_t error_buff[] = {slaves[i]->segment, j, (buff[j]) >> 8, buff[j]};
				handle_error(0x42, error_buff, 4);
				slaves[i]->state = ERROR;
				quit = true;
				break;
			}

			if (publish_voltages)
			{

				if (DEBUG)
				{
					Serial.printf("VOL (%d) [%d] ", i, j);
					Serial.print(voltage_level);
					Serial.println(" mV");
				}
				CAN_message_t m;
				m.id = 0x04;
				m.buf[0] = slaves[i]->segment;
				m.buf[1] = j;
				m.buf[2] = buff[j] >> 8;
				m.buf[3] = buff[j];
				can.write(m);

				voltage_pub_time = millis();
			}

			delayMicroseconds(10);

			// Temperature readings
			if (temperature_error_counter > ERROR_COUNTER_LAG)
			{



				//! Disable this whole code block if you want to disable temperature sensor errors
				
				
				
				// Overtemp error
				Serial.println("[ERROR]> Overtemp!");
				uint8_t error_buff[] = {};
				handle_error(0x41, error_buff, 0);
				isError = true;
				quit = true;
				break;
			}
			else if (!digitalRead(21) || !digitalRead(19) || !digitalRead(18) || !digitalRead(17))
			{
				temperature_error_counter++;
			}
		}
		if (quit) {
			break;
		}
	}

	if (!areAllInitd)
	{
		blink_delay = INIT_BLINK;
	}
	else if (isError)
	{
		blink_delay = ERROR_BLINK;
	}
	else
	{
		blink_delay = OK_BLINK;
	}

	delay(10);
	can.events();
}
