/**
 * @file Keypad_Cfg.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the implementation for the keypad driver configurations
 * @version 0.1
 * @date 2020-03-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"
#include "Gpio.h"
#include "Keypad_Cfg.h"
#include "Keypad.h"

const keypad_t Keypad_keypad = {
    .outputPin = { GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11},
    .inputPin = { GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15},
    .outputPort = { GPIO_PORTA, GPIO_PORTA, GPIO_PORTA, GPIO_PORTA},
    .inputPort = { GPIO_PORTC, GPIO_PORTC, GPIO_PORTC}
};
