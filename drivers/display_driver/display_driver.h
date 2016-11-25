/*
 * display_driver.h
 *
 *  Created on: Nov 10, 2016
 *      Author: root
 */

#ifndef DISPLAY_DRIVER_H_
#define DISPLAY_DRIVER_H_

/**
 * ------------------------------------------------------------------------------------------------------
 * 											INCLUDES
 * ------------------------------------------------------------------------------------------------------
 */
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <float.h>
#include "diag/Trace.h"
#include "cmsis_device.h"
#include "sam3x_conf.h"

#include "../pio_driver/pio_driver.h"

/**
 * ------------------------------------------------------------------------------------------------------
 * 											DEFINES
 * ------------------------------------------------------------------------------------------------------
 */
#define DISP_PORT 		PIOC

/* Pin connections */
#define DISP_DB0		PIO_PC2		/* arduino pin: 34 */
#define DISP_DB1		PIO_PC3		/* arduino pin: 35 */
#define DISP_DB2		PIO_PC4		/* arduino pin: 36 */
#define DISP_DB3		PIO_PC5		/* arduino pin: 37 */
#define DISP_DB4		PIO_PC6		/* arduino pin: 38 */
#define DISP_DB5		PIO_PC7		/* arduino pin: 39 */
#define DISP_DB6		PIO_PC8		/* arduino pin: 40 */
#define DISP_DB7		PIO_PC9		/* arduino pin: 41 */
#define DISP_DB_MSK		0xFF << 2

#define DISP_WR			PIO_PC14	/* arduino pin: 49 Read/Write select signal, RW=1: Read  RW: =0: Write */
#define DISP_RD			PIO_PC15	/* arduino pin: 48 Active LOW read */
#define DISP_CE			PIO_PC16	/* arduino pin: 47 Active LOW chip select signal */
#define DISP_CD			PIO_PC17	/* arduino pin: 46 Register select signal. CD=1: Command, CD=0: Data */
#define DISP_FS			PIO_PC18	/* arduino pin: 45 Font select signal.     H:6x8,  L:8x8 */
#define DISP_RV			PIO_PC19	/* arduino pin: 44 Display mode signal. RV=1:  Reverse display, RV=0: normal */

#define DISP_74HC_OE	PIO_PC12	/* arduino pin: 51 Output enable, a high will turn off outputs */
#define DISP_74HC_DIR	PIO_PC13	/* arduino pin: 50 DIR=0: MCU->Display, DIR=1: Display->MCU */

/* Reset pin and port */
#define DISP_RST_PORT	PIOB		/*  */
#define DISP_RST		PIO_PB21	/* Active LOW reset signal */

/* Status codes */
#define DISP_STA0_MSK	1 << 0		/* Command execution capability, 	0:Disabled, 1:Enabled */
#define DISP_STA1_MSK	1 << 1		/* Data read/write capability		0:Disabled, 1:Enabled */
#define DISP_STA2_MSK	1 << 2		/* Auto mode data read capability	0:Disabled, 1:Enabled */
#define DISP_STA3_MSK	1 << 3		/* Auto mode data write capability 	0:Disabled, 1:Enabled */
#define DISP_STA4_MSK	1 << 4		/* Not used */
#define DISP_STA5_MSK	1 << 5		/* Controller operation capability 	0:Disabled, 1:Enabled */
#define DISP_STA6_MSK	1 << 6		/* Error flag. Used for Screen copy commands. 0:No error, 1: error */
#define DISP_STA7_MSK	1 << 7		/* Blink condition 0:Display off, 1:Normal display */

/* Display memory regions */
#define TEXT_HOME_ADDRESS		0x0000
#define GRAPHIC_HOME_ADDRESS	0x3000

/* Specific defines */
#define DATABUS_OUTPUT 			0
#define DATABUS_INPUT  			1
#define STATUS_OK				0x3
#define TOTAL_CHARS				640 	/* total number of characters on the screen */
#define TOTAL_PIXELS			30720	/* Total number of pixels on the screen 240x128 */
#define TOTAL_GRAPHIC_SEGMENTS	5120	/* Total number of 6dot in line segments */

/* Display registers */

/* Setting registers */
#define SET_CURSOR_PIONTER 			0x21
#define SET_OFFSET_REGISTER			0x22
#define SET_ADDRESS_PIONTER			0x24
/* Set control word */
#define SET_TEXT_HOME_ADDRESS		0x40
#define SET_TEXT_AREA				0x41
#define SET_GRAPHIC_HOME_ADDRESS	0x42
#define SET_GRAPHIC_AREA			0x43
/* Mode set */
#define OR_MODE						0b10000000
#define EXOR_MODE					0b10000001
#define AND_MODE					0b10000011
#define TEXT_ATTRIBUTE_MODE			0b10000100
#define INTERNAL_CHARGEN_MODE		0b10000000
#define EXTERNAL_CHARGEN_MODE		0b10001000
/* Display modes */
#define DISPLAY_OFF					0b10010000
#define CURS_ON_BLINK_OFF			0b10010010
#define CURS_ON_BLINK_ON			0b10010011
#define TEXT_ON_GRAPHIC_OFF			0b10010100
#define TEXT_OFF_GRAPHIC_ON			0b10011000
#define TEXT_ON_GRAPHIC_ON			0b10011100
/* Cursor pattern select */
#define CURSOR_1_LINE				0b10100000
#define CURSOR_2_LINE				0b10100001
#define CURSOR_3_LINE				0b10100010
#define CURSOR_4_LINE				0b10100011
#define CURSOR_5_LINE				0b10100100
#define CURSOR_6_LINE				0b10100101
#define CURSOR_7_LINE				0b10100110
#define CURSOR_8_LINE				0b10100111
/* Data auto read/write */
#define SET_DATA_AUTO_WRITE			0xB0
#define SET_DATA_AUTO_READ			0xB1
#define AUTO_RESET					0xB2
/* Data read/write */
#define DATA_WR_INC_ADP				0xC0
#define DATA_RD_INC_ADP				0xC1
#define DATA_WR_DEC_ADP				0xC2
#define DATA_RD_DEC_ADP				0xC3
#define DATA_WR						0xC4
#define DATA_RD						0xC5
/* Cursor auto moving */
#define CURSOR_AUTOMOVE				0x60

/* Frames */
#define FRAME_TEXT_1			TEXT_HOME_ADDRESS
#define FRAME_TEXT_2			0x0280
#define FRAME_GRAPHIC_1			GRAPHIC_HOME_ADDRESS
#define FRAME_GRAPHIC_2			0x4400
#define FRAME_1					0x00
#define FRAME_2					0x01

/**
 * ------------------------------------------------------------------------------------------------------
 * 											PROTOTYPES
 * ------------------------------------------------------------------------------------------------------
 */

/**
 * Initializes the display.
 */
void disp_init(void);

/**
 * Switch frames.
 */
void disp_set_frame(uint16_t frame);

/**
 * Returns the status of display.
 */
uint8_t disp_readStatus(void);

/**
 * Write a command to the display.
 */
void disp_wr_cmd(uint8_t cmd);

/**
 * Write a byte to the display.
 */
void disp_wr(uint8_t data);

/**
 * Write 1 byte to display together with command.
 */
void disp_wr_byte(uint8_t cmd, uint8_t D1);

/**
 * Read one byte from display.
 */
uint8_t disp_rd_byte(uint8_t cmd);

/**
 * write a half-word to the display followed by the command.
 */
void disp_wr_hword(uint8_t cmd, uint16_t halfWord);

/**
 * Completely clear screen.
 */
void disp_full_clear(void);

#endif /* DISPLAY_DRIVER_H_ */
