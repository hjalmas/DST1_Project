/*
 * MPL311_driver.h
 *
 *  Created on: Nov 21, 2016
 *      Author: Albin Hjälmås
 */

#ifndef MPL311_DRIVER_H_
#define MPL311_DRIVER_H_

/* -----------------------------------------------------------------------------------
 * 										INCLUDES
 * -----------------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "diag/Trace.h"
#include "cmsis_device.h"
#include "sam3x_conf.h"

#include "../pio_driver/pio_driver.h"
#include "../twi_driver/twi_driver.h"

/* -----------------------------------------------------------------------------------
 * 										DEFINES
 * -----------------------------------------------------------------------------------
 */

/* Flags ---------------*/
#define FLAG_DATARDY			1 << 2	/* Data is ready */
#define FLAG_PR_RDY				1 << 1	/* Preassure data is ready */
#define FLAG_TMP_RDY			1		/* Temperature data is ready */
#define FLAG_SRC_DRDY			1 << 7	/* Data ready interrupt flag */

/* Operating states ----*/
#define MPL_ACTIVE				1
#define MPL_STANDBY				0

/* Interrupt Pin -------*/
#define MPL_INT_PIN 			PIO_PB14

/* -----------------------------------------------------------------------------------
 * 										PROTOTYPES
 * -----------------------------------------------------------------------------------
 */

/**
 * Initializes the MPL311.
 */
void MPL_init(void);

/**
 * Read the flags of the MPL311.
 * use in conjunction with the defines:
 * FLAG_DATARDY		Data is ready
 * FLAG_PR_RDY		Preassure data is ready
 * FLAG_TMP_RDY		Temperature data is ready
 */
uint8_t MPL_read_flags(void);

/**
 * Set operating state of MPL311 sensor.
 * use in conjuction with the defines:
 * MPL_ACTIVE
 * MPL_STANDBY
 */
void MPL_set_active(uint8_t operatingState);

/**
 * Sets the time interval for pressure accquisition, in ms.
 */
void MPL_set_acqTimeInterval(uint32_t t_ms);

/**
 * Resets the device.
 */
void MPL_reset(void);

/**
 * Returns the 20bit pressure data.
 */
uint32_t MPL_read_pressure(void);

/**
 * Returns the 12bit temperature data.
 */
float MPL_read_temperature(void);

/**
 * Clears the interrupts.
 */
void MPL_clear_interrupts(void);

#endif /* MPL311_DRIVER_H_ */
