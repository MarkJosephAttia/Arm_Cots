/**
 * @file Alloc.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the implementation for the memory allocation tool
 * @version 0.1
 * @date 2020-04-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"

#define MEM_MAX_SIZE        50000

static uint8_t Alloc_rawMem[MEM_MAX_SIZE];
static uint16_t Alloc_memItr;

/**
 * @brief This function allocates free bytes for the user
 * 
 * @param ptr a place to return the address of the allocated bytes
 * @param sizeInBytets the size of memory allocation in bytes
 * @return Std_ReturnType a status
 *          E_OK        If bytes allocated successfully
 *          E_NOT_OK    If the function failed to allocate bytes 
 */
extern Std_ReturnType AllocBytes(void** ptr, uint16_t sizeInBytets)
{
    Std_ReturnType error = E_NOT_OK;
    if (Alloc_memItr + sizeInBytets < MEM_MAX_SIZE)
    {
        *ptr = (void*)&(Alloc_rawMem[Alloc_memItr]);
        Alloc_memItr += sizeInBytets;
        error = E_OK;
    }
    return error;
}