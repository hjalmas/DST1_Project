/*
 * display_driver.c
 *
 *  Created on: Nov 10, 2016
 *      Author: root
 */

/**
 * ------------------------------------------------------------------------------------------------------
 * 											INCLUDES
 * ------------------------------------------------------------------------------------------------------
 */
#include "display_driver.h"

/**
 * ------------------------------------------------------------------------------------------------------
 * 											VARIABLES
 * ------------------------------------------------------------------------------------------------------
 */
uint16_t currTxtFrame = FRAME_TEXT_1;
uint16_t currGraphFrame = FRAME_GRAPHIC_1;
uint16_t currFrame = FRAME_1;

/**
 * ------------------------------------------------------------------------------------------------------
 * 											PROTOTYPES
 * ------------------------------------------------------------------------------------------------------
 */
static inline void set_databusDir(uint8_t DATABUS_DIR) 	__attribute__((always_inline));
static inline void enable_databus(void)					__attribute__((always_inline));
static inline void disable_databus(void)				__attribute__((always_inline));
static inline uint8_t read_databus(void) 				__attribute__((always_inline));
static inline void write_databus(uint8_t data) 			__attribute__((always_inline));
static void delay(uint32_t cycles);

/**
 * ------------------------------------------------------------------------------------------------------
 * 											DECLARATIONS
 * ------------------------------------------------------------------------------------------------------
 */

/**
 * Initializes the display.
 */
void disp_init(void) {
	/* Configure I/O pins */
	pio_init_pin(DISP_PORT, DISP_DB_MSK, PIO_INPUT);
	pio_init_pin(DISP_PORT,
			DISP_WR | DISP_RD |
			DISP_CE | DISP_CD |
			DISP_FS | DISP_RV |
			DISP_74HC_OE | DISP_74HC_DIR,
			PIO_OUTPUT
	);
	pio_init_pin(DISP_RST_PORT, DISP_RST, PIO_OUTPUT);

	/* Set default pin states */
	pio_set_pin(DISP_PORT, DISP_RV, 0);
	pio_set_pin(DISP_PORT,
			DISP_WR | DISP_RD |
			DISP_CE | DISP_CD |
			DISP_FS,
			1
	);

	/* Reset the display */
	pio_set_pin(DISP_RST_PORT, DISP_RST, 0);
	delay(1000);
	pio_set_pin(DISP_RST_PORT, DISP_RST, 1);

	/* set up display */
	disp_wr_hword(SET_TEXT_HOME_ADDRESS, TEXT_HOME_ADDRESS);
	disp_wr_hword(SET_GRAPHIC_HOME_ADDRESS, GRAPHIC_HOME_ADDRESS);
	disp_wr_hword(SET_TEXT_AREA, 40);
	disp_wr_hword(SET_GRAPHIC_AREA, 40);
	disp_wr_cmd(OR_MODE);
	disp_full_clear();
	disp_wr_cmd(TEXT_ON_GRAPHIC_ON);
}

/**
 * Switch frames.
 */
void disp_set_frame(uint16_t frame) {
	switch(frame) {
	case FRAME_1:
		currFrame = FRAME_1;
		currTxtFrame = FRAME_TEXT_1;
		currGraphFrame = FRAME_GRAPHIC_1;
		break;
	case FRAME_2:
		currFrame = FRAME_2;
		currTxtFrame = FRAME_TEXT_2;
		currGraphFrame = FRAME_GRAPHIC_2;
		break;
	}

	disp_wr_hword(SET_TEXT_HOME_ADDRESS, currTxtFrame);
	disp_wr_hword(SET_GRAPHIC_HOME_ADDRESS, currGraphFrame);
}

/**
 * Returns the status of display.
 * {RD = 0, WR = 1, CS = 0, CD = H}
 */
uint8_t disp_readStatus(void) {
	volatile uint8_t tmp = 0;

	set_databusDir(DATABUS_INPUT);				/* Set databus to input */
	enable_databus();
	DISP_PORT->PIO_SODR = DISP_CD;				/* Enable Command mode */
	DISP_PORT->PIO_CODR = DISP_RD | DISP_CE;	/* Select display and enable read mode */
	delay(10);
	tmp = read_databus();						/* Read data from display */
	DISP_PORT->PIO_SODR = DISP_RD | DISP_CE;	/* Deselect display and disable read mode */
	disable_databus();
	return tmp;
}

/**
 * Write a command to the display.
 */
void disp_wr_cmd(uint8_t cmd) {
	/* Wait for display to return ok */
	while((disp_readStatus() & STATUS_OK) != STATUS_OK) {
		asm("NOP");
	};

	set_databusDir(DATABUS_OUTPUT);
	enable_databus();
	write_databus(cmd);
	DISP_PORT->PIO_SODR = DISP_CD;				/* Enable Command mode */
	DISP_PORT->PIO_CODR = DISP_WR | DISP_CE;	/* Select display and enable write mode */
	DISP_PORT->PIO_SODR = DISP_WR | DISP_CE;	/* Deselect display and disable write mode */
	disable_databus();
	set_databusDir(DATABUS_INPUT);
}

