/*
 * tempsensor_driver.c
 *
 *  Created on: Nov 18, 2016
 *      Author: root
 */

/**
 * -----------------------------------------------------------------------------------------------------
 * 												INCLUDES
 * -----------------------------------------------------------------------------------------------------
 */
#include "tempsensor_driver.h"

/**
 * -----------------------------------------------------------------------------------------------------
 * 												DEFINES
 * -----------------------------------------------------------------------------------------------------
 */
#define microsec(val) (val)/42

/**
 * -----------------------------------------------------------------------------------------------------
 * 												TYPEDEFS, variables
 * -----------------------------------------------------------------------------------------------------
 */
uint32_t temperatureAcqInterval = 100;

/**
 * -----------------------------------------------------------------------------------------------------
 * 												PROTOTYPES
 * -----------------------------------------------------------------------------------------------------
 */
extern void delay_ms(uint32_t t_ms);
static void delay(uint32_t cycles);

/**
 * -----------------------------------------------------------------------------------------------------
 * 												DECLARATIONS
 * -----------------------------------------------------------------------------------------------------
 */

/**
 * Initializes the tempsensor.
 */
void temp_init(void) {
	/* Setup pin */
	pio_init_pin(PIOB, PIO_PB25B_TIOA0, PIO_OUTPUT | PIO_OPENDRAIN | PIO_PULLUP);

	/* Reset tempsensor */
	pio_set_pin(TEMP_PIO, TEMP_PIN, 0);
	delay_ms(12);
	pio_set_pin(TEMP_PIO, TEMP_PIN, 1);

	/* Setup TC */
	tc_set_writeprotect(TEMP_TC, 0);
	PMC->PMC_PCER0 = 1 << TC0_IRQn;
	volatile uint32_t settings =
			TC_CMR_TCCLKS_TIMER_CLOCK1  |	/* 42Mhz */
			TC_CMR_LDRA_FALLING			| 	/* RA Loads on falling edge on TIOA */
			TC_CMR_LDRB_FALLING			; 	/* RB Loads on falling edge on TIOA */
	tc_init(TEMP_TC, TEMP_TC_CHANNEL, settings);

	NVIC_EnableIRQ(TC0_IRQn);
}

/**
 * Trigger a read from the temp sensor.
 */
void temp_trig(void) {

	/* Disable interrupts */
	tc_disable_interrupt(TEMP_TC, TEMP_TC_CHANNEL, TC_IER_LDRBS);

	/* Start timer and clear clock */
	tc_start(TEMP_TC, TEMP_TC_CHANNEL);

	/* Initiate a reading of the tempsensor */
	pio_set_pin(TEMP_PIO, TEMP_PIN, 0);
	delay(25);
	pio_set_pin(TEMP_PIO, TEMP_PIN, 1);

	/* Enable interrupts */
	tc_enable_interrupt(TEMP_TC, TEMP_TC_CHANNEL, TC_IER_LDRBS);
}

/**
 * Sets the time interval between consecutive samples.
 */
void temp_set_acqInterval(uint32_t t_ms) {

}

/**
 * Returns one reading from the tempSensor.
 */
uint32_t temp_read(void) {
	volatile uint32_t ra = 0, rb = 0, td = 0;
	uint32_t result = 0;

	tc_stop(TEMP_TC, TEMP_TC_CHANNEL);

	/* calculate value */
	ra = tc_read_ra(TEMP_TC, TEMP_TC_CHANNEL);
	rb = tc_read_rb(TEMP_TC, TEMP_TC_CHANNEL);
	td = microsec(rb - ra);
	result = (100*td)/5 - 27315;

	return result;
}

static void delay(uint32_t cycles) {
	for(uint32_t i=0; i<cycles; i++) {
		asm("NOP");
	}
}

