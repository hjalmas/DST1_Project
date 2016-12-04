/*
 * rtc_driver.c
 *
 *  Created on: Dec 4, 2016
 *      Author: Albin Hjälmås
 */

/**
 * --------------------------------------------------------------------------------------------------
 * 										INCLUDES
 * --------------------------------------------------------------------------------------------------
 */
#include "rtc_driver.h"
#include "../twi_driver/twi_driver.h"
#include "../pio_driver/pio_driver.h"

/**
 * --------------------------------------------------------------------------------------------------
 * 										VARIABLES
 * --------------------------------------------------------------------------------------------------
 */
/* Packets */
twi_packet_t rxPacket;
twi_packet_t txPacket;

/* Buffer */
uint8_t buffer[10];

/**
 * --------------------------------------------------------------------------------------------------
 * 										DEFINES
 * --------------------------------------------------------------------------------------------------
 */

/* I2C address */
#define DS1307_ADDRESS		0b1101000

/* Registers */
#define SECONDS 			0x00	/* Seconds of minute    range: 00-59 */
#define MINUTES				0x01	/* Minute of hour       range: 00-59 */
#define HOURS				0x02	/* Hour of day			range: 00-23 if B6 = 0 else 00-12 */
#define DAY					0x03	/* Day of week			range: 0-7 */
#define DATE				0x04	/* Day of month			range: 01-31 */
#define MONTH				0x05	/* Month of year		range: 01-12 */
#define YEAR				0x06	/* Year of century		range: 00-99 */
#define CONTROL				0x07	/* Control register, B7 = OUT, B4 = SQWE, B1 = RS1, B0 = RS0 */

/* Masks */
#define OSC_EN_MSK			0x80
#define SECOND10_MSK 		0x70
#define SECOND_MSK			0x0F
#define MIN10_MSK			0x70
#define MIN_MSK				0x0F
#define HOUR_SELECT_MSK		0x40
#define HOUR_AMPM_MSK		0x20
#define HOUR10_MSK			0x30
#define HOUR_MSK			0x0F
#define DAY_MSK				0x07
#define DATE10_MSK			0x30
#define DATE_MSK			0x0F
#define MONTH10_MSK			0x10
#define MONTH_MSK			0x0F
#define YEAR10_MSK			0xF0
#define YEAR_MSK			0x0F
#define CTRL_OUT_MSK		0x80
#define CTRL_SQWE_MSK		0x10
#define CTRL_RS1_MSK		0x02
#define CTRL_RS0_MSK		0x01

/* Macro's */
#define osc_en(a) 		((a) << 7 & OSC_EN_MSK)
#define second10(a) 	(((a) << 4) & SECOND10_MSK)
#define second(a)		((a) & SECOND_MSK)
#define minute10(a)		(((a) << 4) & MIN10_MSK)
#define minute(a)		((a) & MIN_MSK)
#define hour10(a)		(((a) << 4) & HOUR10_MSK)
#define hour(a)			((a) & HOUR_MSK)
#define date10(a) 		(((a) << 4) & DATE10_MSK)
#define date(a)			((a) & DATE_MSK)
#define month10(a) 		(((a) << 4) & MONTH10_MSK)
#define month(a)		((a) & MONTH_MSK)
#define year10(a)		(((a) << 4) & YEAR10_MSK)
#define year(a)			((a) & YEAR_MSK)

/**
 * --------------------------------------------------------------------------------------------------
 * 										PROTOTYPES
 * --------------------------------------------------------------------------------------------------
 */
/**
 * Reads one register on the rtc module.
 */
static uint8_t rtc_rd_reg(uint8_t reg);

/**
 * Writes one register on the rtc module.
 */
static uint8_t rtc_wr_reg(uint8_t reg, uint8_t data);

/**
 * Stops the rtc module.
 */
static void rtc_startStop(bool start);

/**
 * --------------------------------------------------------------------------------------------------
 * 										DECLARATIONS
 * --------------------------------------------------------------------------------------------------
 */

timestamp_t ts;

/**
 * Initializes the rtc module.
 */
