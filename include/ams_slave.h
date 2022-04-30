#ifndef _H_AMS_SLAVE
#define _H_AMS_SLAVE

/* Ensure compatibiltiy with C++ compiler */
#ifdef __cplusplus
extern "C" {
#endif

/* Struct Definition */
typedef struct {
	uint8_t id; /* Unique ID of this slave */
	uint8_t segment; /* Segment where this slave is located */
	uint8_t pin_chip_select; /* Chip Select pin for the SPI communication */

	void (*spi_send)(char); /* This functions gets called to send a byte of data to slave IC */
	char (*spi_receive)(void); /* This functions gets called to receive a byte of data from slave IC */
} ams_slave;


/* Initializes a new ams_slave struct and return pointer to the object */
ams_slave* ams_slave_init(
	uint8_t id,
	uint8_t segment,
	uint8_t pin_chip_select
);

/* Frees all allocated memory */
void ams_slave_free(ams_slave* s);

/* Test if can communicate with slave over SPI */
char ams_slave_test_spi(ams_slave* s);

/* Setup all registers of the slave IC with the correct values */
void ams_slave_setup(ams_slave* s);



#ifdef __cplusplus
}
#endif

#endif
