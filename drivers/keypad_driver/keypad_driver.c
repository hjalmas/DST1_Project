/*
 * keypad_driver.c
 *
 *  Created on: Nov 13, 2016
 *      Author: Albin Hjälmås
 */

/**
 * -----------------------------------------------------------------------------------------------------
 * 												INCLUDES
 * -----------------------------------------------------------------------------------------------------
 */
#include "keypad_driver.h"

/**
 * -----------------------------------------------------------------------------------------------------
 * 												DEFINES
 * -----------------------------------------------------------------------------------------------------
 */

/**
 * -----------------------------------------------------------------------------------------------------
 * 												VARIABLES
 * -----------------------------------------------------------------------------------------------------
 */
bool kpadFlag = false;
uint32_t kpadRow = 0;
uint32_t kpadCol = 0;
uint32_t currKey = 0;
uint32_t selectedCol = 0;
uint32_t kpadCols[] = { KPAD_COL_1, KPAD_COL_2, KPAD_COL_3, KPAD_COL_4 };

/**
 * -----------------------------------------------------------------------------------------------------
 * 												PROTOTYPES
 * -----------------------------------------------------------------------------------------------------
 */

/**
 * -----------------------------------------------------------------------------------------------------
 * 												DECLARATIONS
 * -----------------------------------------------------------------------------------------------------
 */

/**
 * Initialize the keypad connections and timer counter channel.
 */
void kpad_init(void) {

	/* Initialize outputs to keypad */
	pio_init_pin(KPAD_PORT, KPAD_COL_MSK, PIO_OUTPUT);
	pio_set_pin(KPAD_PORT, KPAD_COL_MSK, 0);

	/* Initialize inputs from keypad */
	pio_init_pin(KPAD_PORT, KPAD_ROW_MSK, PIO_INPUT | PIO_PULLUP);
	pio_init_debounce(KPAD_PORT, KPAD_ROW_MSK, 16);
	pio_init_interrupt(KPAD_PORT, KPAD_ROW_MSK, INT_EDG_FALLING);
}

/**
 * Returns the currently pressed key.
 */
uint32_t kpad_get_key(void) {

	pio_set_pin(KPAD_PORT, KPAD_COL_MSK, 1);

	for (int i = 0; i < 4; i++) {
		selectedCol = kpadCols[i];
		pio_set_pin(KPAD_PORT, selectedCol, 0);
		pio_set_pin(KPAD_PORT, selectedCol, 1);
	}

	/* Disable interrupts to avoid interrupting
	 * When setting keybus to 0 again.
	 */
	KPAD_PORT->PIO_IDR = KPAD_ROW_MSK; /* Disable interrupts */
	pio_set_pin(KPAD_PORT, KPAD_COL_MSK, 0);
	KPAD_PORT->PIO_ISR; /* Read flag that occured */
	KPAD_PORT->PIO_IER = KPAD_ROW_MSK; /* Enable interrupts */

	volatile uint32_t newRow, newCol;

	switch (kpadRow) {
	case KPAD_ROW_1:
		newRow = 0;
		break;
	case KPAD_ROW_2:
		newRow = 1;
		break;
	case KPAD_ROW_3:
		newRow = 2;
		break;
	case KPAD_ROW_4:
		newRow = 3;
		break;
	default:
		kpadRow = kpadCol = 0;
		kpadFlag = false;
		return KEY_NONE;
	}

	switch (kpadCol) {
	case KPAD_COL_1:
		newCol = 1;
		break;
	case KPAD_COL_2:
		newCol = 2;
		break;
	case KPAD_COL_3:
		newCol = 3;
		break;
	case KPAD_COL_4:
		newCol = 4;
		break;
	default:
		kpadRow = kpadCol = 0;
		kpadFlag = false;
		return KEY_NONE;
	}

	kpadRow = kpadCol = 0;
	kpadFlag = false;

	return newRow * 4 + newCol;
}

