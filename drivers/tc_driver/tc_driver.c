/*
 * -------------------------------------------------------------------
 * 							INCLUDES
 * -------------------------------------------------------------------
 */
#include "tc_driver.h"

/*
 * -------------------------------------------------------------------
 * 							DEFINES
 * -------------------------------------------------------------------
 */
#define TC_WPMR_WPKEY_PASSWD TC_WPMR_WPKEY((uint32_t)0x54494D)
#define TC_DIV_FACTOR 65536 		/* TC divisor used to find the lowest acceptable timer frequency */
#define FREQ_SLOW_CLOCK_EXT 32768 	/* External slow clock frequency (hz) */

/*
 * -------------------------------------------------------------------
 * 							DECLARATIONS
 * -------------------------------------------------------------------
 */

/**
 * Configure TC for timer, waveform generation, or capture.
 */
void tc_init(Tc *p_tc, uint32_t ul_channel, uint32_t ul_mode) {
	TcChannel *tc_channel;
	tc_channel = p_tc->TC_CHANNEL + ul_channel;

	/*  Disable TC clock. */
	tc_channel->TC_CCR = TC_CCR_CLKDIS;

	/*  Disable interrupts. */
	tc_channel->TC_IDR = 0xFFFFFFFF;

	/*  Clear status register. */
	tc_channel->TC_SR;

	/*  Set mode. */
	tc_channel->TC_CMR = ul_mode;
}

/**
 * Configure TC for 2-bit Gray Counter.
 */
uint32_t tc_init_2bit_gray(Tc *p_tc, uint32_t ul_channel, uint32_t ul_steppermode) {
	p_tc->TC_CHANNEL[ul_channel].TC_SMMR = ul_steppermode;
	return 0;
}

/**
 * Start the TC clock on the specified channel.
 */
void tc_start(Tc *p_tc, uint32_t ul_channel) {
	p_tc->TC_CHANNEL[ul_channel].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

/**
 * Stop the TC clock on the specified channel.
 */
void tc_stop(Tc *p_tc, uint32_t ul_channel) {
	p_tc->TC_CHANNEL[ul_channel].TC_CCR = TC_CCR_CLKDIS;
}

/**
 * Read the counter value on the specified channel.
 * returns The counter value.
 */
uint32_t tc_read_cv(Tc *p_tc, uint32_t ul_channel) {
	return p_tc->TC_CHANNEL[ul_channel].TC_CV;
}

/**
 * Read TC Register A (RA) on the specified channel.
 * Returns The TC Register A (RA) value.
 */
uint32_t tc_read_ra(Tc *p_tc, uint32_t ul_channel) {
	return p_tc->TC_CHANNEL[ul_channel].TC_RA;
}

/**
 * Read TC Register B (RB) on the specified channel.
 * Returns The TC Register B (RB) value.
 */
uint32_t tc_read_rb(Tc *p_tc, uint32_t ul_channel) {
	return p_tc->TC_CHANNEL[ul_channel].TC_RB;
}

/**
 * TC Register C (RC) on the specified channel.
 * Returns The Register C (RC) value.
 */
uint32_t tc_read_rc(Tc *p_tc, uint32_t ul_channel) {
	return p_tc->TC_CHANNEL[ul_channel].TC_RC;
}

/**
 * Write to TC Register A (RA) on the specified channel.
 */
void tc_write_ra(Tc *p_tc, uint32_t ul_channel, uint32_t ul_value) {
	p_tc->TC_CHANNEL[ul_channel].TC_RA = ul_value;
}

/**
 * Write to TC Register B (RB) on the specified channel.
 */
void tc_write_rb(Tc *p_tc, uint32_t ul_channel, uint32_t ul_value) {
	p_tc->TC_CHANNEL[ul_channel].TC_RB = ul_value;
}

/**
 * Write to TC Register C (RC) on the selected channel.
 */
void tc_write_rc(Tc *p_tc, uint32_t ul_channel, uint32_t ul_value) {
	p_tc->TC_CHANNEL[ul_channel].TC_RC = ul_value;
}

/**
 * Enable the TC interrupts on the specified channel.
 */
void tc_enable_interrupt(Tc *p_tc, uint32_t ul_channel, uint32_t ul_sources) {
	TcChannel *tc_channel;
	tc_channel = p_tc->TC_CHANNEL + ul_channel;
	tc_channel->TC_IER = ul_sources;
}

/**
 * Disable TC interrupts on the specified channel.
 */
void tc_disable_interrupt(Tc *p_tc, uint32_t ul_channel, uint32_t ul_sources) {
	TcChannel *tc_channel;
	tc_channel = p_tc->TC_CHANNEL + ul_channel;
	tc_channel->TC_IDR = ul_sources;
}

/**
 * Read the TC interrupt mask for the specified channel.
 */
uint32_t tc_get_interrupt_mask(Tc *p_tc, uint32_t ul_channel) {
	TcChannel *tc_channel;
	tc_channel = p_tc->TC_CHANNEL + ul_channel;
	return tc_channel->TC_IMR;
}

/**
 * Get the current status for the specified TC channel.
 */
uint32_t tc_get_status(Tc *p_tc, uint32_t ul_channel) {
	TcChannel *tc_channel;
	tc_channel = p_tc->TC_CHANNEL + ul_channel;
	return tc_channel->TC_SR;
}


/**
 * Enable or disable write protection of TC registers.
 */
void tc_set_writeprotect(Tc *p_tc, uint32_t ul_enable) {
	if (ul_enable) {
		p_tc->TC_WPMR = TC_WPMR_WPKEY_PASSWD | TC_WPMR_WPEN;
	} else {
		p_tc->TC_WPMR = TC_WPMR_WPKEY_PASSWD;
	}
}
