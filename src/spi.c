/*
Fast and dead simple SPI library for Arduino-based microcontrollers
¯\_(ツ)_/¯

Check `spi.h` for more information!

Written by Thomas Van Acker

The source code of this library is licensed under the GNU General Public License version 3. You may obtain a copy of the license at https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "spi.h"

/* Initializes SPI */
void spi_init(void)
{
	/* Init IO */
	pinMode(SPI_PIN_MOSI, OUTPUT);
	pinMode(SPI_PIN_MISO, INPUT);
	pinMode(SPI_PIN_CLK, OUTPUT);
	digitalWriteFast(SPI_PIN_MOSI, LOW);
	digitalWriteFast(SPI_PIN_CLK, LOW);
}

/* Sends data */
void spi_send(char data)
{
	/* Iterate over all bits */
	uint8_t i;
	for (i = 7; i < 8; i--)
	{ /* MSB first, (i < 8 because unsigned int will overflow and never go below zero! */
		/* Get bit to send */
		const char bit = (data >> i) & 0b1;

		/* Clock out bit */
		digitalWriteFast(SPI_PIN_MOSI, bit);
		delayMicroseconds(SPI_T_4); /* Setup time */
		digitalWriteFast(SPI_PIN_CLK, HIGH);
		delayMicroseconds(SPI_T_4); /* Hold time */
		delayMicroseconds(SPI_T_4); /* Wait time clock high */
		digitalWriteFast(SPI_PIN_CLK, LOW);
		delayMicroseconds(SPI_T_4); /* Wait time clock low */
	}
	digitalWriteFast(SPI_PIN_MOSI, LOW); /* Reset MOSI */
	delayMicroseconds(SPI_T_4);			 /* Wait time clock low */
}

/* Receives data */
void spi_receive(char *buffer)
{
	*buffer = 0b0; /* Reset Buffer */

	/* Read 8 bits */
	uint16_t i;
	for (i = 0; i < 8; i++)
	{
		/* Clock in bit */
		digitalWriteFast(SPI_PIN_CLK, HIGH);
		delayMicroseconds(SPI_T_4); /* Wait time clock high */

		/* Store value in buffer */
		const char bit = digitalReadFast(SPI_PIN_MISO);
	
		*buffer = (*buffer << 1) | bit;

		delayMicroseconds(SPI_T_4); /* Wait time clock high */
		digitalWriteFast(SPI_PIN_CLK, LOW);
		delayMicroseconds(SPI_T_4); /* Setup time */
		delayMicroseconds(SPI_T_4); /* Setup time */
	}
}
