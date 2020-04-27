/**
 * @file Nokia.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the user interface for the Nokia 5110 LCD driver
 * @version 0.1
 * @date 2020-04-22
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef NOKIA_H_
#define NOKIA_H_

typedef struct
{
    uint32_t rstPin;
    uint32_t rstPort;
    uint32_t cePin;
    uint32_t cePort;
    uint32_t dcPin;
    uint32_t dcPort;
    uint8_t spiModule;
}nokia_t;

typedef void (*nokiaCb_t)(void);

/**
 * @brief The Nokia LCD initialization
 * 
 * @return Std_ReturnType 
 *              E_OK : If the initialization started successfully
 *              E_NOT_OK : If the initialization is not able to start right now
 */
extern Std_ReturnType Nokia_Init(void);

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
extern Std_ReturnType Nokia_WriteString(uint8_t* str, uint16_t size, uint16_t x, uint16_t y);

/**
 * @brief Sets the application notify callback function
 * 
 * @param func The function to set
 * @return Std_ReturnType 
 *              E_OK : If the function was executed successfully
 *              E_NOT_OK : If the function was not executed successfully
 */
extern Std_ReturnType Nokia_SetAppNotify(nokiaCb_t func);

#endif