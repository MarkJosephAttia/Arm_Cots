/**
 * @file Lin.h
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the user interface for the Lin driver
 * @version 0.1
 * @date 2020-03-26
 *
 * @copyright Copyright (c) 2020
 *
 */
#ifndef LIN_H_
#define LIN_H_


#define LIN_UART_MODULE_1       0
#define LIN_UART_MODULE_2       1
#define LIN_UART_MODULE_3       2

#define LIN_MASTER              0
#define LIN_SLAVE               1

#define LIN_SEND                0
#define LIN_RECEIVE             1
#define LIN_NONE                2

#define LIN_MAX_DATA_SIZE       8


typedef struct
{
    uint8_t  id;                        /* The ID of the frame */
    uint8_t  size;                       /* A Number From 1 to 8 */
    uint8_t  direction;                  /* LIN_SEND / LIN_RECIEVE */
    uint16_t  ticks;                      /* The Number Of Ticks For This Main Slot */
}Lin_msg_t;


/**
 * @brief Initializes the Lin
 *
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
extern Std_ReturnType Lin_Init(void);

/**
 * @brief Sets The Buffer For A Certain Message
 * 
 * @param buffer A pointer to the buffer 
 * @param msgID The Lin Message ID You Want To Set Buffer For
 * @return Std_ReturnType 
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
extern Std_ReturnType Lin_SetMsgBuffer(uint8_t* buffer, uint8_t msgID);

#endif
