/**
 * @file Clcd.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This file contains the implementation for the Character LCD Driver
 * @version 0.1
 * @date 2020-03-26
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"
#include "HRcc.h"
#include "Gpio.h"
#include "CLcd.h"
#include "Sched.h"

#define CLCD_INITIALIZED					0
#define CLCD_NOT_INITIALIZED				1

#define CLCD_EMPTY_CMD						0x0
#define CLCD_INIT_CONST						0x3
#define CLCD_FUNC_SET						0x2
#define CLCD_CLEAR_DISP						0x1
#define CLCD_INC							0x6
#define CLCD_DDRAM							0x80
#define CLCD_SECOND_LINE					0x40
#define CLCD_DISP_SETTING					0x8
#define CLCD_CONFIG_DISP_CLR				0xF7

/**
 * @brief The initialization steps that will be executed
 * 
 */
typedef enum 
{
	hardwareInit_s,				/* Wait for 100 ms for the hardware to initialize */
	specialCaseFunctionSet_s,	/* Special commands to initialize the CLCD */
	functionSet_s, 				/* Setup the CLCD for a four bit mode */
	display_s, 					/* Setup the display settings */
	clear_s,					/* Clear the display */
	entry_s						/* Setup the Entry mode */
}clcdInitState_t;

/**
 * @brief The Write steps that will be executed
 * 
 */
typedef enum 
{
	setAddress_s,	/* Set the Address for the data that will be written */
	writeData_s		/* Write the data */
}clcdWriteState_t;

/**
 * @brief The processes of the CLcd
 * 
 */
typedef enum 
{
	init_p,
	write_p,
	clear_p,
	goto_p,
	setup_p,
	idle_p
}clcdProcess_t;

/**
 * @brief The eneble states
 * 
 */
typedef enum 
{
	low_s,
	high_s
}clcdEnable_t;

volatile static clcdProcess_t CLcd_process  = idle_p;
volatile static clcdEnable_t CLcd_enState;

volatile static uint8_t CLcd_str[255];	/* The string buffer */
volatile static uint8_t CLcd_strLen;	/* The string buffer length */
volatile static uint8_t CLcd_x;			/* The string position X */
volatile static uint8_t CLcd_y;			/* The string position Y */

volatile static uint8_t CLcd_numberOfLines;			/* The CLCD number of lines */
volatile static uint8_t CLcd_configDisplay;			/* The CLCD display configurations */

volatile static uint8_t CLcd_isInitialized = CLCD_NOT_INITIALIZED; 	/* The CLCD initialization state */

extern const clcd_t CLcd_clcd;

volatile static lcdCb_t appNotify = NULL;			/* The CLCD Application Notification */

static Std_ReturnType CLcd_WriteData(uint8_t data);
static Std_ReturnType CLcd_WriteCommand(uint8_t cmd);
static Std_ReturnType CLcd_InitProcess(void);
static Std_ReturnType CLcd_ClearProcess(void);
static Std_ReturnType CLcd_SetupProcess(void);
static Std_ReturnType CLcd_WriteProcess(void);
static Std_ReturnType CLcd_GotoProcess(void);
static void CLcd_Task(void);

/**
 * @brief The Character LCD initialization
 * 
 * @param nLines The number of lines on display
 *              @arg CLCD_TWO_LINES : Two lines display
 *              @arg CLCD_ONE_LINE  : One line display
 * @param cursor The State of the cursor (Visible or not)
 *              @arg CLCD_CURSOR_ON
 *              @arg CLCD_CURSOR_OFF
 * @param blink The blinking option (no/off)
 *              @arg CLCD_BLINKING_ON
 *              @arg CLCD_BLINKING_OFF
 * @return Std_ReturnType 
 *              E_OK : If the initialization started successfully
 *              E_NOT_OK : If the initialization is not able to start right now
 */
