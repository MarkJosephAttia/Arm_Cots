/**
 * @file HUart.h
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the user interface for the uart handler
 * @version 0.1
 * @date 2020-03-29
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef HUART_H
#define HUART_H
#include "HUart_Cfg.h"

#define HUART_MODULE_1       0
#define HUART_MODULE_2       1
#define HUART_MODULE_3       2

#define HUART_ODD_PARITY 0x00000200
#define HUART_EVEN_PARITY 0x00000000
#define HUART_NO_PARITY 0xFFFFFBFF
#define HUART_STOP_ONE_BIT 0x00000000
#define HUART_STOP_TWO_BITS 0x00003000

#define HUART_FLOW_CONTROL_EN 0x00000100
#define HUART_FLOW_CONTROL_DIS 0x00000000

typedef void (*hUartAppNotify_t)(void);

/**
 * @brief Initializes the UART Module
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
extern Std_ReturnType HUart_Init(void);
/**
 * @brief Sets configurations for the UART module
 * *The UART must be initialized after setting configurations to apply the changes
 * 
 * @param baudRate the baud rate of the UART (uint32_t)
 * @param stopBits The number of the stop bits
 *                 @arg HUART_ONE_STOP_BIT
 *                 @arg HUART_TWO_STOP_BITS
 * @param parity The parity of the transmission
 *                 @arg HUART_ODD_PARITY
 *                 @arg HUART_EVEN_PARITY
 *                 @arg HUART_NO_PARITY
 * @param flowControl the flow control
 *                 @arg HUART_FLOW_CONTROL_EN
 *                 @arg HUART_FLOW_CONTROL_DIS
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
extern Std_ReturnType HUart_Config(uint32_t baudRate, uint32_t stopBits, uint32_t parity, uint32_t flowControl);
/**
 * @brief Sets the module that you will be using
 * 
 * @param uartModule The UART module
 *                  @arg HUART_MODULE_1
 *                  @arg HUART_MODULE_2
 *                  @arg HUART_MODULE_3
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
extern Std_ReturnType HUart_SetModule(uint8_t uartModule);
/**
 * @brief Sends data through the UART
 *
 * @param data The data to send
 * @param length the length of the data in bytes
 * @param notify The application notification
 * @return Std_ReturnType A Status
 *                  E_OK: If the driver is ready to send
 *                  E_NOT_OK: If the driver can't send data right now
 */
extern Std_ReturnType HUart_Send(uint8_t *data, uint16_t length, hUartAppNotify_t notify);
/**
 * @brief Receives data through the UART
 *
 * @param data The buffer to receive data in
 * @param length the length of the data in bytes
 * @param notify The application notification
 * @return Std_ReturnType A Status
 *                  E_OK: If the driver is ready to receive
 *                  E_NOT_OK: If the driver can't receive data right now
 */
extern Std_ReturnType HUart_Receive(uint8_t *data, uint16_t length, hUartAppNotify_t notify);

#endif