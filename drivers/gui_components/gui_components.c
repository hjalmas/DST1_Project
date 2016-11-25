/*
 * gui_components.c
 *
 *  Created on: Nov 14, 2016
 *      Author: Albin Hjälmås
 */

/**
 * -----------------------------------------------------------------------------------------------------
 * 												INCLUDES
 * -----------------------------------------------------------------------------------------------------
 */
#include "gui_components.h"

/**
 * -----------------------------------------------------------------------------------------------------
 * 												DEFINES
 * -----------------------------------------------------------------------------------------------------
 */

/**
 * -----------------------------------------------------------------------------------------------------
 * 												TYPEDEFS, variables
 * -----------------------------------------------------------------------------------------------------
 */
extern uint16_t currTxtFrame;
extern uint16_t currGraphFrame;
extern uint16_t currFrame;

void* selectedComponent; 		/* the currently selected guiComponent */

/**
 * -----------------------------------------------------------------------------------------------------
 * 												PROTOTYPES
 * -----------------------------------------------------------------------------------------------------
 */
static void print_TextField(TextField_t* field);
static void print_TextFeed(TextFeed_t* feed);
static void clear_TextField(TextField_t* field);
static void clear_TextFeed(TextFeed_t* feed);
static void set_ComponentFocus(void* component);
static void set_addressPointer(uint16_t row, uint16_t col);
static uint16_t inc(uint16_t* pVal);
static void setButtonFocus(Button_t* button, bool state);

/**
 * -----------------------------------------------------------------------------------------------------
 * 												Declarations
 * -----------------------------------------------------------------------------------------------------
 */

/**
 * Handles a keypress.
 */
void gui_handle_keypress(char ch) {

	if(((GuiCast*)selectedComponent)->type == TEXTFIELD) {
		TextField_t* f = (TextField_t*) selectedComponent;
		size_t length = strlen(f->text);

		/* if a carrriage return (enter key) is detected, then run handler */
		if (ch == 0x0D && f != NULL) {
			f->cbTextField();
			return;
			/* If a backspace key was pressed */
		} else if (ch == 0x08 && length > 0 && f != NULL) {
			f->text[length - 1] = '\0';
			print_TextField(f);
			/* just a normal keypress */
		} else if (f != NULL && ch != 0x08 && length < 40) {
			f->text[length] = ch;
			f->text[length + 1] = '\0';
			print_TextField(f);
		}
	} else if(((GuiCast*)selectedComponent)->type == BUTTON) {
		Button_t* button = (Button_t*)selectedComponent;

		/* if a carrriage return (enter key) is detected, then run handler */
		if (ch == 0x0D) {
			button->cbButton();
			return;
		}
	}
}

/**
 * Select component.
 */
void gui_select_component(void* component) {

	/* Clear the extra line around the currently
	 * selected button before selecting next component */
	if(((GuiCast*)selectedComponent)->type == BUTTON) {
		setButtonFocus((Button_t*)selectedComponent, false);
	}

	selectedComponent = component;
	set_ComponentFocus(component);
}

/**
 * Initializes a TextField.
 */
void gui_TextField_init(
		TextField_t* field,			/* The TextField to initialize */
		char* initText,				/* Text in the TextField */
		uint16_t row, 				/* Row of the first character */
		uint16_t col, 				/* column of the first character, defines location for the TextField */
		uint16_t width, 			/* width in terms of characters */
		void (*cbTextField)(void) 	/* Callback function for this TextField */
		) {
	field->type = TEXTFIELD;
	strcpy(field->text, initText);
	field->row = row;
	field->col = col;
	field->width = width;
	field->cbTextField = cbTextField;
}

/**
 * Shows the specified TextField on the screen.
 */
void gui_TextField_show(TextField_t* field) {
	volatile uint16_t rectWidth = (field->width + 1) * 6 + 3;
	volatile uint16_t x0 = (field->col - 1) * 6 - 1, y0 = (field->row - 1)
			* 8 - 2;
	graph_draw_rect(x0, y0, rectWidth, 11, true);
	print_TextField(field);

	gui_select_component(field);
}

/**
 * Set the text contained within a TextField.
 */
void gui_TextField_setText(TextField_t* field, char* text) {
	uint32_t length = strlen(text);
	for (uint32_t i = 0; i < length; i++) {
		field->text[i] = *(text + i);
	}
	field->text[length] = '\0';
	clear_TextField(field);
	print_TextField(field);

	set_ComponentFocus(selectedComponent);
}

/*
 * Creates a TextFeed.
 */
void gui_TextFeed_init(
		TextFeed_t* feed,
		uint16_t row,				/* Row of the first character */
		uint16_t col,				/* column of the first character, defines location for the TextFeed */
		uint16_t width,				/* width in terms of characters */
		uint16_t height				/* height in terms of characters */
	) {
	feed->type = TEXTFEED;
	feed->head = 0;
	feed->tail = 0;
	feed->size = 0;
	feed->row = row;
	feed->col = col;
	feed->width = width;
	feed->height = height;
}

/*
 * Displays the TextFeed on the screen.
 */
