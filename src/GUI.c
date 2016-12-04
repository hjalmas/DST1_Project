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
#define PAGE_SETTINGS			3
#define PAGE_SETTINGS_DATETIME	4

static Page_t page_main_menu;
static Page_t page_live_weather;
static Page_t page_error;
static Page_t page_settings;
static Page_t page_settings_datetime;

/**
 * -----------------------------------------------------------------------------------------------------
 * 												TYPEDEFS, variables
 * -----------------------------------------------------------------------------------------------------
 */

static Page_t* currPage = &page_main_menu;
static Page_t* previousPage = &page_main_menu;

/* time display box */
TextField_t timeTxtField;
char timeBuffer[20];
const uint8_t timeRow = 16;
const uint8_t timeCol = 1;

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

/* Settings page components ----------------------*/
Button_t dateTimeBtn;
Button_t settingsReturnBtn;
void* componentsSetting[] = {
		&dateTimeBtn,
		&settingsReturnBtn
};
uint32_t settingsIdx = 0;

/* Settings_datetime components ------------------*/
Button_t dateTimeOkBtn;
Button_t dateTimeCancelBtn;
TextField_t secondsTxtField;
TextField_t minutesTxtField;
TextField_t hoursTxtField;
TextField_t dateTxtField;
TextField_t monthTxtField;
TextField_t yearTxtField;
void* componentsDateTime[] = {
	&secondsTxtField,
	&minutesTxtField,
	&hoursTxtField,
	&dateTxtField,
	&monthTxtField,
	&yearTxtField,
	&dateTimeOkBtn,
	&dateTimeCancelBtn
};
uint32_t dateTimeIdx = 0;

/**
 * -----------------------------------------------------------------------------------------------------
 * 												PROTOTYPES
 * -----------------------------------------------------------------------------------------------------
 */
/* Dummy callback -------------------------*/
void cbDummy(void);

/* GUI functions ---------------------------*/
void gui_init(void);
void showMainMenu(void);
void showLiveWeatherData(void);
void showErrorPage(char** messages, uint32_t length);
void showSettingsPage(void);
void showSettingsDateTimePage(void);

/* Peripheral callback functions------------*/
void cbKeyPress(void);
void cbTempSensor(void);
void cbBarometer(void);
void cbTimeUpdate(void);

/* Main menu callbacks ---------------------*/
void cbMenuLiveWeatherDataBtn(void);
void cbMenuItem2Btn(void);
void cbMenuItem3Btn(void);
void cbMenuSettingsBtn(void);

/* Live weather data Callbacks -------------*/
void cbReturnButton(void);

/* Error page callbacks --------------------*/
void cbErrorOkBtn(void);

/* Settings page callbacks -----------------*/
void cbDateTimeBtn(void);
void cbSettingsReturnBtn(void);

/* Settings_datetime callback functions */
void cbDateTimeOkBtn(void);
void cbDateTimeCancelBtn(void);

/* Private functions -----------------------*/
static void degCelsius(uint16_t row, uint16_t col);
static void toTmpStr(char* strBuffer, uint32_t val);
static void toPressStr(char* strBuffer, uint32_t val);
static void showPage(Page_t* page);
static void drawFrame(void);

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

	page_settings.PAGE = PAGE_SETTINGS;
	page_settings.showPageFn = showSettingsPage;
	page_settings.selectedComponent = &dateTimeBtn;

	page_settings_datetime.PAGE = PAGE_SETTINGS_DATETIME;
	page_settings_datetime.showPageFn = showSettingsDateTimePage;
	page_settings_datetime.selectedComponent = &secondsTxtField;

	/* Initialize time text field */
	gui_TextField_init(&timeTxtField, "", 15, 1, 18, cbDummy);

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

	/* Initialize settings page components */
	gui_Button_init(&dateTimeBtn, "Date/time", 4, 2, 12, cbDateTimeBtn);
	gui_Button_init(&settingsReturnBtn, "Return", 14, 16, 8, cbSettingsReturnBtn);

	/* Initialize settings_datetime components */
	gui_Button_init(&dateTimeCancelBtn, "Cancel", 15, 32, 6, cbDateTimeCancelBtn);
	gui_Button_init(&dateTimeOkBtn, "Ok", 15, 26, 4, cbDateTimeOkBtn);
	gui_TextField_init(&secondsTxtField, "", 4, 19, 5, cbDummy);
	gui_TextField_init(&minutesTxtField, "", 6, 19, 5, cbDummy);
	gui_TextField_init(&hoursTxtField, "", 8, 19, 5, cbDummy);
	gui_TextField_init(&dateTxtField, "", 10, 19, 5, cbDummy);
	gui_TextField_init(&monthTxtField, "", 12, 19, 5, cbDummy);
	gui_TextField_init(&yearTxtField, "", 14, 19, 5, cbDummy);

	showMainMenu();
}

