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

/* Sends data, based on Figure 122 (p130) of the datasheet */
void spi_send(char data)
{
	/* Iterate over all bits */
	uint8_t i;
	for (i = 7; i < 8; i--)
	{ /* MSB first, (i < 8 because unsigned int will overflow and never go below zero! */
		/* Get bit to send */
		char bit = (data >> i) & 0b1;

		/* Clock out bit */
		digitalWriteFast(SPI_PIN_MOSI, bit);
		delayNanoseconds(SPI_T_4); /* Setup time */
		digitalWriteFast(SPI_PIN_CLK, HIGH);
		delayNanoseconds(SPI_T_4); /* Hold time */
		delayNanoseconds(SPI_T_4); /* Wait time clock high */
		digitalWriteFast(SPI_PIN_CLK, LOW);
		delayNanoseconds(SPI_T_4); /* Wait time clock low */
	}
	digitalWriteFast(SPI_PIN_MOSI, LOW); /* Reset MOSI */
	delayNanoseconds(SPI_T_4); /* Wait time clock low */
}

/* Receives data */
void spi_receive(char *buffer)
{
	*buffer = 0b0; /* Reset Buffer */

	/* Start with one clock cycle */
	digitalWriteFast(SPI_PIN_CLK, HIGH);
	delayNanoseconds(SPI_T_4); /* Wait time clock high */
	delayNanoseconds(SPI_T_4); /* Wait time clock high */
	digitalWriteFast(SPI_PIN_CLK, LOW);
	delayNanoseconds(SPI_T_4); /* Setup time */
	delayNanoseconds(SPI_T_4); /* Setup time */


	/* Read 8 bits */
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		/* Clock in bit */
		digitalWriteFast(SPI_PIN_CLK, HIGH);
		delayNanoseconds(SPI_T_4); /* Wait time clock high */
		
		/* Store value in buffer */
		char bit = digitalReadFast(SPI_PIN_MISO);
		*buffer = (*buffer << 1) | (bit & 0b1);

		delayNanoseconds(SPI_T_4); /* Wait time clock high */
		digitalWriteFast(SPI_PIN_CLK, LOW);
		delayNanoseconds(SPI_T_4); /* Setup time */
		delayNanoseconds(SPI_T_4); /* Setup time */
	}
}
