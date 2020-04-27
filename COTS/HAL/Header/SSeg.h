/**
 * @file SSeg.h
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the user interface for the seven segment driver
 * @version 0.1
 * @date 2020-03-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef SEVEN_SEGMENT_H
#define SEVEN_SEGMENT_H
#include "SSeg_Cfg.h"

typedef struct
{
    uint32_t dPin[SSEG_NUMBER_OF_PINS];
    uint32_t dPort[SSEG_NUMBER_OF_PINS];
    uint32_t enPin[SSEG_NUMBER_OF_SSEGS];
    uint32_t enPort[SSEG_NUMBER_OF_SSEGS];
    uint32_t common[SSEG_NUMBER_OF_SSEGS];
} sseg_t;

/**
 * @brief The initialization function for the Seven segment
 * 
 */
extern void SSeg_Init(void);
/**
 * @brief Seven Segment Set on
 * 
 * @param num The number of the seven segment
 */
extern void SSeg_SetOn(u8 num);
/**
 * @brief Seven Segment Set off
 * 
 * @param num The number of the seven segment
 */
extern void SSeg_SetOff(u8 num);
/**
 * @brief Sets a digit on a specific seven segment
 * 
 * @param num the number of the seven segment
 * @param digit the digit you want to set
 */
extern void SSeg_SetNum(u8 num, u8 digit);
/**
 * @brief Sets the dashes on a spevific seven segment
 * 
 * @param num the dashing number
 */
extern void SSeg_SetDash(u8 num);

#endif
