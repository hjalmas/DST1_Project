/*
 * GUI.c
 *
 *  Created on: Nov 18, 2016
 *      Author: root
 */
/**
 * -----------------------------------------------------------------------------------------------------
 * 												INCLUDES
 * -----------------------------------------------------------------------------------------------------
 */
#include <main.h>

/**
 * -----------------------------------------------------------------------------------------------------
 * 												DEFINES
 * -----------------------------------------------------------------------------------------------------
 */

/* Defines a gui page */
typedef struct PAGE {
	uint32_t PAGE;					/* The page */
	void (*showPageFn)(void);		/* Function to display page */
	void* selectedComponent;		/* The component to autohighlight */
} Page_t;

#define PAGE_MAIN_MENU 			0
#define PAGE_LIVE_WEATHER 		1
#define PAGE_ERROR				2

static Page_t page_main_menu;
static Page_t page_live_weather;
static Page_t page_error;

/**
 * -----------------------------------------------------------------------------------------------------
 * 												TYPEDEFS, variables
 * -----------------------------------------------------------------------------------------------------
 */

static Page_t* currPage = &page_main_menu;
static Page_t* previousPage = &page_main_menu;

/* Main menu components --------------------------*/
Button_t menuLiveWeatherDataBtn;
Button_t menuItem2Btn;
Button_t menuItem3Btn;
Button_t menuSettingsBtn;
void* componentsMM[] = {
		&menuLiveWeatherDataBtn,
		&menuItem2Btn,
		&menuItem3Btn,
		&menuSettingsBtn,
};
uint32_t menuIdx = 0;

/* Live wather data page components --------------*/
TextField_t tempTxtField;
TextField_t presTxtField;
TextField_t txtField3;
TextField_t txtField4;
Button_t returnButton;

/* Error page components -------------------------*/
Button_t errorOkBtn;

/**
 * -----------------------------------------------------------------------------------------------------
 * 												PROTOTYPES
 * -----------------------------------------------------------------------------------------------------
 */
/* GUI functions ---------------------------*/
void gui_init(void);
void showMainMenu(void);
void showLiveWeatherData(void);
void showErrorPage(char** messages, uint32_t length);

/* Peripheral callback functions------------*/
void cbKeyPress(void);
void cbTempSensor(void);
void cbBarometer(void);

/* Main menu callbacks ---------------------*/
void cbMenuLiveWeatherDataBtn(void);
void cbMenuItem2Btn(void);
void cbMenuItem3Btn(void);
void cbMenuSettingsBtn(void);

/* Live weather data Callbacks -------------*/
void cbReturnButton(void);

/* Error page callbacks --------------------*/
void cbErrorOkBtn(void);

/* Private functions -----------------------*/
static void degCelsius(uint16_t row, uint16_t col);
static void toTmpStr(char* strBuffer, uint32_t val);
static void toPressStr(char* strBuffer, uint32_t val);
static void showPage(Page_t* page);

/**
 * -----------------------------------------------------------------------------------------------------
 * 												DECLARATIONS
 * -----------------------------------------------------------------------------------------------------
 */

/**
 * Initialize the graphical user interface.
 */
void gui_init(void) {

	/* Initialize pages */
	page_main_menu.PAGE = PAGE_MAIN_MENU;
	page_main_menu.showPageFn = showMainMenu;
	page_main_menu.selectedComponent = &menuLiveWeatherDataBtn;

	page_live_weather.PAGE = PAGE_LIVE_WEATHER;
	page_live_weather.showPageFn = showLiveWeatherData;
	page_live_weather.selectedComponent = &returnButton;

	page_error.PAGE = PAGE_ERROR;
	page_error.showPageFn = NULL;
	page_error.selectedComponent = &errorOkBtn;

	/* Initialize main menu components */
	gui_Button_init(&menuLiveWeatherDataBtn,"Live Weather Data", 7, 10, 20, cbMenuLiveWeatherDataBtn);
	gui_Button_init(&menuItem2Btn,"Page 2", 9, 10, 20, cbMenuItem2Btn);
	gui_Button_init(&menuItem3Btn,"Page 3", 11, 10, 20, cbMenuItem3Btn);
	gui_Button_init(&menuSettingsBtn,"Settings", 13, 10, 20, cbMenuSettingsBtn);

	/* Initialize Live weather data components */
	gui_TextField_init(&tempTxtField, "", 6, 23, 6, NULL);
	gui_TextField_init(&presTxtField, "", 8, 23, 6, NULL);
	gui_TextField_init(&txtField3, "", 10, 23, 6, NULL);
	gui_TextField_init(&txtField4, "", 12, 23, 6, NULL);
	gui_Button_init(&returnButton,"Return To Main Menu", 14, 9, 22, cbReturnButton);

	/* Initialize error page components */
	gui_Button_init(&errorOkBtn, "OK", 14, 15, 10, cbErrorOkBtn);

	showMainMenu();
}

