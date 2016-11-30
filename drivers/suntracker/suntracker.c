/*
 * suntracker.c
 *
 *  Created on: Nov 30, 2016
 *      Author: Albin Hjälmås
 */

/**
 * --------------------------------------------------------------------------------------------------
 * 										INCLUDES
 * --------------------------------------------------------------------------------------------------
 */
#include "suntracker.h"

/**
 * --------------------------------------------------------------------------------------------------
 * 										VARIABLES
 * --------------------------------------------------------------------------------------------------
 */

/* ON/OFF state of the suntracker */
bool suntrackerState = false;

/* true if the suntracker has locked on to the sun */
bool suntrackerLocked = false;

/* Current angle of the servo */
int32_t currAng = 90;

/* PID variables */
static int32_t integral = 0;
static int32_t prevErr = 0;

/**
 * --------------------------------------------------------------------------------------------------
 * 										PROTOTYPES
 * --------------------------------------------------------------------------------------------------
 */
extern void delay_ms(uint32_t t_ms);

/**
 * --------------------------------------------------------------------------------------------------
 * 										DECLARATIONS
 * --------------------------------------------------------------------------------------------------
 */

/**
 * Initializes the suntracker.
 */
void suntracker_init(void) {
	ldr_init();
	servo_init();
	servo_rotate(currAng);
	delay_ms(500);
}

/**
 * Starts the operation of the suntracker.
 */
void suntracker_start(void) {
	suntrackerState = true;
}

/**
 * Stops the operation of the suntracker.
 */
void suntracker_stop(void) {
	suntrackerState = false;
	servo_rotate(90);
}

/**
 * Updates the position of the suntracker
 * if the suntracker is in ON state.
 */
void cbSuntracker(void) {
	if(!suntrackerState) {
		return;
	}

	/* PID regulator stuff */
	volatile int32_t err = ldr_rd_right() - ldr_rd_left();
	volatile int32_t derivative = (err - prevErr) * 50;
	integral += (err * SUNTRACKER_DT) / 1000;
	currAng += (20*err + 5*integral + 1*derivative) / 10000;

	if (currAng < 0) {
		integral = 0;
	} else if (currAng > 180) {
		integral = 0;
	} else {
		servo_rotate((uint32_t) currAng);
	}

	prevErr = err;
}
