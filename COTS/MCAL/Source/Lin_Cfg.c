/**
 * @file Lin_Cfg.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief These are the configurations implementation for the Lin Driver
 * @version 0.1
 * @date 2020-05-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "Std_Types.h"
#include "Lin_Cfg.h"
#include "Lin.h"

const Lin_msg_t Lin_msg[LIN_NUMBER_OF_MSGS] = {
        {.id = 15, .size = 5, .direction = LIN_SEND, .ticks = 9},
        {.id = 16, .size = 8, .direction = LIN_SEND, .ticks = 10}
};