/**
 * @file SSeg_Cfg.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the implementation for the seven segment driver configurations
 * @version 0.1
 * @date 2020-03-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"
#include "Gpio.h"
#include "SSeg_Cfg.h"
#include "SSeg.h"

const sseg_t SSeg_sseg = {
    .dPin = { GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6},
    .dPort = { GPIO_PORTA, GPIO_PORTA, GPIO_PORTA, GPIO_PORTA, GPIO_PORTA, GPIO_PORTA, GPIO_PORTA},
    .enPin = { GPIO_PIN_13, GPIO_PIN_14},
    .enPort = { GPIO_PORTC, GPIO_PORTC},
    .common = {'A','A'}
};
