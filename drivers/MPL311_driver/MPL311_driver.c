/*
 * MPL311_driver.c
 *
 *  Created on: Nov 21, 2016
 *      Author: Albin Hjälmås
 */

/* -----------------------------------------------------------------------------------
 * 										INCLUDES
 * -----------------------------------------------------------------------------------
 */
#include "MPL311_driver.h"

/* -----------------------------------------------------------------------------------
 * 										DEFINES
 * -----------------------------------------------------------------------------------
 */

/* Standard 7-bit I2C slave address is 1100000 = 0x60, 8-bit read address is 0xC1, 8-bit write is 0xC0 */
#define MPL311_ADDRESS 	0x60  /* SA0 is high, 0x1C if low */

/* Registers -------------*/
#define STATUS			0x00	/* Sensor Status Register */
#define OUT_P_MSB		0x01	/* Pressure Data Out MSB, Bits 12-19 of 20 */
#define	OUT_P_CSB		0x02	/* Pressure Data Out CSB, Bits 4-11  of 20 */
#define OUT_P_LSB		0x03	/* Pressure Data Out LSB, Bits 0-3   of 20 */
#define OUT_T_MSB  		0x04	/* Temperature Data Out MSB, Bits 4-11 of 12-bit */
#define OUT_T_LSB  		0x05	/* Temperature Data Out LSB, Bits 1-3 of 12-bit */
#define DR_STATUS  		0x06	/* Sensor Status Register, Data Ready status information */
#define OUT_P_DELTA_MSB 0x07	/* Pressure Data Out Delta MSB, Bits 12-19 of 20-bit Pressure change data */
#define OUT_P_DELTA_CSB 0x08	/* Pressure Data Out Delta CSB, Bits 4-11 of 20-bit Pressure change data */
#define OUT_P_DELTA_LSB 0x09	/* Pressure Data Out Delta LSB, Bits 0-3 of 20-bit Pressure change data */
#define OUT_T_DELTA_MSB 0x0A	/* Temperature Data Out Delta MSB, Bits 4-11 of 12-bit Temperature change data */
#define OUT_T_DELTA_LSB 0x0B	/* Temperature Data Out Delta LSB */
#define WHO_AM_I   		0x0C	/* Device Identification Register */
#define F_STATUS   		0x0D	/* FIFO Status Register */
#define F_DATA     		0x0E	/* FIFO 8-bit Data Access */
#define F_SETUP    		0x0F	/* FIFO Setup Register */
#define TIME_DLY   		0x10	/* Time Delay */
#define SYSMOD     		0x11	/* System Mode Register */
#define INT_SOURCE 		0x12	/* Interrupt Source Register */
#define PT_DATA_CFG 	0x13	/* PT Data Configuration Register */
#define BAR_IN_MSB 		0x14 	/* Set at factory to equivalent sea level pressure for 	*/
#define BAR_IN_LSB 		0x15 	/* measurement location, generally no need to change	*/
#define P_TGT_MSB  		0x16	/* Pressure Target MSB */
#define P_TGT_LSB  		0x17	/* Pressure Target LSB */
#define T_TGT      		0x18	/* Temperature Target */
#define P_WND_MSB  		0x19	/* Pressure/Altitude Window MSB */
#define P_WND_LSB  		0x1A	/* Pressure/Altitude Window LSB */
#define T_WND      		0x1B	/* Temperature Window */
#define P_MIN_MSB  		0x1C	/* Minimum Pressure Data Out MSB */
#define P_MIN_CSB  		0x1D	/* Minimum Pressure Data Out CSB */
#define P_MIN_LSB  		0x1E	/* Minimum Pressure Data Out LSB */
#define T_MIN_MSB  		0x1F	/* Minimum Temperature Data Out */
#define T_MIN_LSB  		0x20	/* Minimum Temperature Data Out */
#define P_MAX_MSB  		0x21	/* Maximum Pressure Data Out */
#define P_MAX_CSB  		0x22	/* Maximum Pressure Data Out */
#define P_MAX_LSB  		0x23	/* Maximum Pressure Data Out LSB */
#define T_MAX_MSB  		0x24	/* Maximum Temperature Data Out */
#define T_MAX_LSB  		0x25	/* Maximum Temperature Data Out */
#define CTRL_REG1  		0x26	/* Control Register 1 */
#define CTRL_REG2  		0x27	/* Control Register 2 */
#define CTRL_REG3  		0x28	/* Control Register 3 */
#define CTRL_REG4  		0x29	/* Control Register 4 */
#define CTRL_REG5  		0x2A	/* Control Register 5 */
#define OFF_P      		0x2B	/* Pressure Data User Offset */
#define OFF_T      		0x2C	/* Temperature Data User Offset */
#define OFF_H      		0x2D	/* Altitude Data User Offset Register */

/* -----------------------------------------------------------------------------------
 * 										VARIABLES
 * -----------------------------------------------------------------------------------
 */

uint32_t pressureAcqInterval = 100;

