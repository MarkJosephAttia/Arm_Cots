/**
 * @file SSeg.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the implementation for the seven segment driver
 * @version 0.1
 * @date 2020-03-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"
#include "HRcc.h"
#include "Gpio.h"
#include "SSeg.h"

/**
 * @brief Array of values for the numbers in the cathode Seven Segment
 * 
 */
const char numsC[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
/**
 * @brief Array of values for the numbers in the anode Seven Segment
 * 
 */
const char numsA[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

/**
 * @brief Array of values for the cyclic data in the anode Seven Segment
 * 
 */
const char numsRot[6] = {1,2,4,8,16,32};

extern const sseg_t SSeg_sseg;
/**
 * @brief The initialization function for the Seven segment
 * 
 */
void SSeg_Init(void)
{
    uint8_t i;
    gpio_t gpio;
	gpio.mode = GPIO_MODE_GP_OUTPUT_PP;
    gpio.speed = GPIO_SPEED_50_MHZ;
    for(i=0; i<SSEG_NUMBER_OF_PINS; i++)
    {
        gpio.pins = SSeg_sseg.dPin[i];
        gpio.port = SSeg_sseg.dPort[i];
        HRcc_EnPortClock(SSeg_sseg.dPort[i]);
        Gpio_InitPins(&gpio);
    }
    for(i=0; i<SSEG_NUMBER_OF_SSEGS; i++)
    {
        gpio.pins = SSeg_sseg.enPin[i];
        gpio.port = SSeg_sseg.enPort[i];
        HRcc_EnPortClock(SSeg_sseg.enPort[i]);
        Gpio_InitPins(&gpio);
    }
}
/**
 * @brief Seven Segment Set on
 * 
 * @param num The number of the seven segment
 */
void SSeg_SetOn(u8 num)
{
    if(SSeg_sseg.common[num] == 'A')
    {
        Gpio_WritePin(SSeg_sseg.enPort[num] , SSeg_sseg.enPin[num], GPIO_PIN_SET);
    }
    else if(SSeg_sseg.common[num] == 'C')
    {
        Gpio_WritePin(SSeg_sseg.enPort[num] , SSeg_sseg.enPin[num], GPIO_PIN_RESET);
    }
}
/**
 * @brief Seven Segment Set off
 * 
 * @param num The number of the seven segment
 */
void SSeg_SetOff(u8 num)
{
    if(SSeg_sseg.common[num] == 'A')
    {
        Gpio_WritePin(SSeg_sseg.enPort[num] , SSeg_sseg.enPin[num], GPIO_PIN_RESET);
    }
    else if(SSeg_sseg.common[num] == 'C')
    {
        Gpio_WritePin(SSeg_sseg.enPort[num] , SSeg_sseg.enPin[num], GPIO_PIN_SET);
    }
}
/**
 * @brief Sets a digit on a specific seven segment
 * 
 * @param num the number of the seven segment
 * @param digit the digit you want to set
 */
void SSeg_SetNum(u8 num, u8 digit)
{
    uint8_t i;
    if(SSeg_sseg.common[num] == 'A')
    {
        for(i=0; i<SSEG_NUMBER_OF_PINS; i++)
        {
           Gpio_WritePin(SSeg_sseg.dPort[i] , SSeg_sseg.dPin[i], !((numsA[digit]>>i)&1)); 
        }
    }
    else if(SSeg_sseg.common[num] == 'C')
    {
        for(i=0; i<SSEG_NUMBER_OF_PINS; i++)
        {
           Gpio_WritePin(SSeg_sseg.dPort[i] , SSeg_sseg.dPin[i], !((numsC[digit]>>i)&1)); 
        }
    }
}
/**
 * @brief Sets the dashes on a spevific seven segment
 * 
 * @param num the dashing number
 */
void SSeg_SetDash(u8 num)
{
    uint8_t i;
    for(i=0; i<SSEG_NUMBER_OF_PINS; i++)
    {
        Gpio_WritePin(SSeg_sseg.dPort[i] , SSeg_sseg.dPin[i], !((numsRot[num]>>i)&1)); 
    }
}
