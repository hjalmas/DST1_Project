/*
 * ldr_driver.c
 *
 *  Created on: Nov 24, 2016
 *      Author: root
 */

/**
 * --------------------------------------------------------------------------------------------------
 * 										INCLUDES
 * --------------------------------------------------------------------------------------------------
 */
#include "ldr_driver.h"

/**
 * --------------------------------------------------------------------------------------------------
 * 										Declarations
 * --------------------------------------------------------------------------------------------------
 */

/**
 * Initializes the ldr driver.
 */
void ldr_init(void) {
	/* Enable ADC module */
	PMC->PMC_PCER1 = 1 << (ID_ADC - 32);

	/* Disable writeprotect */
	ADC->ADC_WPMR = 0x414443 << 8;

	/* Reset ADC */
	ADC->ADC_CR = ADC_CR_SWRST;

	/* Set up adc module */
	ADC->ADC_MR =
	ADC_MR_PRESCAL(1) 		| 	/* ADC clock = MCK/4 = 21MHz */
	ADC_MR_STARTUP_SUT16 	| 	/* STARTUP = 0.76 µs */
	ADC_MR_TRANSFER(1)		| 	/* TRANSFER = 1 -> transfer time = 0.12µs */
	ADC_MR_FREERUN_ON		;	/* Never wait for a trigger */

	/* Enable ADC channels */
	ADC->ADC_CHER = ADC_CHER_CH6 | ADC_CHER_CH7;
}

/*
 * Read the left ldr.
 */
int32_t ldr_rd_left(void) {
	return ADC->ADC_CDR[7] & ADC_CDR_DATA_Msk;
}

/*
 * Read the right ldr.
 */
int32_t ldr_rd_right(void) {
	return ADC->ADC_CDR[6] & ADC_CDR_DATA_Msk;
}








