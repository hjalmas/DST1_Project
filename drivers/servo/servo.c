/*
 * servo.c
 *
 *  Created on: Nov 22, 2016
 *      Author: Albin Hjälmås
 */
/* -----------------------------------------------------------------------------------
 * 										INCLUDES
 * -----------------------------------------------------------------------------------
 */
#include "servo.h"

/* -----------------------------------------------------------------------------------
 * 										DEFINES
 * -----------------------------------------------------------------------------------
 */
#define MAX_DUTY		8079					/* Maximum duty value */
#define MIN_DUTY		1939					/* Minimum duty value */
#define DELTA_DUTY		6140					/* Difference btween max and min duty */
#define ROT_ANGLE		180						/* Max rotation angle */

/* -----------------------------------------------------------------------------------
 * 										DECLARATIONS
 * -----------------------------------------------------------------------------------
 */

/**
 * Initializes the servo.
 */
void servo_init(void) {
	/* Enable PIOA */
	PMC->PMC_PCER0 = 1 << ID_PIOA;
	/* Use B peripheral */
	pio_init_pin(PIOA, PIO_PA19B_PWMH1, PIO_OUTPUT);
	pio_select_AB(PIOA, PIO_PA19B_PWMH1, PIO_B_PERIPHERAL);

	/* Enable PWM */
	PMC->PMC_PCER1 = 1 << (ID_PWM - 32);
	/* disable writeprotect */
	PWM->PWM_WPCR =
			PWM_WPCR_WPRG0 | PWM_WPCR_WPRG1 |
			PWM_WPCR_WPRG2 | PWM_WPCR_WPRG3 |
			PWM_WPCR_WPRG4 | PWM_WPCR_WPRG5 |
			PWM_WPCR_WPKEY(0x50574D);
	PWM->PWM_WPCR = PWM_WPCR_WPCMD(0);
	/* Set up pwm clock PREA = PREB = 0, DIVA = DIVB = 26 -> f = 49.3Hz*/
	PWM->PWM_CLK |= PWM_CLK_DIVA(26) | PWM_CLK_DIVB(26);
	/* Select clock for channel 1 */
	PWM->PWM_CH_NUM[1].PWM_CMR = PWM_CMR_CPRE_CLKA | PWM_CMR_CPOL;
	/* Set period of pwm channel */
	PWM->PWM_CH_NUM[1].PWM_CPRD |= 0xFFFF;
	/* Set 90 degrees rotation to begin with */
	servo_rotate(90);
	/* Enable PWM channel 1 */
	PWM->PWM_ENA = PWM_ENA_CHID1;
}

/**
 * Rotates the servo to the specified angle.
 */
void servo_rotate(uint32_t angle) {
	/* Update dutycycle */
	PWM->PWM_CH_NUM[1].PWM_CDTYUPD = MIN_DUTY + (DELTA_DUTY/ROT_ANGLE) * angle;
}
