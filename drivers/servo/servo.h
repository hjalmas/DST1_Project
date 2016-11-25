/*
 * servo.h
 *
 *  Created on: Nov 22, 2016
 *      Author: root
 */

#ifndef SERVO_H_
#define SERVO_H_

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

/* -----------------------------------------------------------------------------------
 * 										PROTOTYPES
 * -----------------------------------------------------------------------------------
 */

/**
 * Initializes the servo.
 */
void servo_init(void);

/**
 * Rotates the servo to the specified angle.
 */
void servo_rotate(uint32_t angle);

#endif /* SERVO_H_ */
