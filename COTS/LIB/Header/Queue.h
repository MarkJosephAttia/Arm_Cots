/**
 * @file Queue.h
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the user interface for a generic queue library
 * @version 0.1
 * @date 2020-04-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#define QUEUE_NOT_EMPTY         0
#define QUEUE_IS_EMPTY          !QUEUE_IS_EMPTY

#define QUEUE_IS_FULL           0
#define QUEUE_NOT_FULL          !QUEUE_IS_FULL

typedef void* queue_t;

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
extern Std_ReturnType Queue_CreateQueue(queue_t* queue, uint16_t sizeOfElement, uint16_t numberOfElements);

/**
 * @brief Adds an Element to a queue
 * 
 * @param queue The queue to add element in
 * @param data The data of the element to add
 * @return Std_ReturnType A status
 *              E_OK            If the element was added successfully
 *              E_NOT_OK        If the function failed to add the element
 */
extern Std_ReturnType Queue_Enqueue(queue_t* queue, uint8_t* data);

/**
 * @brief Gets an element and removes it from the queue
 * 
 * @param queue The queue to get element from
 * @param data The data of the element to get
 * @return Std_ReturnType A status
 *              E_OK            If the element was added successfully
 *              E_NOT_OK        If the function failed to add the element
 */
extern Std_ReturnType Queue_Dequeue(queue_t* queue, uint8_t* data);

/**
 * @brief Gets the element from the front of the queue without removing it from the queue
 * 
 * @param queue The queue to get element from
 * @param data The data of the element to get
 * @return Std_ReturnType 
 *              E_OK            If an element was found
 *              E_NOT_OK        If the function failed to find an element
 */
extern Std_ReturnType Queue_GetFront(queue_t* queue, uint8_t* data);

/**
 * @brief Gets the element from the back of the queue without removing it from the queue
 * 
 * @param queue The queue to get element from
 * @param data The data of the element to get
 * @return Std_ReturnType 
 *              E_OK            If an element was found
 *              E_NOT_OK        If the function failed to find an element
 */
extern Std_ReturnType Queue_GetBack(queue_t* queue, uint8_t* data);

/**
 * @brief Gets the number of elements in the queue
 * 
 * @param queue The queue to get size
 * @param queueSize The size of the queue
 * @return Std_ReturnType A status 
 *              E_OK            If the function was executed successfully
 *              E_NOT_OK        If the function failed execute
 */
extern Std_ReturnType Queue_GetSize(queue_t* queue, uint16_t* queueSize);

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
extern Std_ReturnType Queue_IsEmpty(queue_t* queue, uint8_t* empty);

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
extern Std_ReturnType Queue_IsFull(queue_t* queue, uint8_t* full);

#endif