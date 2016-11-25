/*
 * tempsensor_driver.h
 *
 *  Created on: Nov 18, 2016
 *      Author: Albin Hjälmås
 */

#ifndef TEMPSENSOR_DRIVER_H_
#define TEMPSENSOR_DRIVER_H_

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

#include "../tc_driver/tc_driver.h"
#include "../pio_driver/pio_driver.h"

/**
 * -----------------------------------------------------------------------------------------------------
 * 												DEFINES
 * -----------------------------------------------------------------------------------------------------
 */

/* Hardware connections */
#define TEMP_TC 				TC0						/* The TC instance used by the tempsensor */
#define TEMP_TC_CHANNEL			0						/* The TC-channel used by the tempsensor */
#define TEMP_PIO				PIOB					/* The port used by the tempsensor */
#define TEMP_PIN				PIO_PB25B_TIOA0			/* Arduino pin 2 */

/**
 * -----------------------------------------------------------------------------------------------------
 * 												TYPEDEFS, variables
 * -----------------------------------------------------------------------------------------------------
 */

/**
 * -----------------------------------------------------------------------------------------------------
 * 												PROTOTYPES
 * -----------------------------------------------------------------------------------------------------
 */

/**
 * Initializes the tempsensor.
 */
void temp_init(void);

/**
 * Trigger a read from the temp sensor.
 */
void temp_trig(void);

/**
 * Returns one reading from the tempSensor.
 */
uint32_t temp_read(void);


#endif /* TEMPSENSOR_DRIVER_H_ */
