/**
 * @file Nokia_Cfg.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief These are the Nokia Lcd Configurations
 * @version 0.1
 * @date 2020-04-23
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "Std_Types.h"
#include "Gpio.h"
#include "Spi.h"
#include "Nokia.h"

const nokia_t Nokia_nokia = {
            .rstPin = GPIO_PIN_3,
            .rstPort = GPIO_PORTA,
            .cePin = GPIO_PIN_2,
            .cePort = GPIO_PORTA,
            .dcPin = GPIO_PIN_1,
            .dcPort = GPIO_PORTA,
            .spiModule = SPI1
};