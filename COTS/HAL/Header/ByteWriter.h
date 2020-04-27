/**
 * @file  ByteWriter.h 
 * @brief This file is to be used as an interface for the user of the Byte Writer.
 *
 * @author Mark Attia
 * @date January 22, 2020
 *
 */
#ifndef BYTE_WRITER_H
#define BYTE_WRITER_H

typedef struct
{
    uint8_t bitPin[8];
    uint8_t bitPort[8];
    uint8_t bitActiveState[8];
} byte_t;

/*
 * Function:  ByteWriter_Init 
 * --------------------
 *  Initializes GPIOs for the Byte Writer
 * 
 *  returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType ByteWriter_Init(void);

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
extern Std_ReturnType ByteWriter_WriteByte(uint8_t by);

#endif