Std_ReturnType CLcd_Init(uint8_t nLines, uint8_t cursor, uint8_t blink)
{
	gpio_t gpio;
	uint8_t i;
	Std_ReturnType error = E_NOT_OK;
	/* If there were no processes execution at the moment */
	if(idle_p == CLcd_process)
	{
		/* Setup the display configurations */
		CLcd_configDisplay = CLCD_DISP_SETTING | CLCD_DISP_ON | cursor | blink;
		CLcd_isInitialized = CLCD_NOT_INITIALIZED;
		CLcd_numberOfLines = nLines;
		/* Setup the GPIO pins and their clock */
		gpio.mode = GPIO_MODE_GP_OUTPUT_PP;
		gpio.speed = GPIO_SPEED_50_MHZ;
		gpio.pins = CLcd_clcd.enPin;
		gpio.port = CLcd_clcd.enPort;
		HRcc_EnPortClock(CLcd_clcd.enPort);
		Gpio_InitPins(&gpio);
		gpio.pins = CLcd_clcd.rwPin;
		gpio.port = CLcd_clcd.rwPort;
		HRcc_EnPortClock(CLcd_clcd.rwPort);
		Gpio_InitPins(&gpio);
		gpio.pins = CLcd_clcd.rsPin;
		gpio.port = CLcd_clcd.rsPort;
		HRcc_EnPortClock(CLcd_clcd.rsPort);
		Gpio_InitPins(&gpio);
		for(i=0; i<CLCD_NUMBER_OF_DATA_PINS; i++)
		{
			gpio.pins = CLcd_clcd.dPin[i];
			gpio.port = CLcd_clcd.dPort[i];
			HRcc_EnPortClock(CLcd_clcd.dPort[i]);
			Gpio_InitPins(&gpio);
		}
		CLcd_process  = init_p;
		error = E_OK;
	}
	return error;
}
/**
 * @brief Writes a string on a specific location on the lcd display
 * 
 * @param str the string to write
 * @param x the location on the x-axis
 * @param y the location on the y-axis
 * @return Std_ReturnType 
 *              E_OK : If the writing started successfully
 *              E_NOT_OK : If the write operation is not able to start right now
 */
Std_ReturnType CLcd_WriteString(uint8_t* str, uint8_t x, uint8_t y)
{
	uint8_t i=0;
	Std_ReturnType error = E_NOT_OK;
	/* If the CLCD was initialized and there where no current processes executing */
	if(idle_p == CLcd_process && CLCD_INITIALIZED == CLcd_isInitialized)
	{
		/* Copy the string into the buffer */
		while(str[i] && i < 16)
		{
			CLcd_str[i] = str[i];
			i++;
		}
		CLcd_str[i] = '\0';
		CLcd_strLen = i;
		CLcd_x = x;
		CLcd_y = y;

		CLcd_process = write_p;
		error = E_OK;
	}
	return error;
}
/**
 * @brief Clears the display
 * 
 * @return Std_ReturnType 
 *              E_OK : If the clear operation started successfully
 *              E_NOT_OK : If the clear operation is not able to start right now
 */
Std_ReturnType CLcd_ClearDisplay(void)
{
	Std_ReturnType error = E_NOT_OK;
	/* If the CLCD was initialized and there where no current processes executing */
	if(idle_p == CLcd_process && CLCD_INITIALIZED == CLcd_isInitialized)
	{
		CLcd_process = clear_p;
		error = E_OK;
	}
	return error;
}
/**
 * @brief jumps to a specific location on the lcd displey
 * 
 * @param x the location on the x-axis
 * @param y the location on the y-axis
 * @return Std_ReturnType 
 *              E_OK : If the goto operation started successfully
 *              E_NOT_OK : If the goto operation is not able to start right now
 */
Std_ReturnType CLcd_GotoXY(uint8_t x, uint8_t y)
{
	Std_ReturnType error = E_NOT_OK;
	/* If the CLCD was initialized and there where no current processes executing */
	if(idle_p == CLcd_process && CLCD_INITIALIZED == CLcd_isInitialized)
	{
		CLcd_x = x;
		CLcd_y = y;

		CLcd_process = goto_p;
		error = E_OK;
	}
	return error;
}
/**
 * @brief Configures the cursor options
 * 
 * @param cursor The State of the cursor (Visible or not)
 *              @arg CLCD_CURSOR_ON
 *              @arg CLCD_CURSOR_OFF
 * @param blink The blinking option (no/off)
 *              @arg CLCD_BLINKING_ON
 *              @arg CLCD_BLINKING_OFF
 * @return Std_ReturnType 
 *              E_OK : If the configuration started successfully
 *              E_NOT_OK : If the configuaration is not able to start right now
 */
