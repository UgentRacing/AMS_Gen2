/*
Fast and dead simple SPI library for Arduino-based microcontrollers
¯\_(ツ)_/¯

** Change the config vars below before compilation **
Currently only SPI mode 0 supported

Written by Thomas Van Acker

The source code of this library is licensed under the GNU General Public License version 3. You may obtain a copy of the license at https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#ifndef _H_SPI
#define _H_SPI

/* ======== CONFIG ======== */
#define SPI_PIN_MOSI 11
#define SPI_PIN_MISO 12
#define SPI_PIN_CLK 13
#define SPI_T_HIGH 200 /* in ns */
#define SPI_T_LOW 200 /* in ns */
/* ======================== */

#include <stdlib.h>
#include <Arduino.h>

/* Ensure compatibility with C++ compiler */
#ifdef __cplusplus
extern "C" {
#endif

/* Initializes SPI */
void spi_init(
	void /* Nothing here yet :( */
);

/* Sends data */
void spi_send(
	char data /* Data to be sent */
);

/* Receives data */
void spi_read(
	char* buffer /* Data will be stored in this buffer */
);

#ifdef __cplusplus
}
#endif

#endif
