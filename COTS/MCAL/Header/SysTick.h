/**
 * @file  SysTick.h 
 * @brief This file is to be used as an interface for the user of SysTick driver.
 *
 * @author Mark Attia
 * @date January 22, 2020
 *
 */

#ifndef SYSTICK_H
#define SYSTICK_H

typedef void (*systickCb_t)(void);

/**
 * Function:  SysTick_InterruptEnable 
 * --------------------
 *  @brief Enables the interrupt for the SysTick
 *
 *  
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType SysTick_InterruptEnable(void);

/**
 * Function:  SysTick_InterruptDisable 
 * --------------------
 *  @brief Disables the interrupt for the SysTick
 *
 *  
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType SysTick_InterruptDisable(void);

/**
 * Function:  SysTick_Start 
 * --------------------
 *  @brief Enables the SysTick timer
 *  
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType SysTick_Start(void);

/**
 * Function:  SysTick_Stop 
 * --------------------
 *  @brief Disables the SysTick timer
 *
 *  
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType SysTick_Stop(void);

/**
 * Function:  SysTick_GetValue 
 * --------------------
 *  @brief Reads the current value inside the SysTick timer
 *
 *  @param val: a pointer to return data in
 *  
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType SysTick_GetValue(uint32_t* val);

/**
 * Function:  SysTick_SetReloadValue 
 * --------------------
 *  @brief Sets the reload value for the SysTick
 *
 *  @param val: the reload value
 *  
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType SysTick_SetReloadValue(uint32_t val);

/**
 * Function:  SysTick_SetCallBack 
 * --------------------
 *  @brief Sets the callback function for the SysTick
 *
 *  @param func: the callback function
 *  
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType SysTick_SetCallBack(systickCb_t func);

/**
 * Function:  SysTick_ClearValue 
 * --------------------
 *  @brief Clears the value of the counter
 *  
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType SysTick_ClearValue(void);

/**
 * Function:  Systick_SetTimeUS 
 * --------------------
 *  @brief Sets The reload time for the SysTick
 *
 *  @param AHB_Clock: The AHB bus clock frequency
 *  @param timeUS: The time in Micro seconds
 *  
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType SysTick_SetTimeUS(uint32_t AHB_Clock, uint32_t timeUS);

#endif