/**
 * @file Uart.h
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the user interface for the UART driver
 * @version 0.1
 * @date 2020-03-26
 *
 * @copyright Copyright (c) 2020
 *
 */
#ifndef UART_H
#define UART_H

#define UART1       0
#define UART2       1
#define UART3       2

#define UART_ODD_PARITY 0x00000200
#define UART_EVEN_PARITY 0x00000000
#define UART_NO_PARITY 0xFFFFFBFF
#define UART_STOP_ONE_BIT 0x00000000
#define UART_STOP_TWO_BITS 0x00003000

#define UART_FLOW_CONTROL_EN 0x00000100
#define UART_FLOW_CONTROL_DIS 0x00000000

typedef void (*txCb_t)(uint8_t);
typedef void (*rxCb_t)(uint8_t);

/**
 * @brief Initializes the UART
 *
 * @param baudRate the baud rate of the UART (uint32_t)
 * @param stopBits The number of the stop bits
 *                 @arg UART_ONE_STOP_BIT
 *                 @arg UART_TWO_STOP_BITS
 * @param parity The parity of the transmission
 *                 @arg UART_ODD_PARITY
 *                 @arg UART_EVEN_PARITY
 *                 @arg UART_NO_PARITY
 * @param flowControl the flow control
 *                 @arg UART_FLOW_CONTROL_EN
 *                 @arg UART_FLOW_CONTROL_DIS
 * @param sysClk the clock of the system
 * @param uartModule the module number of the UART
 *                 @arg UART1
 *                 @arg UART2
 *                 @arg UART3
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
extern Std_ReturnType Uart_Init(uint32_t baudRate, uint32_t stopBits,
                                uint32_t parity, uint32_t flowControl, uint32_t sysClk, uint8_t uartModule);
/**
 * @brief Sends data through the UART
 *
 * @param data The data to send
 * @param length the length of the data in bytes
 * @param uartModule the module number of the UART
 *                 @arg UART1
 *                 @arg UART2
 *                 @arg UART3
 * @return Std_ReturnType A Status
 *                  E_OK: If the driver is ready to send
 *                  E_NOT_OK: If the driver can't send data right now
 */
extern Std_ReturnType Uart_Send(uint8_t *data, uint16_t length, uint8_t uartModule);
/**
 * @brief Receives data through the UART
 *
 * @param data The buffer to receive data in
 * @param length the length of the data in bytes
 * @param uartModule the module number of the UART
 *                 @arg UART1
 *                 @arg UART2
 *                 @arg UART3
 * @return Std_ReturnType A Status
 *                  E_OK: If the driver is ready to receive
 *                  E_NOT_OK: If the driver can't receive data right now
 */
extern Std_ReturnType Uart_Receive(uint8_t *data, uint16_t length, uint8_t uartModule);
/**
 * @brief Sets the callback function that will be called when transmission is
 * completed
 *
 * @param func the callback function
 * @param uartModule the module number of the UART
 *                 @arg UART1
 *                 @arg UART2
 *                 @arg UART3
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
extern Std_ReturnType Uart_SetTxCb(txCb_t func, uint8_t uartModule);
/**
 * @brief Sets the callback function that will be called when receive is
 * completed
 *
 * @param func the callback function
 * @param uartModule the module number of the UART
 *                 @arg UART1
 *                 @arg UART2
 *                 @arg UART3
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
extern Std_ReturnType Uart_SetRxCb(rxCb_t func, uint8_t uartModule);

#endif
