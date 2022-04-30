#include "ams_slave.h"


/* Initializes a new ams_slave struct and return pointer to the object */
ams_slave* ams_slave_init(
	uint8_t id,
	uint8_t segment,
	uint8_t pin_chip_select,
	void (*spi_send)(char),
	char (*spi_receive)(void)
){
	/* Check inputs */
	if(spi_send == NULL) return NULL;
	if(spi_receive == NULL) return NULL;

	/* Allocate memory */
	ams_slave* s = (ams_slave*) malloc(sizeof(ams_slave));

	/* Store config */
	s->id = id;
	s->segment = segment;
	s->pin_chip_select = pin_chip_select;

	/* Set callbacks */
	s->spi_send = spi_send;
	s->spi_receive = spi_receive;

	/* Init IO */
	pinMode(s->pin_chip_select, OUTPUT);
	digitalWrite(s->pin_chip_select, HIGH); /* Active Low */

	return s;
}

/* Frees all allocated memory */
void ams_slave_free(ams_slave* s){
	free(s);
}

/* Write register over SPI */
void ams_slave_write(
	ams_slave* s,
	char reg, /* Register to write to */
	char data /* Data to write to register */
){
	/* Enable chip */
	digitalWriteFast(s->pin_chip_select, LOW); /* Active Low */

	/* Send data */
	s->spi_send(0b00010100); /* Slave address + Write bit */
	delayNanoseconds(7000); /* SPI_t_WAIT */
	s->spi_send(reg); /* Register address */
	delayNanoseconds(7000); /* SPI_t_WAIT */
	s->spi_send(data); /* Data */

	/* Disable chip */
	delayNanoseconds(250); /* SPI_t_LAG */
	digitalWriteFast(s->pin_chip_select, HIGH);
}

/* Test if can communicate with slave over SPI */
char ams_slave_test_spi(ams_slave* s){
	/* Read Product ID Register */
}