void rtc_init(void) {
	/* Enable TWI peripheral */
	PMC->PMC_PCER0 = 1 << ID_TWI1;

	/* Configure pins used by TWI */
	pio_init_pin(PIOB, PIO_PB13A_TWCK1 | PIO_PB12A_TWD1, PIO_INPUT);
	pio_select_AB(PIOB, PIO_PB13A_TWCK1 | PIO_PB12A_TWD1, PIO_A_PERIPHERAL);

	/* Configure TWI instance */
	twi_enable_master_mode(TWI1);
	twi_options_t options;
	options.master_clk = SystemCoreClock;
	options.speed = 400000;
	twi_master_init(TWI1, &options);

	/* Configure packets */
	rxPacket.chip = DS1307_ADDRESS;
	rxPacket.buffer = buffer;
	rxPacket.addr_length = 1;
	rxPacket.length = 1;

	txPacket.chip = DS1307_ADDRESS;
	txPacket.buffer = buffer;
	txPacket.addr_length = 1;
	txPacket.length = 1;

	/* Set 24H mode */
	volatile uint8_t tmp = rtc_rd_reg(HOURS);
	rtc_wr_reg(HOURS, tmp & ~HOUR_AMPM_MSK);
}

/**
 * Sets the time of the rtc module.
 */
void rtc_set_time(timestamp_t* time) {
	/* Stop the clock */
	rtc_startStop(false);

	/* Set seconds */
	rtc_wr_reg(SECONDS, OSC_EN_MSK | second10(time->seconds / 10) | second(time->seconds % 10));

	/* Set minutes */
	rtc_wr_reg(MINUTES, minute10(time->minutes / 10) | minute(time->minutes % 10));

	/* Set hours */
	rtc_wr_reg(HOURS, hour10(time->hours / 10) | hour(time->hours % 10));

	/* Set day of week */
	rtc_wr_reg(DAY, time->day & DAY_MSK);

	/* Set date (day of month) */
	rtc_wr_reg(DATE, date10(time->date / 10) | date(time->date % 10));

	/* Set month */
	rtc_wr_reg(MONTH, month10(time->month / 10) | month(time->month % 10));

	/* Set year */
	rtc_wr_reg(YEAR, year10(time->year / 10) | year(time->year % 10));

	/* Start the clock */
	rtc_startStop(true);
}

/**
 * Returns a pointer to a timestamp_t instance.
 */
timestamp_t* rtc_get_timestamp(void) {
	timestamp_t* result = malloc(sizeof(timestamp_t));

	/* Set seconds */
	volatile uint8_t tmp = rtc_rd_reg(SECONDS);
	result->seconds = ((tmp & SECOND10_MSK) >> 4) * 10 + tmp & SECOND_MSK;

	/* Set minutes */
	tmp = rtc_rd_reg(MINUTES);
	result->minutes = ((tmp & MIN10_MSK) >> 4) * 10 + tmp & MIN_MSK;

	/* Set hours */
	tmp = rtc_rd_reg(HOURS);
	result->hours = ((tmp & HOUR10_MSK) >> 4) * 10 + tmp & HOUR_MSK;

	/* Set Day */
	tmp = rtc_rd_reg(DAY);
	result->day = tmp & DAY_MSK;

	/* Set date */
	tmp = rtc_rd_reg(DATE);
	result->date = ((tmp & DATE10_MSK) >> 4) * 10 + tmp & DATE_MSK;

	/* Set month */
	tmp = rtc_rd_reg(MONTH);
	result->month = ((tmp & MONTH10_MSK) >> 4) * 10 + tmp & MONTH_MSK;

	/* Set year */
	tmp = rtc_rd_reg(YEAR);
	result->year = ((tmp & YEAR10_MSK) >> 4) * 10 + tmp & YEAR_MSK;

	return result;
}

/**
 * --------------------------------------------------------------------------------------------------
 * 										PRIVATE DECLARATIONS
 * --------------------------------------------------------------------------------------------------
 */

/**
 * Reads one register on the rtc module.
 */
static uint8_t rtc_rd_reg(uint8_t reg) {
	rxPacket.addr[0] = reg;
	twi_master_read(TWI1, &rxPacket);
	return buffer[0];
}

/**
 * Writes one register on the rtc module.
 */
static uint8_t rtc_wr_reg(uint8_t reg, uint8_t data) {
	buffer[0] = data;
	txPacket.addr[0] = reg;
	twi_master_write(TWI1, &txPacket);
}

/**
 * Stops the rtc module.
 */
static void rtc_startStop(bool start) {
	volatile uint8_t tmp = rtc_rd_reg(SECONDS);

	if(start) {
		rtc_wr_reg(SECONDS, tmp & ~OSC_EN_MSK);
	} else {
		rtc_wr_reg(SECONDS, tmp | OSC_EN_MSK);
	}
}