Std_ReturnType CLcd_ConfigCursor(uint8_t cursor, uint8_t blink)
{
	Std_ReturnType error = E_NOT_OK;
	/* If the CLCD was initialized and there where no current processes executing */
	if(idle_p == CLcd_process && CLCD_INITIALIZED == CLcd_isInitialized)
	{
		CLcd_process = setup_p;
		CLcd_configDisplay = CLCD_DISP_SETTING | CLCD_DISP_ON | cursor | blink;
		error = E_OK;
	}
	return error;
}
/**
 * @brief Sets the display on and off
 * 
 * @param disp the display state
 *              @arg CLCD_DISP_ON
 *              @arg CLCD_DISP_OFF
 * @return Std_ReturnType 
 *              E_OK : If the configuration started successfully
 *              E_NOT_OK : If the configuaration is not able to start right now
 */
Std_ReturnType CLcd_ConfigDisplay(uint8_t disp)
{
	Std_ReturnType error = E_NOT_OK;
	/* If the CLCD was initialized and there where no current processes executing */
	if(idle_p == CLcd_process && CLCD_INITIALIZED == CLcd_isInitialized)
	{
		CLcd_process = setup_p;
		CLcd_configDisplay &= CLCD_CONFIG_DISP_CLR;
		CLcd_configDisplay |= disp;
		error = E_OK;
	}
	return error;
}
/**
 * @brief writes 4-bit data into the lcd
 * 
 * @param data the data to write
 * @return Std_ReturnType 
 * 				E_OK : If executed successfully
 * 				E_NOT_OK : If it failed to execute
 */
static Std_ReturnType CLcd_WriteData(uint8_t data)
{
	Gpio_WritePin(CLcd_clcd.rsPort, CLcd_clcd.rsPin, GPIO_PIN_SET);
	Gpio_WritePin(CLcd_clcd.rwPort, CLcd_clcd.rwPin, GPIO_PIN_RESET);
	Gpio_WritePin(CLcd_clcd.dPort[0], CLcd_clcd.dPin[0], !((data>>0)&1));
	Gpio_WritePin(CLcd_clcd.dPort[1], CLcd_clcd.dPin[1], !((data>>1)&1));
	Gpio_WritePin(CLcd_clcd.dPort[2], CLcd_clcd.dPin[2], !((data>>2)&1));
	Gpio_WritePin(CLcd_clcd.dPort[3], CLcd_clcd.dPin[3], !((data>>3)&1));
	return E_OK;
}
/**
 * @brief writes 4-bit command into the lcd
 * 
 * @param cmd the command to write
 * @return Std_ReturnType
 * 				E_OK : If executed successfully
 * 				E_NOT_OK : If it failed to execute
 */
static Std_ReturnType CLcd_WriteCommand(uint8_t cmd)
{
	Gpio_WritePin(CLcd_clcd.rsPort, CLcd_clcd.rsPin, GPIO_PIN_RESET);
	Gpio_WritePin(CLcd_clcd.rwPort, CLcd_clcd.rwPin, GPIO_PIN_RESET);
	Gpio_WritePin(CLcd_clcd.dPort[0], CLcd_clcd.dPin[0], !((cmd>>0)&1));
	Gpio_WritePin(CLcd_clcd.dPort[1], CLcd_clcd.dPin[1], !((cmd>>1)&1));
	Gpio_WritePin(CLcd_clcd.dPort[2], CLcd_clcd.dPin[2], !((cmd>>2)&1));
	Gpio_WritePin(CLcd_clcd.dPort[3], CLcd_clcd.dPin[3], !((cmd>>3)&1));
	return E_OK;
}

/**
 * @brief The initialization process
 * 
 * @return Std_ReturnType 
 * 				E_OK : If executed successfully
 * 				E_NOT_OK : If it failed to execute
 */