/**
 * Display the main menu on the screen.
 */
void showMainMenu(void) {
	currPage = &page_main_menu;
	menuIdx = 0;
	disp_full_clear();

	gui_print_text(timeBuffer, timeRow, timeCol);

	drawFrame();

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

	gui_print_text(timeBuffer, timeRow, timeCol);

	drawFrame();

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
 * Show the settings page on the screen.
 */
void showSettingsPage(void) {
	currPage = &page_settings;
	settingsIdx = 0;
	disp_full_clear();

	gui_print_text(timeBuffer, timeRow, timeCol);

	drawFrame();

	graph_print_textBox("~Settings~", 2, 1, TEXT_ALIGN_CENTER);

	gui_Button_show(&settingsReturnBtn);
	gui_Button_show(&dateTimeBtn);
}

/**
 * Shows the dateTime settings page on the screen.
 */
void showSettingsDateTimePage(void) {
	currPage = &page_settings_datetime;
	dateTimeIdx = 0;
	disp_full_clear();

	gui_print_text(timeBuffer, timeRow, timeCol);

	drawFrame();

	graph_print_textBox("~Set Date/Time~", 2, 1, TEXT_ALIGN_CENTER);

	gui_Button_show(&dateTimeCancelBtn);
	gui_Button_show(&dateTimeOkBtn);
	timestamp_t* time = rtc_get_timestamp();

	graph_print_text("Minutes: ", 6, 9, TEXT_ALIGN_LEFT);
	sprintf(minutesTxtField.text, "%d", time->minutes);
	gui_TextField_show(&minutesTxtField);

	graph_print_text("Hours: ", 8, 9, TEXT_ALIGN_LEFT);
	sprintf(hoursTxtField.text, "%d", time->hours);
	gui_TextField_show(&hoursTxtField);

	graph_print_text("Day: ", 10, 9, TEXT_ALIGN_LEFT);
	sprintf(dateTxtField.text, "%d", time->date);
	gui_TextField_show(&dateTxtField);

	graph_print_text("Month: ", 12, 9, TEXT_ALIGN_LEFT);
	sprintf(monthTxtField.text, "%d", time->month);
	gui_TextField_show(&monthTxtField);

	graph_print_text("Year: ", 14, 9, TEXT_ALIGN_LEFT);
	sprintf(yearTxtField.text, "%d", time->year);
	gui_TextField_show(&yearTxtField);

	graph_print_text("Seconds: ", 4, 9, TEXT_ALIGN_LEFT);
	sprintf(secondsTxtField.text, "%d", time->seconds);
	gui_TextField_show(&secondsTxtField);

	free(time);
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

	 	} else if(currPage->PAGE == PAGE_SETTINGS) {
	 		settingsIdx == 0 ? settingsIdx = 1 : settingsIdx--;
	 		gui_select_component(componentsSetting[settingsIdx]);

	 	} else if(currPage->PAGE == PAGE_SETTINGS_DATETIME) {
	 		dateTimeIdx == 0 ? dateTimeIdx = 7 : dateTimeIdx--;
	 		gui_select_component(componentsDateTime[dateTimeIdx]);
	 	}

		break;
	case KEY_B:

		if(currPage->PAGE == PAGE_MAIN_MENU) {
			menuIdx == 3 ? menuIdx = 0 : menuIdx++;
			gui_select_component(componentsMM[menuIdx]);

		} else if(currPage->PAGE == PAGE_SETTINGS) {
	 		settingsIdx == 1 ? settingsIdx = 0 : settingsIdx++;
	 		gui_select_component(componentsSetting[settingsIdx]);

	 	} else if(currPage->PAGE == PAGE_SETTINGS_DATETIME) {
	 		dateTimeIdx == 7 ? dateTimeIdx = 0 : dateTimeIdx++;
	 		gui_select_component(componentsDateTime[dateTimeIdx]);
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
 * Updates the time field.
 */
void cbTimeUpdate(void) {
	timestamp_t* t = rtc_get_timestamp();
	sprintf(timeBuffer, "%02d-%02d-%02d %02d:%02d:%02d",
			t->year,
			t->month,
			t->date,
			t->hours,
			t->minutes,
			t->seconds
	);
	free(t);

	gui_print_text(timeBuffer, timeRow, timeCol);
}

/**
 * -----------------------------------------------------------------------------------------------------
 * 												CALLBACK FUNCTIONS
 * -----------------------------------------------------------------------------------------------------
 */

/* Dummy callback */
void cbDummy(void) {

}

/* Main menu callbacks ---------------------*/
void cbMenuLiveWeatherDataBtn(void) {
	showPage(&page_live_weather);
}

void cbMenuItem2Btn(void) {

}

void cbMenuItem3Btn(void) {

}

void cbMenuSettingsBtn(void) {
	showPage(&page_settings);
}

/* Live weather data callbacks -------------*/
void cbReturnButton(void) {
	showPage(&page_main_menu);
}

/* Error page callbacks --------------------*/
void cbErrorOkBtn(void) {
	showPage(previousPage);
}

/* Settings page callbacks -----------------*/
void cbDateTimeBtn(void) {
	showPage(&page_settings_datetime);
}

void cbSettingsReturnBtn(void) {
	showPage(&page_main_menu);
}

/* Settings_datetime callback functions ----*/
void cbDateTimeOkBtn(void) {
	volatile timestamp_t time;
	time.seconds = atoi(secondsTxtField.text);
	time.minutes = atoi(minutesTxtField.text);
	time.hours = atoi(hoursTxtField.text);
	time.date = atoi(dateTxtField.text);
	time.month = atoi(monthTxtField.text);
	time.year = atoi(yearTxtField.text);

	if(time.seconds > 59) {
		char* msg[] = {
				"Seconds out of range!",
				"Acceptable range:",
				"00 - 59",
				"Please try again"
		};
		showErrorPage(msg, 4);
		return;

	} else if(time.minutes > 59) {
		char* msg[] = {
				"Minutes out of range!",
				"Acceptable range:",
				"00 - 59",
				"Please try again"
		};
		showErrorPage(msg, 4);
		return;

	} else if(time.hours > 23) {
		char* msg[] = {
				"Hours out of range!",
				"Acceptable range:",
				"00 - 23",
				"Please try again"
		};
		showErrorPage(msg, 4);
		return;

	} else if(time.date < 1 || time.date > 31) {
		char* msg[] = {
				"Day out of range!",
				"Acceptable range:",
				"01 - 31",
				"Please try again"
		};
		showErrorPage(msg, 4);
		return;

	} else if(time.month < 1 || time.month > 12) {
		char* msg[] = {
				"Month out of range!",
				"Acceptable range:",
				"01 - 12",
				"Please try again"
		};
		showErrorPage(msg, 4);
		return;

	} else if(time.year > 99) {
		char* msg[] = {
				"Year out of range!",
				"Acceptable range:",
				"00 - 99",
				"Please try again"
		};
		showErrorPage(msg, 4);
		return;
	}

	rtc_set_time(&time);

	showPage(&page_settings);
}

void cbDateTimeCancelBtn(void) {
	showPage(&page_settings);
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

/**
 * Draw the frame onto the screen.
 */
static void drawFrame(void) {
	graph_draw_line(0, 0, 239, 0, true);
	graph_draw_line(239, 0, 239, 127, true);
	graph_draw_line(239, 127, 103, 127, true);
	graph_draw_line(0, 118, 0, 0, true);
}

