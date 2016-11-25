/*
 * gui_components.h
 *
 *  Created on: Nov 14, 2016
 *      Author: Albin Hjälmås
 */

#ifndef GUI_COMPONENTS_H_
#define GUI_COMPONENTS_H_


/**
 * -----------------------------------------------------------------------------------------------------
 * 												INCLUDES
 * -----------------------------------------------------------------------------------------------------
 */
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "cmsis_device.h"
#include "sam3x_conf.h"

#include "../display_driver/display_driver.h"
#include "../graphic_driver/graphic_driver.h"

/**
 * -----------------------------------------------------------------------------------------------------
 * 											USED COMPONENTS
 * -----------------------------------------------------------------------------------------------------
 */
#define USE_TEXTFIELD
#define USE_TEXTFEED

/**
 * -----------------------------------------------------------------------------------------------------
 * 												DEFINES
 * -----------------------------------------------------------------------------------------------------
 */

#define TEXTFIELD 0
#define TEXTFEED 1
#define BUTTON 2

/**
 * -----------------------------------------------------------------------------------------------------
 * 										   TYPEDEFS, variables
 * -----------------------------------------------------------------------------------------------------
 */

typedef uint32_t type_t;

typedef struct Dummy {
	type_t type;
} GuiCast;

typedef struct TextField {
	type_t type;				/* The type of this object */
	char text[40];				/* The text contained within this TextField_t instance. */
	uint16_t row;				/* Row of the first character */
	uint16_t col;				/* column of the first character, defines location for the TextField */
	uint16_t width;				/* width in terms of characters */
	uint16_t align;				/* Align struct */
	void (*cbTextField)(void);	/* Callback function for this TextField */
} TextField_t;

typedef struct TextFeed {
	type_t type;				/* The type of this object */
	char strings[16][40];		/* The strings displayed in this TextFeed */
	uint16_t head;				/* The index of the first string */
	uint16_t tail;				/* The index of the last added string */
	uint16_t size;
	uint16_t row;				/* Row of the first character */
	uint16_t col;				/* column of the first character, defines location for the TextFeed */
	uint16_t width;				/* width in terms of characters */
	uint16_t height;			/* height in terms of characters */
	uint16_t align;
} TextFeed_t;

typedef struct Button {
	type_t type;				/* The type of this object */
	char text[32];				/* The text displayed on the button */
	uint16_t row;				/* The position of this button in "y" direction */
	uint16_t col;				/* The position of this button in "x" direction */
	uint16_t width;				/* The width of this button */
	uint16_t align;
	void (*cbButton)(void);		/* Callback function for this button */
} Button_t;

/**
 * -----------------------------------------------------------------------------------------------------
 * 												PROTOTYPES
 * -----------------------------------------------------------------------------------------------------
 */
/**
 * Creates a TextField.
 */
void gui_TextField_init(
		TextField_t* field,
		char* initText,
		uint16_t row,				/* Row of the first character */
		uint16_t col,				/* column of the first character, defines location for the TextField */
		uint16_t width,				/* width in terms of characters */
		void (*cbTextField)(void)   /* Callback function for this TextField */
);

/**
 * Select textField.
 */
void gui_select_component(void* component);

/**
 * Shows the specified TextField on the screen.
 */
void gui_TextField_show(TextField_t* txtField);

/**
 * Set the text contained within a TextField.
 */
void gui_TextField_setText(TextField_t* field, char* text);

/**
 * Handles a keypress.
 */
void gui_handle_keypress(char ch);

/*
 * Creates a TextFeed.
 */
void gui_TextFeed_init(
		TextFeed_t* feed,
		uint16_t row,				/* Row of the first character */
		uint16_t col,				/* column of the first character, defines location for the TextFeed */
		uint16_t width,				/* width in terms of characters */
		uint16_t height				/* height in terms of characters */
);

/*
 * Displays the TextFeed on the screen.
 */
void gui_TextFeed_show(TextFeed_t* feed);

/**
 * Adds a string to the TextFeed.
 */
void gui_TextFeed_puts(TextFeed_t* feed, char* text);

/**
 * Initializes the specified button.
 */
void gui_Button_init(
		Button_t* button,
		char* text,					/* The text displayed on the button */
		uint16_t row,				/* The position of this button in "y" direction */
		uint16_t col,				/* The position of this button in "x" direction */
		uint16_t width,				/* The width of this button */
		void (*cbButton)(void)		/* Callback function for this button */
);

/**
 * Displays the specified button on the screen.
 */
void gui_Button_show(Button_t* button);

#endif /* GUI_COMPONENTS_H_ */