static Std_ReturnType CLcd_InitProcess(void)
{
	static uint8_t counter;
	static clcdInitState_t state = hardwareInit_s;
	/* Set Enable pin to low and wait */
	if(CLcd_enState == high_s)
	{
		Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_RESET);
		CLcd_enState = low_s;
	}
	else
	{
		switch(state)
		{
			case hardwareInit_s:
				if(100 == counter++)
				{
					counter = 0;
					state = specialCaseFunctionSet_s;
					CLcd_enState = high_s;
				}			
				break;
			case specialCaseFunctionSet_s:
				switch(counter++)
				{
					case 0:
						CLcd_WriteCommand(CLCD_INIT_CONST);
						Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
						CLcd_enState = high_s;
						break;
					case 5:
						CLcd_WriteCommand(CLCD_INIT_CONST);
						Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
						CLcd_enState = high_s;
						break;
					case 6:
						CLcd_WriteCommand(CLCD_INIT_CONST);
						Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
						CLcd_enState = high_s;
						counter = 0;
						state = functionSet_s;
						break;
				}
				break;
			case functionSet_s:
				switch(counter++)
				{
					case 0:
						CLcd_WriteCommand(CLCD_FUNC_SET);
						Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
						CLcd_enState = high_s;
						break;
					case 1:
						CLcd_WriteCommand(CLCD_FUNC_SET);
						Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
						CLcd_enState = high_s;
						break;
					case 2:
						CLcd_WriteCommand(CLcd_numberOfLines);
						Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
						CLcd_enState = high_s;
						counter = 0;
						state = display_s;
						break;
				}
				break;
			case display_s:
				switch(counter++)
				{
					case 0:
						CLcd_WriteCommand(CLCD_EMPTY_CMD);
						Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
						CLcd_enState = high_s;
						break;
					case 1:
						CLcd_WriteCommand(CLcd_configDisplay);
						Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
						CLcd_enState = high_s;
						counter = 0;
						state = clear_s;
						break;
				}
				break;
			case clear_s:
				switch(counter++)
				{
					case 0:
						CLcd_WriteCommand(CLCD_EMPTY_CMD);
						Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
						CLcd_enState = high_s;
						break;
					case 1:
						CLcd_WriteCommand(CLCD_CLEAR_DISP);	
						Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
						CLcd_enState = high_s;
						break;
					case 3:
						counter = 0;
						state = entry_s;
						break;
				}
				break;
			case entry_s:
				switch(counter++)
				{
					case 0:
						CLcd_WriteCommand(CLCD_EMPTY_CMD);	
						Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
						CLcd_enState = high_s;
						break;
					case 1:
						CLcd_WriteCommand(CLCD_INC);	
						Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
						CLcd_enState = high_s;
						break;
					case 2:
						counter = 0;
						state = hardwareInit_s;
						CLcd_isInitialized = CLCD_INITIALIZED;
						CLcd_process = idle_p;
						if(appNotify)
						{
							appNotify();
						}
						break;
				}
				break;
		}
	}
	return E_OK;
}

/**
 * @brief The Clear process
 * 
 * @return Std_ReturnType 
 * 				E_OK : If executed successfully
 * 				E_NOT_OK : If it failed to execute
 */
static Std_ReturnType CLcd_ClearProcess(void)
{
	static uint8_t counter;
	/* Set Enable pin to low and wait */
	if(CLcd_enState == high_s)
	{
		Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_RESET);
		CLcd_enState = low_s;
	}
	else
	{
		switch(counter++)
		{
			case 0:
				CLcd_WriteCommand(CLCD_EMPTY_CMD);
				Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
				CLcd_enState = high_s;
				break;
			case 1:
				CLcd_WriteCommand(CLCD_CLEAR_DISP);	
				Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
				CLcd_enState = high_s;
				break;
			case 3:
				counter = 0;
				CLcd_process = idle_p;
				if(appNotify)
				{
					appNotify();
				}
				break;
		}
	}
	return E_OK;
}
/**
 * @brief The Setup process
 * 
 * @return Std_ReturnType 
 * 				E_OK : If executed successfully
 * 				E_NOT_OK : If it failed to execute
 */
