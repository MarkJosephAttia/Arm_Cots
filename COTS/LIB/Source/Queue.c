/**
 * @file Queue.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the implementation for the queue library
 * @version 0.1
 * @date 2020-04-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"
#include "Alloc.h"
#include "Queue.h"

#define MAXIMUM_SIZE_OF_DATA_BYTES                      1000

typedef struct
{
    uint16_t nElements;
    uint16_t maxElements;
    uint16_t front;
    uint16_t back;
    uint16_t elementSize;
    uint16_t lastElement;
    uint8_t data[MAXIMUM_SIZE_OF_DATA_BYTES];
}queueData_t;


/**
 * @brief This function creates a new queue
 * 
 * @param queue The address of the queue to be allocated
 * @param sizeOfElement The size of one element in bytes
 * @param numberOfElements The number of elements in a queue
 * @return Std_ReturnType A status
 *              E_OK            If the Queue was created successfully
 *              E_NOT_OK        If the function failed to create a Queue
 */
Std_ReturnType Queue_CreateQueue(queue_t* queue, uint16_t sizeOfElement, uint16_t numberOfElements)
{
    Std_ReturnType error;
    error = AllocBytes(queue, (sizeOfElement*numberOfElements)+12);
    queueData_t* myQueue = (queueData_t*)*queue;
    if(error == E_OK)
    {
        myQueue->maxElements = numberOfElements;
        myQueue->elementSize = sizeOfElement;
        myQueue->front = 0;
        myQueue->back = 0;
        myQueue->nElements = 0;
        myQueue->lastElement =  sizeOfElement * (numberOfElements-1);
    }
    return error;
}

/**
 * @brief Adds an Element to a queue
 * 
 * @param queue The queue to add element in
 * @param data The data of the element to add
 * @return Std_ReturnType A status
 *              E_OK            If the element was added successfully
 *              E_NOT_OK        If the function failed to add the element
 */
Std_ReturnType Queue_Enqueue(queue_t* queue, uint8_t* data)
{
    Std_ReturnType error = E_NOT_OK;
    uint8_t i;
    queueData_t* myQueue = (queueData_t*)*queue;
    if(myQueue->nElements < myQueue->maxElements)
    {
        for(i=0; i<myQueue->elementSize; i++)
        {
            myQueue->data[myQueue->back + i] = data[i]; 
        }
        if(myQueue->back == myQueue->lastElement)
        {
            myQueue->back = 0;
        }
        else
        {
            myQueue->back += myQueue->elementSize;
        }
        myQueue->nElements++;
        error = E_OK;
    }
    return error;
}

/**
 * @brief Gets an element and removes it from the queue
 * 
 * @param queue The queue to get element from
 * @param data The data of the element to get
 * @return Std_ReturnType A status
 *              E_OK            If the element was added successfully
 *              E_NOT_OK        If the function failed to add the element
 */
Std_ReturnType Queue_Dequeue(queue_t* queue, uint8_t* data)
{
    Std_ReturnType error = E_NOT_OK;
    uint8_t i;
    queueData_t* myQueue = (queueData_t*)*queue;
    if(myQueue->nElements != 0)
    {
        for(i=0; i<myQueue->elementSize; i++)
        {
            data[i] = myQueue->data[myQueue->front + i]; 
        }
        if(myQueue->front == myQueue->lastElement)
        {
            myQueue->front = 0;
        }
        else
        {
            myQueue->front += myQueue->elementSize;
        }
        myQueue->nElements--;
        error = E_OK;
    }
    return error;
}

/**
 * @brief Gets the element from the front of the queue without removing it from the queue
 * 
 * @param queue The queue to get element from
 * @param data The data of the element to get
 * @return Std_ReturnType 
 *              E_OK            If an element was found
 *              E_NOT_OK        If the function failed to find an element
 */
Std_ReturnType Queue_GetFront(queue_t* queue, uint8_t* data)
{
    Std_ReturnType error = E_NOT_OK;
    uint8_t i;
    queueData_t* myQueue = (queueData_t*)*queue;
    if(myQueue->nElements != 0)
    {
        for(i=0; i<myQueue->elementSize; i++)
        {
            data[i] = myQueue->data[myQueue->front + i]; 
        }
        error = E_OK;
    }
    return error;
}

/**
 * @brief Gets the element from the back of the queue without removing it from the queue
 * 
 * @param queue The queue to get element from
 * @param data The data of the element to get
 * @return Std_ReturnType 
 *              E_OK            If an element was found
 *              E_NOT_OK        If the function failed to find an element
 */
Std_ReturnType Queue_GetBack(queue_t* queue, uint8_t* data)
{
    Std_ReturnType error = E_NOT_OK;
    uint8_t i;
    queueData_t* myQueue = (queueData_t*)*queue;
    if(myQueue->nElements != 0)
    {
        for(i=0; i<myQueue->elementSize; i++)
        {
            data[i] = myQueue->data[myQueue->back - myQueue->elementSize + i]; 
        }
        error = E_OK;
    }
    return error;
}

/**
 * @brief Gets the number of elements in the queue
 * 
 * @param queue The queue to get size
 * @param queueSize The size of the queue
 * @return Std_ReturnType A status 
 *              E_OK            If the function was executed successfully
 *              E_NOT_OK        If the function failed execute
 */
Std_ReturnType Queue_GetSize(queue_t* queue, uint16_t* queueSize)
{
    queueData_t* myQueue = (queueData_t*)*queue;
    *queueSize = myQueue->nElements;
    return E_OK;
}

/**
 * @brief Checks if the queue is empty
 * 
 * @param queue The queue
 * @param empty The state of the queue
 *                  @arg QUEUE_IS_EMPTY      If the queue is empty
 *                  @arg QUEUE_NOT_EMPTY     If the queue is not empty
 * @return Std_ReturnType A status
 *              E_OK            If the function was executed successfully
 *              E_NOT_OK        If the function failed execute
 */
Std_ReturnType Queue_IsEmpty(queue_t* queue, uint8_t* empty)
{
    queueData_t* myQueue = (queueData_t*)*queue;
    *empty = !myQueue->nElements;
    return E_OK;
}

/**
 * @brief Checks if the queue is full
 * 
 * @param queue The queue
 * @param empty The state of the queue
 *                  @arg QUEUE_IS_FULL      If the queue is full
 *                  @arg QUEUE_NOT_FULL     If the queue is not full
 * @return Std_ReturnType A status
 *              E_OK            If the function was executed successfully
 *              E_NOT_OK        If the function failed execute
 */
Std_ReturnType Queue_IsFull(queue_t* queue, uint8_t* full)
{
    queueData_t* myQueue = (queueData_t*)*queue;
    *full = !(myQueue->nElements == myQueue->maxElements);
    return E_OK;
}