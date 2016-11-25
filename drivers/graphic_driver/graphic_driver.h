/*
 * graphic_driver.h
 *
 *  Created on: Nov 11, 2016
 *      Author: Albin Hjälmås
 */

#ifndef GRAPHIC_DRIVER_H_
#define GRAPHIC_DRIVER_H_

/**
 * ------------------------------------------------------------------------------------------------------
 * 											INCLUDES
 * ------------------------------------------------------------------------------------------------------
 */
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "diag/Trace.h"
#include "cmsis_device.h"
#include "sam3x_conf.h"

#include "../pio_driver/pio_driver.h"
#include "../display_driver/display_driver.h"

/**
 * ------------------------------------------------------------------------------------------------------
 * 											DEFINES
 * ------------------------------------------------------------------------------------------------------
 */

/* Text alignment relative to screen boundary */
#define TEXT_ALIGN_LEFT			0x00
#define TEXT_ALIGN_CENTER		0x01
#define TEXT_ALIGN_RIGHT		0x02


/**
 * ------------------------------------------------------------------------------------------------------
 * 											PROTOTYPES
 * ------------------------------------------------------------------------------------------------------
 */

/**
 * prints text on the specified location on the screen.
 */
void graph_print_text(char* txt, uint8_t row, uint8_t col, uint8_t TEXT_ALIGN);

/**
 * prints text in a box on the specified location on the screen.
 * Only single-line text.
 */
void graph_print_textBox(char* txt, uint8_t row, uint8_t col, uint8_t TEXT_ALIGN);

/**
 * Clears 1 character on the display.
 */
void graph_clear_char(uint8_t row, uint8_t col);

/**
 * Draws a pixel at the specified location on the screen;
 */
void graph_draw_pixel(uint16_t x, uint16_t y, bool value);

/**
 * Draws a line between (x0, y0) and (x1, y1) on the display.
 */
void graph_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, bool value);

/**
 * Draws a rectangle with (x0, y0) in the upper left corner.
 */
void graph_draw_rect(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, bool value);

/**
 * Draws a circle with center (x0, y0) and radius r.
 */
void graph_draw_circle(uint16_t x0, uint16_t y0, uint16_t r, bool value);

/**
 * Converts from ascii to display text.
 */
uint8_t a2dispTxt(char ascii);

#endif /* GRAPHIC_DRIVER_H_ */
