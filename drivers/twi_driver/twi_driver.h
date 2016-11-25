
#ifndef TWI_H_INCLUDED
#define TWI_H_INCLUDED

/**
 * -----------------------------------------------------------------------------------------------------
 * 												INCLUDES
 * -----------------------------------------------------------------------------------------------------
 */
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "diag/Trace.h"
#include "cmsis_device.h"
#include "sam3x_conf.h"

/**
 * -----------------------------------------------------------------------------------------------------
 * 												DEFINES
 * -----------------------------------------------------------------------------------------------------
 */
/* Time-out value (number of attempts) ---*/
#define TWI_TIMEOUT              15000

/* Return codes --------------------------*/
#define TWI_SUCCESS              0
#define TWI_INVALID_ARGUMENT     1
#define TWI_ARBITRATION_LOST     2
#define TWI_NO_CHIP_FOUND        3
#define TWI_RECEIVE_OVERRUN      4
#define TWI_RECEIVE_NACK         5
#define TWI_SEND_OVERRUN         6
#define TWI_SEND_NACK            7
#define TWI_BUSY                 8
#define TWI_ERROR_TIMEOUT        9

#define FAIL 0
#define PASS 1

/**
 * -----------------------------------------------------------------------------------------------------
 * 												VARIABLES
 * -----------------------------------------------------------------------------------------------------
 */
/**
 * \brief Input parameters when initializing the TWI module mode.
 */
typedef struct twi_options {
	//! MCK for TWI.
	uint32_t master_clk;
	//! The baud rate of the TWI bus.
	uint32_t speed;
	//! The desired address.
	uint8_t chip;
	//! SMBUS mode (set 1 to use SMBUS quick command, otherwise don't).
	uint8_t smbus;
} twi_options_t;

/**
 * \brief Information concerning the data transmission.
 */
typedef struct twi_packet {
	//! TWI address/commands to issue to the other chip (node).
	uint8_t addr[3];
	//! Length of the TWI data address segment (1-3 bytes).
	uint32_t addr_length;
	//! Where to find the data to be transferred.
	void *buffer;
	//! How many bytes do we want to transfer.
	uint32_t length;
	//! TWI chip address to communicate with.
	uint8_t chip;
} twi_packet_t;


/**
 * -----------------------------------------------------------------------------------------------------
 * 												PROTOTYPES
 * -----------------------------------------------------------------------------------------------------
 */
void twi_enable_master_mode(Twi *p_twi);
void twi_disable_master_mode(Twi *p_twi);
uint32_t twi_master_init(Twi *p_twi, const twi_options_t *p_opt);
uint32_t twi_set_speed(Twi *p_twi, uint32_t ul_speed, uint32_t ul_mck);
uint32_t twi_probe(Twi *p_twi, uint8_t uc_slave_addr);
uint32_t twi_master_read(Twi *p_twi, twi_packet_t *p_packet);
uint32_t twi_master_write(Twi *p_twi, twi_packet_t *p_packet);
void twi_enable_interrupt(Twi *p_twi, uint32_t ul_sources);
void twi_disable_interrupt(Twi *p_twi, uint32_t ul_sources);
uint32_t twi_get_interrupt_status(Twi *p_twi);
uint32_t twi_get_interrupt_mask(Twi *p_twi);
uint8_t twi_read_byte(Twi *p_twi);
void twi_write_byte(Twi *p_twi, uint8_t uc_byte);
void twi_enable_slave_mode(Twi *p_twi);
void twi_disable_slave_mode(Twi *p_twi);
void twi_slave_init(Twi *p_twi, uint32_t ul_device_addr);
void twi_set_slave_addr(Twi *p_twi, uint32_t ul_device_addr);
uint32_t twi_slave_read(Twi *p_twi, uint8_t *p_data);
uint32_t twi_slave_write(Twi *p_twi, uint8_t *p_data);
void twi_reset(Twi *p_twi);
uint32_t twi_mk_addr(const uint8_t *addr, int len);
Pdc *twi_get_pdc_base(Twi *p_twi);

#endif /* TWI_H_INCLUDED */
