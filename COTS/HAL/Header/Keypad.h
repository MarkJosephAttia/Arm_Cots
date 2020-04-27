/**
 * @file Keypad_Cfg.h
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the user interface for the keypad driver
 * @version 0.1
 * @date 2020-03-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef	KEYPAD_H
#define	KEYPAD_H
#include "Keypad_Cfg.h"

/**
 * @brief The keypad configuration pins
 * 
 */
typedef struct
{
    uint32_t outputPin[Keypad_NUMBER_OF_ROWS];      /* The output pins */
    uint32_t inputPin[Keypad_NUMBER_OF_COLUMNS];    /* The input pins */
    uint32_t outputPort[Keypad_NUMBER_OF_ROWS];     /* The output ports */
    uint32_t inputPort[Keypad_NUMBER_OF_COLUMNS];   /* The input ports */
} keypad_t;

#define KEYPAD_KEY_ZERO                 (0)
#define KEYPAD_KEY_ONE                  (1)
#define KEYPAD_KEY_TWO                  (2)
#define KEYPAD_KEY_THREE                (3)
#define KEYPAD_KEY_FOUR                 (4)
#define KEYPAD_KEY_FIVE                 (5)
#define KEYPAD_KEY_SIX                  (6)
#define KEYPAD_KEY_SEVEN                (7)
#define KEYPAD_KEY_EIGHT                (8)
#define KEYPAD_KEY_NINE                 (9)
#define KEYPAD_KEY_AST                  (10)
#define KEYPAD_KEY_HASH                 (11)
#define KEYPAD_KEY_NONE                 (-1)



/**
 * @brief The initialization of the GPIOs used in the keypad
 * 
 * @return Std_ReturnType A status of wether the function worked correctly or not 
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType Keypad_Init(void);

/**
 * @brief Reads the Value of the pressed key in the keypad
 * 
 * @param value The value of the key pressed
 *                      KEYPAD_KEY_ZERO 
 *                      KEYPAD_KEY_ONE  
 *                      KEYPAD_KEY_TWO  
 *                      KEYPAD_KEY_THREE
 *                      KEYPAD_KEY_FOUR 
 *                      KEYPAD_KEY_FIVE 
 *                      KEYPAD_KEY_SIX  
 *                      KEYPAD_KEY_SEVEN
 *                      KEYPAD_KEY_EIGHT
 *                      KEYPAD_KEY_NINE 
 *                      KEYPAD_KEY_AST  
 *                      KEYPAD_KEY_HASH 
 *                      KEYPAD_KEY_NONE 
 * @return Std_ReturnType A status of wether the function worked correctly or not 
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType Keypad_ReadValue(sint8_t* value);

#endif
