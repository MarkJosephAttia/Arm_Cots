/**
 * @file  ByteWriter_Cfg.c
 * @brief This file is to be used as an implementation of the configurations the user configured in the ByteWriter_Cfg.h.
 *
 * @author Mark Attia
 * @date January 22, 2020
 *
 */
#include "Std_Types.h"
#include "Gpio.h"
#include "ByteWriter_Cfg.h"
#include "ByteWriter.h"

const byte_t ByteWriter_bytes = {
    .bitPin = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7},
    .bitPort = {GPIO_PORTA, GPIO_PORTA, GPIO_PORTA, GPIO_PORTA, GPIO_PORTA, GPIO_PORTA, GPIO_PORTA, GPIO_PORTA},
    .bitActiveState = {GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET}
};
