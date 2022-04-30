#include "ams_slave.h"


/* Initializes a new ams_slave struct and return pointer to the object */
ams_slave* ams_slave_init(
	uint8_t id,
	uint8_t segment,
	uint8_t pin_chip_select
){
	/* Allocate memory */
	ams_slave* s = (ams_slave*) malloc(sizeof(ams_slave));

	/* Store config */
	s->id = id;
	s->segment = segment;
	s->pin_chip_select = pin_chip_select;

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
	spi_send(0b00010100); /* Slave address + Write bit */
	delayNanoseconds(7000); /* SPI_t_WAIT */
	spi_send(reg); /* Register address */
	delayNanoseconds(7000); /* SPI_t_WAIT */
	spi_send(data); /* Data */

	/* Disable chip */
	delayNanoseconds(250); /* SPI_t_LAG */
	digitalWriteFast(s->pin_chip_select, HIGH);
}


/* Read register over SPI */
void ams_slave_read(
	ams_slave* s,
	char reg, /* Register to write to */
	char* buffer /* Buffer to write data to */
){
	/* Enable chip */
	digitalWriteFast(s->pin_chip_select, LOW); /* Active Low */

	/* Send data */
	spi_send(0b00010101); /* Slave address + Read bit */
	delayNanoseconds(7000); /* SPI_t_WAIT */
	spi_send(reg); /* Register address */
	delayNanoseconds(7000); /* SPI_t_WAIT */
	spi_receive(buffer); /* Write data to buffer */

	/* Disable chip */
	delayNanoseconds(250); /* SPI_t_LAG */
	digitalWriteFast(s->pin_chip_select, HIGH);
};


/* Test if can communicate with slave over SPI */
char ams_slave_test_spi(ams_slave* s){
	/* Read Product ID Register */
	char buff;
	ams_slave_read(s, 0x00, &buff);

	/* Check value */
	return buff == 0b11110000;
}


