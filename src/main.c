/**
 * --------------------------------------------------------------------------------------------------
 * 										INCLUDES
 * --------------------------------------------------------------------------------------------------
 */

#include <main.h>

/**
 * --------------------------------------------------------------------------------------------------
 * 										VARIABLES
 * --------------------------------------------------------------------------------------------------
 */

/* The function queue used for scheduling */
bool fnQPending = false;					/* The state of the queue */
#define fnQLength		50					/* The length of the queue */
void* fnQ[fnQLength];						/* The function queue */
static uint32_t QSize = 0;					/* The current size of the queue */
static uint32_t QFront = 0, QBack = -1;		/* The back and front of the queue */
#define Qinc(a) a >= fnQLength - 1 ? a = 0 : a++ ;
#define fnQOffer(a) {Qinc(QBack); fnQ[QBack] = a; fnQPending = true; QSize++;}

/* Timebase variable ---------------*/
static uint32_t ticks = 0;

/* Extern variables ----------------*/
extern bool kpadFlag;
extern uint32_t kpadRow;
extern uint32_t kpadCol;
extern uint32_t selectedCol;
extern uint32_t pressureAcqInterval;
extern uint32_t temperatureAcqInterval;

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
void lcd_init(void);
extern void gui_init(void);
extern void cbKeyPress(void);
extern void cbTempSensor(void);
extern void cbBarometer(void);
extern void cbTimeUpdate(void);

void delay_ms(uint32_t t_ms);
uint32_t getTicks(void);

/**
 * --------------------------------------------------------------------------------------------------
 * 										COMPILER STUFF
 * --------------------------------------------------------------------------------------------------
 */

/**
 * Sample pragmas to cope with warnings. Please note the related line at
 * the end of this function, used to pop the compiler diagnostics status.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Wconversion"

/**
 * --------------------------------------------------------------------------------------------------
 * 										MAIN
 * --------------------------------------------------------------------------------------------------
 */

/**
 * Normally at this stage most of the microcontroller subsystems, including
 * the clock, were initialised by the CMSIS SystemInit() function invoked
 * from the startup file, before calling main().
 * (see system/src/cortexm/_initialize_hardware.c)
 */
int main(int argc, char* argv[]) {

	// Disable watchdog timer
	WDT->WDT_MR = WDT_MR_WDDIS;

	//Print simple message through SWD debugger.
#ifdef DEBUG
	trace_puts("DEBUG is running...\n");
#endif
	/* initialize 1ms timebase */
	SysTick_Config(SystemCoreClock/1000);

	/* Initialize hardware and software stack*/
	kpad_init();
	lcd_init();
	gui_init();
	temp_init();
	MPL_init();
	suntracker_init();
	suntracker_start();
	rtc_init();

	while (1) {
		if(QSize > 0) {
			void (*exec)(void) = fnQ[QFront];
			Qinc(QFront);
			QSize--;
			exec();
		}
	}

	/* Should never be reached */
	return EXIT_SUCCESS;
}

/**
 * Initializes the lcd-display.
 */
void lcd_init(void) {
	disp_init();
	disp_wr_cmd(CURS_ON_BLINK_ON | TEXT_ON_GRAPHIC_ON);
	disp_wr_cmd(CURSOR_8_LINE);
	disp_wr_hword(CURSOR_AUTOMOVE, 0x0001);
	disp_wr_hword(SET_ADDRESS_PIONTER, FRAME_TEXT_1);
	disp_set_frame(FRAME_1);
	disp_full_clear();
}

/**
 * returns the current ticks value.
 */
uint32_t getTicks(void) {
	return ticks;
}

/**
 * Delays execution for the specified amount of ms.
 */
void delay_ms(uint32_t t_ms) {
	volatile uint32_t t0 = ticks;
	while(t_ms > getTicks() - t0) {
		asm("NOP");
	};
}

/**
 * --------------------------------------------------------------------------------------------------
 * 										INTERRUPT HANDLERS
 * --------------------------------------------------------------------------------------------------
 */

/**
 * This function enters once each ms.
 */
void SysTick_Handler(void) {
	ticks++;

	if(ticks % temperatureAcqInterval == 0) {
		/* trigger a temerature reading */
		temp_trig();
	}

	if(ticks % pressureAcqInterval == 0) {
		/* Add callback routine to function queue */
		fnQOffer(cbBarometer);
	}

	if(ticks % SUNTRACKER_DT == 0) {
		/* Add suntracker callback routine to function queue */
		fnQOffer(cbSuntracker);
	}

	if(ticks % 1000 == 0) {
		/* Add time update callback routine to function queue */
		fnQOffer(cbTimeUpdate);
	}

}

/**
 * Interrupts when a temperature has been measured.
 */
void TC0_Handler(void) {
	/* Clear flags */
	TC0->TC_CHANNEL[0].TC_SR;

	/* Add callback routine to function queue */
	fnQOffer(cbTempSensor);
}

/**
 * Handles presses on the keypad.
 */
void PIOD_Handler(void) {
	kpadRow = PIOD->PIO_ISR & KPAD_ROW_MSK;

	/* if keypad was pressed */
	if (!kpadFlag) {
		kpadFlag = true;
		KPAD_PORT->PIO_IFDR = KPAD_ROW_MSK; 	/* Disable debounce */

		/* Add callback-routine to function queue */
		fnQOffer(cbKeyPress);

	} else {
		kpadCol = selectedCol;
		KPAD_PORT->PIO_IFER = KPAD_ROW_MSK; 	/* Enable debounce */
	}
}

#pragma GCC diagnostic pop

/**
 * --------------------------------------------------------------------------------------------------
 * 										EOF
 * --------------------------------------------------------------------------------------------------
 */
