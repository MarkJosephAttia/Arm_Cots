/**
 * @file  Switch.c
 * @brief This file is to be used as an implementation for the Byte Writer.
 *
 * @author Mark Attia
 * @date January 22, 2020
 *
 */
#include "Std_Types.h"
#include "HRcc.h"
#include "Gpio.h"
#include "ByteWriter_Cfg.h"
#include "ByteWriter.h"

extern const byte_t ByteWriter_bytes;

/*
 * Function:  Byte_Init 
 * --------------------
 *  Initializes GPIOs for the Byte Writer
 * 
 *  returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
Std_ReturnType ByteWriter_Init(void)
{
    uint8_t i;
    gpio_t gpio;
	gpio.mode = GPIO_MODE_GP_OUTPUT_PP;
    gpio.speed = GPIO_SPEED_50_MHZ;
    for(i=0; i<8; i++)
    {
        HRcc_EnPortClock(ByteWriter_bytes.bitPort[i]);
        gpio.pins = ByteWriter_bytes.bitPin[i];
        gpio.port = ByteWriter_bytes.bitPort[i];
        Gpio_InitPins(&gpio);
    }
    return E_OK;
}


/*
 * Function:  ByteWriter_WriteByte 
 * --------------------
 *  Writes a byte to the defined pins
 * 
 *  by: The byte to write
 *                   
 *  returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
Std_ReturnType ByteWriter_WriteByte(uint8_t by)
{
    uint8_t i;
    for(i=0; i<8; i++)
    {
        switch(((by>>i)&1))
        {
            case 0:
                Gpio_WritePin(ByteWriter_bytes.bitPort[i], ByteWriter_bytes.bitPin[i], !ByteWriter_bytes.bitActiveState[i]);
                break;
            case 1:
                Gpio_WritePin(ByteWriter_bytes.bitPort[i], ByteWriter_bytes.bitPin[i], ByteWriter_bytes.bitActiveState[i]);
                break;
        }
    }
    return E_OK;
}
