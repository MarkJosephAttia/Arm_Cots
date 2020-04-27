/**
 * @file Clcd.h
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This file is the user interface for the Character LCD Driver
 * @version 0.1
 * @date 2020-03-26
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef	CLCD_H
#define	CLCD_H

#define CLCD_NUMBER_OF_DATA_PINS                4

#define CLCD_TWO_LINES						    0x8
#define CLCD_ONE_LINE						    0x0
#define CLCD_DISP_ON						    0x4
#define CLCD_DISP_OFF						    0x0
#define CLCD_CURSOR_ON						    0x2
#define CLCD_CURSOR_OFF						    0x0
#define CLCD_BLINKING_ON						0x1
#define CLCD_BLINKING_OFF						0x0

typedef void (*lcdCb_t)(void);
/**
 * @brief The CLcd Configuration Pins
 * 
 */
typedef struct
{
    uint32_t enPin;         /* The Enable pin */
    uint32_t enPort;        /* The Enable port */
    uint32_t rwPin;         /* The Read/Write pin */
    uint32_t rwPort;        /* The Read/Write port */
    uint32_t rsPin;         /* The RS pin */
    uint32_t rsPort;        /* The RS port */
    uint32_t dPin[CLCD_NUMBER_OF_DATA_PINS];        /* The Data pins */
    uint32_t dPort[CLCD_NUMBER_OF_DATA_PINS];       /*The Data Ports */
} clcd_t;

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
extern Std_ReturnType CLcd_Init(uint8_t nLines, uint8_t cursor, uint8_t blink);
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
extern Std_ReturnType CLcd_WriteString(uint8_t* str, uint8_t x, uint8_t y);
/**
 * @brief Clears the display
 * 
 * @return Std_ReturnType 
 *              E_OK : If the clear operation started successfully
 *              E_NOT_OK : If the clear operation is not able to start right now
 */
extern Std_ReturnType CLcd_ClearDisplay(void);
/**
 * @brief jumps to a specific location on the lcd displey
 * 
 * @param x the location on the x-axis
 * @param y the location on the y-axis
 * @return Std_ReturnType 
 *              E_OK : If the goto operation started successfully
 *              E_NOT_OK : If the goto operation is not able to start right now
 */
extern Std_ReturnType CLcd_GotoXY(uint8_t x, uint8_t y);
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
extern Std_ReturnType CLcd_ConfigCursor(uint8_t cursor, uint8_t blink);
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
extern Std_ReturnType CLcd_ConfigDisplay(uint8_t disp);
/**
 * @brief Sets the callback function executed when done
 * 
 * @param cb the callback function
 * @return Std_ReturnType 
 */
extern Std_ReturnType CLcd_SetDoneNotification(lcdCb_t cb);

#endif
