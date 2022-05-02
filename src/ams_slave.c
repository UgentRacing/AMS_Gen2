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

/* Setup all registers of the slave IC with the correct values */
void ams_slave_setup(ams_slave* s){
	/* Checkout "slave_registers.txt" for more info */
	ams_slave_write(s, 0x01, 0b00000010);
	ams_slave_write(s, 0x02, 0b10000000);
	ams_slave_write(s, 0x03, 0b10000000);
	ams_slave_write(s, 0x04, 0b11111100); /* CHECK! */
	ams_slave_write(s, 0x05, 0b01111111); /* CHECK! */
	ams_slave_write(s, 0x06, 0b11111111); /* TODO */
	ams_slave_write(s, 0x07, 0b00000000); /* TODO */
	ams_slave_write(s, 0x08, 0b11111111); /* TODO */
	ams_slave_write(s, 0x09, 0b00000000);
	ams_slave_write(s, 0x0a, 0b00001111); /* TODO */
	ams_slave_write(s, 0x0b, 0b11111111); /* TODO */
	ams_slave_write(s, 0x0c, 0b00000000);
	ams_slave_write(s, 0x0d, 0b00000000);
	ams_slave_write(s, 0x0e, 0b10110100); /* TODO */
	ams_slave_write(s, 0x0f, 0b11111111); /* TODO */
	ams_slave_write(s, 0x10, 0b00000000); /* TODO */
	ams_slave_write(s, 0x11, 0b00000000);
	ams_slave_write(s, 0x12, 0b00010000);
	ams_slave_write(s, 0x13, 0b00000000); /* TODO */
	ams_slave_write(s, 0x14, 0b00000000); /* TODO */
	ams_slave_write(s, 0x15, 0b00000000); /* TODO */
	ams_slave_write(s, 0x16, 0b00000000); /* TODO */
	ams_slave_write(s, 0x17, 0b00000000); /* TODO */
	ams_slave_write(s, 0x18, 0b00000000); /* TODO */
	ams_slave_write(s, 0x19, 0b00000000); /* TODO */
	ams_slave_write(s, 0x1a, 0b00000000); /* TODO */
	ams_slave_write(s, 0x1b, 0b11000000); /* TODO */
	ams_slave_write(s, 0x1c, 0b00000000); /* TODO */
	ams_slave_write(s, 0x1d, 0b00000000); /* TODO */
	ams_slave_write(s, 0x1e, 0b00000000); /* TODO */
	ams_slave_write(s, 0x1f, 0b00000000); /* TODO */
	ams_slave_write(s, 0x20, 0b00000000); /* TODO */
	ams_slave_write(s, 0x21, 0b00000000); /* TODO */
	ams_slave_write(s, 0x22, 0b00000000); /* TODO */
	ams_slave_write(s, 0x23, 0b00000000); /* TODO */
	ams_slave_write(s, 0x24, 0b00000000); /* TODO */
	ams_slave_write(s, 0x25, 0b00000000); /* TODO */
	ams_slave_write(s, 0x26, 0b00000000); /* TODO */
	ams_slave_write(s, 0x27, 0b00000000); /* TODO */
	ams_slave_write(s, 0x28, 0b00000000); /* TODO */
	ams_slave_write(s, 0x29, 0b00000000); /* TODO */
	ams_slave_write(s, 0x2a, 0b00000000); /* TODO */
	ams_slave_write(s, 0x2b, 0b00000000); /* TODO */
	ams_slave_write(s, 0x2c, 0b00000000); /* TODO */
	ams_slave_write(s, 0x2d, 0b00000000); /* TODO */
	ams_slave_write(s, 0x2e, 0b00000000); /* TODO */
	ams_slave_write(s, 0x2f, 0b00000000); /* TODO */
}


