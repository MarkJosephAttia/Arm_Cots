/**
 * @file  SysTick.c 
 * @brief This file is to be used as an implementation of the SysTick driver.
 *
 * @author Mark Attia
 * @date January 22, 2020
 *
 */
#include "Std_Types.h"
#include "SysTick.h"

#define SYSTICK_CTRL            *((volatile uint32_t*)0xE000E010)       /* The SysTick Control Register */
#define SYSTICK_LOAD            *((volatile uint32_t*)0xE000E014)       /* The SysTick Load Register */
#define SYSTICK_VAL             *((volatile uint32_t*)0xE000E018)       /* The SysTick Value Register */

#define SYSTICK_INT_EN       0x00000002
#define SYSTICK_INT_DIS      0xFFFFFFFD

#define SYSTICK_TMR_EN       0x00000001
#define SYSTICK_TMR_DIS      0xFFFFFFFE

#define SYSTICK_VALUE_MSK    0x00FFFFFF

static systickCb_t SysTick_callBack = NULL;


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
Std_ReturnType SysTick_InterruptEnable(void)
{
    SYSTICK_CTRL |= SYSTICK_INT_EN;
    return E_OK;
}

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
Std_ReturnType SysTick_InterruptDisable(void)
{
    SYSTICK_CTRL &= SYSTICK_INT_DIS;
    return E_OK;
}

/**
 * Function:  SysTick_Start 
 * --------------------
 *  @brief Enables the SysTick timer
 *  
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
Std_ReturnType SysTick_Start(void)
{
    SYSTICK_CTRL |= SYSTICK_TMR_EN;
    return E_OK;
}

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
Std_ReturnType SysTick_Stop(void)
{
    SYSTICK_CTRL &= SYSTICK_TMR_DIS;
    return E_OK;
}

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
Std_ReturnType SysTick_GetValue(uint32_t* val)
{
    *val = SYSTICK_VAL & SYSTICK_VALUE_MSK;
    return E_OK;
}

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
Std_ReturnType SysTick_SetReloadValue(uint32_t val)
{
    SYSTICK_LOAD = val;
    return E_OK;
}

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
Std_ReturnType SysTick_SetCallBack(systickCb_t func)
{
    SysTick_callBack = func;
    return E_OK;
}


/**
 * Function:  SysTick_SetCallBack 
 * --------------------
 *  @brief The SysTick Handler
 *
 */
void SysTick_Handler(void)
{
    if(SysTick_callBack)
	{
        SysTick_callBack();
    }
}

/**
 * Function:  SysTick_ClearValue 
 * --------------------
 *  @brief Clears the value of the counter
 *  
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
Std_ReturnType SysTick_ClearValue(void)
{
    SYSTICK_VAL = 1;
    return E_OK;
}

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
Std_ReturnType SysTick_SetTimeUS(uint32_t AHB_Clock, uint32_t timeUS)
{
    f64 val = (f64)AHB_Clock * (f64)timeUS /1000000.0;
    SYSTICK_LOAD = (uint32_t)val;
    return E_OK;
}