/* Transmit and receive packets ------------*/
static twi_packet_t txPacket, rxPacket;
static uint8_t buffer[5];

/* -----------------------------------------------------------------------------------
 * 										PROTOTYPES
 * -----------------------------------------------------------------------------------
 */
static void 	MPL_wr_reg(uint8_t reg, uint8_t data);
static uint8_t 	MPL_rd_reg(uint8_t reg);
static void 	MPL_multi_rd(uint8_t reg, uint8_t* buffer, uint8_t length);
extern void delay_ms(uint32_t t_ms);


/* -----------------------------------------------------------------------------------
 * 										DECLARATIONS
 * -----------------------------------------------------------------------------------
 */

/**
 * Initializes the MPL311.
 */
void MPL_init(void) {
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
	txPacket.chip = MPL311_ADDRESS;
	txPacket.buffer = buffer;
	txPacket.addr_length = 1;
	txPacket.length = 1;

	rxPacket.chip = MPL311_ADDRESS;
	rxPacket.buffer = buffer;
	rxPacket.addr_length = 1;
	rxPacket.length = 1;

	/* Configure the MPL311 sensor */

	/* Reset the device */
	MPL_reset();

	/* Set to Barometer with an OSR = 16 (66ms)*/
	MPL_wr_reg(CTRL_REG1, 0x20);
	/* Set Active */
	MPL_set_active(MPL_ACTIVE);
	/* Clear Flags */
	MPL_clear_interrupts();
}

/**
 * Read the flags of the MPL311.
 * use in conjunction with the defines:
 * FLAG_DATARDY		Data is ready
 * FLAG_PR_RDY		Preassure data is ready
 * FLAG_TMP_RDY		Temperature data is ready
 */
uint8_t MPL_read_flags(void) {
	return MPL_rd_reg(DR_STATUS);
}

/**
 * Set operating state of MPL311 sensor.
 * use in conjuction with the defines:
 * MPL_ACTIVE
 * MPL_STANDBY
 */
void MPL_set_active(uint8_t operatingState) {
	switch(operatingState) {
	case MPL_STANDBY:
		MPL_wr_reg(CTRL_REG1, 0x20);
		break;
	case MPL_ACTIVE:
		MPL_wr_reg(CTRL_REG1, 0x21);
		break;
	default:
		break;
	}
}

/**
 * Resets the device.
 */
void MPL_reset(void) {
	/* Write 1 to the reset bit, which should reset the MPL311 */
	MPL_wr_reg(CTRL_REG1, 1 << 2);
	delay_ms(10);
}

/**
 * Sets the time interval for pressure accquisition, in ms.
 */
void MPL_set_acqTimeInterval(uint32_t t_ms) {
	pressureAcqInterval = t_ms;
}

/**
 * Set the accusition timestep.
 * Step value is 2 ST : Giving a range of 1 second to 2 15 seconds (9 hours)
 */
void MPL_set_timestep(uint8_t ST) {
	uint8_t currData = MPL_rd_reg(CTRL_REG2);
	currData &= 0xF0;
	MPL_wr_reg(CTRL_REG2, currData | ST);
}

/**
 * Returns the 20bit pressure data.
 */
uint32_t MPL_read_pressure(void) {
	volatile uint32_t result = 0;
	MPL_multi_rd(OUT_P_MSB, buffer, 3);
	result = buffer[0]<<12 | buffer[1]<<4 | buffer[2]>>4;
	return result;
}

/**
 * Returns the 12bit temperature data.
 */
float MPL_read_temperature(void) {
	volatile int8_t msb;
	volatile uint8_t lsb;
	float result;
	MPL_multi_rd(OUT_T_MSB, buffer, 2);
	msb = (int8_t) buffer[0];
	result = (float)buffer[1] / 256.0f + (float)msb;
	return result;
}

/**
 * Clears the interrupts.
 */
void MPL_clear_interrupts(void) {
	MPL_read_pressure();
	MPL_read_temperature();
}

/* -----------------------------------------------------------------------------------
 * 										PRIVATE FUNCTIONS
 * -----------------------------------------------------------------------------------
 */

/**
 * Write to a single register.
 */
static void MPL_wr_reg(uint8_t reg, uint8_t data) {
	buffer[0] = data;
	txPacket.addr[0] = reg;
	twi_master_write(TWI1, &txPacket);
}

/**
 * Read from a single register.
 */
static uint8_t MPL_rd_reg(uint8_t reg) {
	rxPacket.addr[0] = reg;
	twi_master_read(TWI1, &rxPacket);
	return buffer[0];
}

/**
 * Read from several consecutive registers.
 */
static void MPL_multi_rd(uint8_t reg, uint8_t* buffer, uint8_t length) {
	for(uint8_t i=0; i<length; i++) {
		rxPacket.addr[0] = (reg + i);
		rxPacket.buffer = &buffer[i];
		twi_master_read(TWI1, &rxPacket);
	}
}