/**
 * Write a byte to the display.
 */
void disp_wr(uint8_t data) {
	/* Wait for display to return ok */
	while((disp_readStatus() & STATUS_OK) != STATUS_OK) {
		asm("NOP");
	};

	set_databusDir(DATABUS_OUTPUT);
	enable_databus();
	write_databus(data);
	DISP_PORT->PIO_CODR = DISP_CD;				/* Enable Data mode */
	DISP_PORT->PIO_CODR = DISP_WR | DISP_CE;	/* Select display and enable write mode */
	DISP_PORT->PIO_SODR = DISP_WR | DISP_CE;	/* Deselect display and disable write mode */
	disable_databus();
	set_databusDir(DATABUS_INPUT);
}

/**
 * Write 1 bytes to display together with command.
 */
void disp_wr_byte(uint8_t cmd, uint8_t D1) {
	disp_wr(D1);
	disp_wr_cmd(cmd);
}

/**
 * Read one byte from display.
 */
uint8_t disp_rd_byte(uint8_t cmd) {
	disp_wr_cmd(cmd);

	/* Wait for display to return ok */
	while((disp_readStatus() & STATUS_OK) != STATUS_OK) {
		asm("NOP");
	};

	volatile uint8_t tmp = 0;
	set_databusDir(DATABUS_INPUT);				/* Set databus to input */
	enable_databus();
	DISP_PORT->PIO_CODR = DISP_CD;				/* Disable Command mode */
	DISP_PORT->PIO_CODR = DISP_RD | DISP_CE;	/* Select display and enable read mode */
	delay(10);
	tmp = read_databus();						/* Read data from display */
	DISP_PORT->PIO_SODR = DISP_RD | DISP_CE;	/* Deselect display and disable read mode */
	disable_databus();
	return tmp;
}

/**
 * write a half-word to the display followed by the command.
 */
void disp_wr_hword(uint8_t cmd, uint16_t halfWord) {
	disp_wr((uint8_t) (halfWord & 0xFF));
	disp_wr((uint8_t) ((halfWord & 0xFF00) >> 8));
	disp_wr_cmd(cmd);
}

/**
 * Completely clear screen.
 */
void disp_full_clear(void) {
	disp_wr_hword(SET_ADDRESS_PIONTER, currTxtFrame);
	for(int i=0; i<TOTAL_CHARS; i++) {
		disp_wr_byte(DATA_WR_INC_ADP, 0x00);
	}

	disp_wr_hword(SET_ADDRESS_PIONTER, currGraphFrame);
	for(int i=0; i<TOTAL_GRAPHIC_SEGMENTS; i++) {
			disp_wr_byte(DATA_WR_INC_ADP, 0x00);
	}
}

/**
 * ------------------------------------------------------------------------------------------------------
 * 											PRIVATE FUNCTIONS
 * ------------------------------------------------------------------------------------------------------
 */

/**
 * Sets the direction of the display databus.
 */
static inline void set_databusDir(uint8_t DATABUS_DIR) {
	if(DATABUS_DIR) { /* Databus is to be configured as input */
		DISP_PORT->PIO_ODR = DISP_DB_MSK;		/* Disable output */
		DISP_PORT->PIO_SODR = DISP_74HC_DIR;	/* Set 74HC245 DIR=1 Display->MCU */
	} else { /* Databus is to be configured as output */
		DISP_PORT->PIO_OER = DISP_DB_MSK;		/* Enable output */
		DISP_PORT->PIO_CODR = DISP_74HC_DIR;	/* Set 74HC245 DIR=0 MCU->Display */
	}
}

/**
 * Enables the 74HC245
 */
static inline void enable_databus(void) {
	DISP_PORT->PIO_CODR = DISP_74HC_OE;
}

/**
 * Disables the 74HC245
 */
static inline void disable_databus(void) {
	DISP_PORT->PIO_SODR = DISP_74HC_OE;
}

/**
 * Reads the current value on the databus.
 */
static inline uint8_t read_databus(void) {
	volatile uint8_t tmp = (uint8_t) (DISP_PORT->PIO_PDSR & DISP_DB_MSK) >> 2;
	return tmp;
}

/**
 * Write a byte on the databus.
 */
static inline void write_databus(uint8_t data) {
	DISP_PORT->PIO_CODR = DISP_DB_MSK;				/* Clear the databus */
	DISP_PORT->PIO_SODR = (uint32_t)(data << 2);	/* Write value to databus */
}

/**
 * Delay for some cycles.
 */
static void delay(uint32_t cycles) {
	for(uint32_t i=0; i<cycles; i++) {
		asm("NOP");
	}
}
