/*
 * rtc_driver.h
 *
 *  Created on: Dec 4, 2016
 *      Author: Albin Hjälmås
 */

#ifndef RTC_DRIVER_H_
#define RTC_DRIVER_H_

/**
 * --------------------------------------------------------------------------------------------------
 * 										INCLUDES
 * --------------------------------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "diag/Trace.h"
#include "cmsis_device.h"
#include "sam3x_conf.h"

/**
 * --------------------------------------------------------------------------------------------------
 * 										VARIABLES
 * --------------------------------------------------------------------------------------------------
 */
/* A struct representing an instant in time */
typedef struct Timestamp {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;
} timestamp_t;


/**
 * --------------------------------------------------------------------------------------------------
 * 										DEFINES
 * --------------------------------------------------------------------------------------------------
 */

/**
 * --------------------------------------------------------------------------------------------------
 * 										PROTOTYPES
 * --------------------------------------------------------------------------------------------------
 */

/**
 * Initializes the rtc module.
 */
void rtc_init(void);

/**
 * Sets the time of the rtc module.
 */
void rtc_set_time(timestamp_t* time);

/**
 * Returns a pointer to a timestamp_t instance.
 */
timestamp_t* rtc_get_timestamp(void);


#endif /* RTC_DRIVER_H_ */
