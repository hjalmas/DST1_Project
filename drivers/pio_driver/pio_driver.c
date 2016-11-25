/*
 * pio_driver.c
 *
 *  Created on: Nov 8, 2016
 *      Author: Albin Hjälmås
 */

/**
 * ------------------------------------------------------------------------------------------------------
 * 											INCLUDES
 * ------------------------------------------------------------------------------------------------------
 */
#include "pio_driver.h"

/**
 * ------------------------------------------------------------------------------------------------------
 * 											DECLARATIONS
 * ------------------------------------------------------------------------------------------------------
 */

/**
 * Configures the specified pin on the specified port according
 * to the specified settings.
 * This function can initiate multiple pins on the same port at once.
 */
void pio_init_pin(Pio* PIO, uint32_t PIN, uint32_t Settings) {

	/* Enable clock to this peripheral */
	volatile uint32_t ID = pio_getID(PIO);
	PMC->PMC_PCER0 = 1 << ID;

	/* Enable PIO to control this pin */
	PIO->PIO_PER = PIN;

	/* Configure pin i/o state */
	if (Settings & PIO_INPUT) {
		PIO->PIO_ODR = PIN;
	} else if (Settings & PIO_OUTPUT) {
		PIO->PIO_OER = PIN;
	}

	/* Config open drain */
	if (Settings & PIO_OPENDRAIN) {
		PIO->PIO_MDER = PIN;
	} else {
		PIO->PIO_MDDR = PIN;
	}

	/* Configure pullup */
	if (Settings & PIO_PULLUP) {
		PIO->PIO_PUER = PIN;
	} else {
		PIO->PIO_PUDR = PIN;
	}

	/* Disable interrupt, to enable interrupts run "pio_init_interrupt()" */
	PIO->PIO_IDR = PIN;

}

/**
 * Select A- or B-peripheral to control this PIN.
 */
void pio_select_AB(Pio* PIO, uint32_t PIN, uint32_t PIO_AB_PERIPHERAL) {
	if(PIO_AB_PERIPHERAL == PIO_A_PERIPHERAL) {
		PIO->PIO_ABSR &= ~PIN;
		PIO->PIO_PDR = PIN;
	} else {
		PIO->PIO_ABSR |= PIN;
		PIO->PIO_PDR = PIN;
	}
}

/**
 * Configures the specified interrupt modes on the specified pin.
 * NOTE: you have to run "pio_init_pin()" before running this function.
 */
void pio_init_interrupt(Pio* PIO, uint32_t PIN, uint32_t Settings) {

	/* Fetch the peripheral id associated to this PIO */
	volatile uint32_t ID = pio_getID(PIO);

	/* Configure interrupt */
	if (Settings & INT_EDG_FALLING) {
		PIO->PIO_ESR = PIN; 		/* Enable interrupt on edge */
		PIO->PIO_FELLSR = PIN; 		/* Enable interrupts on falling edge */
		PIO->PIO_AIMER = PIN; 		/* Enable additional interrupt modes */
	} else if (Settings & INT_EDG_RISING) {
		PIO->PIO_ESR = PIN; 		/* Enable interrupt on edge */
		PIO->PIO_REHLSR = PIN; 		/* Enable interrupts on rising edge */
		PIO->PIO_AIMER = PIN; 		/* Enable additional interrupt modes */
	} else if (Settings & INT_LVL_LOW) {
		PIO->PIO_LSR = PIN; 		/* Enable interrupt on level */
		PIO->PIO_FELLSR = PIN; 		/* Enable interrupts on low level*/
		PIO->PIO_AIMER = PIN; 		/* Enable additional interrupt modes */
	} else if (Settings & INT_LVL_HIGH) {
		PIO->PIO_LSR = PIN; 		/* Enable interrupt on level */
		PIO->PIO_REHLSR = PIN; 		/* Enable interrupts on high level*/
		PIO->PIO_AIMER = PIN; 		/* Enable additional interrupt modes */
	}

	/* Enable interrupt and register it in the NVIC */
	PIO->PIO_IER = PIN;
	volatile uint32_t dummy = PIO->PIO_ISR; /* Clear flag */
	NVIC_ClearPendingIRQ(ID);
	NVIC_EnableIRQ(ID);

}

/**
 * Configures debounce on the specified pin with the specified
 * amount of debounce time (measured in ms).
 */
void pio_init_debounce(Pio* PIO, uint32_t PIN, uint32_t time_ms) {
	/* Calculate division factor */
	volatile uint32_t DIV = (16384 * time_ms) / 1000 - 1;
	//Set up slow clock divide fo c:a 16ms debounce
	PIO->PIO_SCDR = DIV;
	//Enable the input filter in debounce mode
	PIO->PIO_DIFSR = PIN;
	//Enable input filter
	PIO->PIO_IFER = PIN;
}

/**
 * Sets the pin (if configured as output) to the specified level (1 or 0).
 */
void pio_set_pin(Pio* PIO, uint32_t PIN, uint32_t state) {
	if (state < 1) {
		PIO->PIO_CODR = PIN;
	} else {
		PIO->PIO_SODR = PIN;
	}
}

/**
 * Toggles the pin (if configured as output).
 */
void pio_toggle_pin(Pio* PIO, uint32_t PIN) {
	if(PIO->PIO_ODSR & PIN) {
		PIO->PIO_CODR = PIN;
	} else {
		PIO->PIO_SODR = PIN;
	}
}

/**
 * Read pin.
 */
uint32_t pio_read_pin(Pio* PIO, uint32_t PIN) {

	if(PIO->PIO_PDSR & PIN) {
		return 1;
	} else {
		return 0;
	}
}

/**
 * Returns the peripheral ID of the specified Pio instance.
 * If the id is not found or something else goes wrong, this method
 * will return PIO_ERROR.
 */
uint32_t pio_getID(Pio* PIO) {
	volatile uint32_t pID = 0;

	switch ((uint32_t) PIO) {
	case (uint32_t) PIOA:
		pID = 11;
		break;
	case (uint32_t) PIOB:
		pID = 12;
		break;
	case (uint32_t) PIOC:
		pID = 13;
		break;
	case (uint32_t) PIOD:
		pID = 14;
		break;
	default:
		return PIO_ERROR;
		break;
	}

	return pID;
}