/**
 * Display the main menu on the screen.
 */
void showMainMenu(void) {
	currPage = &page_main_menu;
	menuIdx = 0;
	disp_full_clear();
	graph_draw_rect(0, 0, 240, 128, true);
	graph_draw_rect(2, 2, 236, 124, true);
	graph_draw_rect(40, 16, 155, 96, true);
	graph_draw_rect(38, 14, 159, 100, true);
	graph_print_textBox("~Main Menu~", 4, 1, TEXT_ALIGN_CENTER);
	gui_Button_show(&menuLiveWeatherDataBtn);
	gui_Button_show(&menuItem2Btn);
	gui_Button_show(&menuItem3Btn);
	gui_Button_show(&menuSettingsBtn);
	gui_select_component(&menuLiveWeatherDataBtn);
}

/**
 * Display the Live weather data page on the screen.
 */
void showLiveWeatherData(void) {
	currPage = &page_live_weather;
	disp_full_clear();
	graph_draw_rect(0, 0, 240, 128, true);
	graph_draw_rect(2, 2, 236, 124, true);
	graph_print_textBox("~Live Weather Data~", 3, 1, TEXT_ALIGN_CENTER);
	graph_print_text("Temperature: ", 6, 9, TEXT_ALIGN_LEFT);
	gui_TextField_show(&tempTxtField);
	graph_print_text("Air Pressure: ", 8, 9, TEXT_ALIGN_LEFT);
	gui_TextField_show(&presTxtField);
	gui_TextField_show(&txtField3);
	gui_TextField_show(&txtField4);
	gui_Button_show(&returnButton);
	gui_select_component(&returnButton);

	degCelsius(tempTxtField.row, tempTxtField.col + 8);
	graph_print_text("kPa", presTxtField.row, presTxtField.col + 8, TEXT_ALIGN_LEFT);
}

/**
 * Show the error page on the screen.
 * With the specified messages.
 */
void showErrorPage(char** messages, uint32_t length) {
	previousPage = currPage;
	currPage = &page_error;

	disp_full_clear();

	graph_print_textBox("ERROR!!", 2, 1, TEXT_ALIGN_CENTER);
	for(uint32_t i=0; i<length & i<=10; i++) {
		graph_print_text(messages[i], 4 + i, 1, TEXT_ALIGN_CENTER);
	}

	gui_Button_show(&errorOkBtn);
	gui_select_component(&errorOkBtn);
}

/**
 * -----------------------------------------------------------------------------------------------------
 * 										PERIPHERAL CALLBACK FUNCTIONS
 * -----------------------------------------------------------------------------------------------------
 */

/**
 * Callback function to handle key-presses.
 */
