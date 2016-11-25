
#ifndef TC_H_INCLUDED
#define TC_H_INCLUDED

/**
 * The Timer Counter (TC) includes several identical 16-bit or 32-bit
 * Timer Counter channels. Each channel can be independently programmed
 * to perform a wide range of functions that includes frequency
 * measurement, event counting, interval measurement, pulse generation,
 * delay timing, and pulse width modulation.
 */


/*
 * -------------------------------------------------------------------
 * 							INCLUDES
 * -------------------------------------------------------------------
 */
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include "diag/Trace.h"
#include "cmsis_device.h"
#include "sam3x_conf.h"


/*
 * -------------------------------------------------------------------
 * 							PROTOTYPES
 * -------------------------------------------------------------------
 */

/**
 * Configure TC for timer, waveform generation, or capture.
 */
void tc_init(Tc *p_tc, uint32_t ul_channel, uint32_t ul_Mode);

/**
 * Configure TC for 2-bit Gray Counter for Stepper Motor.
 */
uint32_t tc_init_2bit_gray(Tc *p_tc, uint32_t ul_channel, uint32_t ul_steppermode);

/**
 * Start the TC clock on the specified channel.
 */
void tc_start(Tc *p_tc, uint32_t ul_channel);

/**
 * Stop the TC clock on the specified channel.
 */
void tc_stop(Tc *p_tc, uint32_t ul_channel);

/**
 * Read the counter value on the specified channel.
 * returns The counter value.
 */
uint32_t tc_read_cv(Tc *p_tc, uint32_t ul_channel);

/**
 * Read TC Register A (RA) on the specified channel.
 * Returns The TC Register A (RA) value.
 */
uint32_t tc_read_ra(Tc *p_tc, uint32_t ul_channel);

/**
 * Read TC Register B (RB) on the specified channel.
 * Returns The TC Register B (RB) value.
 */
uint32_t tc_read_rb(Tc *p_tc, uint32_t ul_channel);

/**
 * TC Register C (RC) on the specified channel.
 * Returns The Register C (RC) value.
 */
uint32_t tc_read_rc(Tc *p_tc, uint32_t ul_channel);

/**
 * Write to TC Register A (RA) on the specified channel.
 */
void tc_write_ra(Tc *p_tc, uint32_t ul_channel, uint32_t ul_value);

/**
 * Write to TC Register B (RB) on the specified channel.
 */
void tc_write_rb(Tc *p_tc, uint32_t ul_channel, uint32_t ul_value);

/**
 * Write to TC Register C (RC) on the selected channel.
 */
void tc_write_rc(Tc *p_tc, uint32_t ul_channel, uint32_t ul_value);

/**
 * Enable the TC interrupts on the specified channel.
 */
void tc_enable_interrupt(Tc *p_tc, uint32_t ul_channel,uint32_t ul_sources);

/**
 * Disable TC interrupts on the specified channel.
 */
void tc_disable_interrupt(Tc *p_tc, uint32_t ul_channel, uint32_t ul_sources);

/**
 * Read the TC interrupt mask for the specified channel.
 */
uint32_t tc_get_interrupt_mask(Tc *p_tc, uint32_t ul_channel);

/**
 * Get the current status for the specified TC channel.
 */
uint32_t tc_get_status(Tc *p_tc, uint32_t ul_channel);

/**
 * Enable or disable write protection of TC registers.
 */
void tc_set_writeprotect(Tc *p_tc, uint32_t ul_enable);

#endif /* TC_H_INCLUDED */
