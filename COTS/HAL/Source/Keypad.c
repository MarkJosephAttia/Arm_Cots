/**
 * @file Keypad.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the implementation for the keypad driver
 * @version 0.1
 * @date 2020-03-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"
#include "HRcc.h"
#include "Gpio.h"
#include "Keypad.h"

extern const keypad_t Keypad_keypad;

/**
 * @brief The initialization of the GPIOs used in the keypad
 * 
 * @return Std_ReturnType A status of wether the function worked correctly or not 
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
Std_ReturnType Keypad_Init(void)
{
    uint8_t i;
    gpio_t gpio;
	gpio.mode = GPIO_MODE_GP_OUTPUT_PP;
    gpio.speed = GPIO_SPEED_50_MHZ;
    for(i=0; i<4; i++)
    {
        gpio.pins = Keypad_keypad.outputPin[i];
        gpio.port = Keypad_keypad.outputPort[i];
        HRcc_EnPortClock(Keypad_keypad.outputPort[i]);
        Gpio_InitPins(&gpio);
    }
    gpio.mode = GPIO_MODE_INPUT_PULL_UP;
    for(i=0; i<3; i++)
    {
        gpio.pins = Keypad_keypad.inputPin[i];
        gpio.port = Keypad_keypad.inputPort[i];
        HRcc_EnPortClock(Keypad_keypad.outputPort[i]);
        Gpio_InitPins(&gpio);
    }
}

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
Std_ReturnType Keypad_ReadValue(sint8_t* value)
{
    uint8_t tmp,garb,errorRet = E_NOT_OK;
    Gpio_WritePin(Keypad_keypad.outputPort[0], Keypad_keypad.outputPin[0], GPIO_PIN_SET);
    Gpio_WritePin(Keypad_keypad.outputPort[1], Keypad_keypad.outputPin[1], GPIO_PIN_RESET);
    Gpio_WritePin(Keypad_keypad.outputPort[2], Keypad_keypad.outputPin[2], GPIO_PIN_RESET);
    Gpio_WritePin(Keypad_keypad.outputPort[3], Keypad_keypad.outputPin[3], GPIO_PIN_RESET);
    
    garb = Gpio_ReadPin(Keypad_keypad.inputPort[0], Keypad_keypad.inputPin[0], &tmp);
    if(tmp == GPIO_PIN_SET)
    {
        *value = KEYPAD_KEY_ONE;
        errorRet = E_OK;
    }
    garb = Gpio_ReadPin(Keypad_keypad.inputPort[1], Keypad_keypad.inputPin[1], &tmp);
    if(tmp == GPIO_PIN_SET)
    {
        *value = KEYPAD_KEY_TWO;
        errorRet = E_OK;
    }
    garb = Gpio_ReadPin(Keypad_keypad.inputPort[2], Keypad_keypad.inputPin[2], &tmp);
    if(tmp == GPIO_PIN_SET)
    {
        *value = KEYPAD_KEY_THREE;
        errorRet = E_OK;
    }

    Gpio_WritePin(Keypad_keypad.outputPort[0], Keypad_keypad.outputPin[0], GPIO_PIN_RESET);
    Gpio_WritePin(Keypad_keypad.outputPort[1], Keypad_keypad.outputPin[1], GPIO_PIN_SET);
    Gpio_WritePin(Keypad_keypad.outputPort[2], Keypad_keypad.outputPin[2], GPIO_PIN_RESET);
    Gpio_WritePin(Keypad_keypad.outputPort[3], Keypad_keypad.outputPin[3], GPIO_PIN_RESET);
    
    garb = Gpio_ReadPin(Keypad_keypad.inputPort[0], Keypad_keypad.inputPin[0], &tmp);
    if(tmp == GPIO_PIN_SET)
    {
        *value = KEYPAD_KEY_FOUR;
        errorRet = E_OK;
    }
    garb = Gpio_ReadPin(Keypad_keypad.inputPort[1], Keypad_keypad.inputPin[1], &tmp);
    if(tmp == GPIO_PIN_SET)
    {
        *value = KEYPAD_KEY_FIVE;
        errorRet = E_OK;
    }
    garb = Gpio_ReadPin(Keypad_keypad.inputPort[2], Keypad_keypad.inputPin[2], &tmp);
    if(tmp == GPIO_PIN_SET)
    {
        *value = KEYPAD_KEY_SIX;
        errorRet = E_OK;
    }

    Gpio_WritePin(Keypad_keypad.outputPort[0], Keypad_keypad.outputPin[0], GPIO_PIN_RESET);
    Gpio_WritePin(Keypad_keypad.outputPort[1], Keypad_keypad.outputPin[1], GPIO_PIN_RESET);
    Gpio_WritePin(Keypad_keypad.outputPort[2], Keypad_keypad.outputPin[2], GPIO_PIN_SET);
    Gpio_WritePin(Keypad_keypad.outputPort[3], Keypad_keypad.outputPin[3], GPIO_PIN_RESET);
    
    garb = Gpio_ReadPin(Keypad_keypad.inputPort[0], Keypad_keypad.inputPin[0], &tmp);
    if(tmp == GPIO_PIN_SET)
    {
        *value = KEYPAD_KEY_SEVEN;
        errorRet = E_OK;
    }
    garb = Gpio_ReadPin(Keypad_keypad.inputPort[1], Keypad_keypad.inputPin[1], &tmp);
    if(tmp == GPIO_PIN_SET)
    {
        *value = KEYPAD_KEY_EIGHT;
        errorRet = E_OK;
    }
    garb = Gpio_ReadPin(Keypad_keypad.inputPort[2], Keypad_keypad.inputPin[2], &tmp);
    if(tmp == GPIO_PIN_SET)
    {
        *value = KEYPAD_KEY_NINE;
        errorRet = E_OK;
    }

    Gpio_WritePin(Keypad_keypad.outputPort[0], Keypad_keypad.outputPin[0], GPIO_PIN_RESET);
    Gpio_WritePin(Keypad_keypad.outputPort[1], Keypad_keypad.outputPin[1], GPIO_PIN_RESET);
    Gpio_WritePin(Keypad_keypad.outputPort[2], Keypad_keypad.outputPin[2], GPIO_PIN_RESET);
    Gpio_WritePin(Keypad_keypad.outputPort[3], Keypad_keypad.outputPin[3], GPIO_PIN_SET);
    
    garb = Gpio_ReadPin(Keypad_keypad.inputPort[0], Keypad_keypad.inputPin[0], &tmp);
    if(tmp == GPIO_PIN_SET)
    {
        *value = KEYPAD_KEY_AST;
        errorRet = E_OK;
    }
    garb = Gpio_ReadPin(Keypad_keypad.inputPort[1], Keypad_keypad.inputPin[1], &tmp);
    if(tmp == GPIO_PIN_SET)
    {
        *value = KEYPAD_KEY_ZERO;
        errorRet = E_OK;
    }
    garb = Gpio_ReadPin(Keypad_keypad.inputPort[2], Keypad_keypad.inputPin[2], &tmp);
    if(tmp == GPIO_PIN_SET)
    {
        *value = KEYPAD_KEY_HASH;
        errorRet = E_OK;
    }
    *value = KEYPAD_KEY_NONE;
    errorRet = E_OK;
    return errorRet;
}
