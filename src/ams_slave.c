#include "ams_slave.h"


/* Initializes a new ams_slave struct and return pointer to the object */
ams_slave* ams_slave_init(
	uint8_t id,
	uint8_t segment,
	slave_type_t type,
	uint8_t pin_chip_select
){
	/* Allocate memory */
	ams_slave* s = (ams_slave*) malloc(sizeof(ams_slave));

	/* Store config */
	s->id = id;
	s->segment = segment;
	s->type = type;
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
	/* Wait until the slave isn't busy */
	/* NOT SURE IF THIS IS NEEDED */
	while(!ams_slave_is_idle(s)){
		delayMicroseconds(10);
	}

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
	delayNanoseconds(7000); /* SPI_t_WAIT */
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
	delayNanoseconds(7000); /* SPI_t_WAIT */
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
	ams_slave_write(s, 0x03, 0b00000000);
	ams_slave_write(s, 0x04, s->type == TYPE_13 ? 0b11111100 : 0b11111000);
	ams_slave_write(s, 0x05, s->type == TYPE_13 ? 0b01111111 : 0b00011111);
	ams_slave_write(s, 0x06, 0b11011110);
	ams_slave_write(s, 0x07, 0b10000110);
	ams_slave_write(s, 0x08, 0b00000100);
	ams_slave_write(s, 0x09, 0b00000000);
	ams_slave_write(s, 0x0a, 0b00000000);
	ams_slave_write(s, 0x0b, 0b11111111);
	ams_slave_write(s, 0x0c, 0b00000000);
	ams_slave_write(s, 0x0d, 0b00000000);
	ams_slave_write(s, 0x0e, 0b00000000);
	ams_slave_write(s, 0x0f, 0b11111111);
	ams_slave_write(s, 0x10, 0b00000000); /* TODO CHARGING */
	ams_slave_write(s, 0x11, 0b00000000);
	ams_slave_write(s, 0x12, 0b00010000);
	ams_slave_write(s, 0x13, 0b11111111); /* TODO TEMPERATURE */
	ams_slave_write(s, 0x14, 0b00000000); /* TODO TEMPERATURE */
	ams_slave_write(s, 0x15, 0b11111111); /* TODO TEMPERATURE */
	ams_slave_write(s, 0x16, 0b00000000); /* TODO TEMPERATURE */
	ams_slave_write(s, 0x17, 0b11111111); /* TODO TEMPERATURE */
	ams_slave_write(s, 0x18, 0b00000000); /* TODO TEMPERATURE */
	ams_slave_write(s, 0x19, 0b11111111); /* TODO TEMPERATURE */
	ams_slave_write(s, 0x1a, 0b00000000); /* TODO TEMPERATURE */
	// ams_slave_write(s, 0x1b, 0b11000010); // SUS
	ams_slave_write(s, 0x1c, 0b00000000);
	ams_slave_write(s, 0x1d, 0b11111111);
	ams_slave_write(s, 0x1e, 0b11111111);
	ams_slave_write(s, 0x1f, 0b11000000);
	ams_slave_write(s, 0x20, s->type == TYPE_13 ? 0b10110100 : 0b10001010);
	ams_slave_write(s, 0x21, s->type == TYPE_13 ? 0b01101011 : 0b01010010);
	ams_slave_write(s, 0x22, 0b01010001);
	ams_slave_write(s, 0x23, 0b01000101);
	ams_slave_write(s, 0x24, 0b01000000);
	ams_slave_write(s, 0x25, 0b00000000); /* TODO CELL BALANCING */
	ams_slave_write(s, 0x26, 0b00000000); /* TODO CELL BALANCING */
	ams_slave_write(s, 0x27, 0b00000000); /* TODO CELL BALANCING */
	ams_slave_write(s, 0x28, 0b00000000); /* TODO CELL BALANCING */
	ams_slave_write(s, 0x29, 0b00000000); /* TODO CELL BALANCING */
	ams_slave_write(s, 0x2a, 0b00000000); /* TODO CELL BALANCING */
	ams_slave_write(s, 0x2b, 0b11111111); /* TODO CELL BALANCING */
	ams_slave_write(s, 0x2c, 0b00000000); /* TODO CELL BALANCING */
	ams_slave_write(s, 0x2d, 0b11111111); /* TODO CELL BALANCING */
	// ams_slave_write(s, 0x2e, 0b00000011); // SUS
	ams_slave_write(s, 0x83, 0b11111111);
	ams_slave_write(s, 0x84, 0b11111111);
	ams_slave_write(s, 0x85, 0b11111111);
	ams_slave_write(s, 0x86, 0b11111111);
	ams_slave_write(s, 0x87, 0b11111111);
	ams_slave_write(s, 0x88, 0b11111111);
	ams_slave_write(s, 0x89, 0b11111111);

	// Try to get into SCAN mode
	delayMicroseconds(100);
	ams_slave_write(s, 0x2e, 0b01011011);
}

/* Check if slave is not busy */
char ams_slave_is_idle(ams_slave* s){
	/* Get busy bit */
	char buff;
	ams_slave_read(s, 0x01, &buff);
	char busy = (buff >> 2) & 0b1; /* bit 2 */
	return (~busy) & 0b1;
}

/* Trigger a single system scan */
void ams_slave_trigger_system_scan(ams_slave* s){
	/* Set trigger bit */
	char buff;
	ams_slave_read(s, 0x01, &buff);
	ams_slave_write(s, 0x01, buff | 0b1); /* bit 0 */
}

/* Read voltages */
void ams_slave_read_voltages(ams_slave* s, uint16_t* buff){
	char buff0, buff1;

	/* Read cell enable register to se which cells are connected */
	ams_slave_read(s, 0x04, &buff0);
	ams_slave_read(s, 0x05, &buff1);
	uint16_t cell_enabled = (buff0 << 8) | buff1;

	/* Read voltage registers */
	uint8_t j = 0;
	char enabled;
	for(uint8_t i=0; i<16; i++){
		/* Only read if enabled */
		enabled = cell_enabled & 0b1;
		cell_enabled = cell_enabled >> 1;
		if(!enabled) continue;

		/* Read value */
		ams_slave_read(s, 0x30 + 2*i, &buff0); /* MSB */
		ams_slave_read(s, 0x30 + 2*i + 1, &buff1); /* LSB */
		buff[j] = (buff0 << 8) | buff1;
		j++;
	}
}

