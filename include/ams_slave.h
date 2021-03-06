#ifndef _H_AMS_SLAVE
#define _H_AMS_SLAVE

#include <stdlib.h>
#include <Arduino.h>
#include "spi.h"

/* Ensure compatibiltiy with C++ compiler */
#ifdef __cplusplus
extern "C" {
#endif

/* Define Types */
enum slave_type {
	TYPE_13,
	TYPE_10
};
typedef enum slave_type slave_type_t;


/* Struct Definition */
typedef struct {
	uint8_t id; /* Unique ID of this slave */
	uint8_t segment; /* Segment where this slave is located */
	slave_type_t type; /* Type of this slave */
	uint8_t pin_chip_select; /* Chip Select pin for the SPI communication */
} ams_slave;


/* Initializes a new ams_slave struct and return pointer to the object */
ams_slave* ams_slave_init(
	uint8_t id,
	uint8_t segment,
	slave_type_t type,
	uint8_t pin_chip_select
);

/* Frees all allocated memory */
void ams_slave_free(ams_slave* s);

/* Write register over SPI */
void ams_slave_write(
	ams_slave* s,
	char reg, /* Register to write to */
	char data /* Data to write to register */
);

/* Read register over SPI */
void ams_slave_read(
	ams_slave* s,
	char reg, /* Register to write to */
	char* buffer /* Buffer to write data to */
);

/* Test if can communicate with slave over SPI */
char ams_slave_test_spi(ams_slave* s);

/* Setup all registers of the slave IC with the correct values */
void ams_slave_setup(ams_slave* s);



#ifdef __cplusplus
}
#endif

#endif
