/**
 * @file Alloc.h
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the user interface for the Allocation tool
 * @version 0.1
 * @date 2020-04-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef ALLOC_H_
#define ALLOC_H_

/**
 * @brief This function allocates free bytes for the user
 * 
 * @param ptr a place to return the address of the allocated bytes
 * @param sizeInBytets the size of memory allocation in bytes
 * @return Std_ReturnType a status
 *          E_OK        If bytes allocated successfully
 *          E_NOT_OK    If the function failed to allocate bytes 
 */
extern Std_ReturnType AllocBytes(void** ptr, uint16_t sizeInBytets);

#endif