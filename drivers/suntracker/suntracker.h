/*
 * suntracker.h
 *
 *  Created on: Nov 30, 2016
 *      Author: root
 */

#ifndef SUNTRACKER_H_
#define SUNTRACKER_H_

/**
 * --------------------------------------------------------------------------------------------------
 * 										INCLUDES
 * --------------------------------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "diag/Trace.h"
#include "cmsis_device.h"
#include "sam3x_conf.h"

#include "../servo/servo.h"
#include "../ldr_driver/ldr_driver.h"


/**
 * --------------------------------------------------------------------------------------------------
 * 										DEFINES
 * --------------------------------------------------------------------------------------------------
 */

/* delta time in ms, update period */
#define SUNTRACKER_DT 20


/**
 * --------------------------------------------------------------------------------------------------
 * 										PROTOTYPES
 * --------------------------------------------------------------------------------------------------
 */

/**
 * Initializes the suntracker.
 */
void suntracker_init(void);

/**
 * Starts the operation of the suntracker.
 */
void suntracker_start(void);

/**
 * Stops the operation of the suntracker.
 */
void suntracker_stop(void);

/**
 * Updates the position of the suntracker
 * if the suntracker is in ON state.
 */
void cbSuntracker(void);


#endif /* SUNTRACKER_H_ */