static Std_ReturnType CLcd_SetupProcess(void)
{
	static uint8_t counter;
	/* Set Enable pin to low and wait */
	if(CLcd_enState == high_s)
	{
		Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_RESET);
		CLcd_enState = low_s;
	}
	else
	{
		switch(counter++)
		{
			case 0:
				CLcd_WriteCommand(CLCD_EMPTY_CMD);
				Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
				CLcd_enState = high_s;
				break;
			case 1:
				CLcd_WriteCommand(CLcd_configDisplay);	
				Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
				CLcd_enState = high_s;
				break;
			case 2:
				counter = 0;
				CLcd_process = idle_p;
				if(appNotify)
				{
					appNotify();
				}
				break;
		}
	}
	return E_OK;
}
/**
 * @brief The Write process
 * 
 * @return Std_ReturnType 
 * 				E_OK : If executed successfully
 * 				E_NOT_OK : If it failed to execute
 */
static Std_ReturnType CLcd_WriteProcess(void)
{
	static uint8_t i = 0;
	static uint8_t counter;
	static clcdWriteState_t state = setAddress_s;
	/* Calculating the address in the DDRAM */
	uint8_t address = CLCD_DDRAM;
	if(CLcd_y==1)
	{
		address |= CLCD_SECOND_LINE;
	}
	address += CLcd_x;
	/* Set Enable pin to low and wait */
	if(CLcd_enState == high_s)
	{
		Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_RESET);
		CLcd_enState = low_s;
	}
	else
	{
		switch(state)
		{
			case setAddress_s:
				switch(counter++)
				{
					case 0:
						CLcd_WriteCommand(address>>4);
						Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
						CLcd_enState = high_s;
						break;
					case 1:
						CLcd_WriteCommand(address);
						Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
						CLcd_enState = high_s;
						counter = 0;
						state = writeData_s;
						break;
				}
				break;
			case writeData_s:
				if(i==CLcd_strLen)
				{
					state = setAddress_s;
					i=0;
					CLcd_process = idle_p;
					if(appNotify)
					{
						appNotify();
					}

				}
				else
				{
					switch(counter++)
					{
						case 0:
							CLcd_WriteData(CLcd_str[i]>>4);
							Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
							CLcd_enState = high_s;
							break;
						case 1:
							CLcd_WriteData(CLcd_str[i]);
							Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
							CLcd_enState = high_s;
							counter = 0;
							i++;
							break;
					}
				}
				break;
		}
	}
	return E_OK;
}
/**
 * @brief The goto process
 * 
 * @return Std_ReturnType 
 * 				E_OK : If executed successfully
 * 				E_NOT_OK : If it failed to execute
 */
static Std_ReturnType CLcd_GotoProcess(void)
{
	static uint8_t counter;
	static clcdWriteState_t state = setAddress_s;
	/* Calculating the address in the DDRAM */
	uint8_t address = CLCD_DDRAM;
	if(CLcd_y==1)
	{
		address |= CLCD_SECOND_LINE;
	}
	address += CLcd_x;
	/* Set Enable pin to low and wait */
	if(CLcd_enState == high_s)
	{
		Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_RESET);
		CLcd_enState = low_s;
	}
	else
	{
		switch(counter++)
		{
			case 0:
				CLcd_WriteCommand(address>>4);
				Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
				CLcd_enState = high_s;
				break;
			case 1:
				CLcd_WriteCommand(address);
				Gpio_WritePin(CLcd_clcd.enPort, CLcd_clcd.enPin, GPIO_PIN_SET);
				CLcd_enState = high_s;
				break;
			case 2:
				counter = 0;
				CLcd_process = idle_p;
				if(appNotify)
				{
					appNotify();
				}
				break;
		}
	}
	return E_OK;
}
/**
 * @brief Sets the callback function executed when done
 * 
 * @param cb the callback function
 * @return Std_ReturnType 
 */
Std_ReturnType CLcd_SetDoneNotification(lcdCb_t cb)
{
	appNotify = cb;
	return E_OK;
}

/**
 * @brief The running task that have to come every 1 milli second
 * 
 */
static void CLcd_Task(void)
{
	switch(CLcd_process)
	{
		case idle_p:
			break;
		case init_p:
			CLcd_InitProcess();
			break;
		case write_p:
			CLcd_WriteProcess();
			break;
		case clear_p:
			CLcd_ClearProcess();
			break;
		case goto_p:
			CLcd_GotoProcess();
			break;
		case setup_p:
			CLcd_SetupProcess();
			break;
	}
}

const task_t CLcd_task = {CLcd_Task, 1};