void cbKeyPress(void) {
	volatile uint32_t key = kpad_get_key();

	switch (key) {
	case KEY_0:
		gui_handle_keypress('0');
		break;
	case KEY_1:
		gui_handle_keypress('1');
		break;
	case KEY_2:
		gui_handle_keypress('2');
		break;
	case KEY_3:
		gui_handle_keypress('3');
		break;
	case KEY_4:
		gui_handle_keypress('4');
		break;
	case KEY_5:
		gui_handle_keypress('5');
		break;
	case KEY_6:
		gui_handle_keypress('6');
		break;
	case KEY_7:
		gui_handle_keypress('7');
		break;
	case KEY_8:
		gui_handle_keypress('8');
		break;
	case KEY_9:
		gui_handle_keypress('9');
		break;
	case KEY_A:

		if(currPage->PAGE == PAGE_MAIN_MENU) {
			menuIdx == 0 ? menuIdx = 3 : menuIdx--;
			gui_select_component(componentsMM[menuIdx]);
	 	}

		break;
	case KEY_B:

		if(currPage->PAGE == PAGE_MAIN_MENU) {
			menuIdx == 3 ? menuIdx = 0 : menuIdx++;
			gui_select_component(componentsMM[menuIdx]);
		}

		break;
	case KEY_C:
		gui_handle_keypress(0x08);
		break;
	case KEY_D:
		gui_handle_keypress(0x0D);
		break;
	default:
		break;

	}
}

/**
 * Callback function to handle temperature sensor events.
 */
void cbTempSensor(void) {

	if(currPage->PAGE != PAGE_LIVE_WEATHER) {
		return;
	}

	char str[6];
	volatile uint32_t val = temp_read();
	toTmpStr(str, val);
	gui_TextField_setText(&tempTxtField, str);
}

/**
 * Callback function to handle Barometer events.
 */
void cbBarometer(void) {

	if(currPage->PAGE != PAGE_LIVE_WEATHER) {
		return;
	}

	char str[7];
	volatile uint32_t val = MPL_read_pressure();
	toPressStr(str, val);
	gui_TextField_setText(&presTxtField, str);
}

/**
 * -----------------------------------------------------------------------------------------------------
 * 												CALLBACK FUNCTIONS
 * -----------------------------------------------------------------------------------------------------
 */

/* Main menu callbacks ---------------------*/

void cbMenuLiveWeatherDataBtn(void) {
	showPage(&page_live_weather);
}

void cbMenuItem2Btn(void) {

}

void cbMenuItem3Btn(void) {

}

void cbMenuSettingsBtn(void) {
	char* errorMsg[3];
	errorMsg[0] = "This page is not defined yet!";
	errorMsg[1] = "Please go ahead and";
	errorMsg[2] = "program the contents!!";
	gui_select_component(&errorOkBtn);
	showErrorPage(errorMsg, 3);
}

/* Live weather data callbacks -------------*/
void cbReturnButton(void) {
	showPage(&page_main_menu);
}

/* Error page callbacks --------------------*/
void cbErrorOkBtn(void) {
	showPage(previousPage);
}

/**
 * -----------------------------------------------------------------------------------------------------
 * 												PRIVATE FUNCTIONS
 * -----------------------------------------------------------------------------------------------------
 */
static void degCelsius(uint16_t row, uint16_t col) {
	graph_print_text("C", row, col+1, TEXT_ALIGN_LEFT);
	volatile uint16_t x0 = 0, y0 = 0;
	y0 = 8 * (row-1) + 1;
	x0 = 6 * (col-1) + 4;
	graph_draw_circle(x0, y0, 2, true);
}

/**
 * Convert a integer value to a string, appropriate for the temperature data.
 */
static void toTmpStr(char* strBuffer, uint32_t val) {
	strBuffer[0] = 0x30 + (val/1000) % 10;
	strBuffer[1] = 0x30 + (val/100) % 10;
	strBuffer[2] = '.';
	strBuffer[3] = 0x30 + (val/10) % 10;
	strBuffer[4] = 0x30 + val % 10;
	strBuffer[5] = '\0';
}

/**
 * Convert a integer value to a string, appropriate for the pressure data.
 */
static void toPressStr(char* strBuffer, uint32_t val) {
	strBuffer[0] = 0x30 + (val/100000) % 10;
	strBuffer[1] = 0x30 + (val/10000) % 10;
	strBuffer[2] = 0x30 + (val/1000) % 10;
	strBuffer[3] = '.';
	strBuffer[4] = 0x30 + (val/100) % 10;
	strBuffer[5] = 0x30 + (val/10) % 10;
	strBuffer[6] = '\0';
}

/**
 * Shown the specified page on the screen.
 */
static void showPage(Page_t* page) {
	gui_select_component(page->selectedComponent);
	page->showPageFn();
}