void gui_TextFeed_show(TextFeed_t* feed) {
	volatile uint16_t width = (feed->width + 1) * 6 + 3;
	volatile uint16_t x0 = (feed->col - 1) * 6 - 1, y0 = (feed->row - 1) * 8 - 2;
	volatile uint16_t height = feed->height * 8 + 3;
	graph_draw_rect(x0, y0, width, height, true);

	set_ComponentFocus(selectedComponent);
}

/**
 * Adds a string to the TextFeed.
 */
void gui_TextFeed_puts(TextFeed_t* feed, char* text) {

	if(feed->size == 0) {
		strcpy(feed->strings[0], text);
		feed->size++;
	} else if(feed->size < feed->height) {
		strcpy(feed->strings[inc(&(feed->tail))], text);
		feed->size++;
	} else {
		strcpy(feed->strings[inc(&(feed->tail))], text);
		inc(&(feed->head));
	}

	print_TextFeed(feed);

	set_ComponentFocus(selectedComponent);
}

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
) {
	button->type = BUTTON;
	strcpy(button->text, text);
	button->row = row;
	button->col = col;
	button->width = width;
	button->cbButton = cbButton;
}

/**
 * Displays the specified button on the screen.
 */
void gui_Button_show(Button_t* button) {
	volatile uint16_t rectWidth = (button->width + 1) * 6 + 3;
	volatile uint16_t x0 = (button->col - 1) * 6 - 1, y0 = (button->row - 1)
			* 8 - 2;
	graph_draw_rect(x0, y0, rectWidth, 11, true);

	size_t length = strlen(button->text);
	if((int32_t)button->width - length < 0) {
		graph_print_text("ERROR", button->row, button->col, TEXT_ALIGN_LEFT);
	} else {
		graph_print_text(button->text, button->row, button->col + (button->width - length)/2, TEXT_ALIGN_LEFT);
	}

	gui_select_component(button);
}

/**
 * ------------------------------------------------------------------------------------------------------
 * 											PRIVATE FUNCTIONS
 * ------------------------------------------------------------------------------------------------------
 */

/**
 * Prints the text in a TextField
 */
static void print_TextField(TextField_t* field) {

	TextField_t* f = field;
	size_t length = strlen(f->text);

	if (length > f->width) {
		graph_print_text(f->text + length - f->width, (uint8_t) f->row,
				(uint8_t) f->col, TEXT_ALIGN_LEFT);
	} else {
		graph_print_text(f->text, (uint8_t) f->row, (uint8_t) f->col,
		TEXT_ALIGN_LEFT);
		disp_wr_byte(DATA_WR, 0x00);
	}
}

/**
 * Prints the text in a TextFeed.
 */
static void print_TextFeed(TextFeed_t* feed) {

	clear_TextFeed(feed);

	volatile uint16_t idx = feed->head;
	volatile uint16_t cnt = 0;
	while(cnt < feed->size) {
		graph_print_text(feed->strings[idx], feed->row + cnt, feed->col, TEXT_ALIGN_LEFT);
		inc(&idx);
		cnt++;
	}
}

/**
 * Clears a TextField.
 */
static void clear_TextField(TextField_t* field) {

	volatile uint16_t startAddress = currTxtFrame
			+ (uint16_t) (40 * field->row + field->col - 41) + field->width;
	disp_wr_hword(SET_ADDRESS_PIONTER, startAddress);
	for (unsigned int i = 0; i <= field->width; i++) {
		disp_wr_byte(DATA_WR_DEC_ADP, 0);
	}
}

/**
 * Clears a TextFeed.
 */
static void clear_TextFeed(TextFeed_t* feed) {
	for(int i=0; i<feed->height; i++) {
		volatile uint16_t startAddress = currTxtFrame
				+ (uint16_t) (40 * (feed->row + i) + feed->col - 41);
		disp_wr_hword(SET_ADDRESS_PIONTER, startAddress);
		for (unsigned int j = 0; j <= feed->width; j++) {
			disp_wr_byte(DATA_WR_INC_ADP, 0);
		}
	}
}

/**
 * Sets the focus on this component.
 */
static void set_ComponentFocus(void* component) {
	if(((GuiCast*)component)->type == TEXTFIELD) {
		TextField_t* field = (TextField_t*) component;

		size_t strLength = strlen(field->text);

		/* Avoid setting the cursor outside of the TextField */
		if(strLength > field->width) {
			set_addressPointer(field->row, field->col + field->width);
		} else {
			set_addressPointer(field->row, field->col + strLength);
		}
	} else if(((GuiCast*)component)->type == BUTTON) {
		Button_t* button = (Button_t*) component;
		setButtonFocus(button, true);
	}
}

/**
 * Set addresspointer to specified location.
 */
static void set_addressPointer(uint16_t row, uint16_t col) {
	volatile uint16_t startAddress = currTxtFrame + (40 * row + col - 41);
	disp_wr_hword(SET_ADDRESS_PIONTER, startAddress);
}

static uint16_t inc(uint16_t* pVal) {
	if(*pVal < 15) {
		(*pVal)++;
	} else {
		*pVal = 0;
	}

	return *pVal;
}

static void setButtonFocus(Button_t* button, bool state) {
	volatile uint16_t rectWidth = (button->width + 1) * 6 + 5;
	volatile uint16_t x0 = (button->col - 1) * 6 - 2, y0 = (button->row - 1) * 8 - 3;
	graph_draw_rect(x0, y0, rectWidth, 13, state);
}

