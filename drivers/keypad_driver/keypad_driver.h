/*
 * keypad_driver.h
 *
 *  Created on: Nov 13, 2016
 *      Author: Albin Hjälmås
 */

#ifndef KEYPAD_DRIVER_H_
#define KEYPAD_DRIVER_H_

/**
 * -----------------------------------------------------------------------------------------------------
 * 												INCLUDES
 * -----------------------------------------------------------------------------------------------------
 */
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <float.h>
#include <stdbool.h>
#include "diag/Trace.h"
#include "cmsis_device.h"
#include "sam3x_conf.h"

#include "../pio_driver/pio_driver.h"
#include "../tc_driver/tc_driver.h"

/**
 * -----------------------------------------------------------------------------------------------------
 * 												DEFINES
 * -----------------------------------------------------------------------------------------------------
 */

/* Outputs from keypad to mcu */
#define KPAD_PORT					PIOD
#define KPAD_ROW_1					PIO_PD0				/* Arduino pin 25 */
#define KPAD_ROW_2					PIO_PD1				/* Arduino pin 26 */
#define KPAD_ROW_3					PIO_PD2				/* Arduino pin 27 */
#define KPAD_ROW_4					PIO_PD3				/* Arduino pin 28 */
#define KPAD_ROW_MSK				0x0000000F

#define KPAD_COL_1					PIO_PD6				/* Arduino pin 29 */
#define KPAD_COL_2					PIO_PD7				/* Arduino pin 11 */
#define KPAD_COL_3					PIO_PD8				/* Arduino pin 12 */
#define KPAD_COL_4					PIO_PD9				/* Arduino pin 30 */
#define KPAD_COL_MSK				0xF << 6

/* Keys */
#define KEY_NONE					0					/* No key was pressed */
#define KEY_1						1
#define KEY_2						2
#define KEY_3						3
#define KEY_A						4
#define KEY_4						5
#define KEY_5						6
#define KEY_6						7
#define KEY_B						8
#define KEY_7						9
#define KEY_8						10
#define KEY_9						11
#define KEY_C						12
#define KEY_ASTERIX					13
#define KEY_0						14
#define KEY_HASH					15
#define KEY_D						16

/**
 * -----------------------------------------------------------------------------------------------------
 * 												PROTOTYPES
 * -----------------------------------------------------------------------------------------------------
 */

/**
 * Initialize the keypad connections and timer counter channel.
 */
void kpad_init(void);


/**
 * Returns the currently pressed key.
 */
uint32_t kpad_get_key(void);


#endif /* KEYPAD_DRIVER_H_ */
