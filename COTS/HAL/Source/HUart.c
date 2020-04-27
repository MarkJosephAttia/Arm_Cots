/**
 * @file HUart.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the implementation for the UART handler
 * @version 0.1
 * @date 2020-03-29
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"
#include "Queue.h"
#include "Uart.h"
#include "Uart_Cfg.h"
#include "HUart.h"
#include "Nvic.h"
#include "Rcc.h"
#include "Gpio.h"

/* Protection */
#ifndef HUART_DEFAULT_MODULE
#define HUART_DEFAULT_MODULE       HUART_MODULE_1
#endif

#define UART_QUEUE_LENGTH             5

#define UART_NUMBER_OF_MODULES        3

#define HUART_NOT_INITIALIZED         1
#define HUART_INITIALIZED             0
#define HUART_NOT_CONFIGURED          0
#define HUART_CONFIGURED              1

/**
 * @brief The packet that will be sent through the Uart
 * 
 */
typedef struct
{
    uint8_t* data;
    uint16_t len;
    hUartAppNotify_t appNotify;
}hUartPacket_t;


static volatile queue_t HUart_rxQueue[UART_NUMBER_OF_MODULES];
static volatile queue_t HUart_txQueue[UART_NUMBER_OF_MODULES];

static volatile uint8_t HUart_module =  HUART_DEFAULT_MODULE;
static volatile uint8_t isInitialized[UART_NUMBER_OF_MODULES] = {HUART_NOT_INITIALIZED, HUART_NOT_INITIALIZED, HUART_NOT_INITIALIZED};
static volatile uint8_t isConfigured[UART_NUMBER_OF_MODULES] =  {HUART_NOT_CONFIGURED, HUART_NOT_CONFIGURED, HUART_NOT_CONFIGURED};


static void HUart_TxCallBack(uint8_t module);
static void HUart_RxCallBack(uint8_t module);


/**
 * @brief Initializes the UART Module
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
Std_ReturnType HUart_Init(void)
{
    gpio_t gpio;
    Queue_CreateQueue(&(HUart_rxQueue[HUart_module]), sizeof(hUartPacket_t), UART_QUEUE_LENGTH);
    Queue_CreateQueue(&(HUart_txQueue[HUart_module]), sizeof(hUartPacket_t), UART_QUEUE_LENGTH);
#ifdef UART_USE_DMA
    Rcc_SetAhbPeriphClockState(RCC_DMA1_CLK_EN, RCC_PERIPH_CLK_ON);
#endif
    /* Configur the Uart modules Clocks, Interrupts and Gpios*/
    switch(HUart_module)
    {
        case HUART_MODULE_1:
            Rcc_SetApb2PeriphClockState(RCC_IOPA_CLK_EN, RCC_PERIPH_CLK_ON);
            gpio.pins = GPIO_PIN_9;
            gpio.port = GPIO_PORTA;
            gpio.mode = GPIO_MODE_AF_OUTPUT_PP;
            gpio.speed = GPIO_SPEED_50_MHZ;
            Gpio_InitPins(&gpio);
            gpio.pins = GPIO_PIN_10;
            gpio.mode = GPIO_MODE_INPUT_PULL_UP;
            Gpio_InitPins(&gpio);
            Uart_SetTxCb(HUart_TxCallBack, UART1);
            Uart_SetRxCb(HUart_RxCallBack, UART1);
            Rcc_SetApb2PeriphClockState(RCC_USART1_CLK_EN, RCC_PERIPH_CLK_ON);
            Nvic_EnableInterrupt(NVIC_IRQNUM_USART1);
#ifdef UART_USE_DMA
            Nvic_EnableInterrupt(NVIC_IRQNUM_DMA1_CHANNEL5);
#endif
            break;
        case HUART_MODULE_2:
            Rcc_SetApb2PeriphClockState(RCC_IOPA_CLK_EN, RCC_PERIPH_CLK_ON);
            gpio.pins = GPIO_PIN_2;
            gpio.port = GPIO_PORTA;
            gpio.mode = GPIO_MODE_AF_OUTPUT_PP;
            gpio.speed = GPIO_SPEED_50_MHZ;
            Gpio_InitPins(&gpio);
            gpio.pins = GPIO_PIN_3;
            gpio.mode = GPIO_MODE_INPUT_PULL_UP;
            Gpio_InitPins(&gpio);
            Uart_SetTxCb(HUart_TxCallBack, UART2);
            Uart_SetRxCb(HUart_RxCallBack, UART2);
            Rcc_SetApb1PeriphClockState(RCC_USART2_CLK_EN, RCC_PERIPH_CLK_ON);
            Nvic_EnableInterrupt(NVIC_IRQNUM_USART2);
#ifdef UART_USE_DMA
            Nvic_EnableInterrupt(NVIC_IRQNUM_DMA1_CHANNEL6);
#endif
            break;
        case HUART_MODULE_3:
            Rcc_SetApb2PeriphClockState(RCC_IOPB_CLK_EN, RCC_PERIPH_CLK_ON);
            gpio.pins = GPIO_PIN_10;
            gpio.port = GPIO_PORTB;
            gpio.mode = GPIO_MODE_AF_OUTPUT_PP;
            gpio.speed = GPIO_SPEED_50_MHZ;
            Gpio_InitPins(&gpio);
            gpio.pins = GPIO_PIN_11;
            gpio.mode = GPIO_MODE_INPUT_PULL_UP;
            Gpio_InitPins(&gpio);
            Uart_SetTxCb(HUart_TxCallBack, UART3);
            Uart_SetRxCb(HUart_RxCallBack, UART3);
            Rcc_SetApb1PeriphClockState(RCC_USART3_CLK_EN, RCC_PERIPH_CLK_ON);
            Nvic_EnableInterrupt(NVIC_IRQNUM_USART3);
#ifdef UART_USE_DMA
            Nvic_EnableInterrupt(NVIC_IRQNUM_DMA1_CHANNEL3);
#endif            
            break;
    }
        /* If the Uart Module was not configured */
    if(HUART_NOT_CONFIGURED == isConfigured[HUart_module])
    {
        Uart_Init(HUART_DEFAULT_BAUDRATE, HUART_DEFAULT_STOP_BITS, HUART_DEFAULT_PARITY, HUART_FLOW_CONTROL_DIS, HUART_SYSTEM_CLK, HUart_module);
    }
    isInitialized[HUart_module] = HUART_INITIALIZED;
    return E_OK;
}
/**
 * @brief Sets configurations for the UART module
 * *The UART must be initialized after setting configurations to apply the changes
 * 
 * @param baudRate the baud rate of the UART (uint32_t)
 * @param stopBits The number of the stop bits
 *                 @arg HUART_ONE_STOP_BIT
 *                 @arg HUART_TWO_STOP_BITS
 * @param parity The parity of the transmission
 *                 @arg HUART_ODD_PARITY
 *                 @arg HUART_EVEN_PARITY
 *                 @arg HUART_NO_PARITY
 * @param flowControl the flow control
 *                 @arg HUART_FLOW_CONTROL_EN
 *                 @arg HUART_FLOW_CONTROL_DIS
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
Std_ReturnType HUart_Config(uint32_t baudRate, uint32_t stopBits, uint32_t parity, uint32_t flowControl)
{
    Uart_SetTxCb(HUart_TxCallBack, HUart_module);
    Uart_SetRxCb(HUart_RxCallBack, HUart_module);
    Uart_Init(baudRate, stopBits, parity, flowControl, HUART_SYSTEM_CLK, HUart_module);
    isConfigured[HUart_module] = HUART_CONFIGURED;
    return E_OK;
}
/**
 * @brief Sets the module that you will be using
 * 
 * @param uartModule The UART module
 *                  @arg HUART_MODULE_1
 *                  @arg HUART_MODULE_2
 *                  @arg HUART_MODULE_3
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
Std_ReturnType HUart_SetModule(uint8_t uartModule)
{
    HUart_module = uartModule;
    return E_OK;
}
/**
 * @brief Sends data through the UART
 *
 * @param data The data to send
 * @param length the length of the data in bytes
 * @param notify The application notification
 * @return Std_ReturnType A Status
 *                  E_OK: If the driver is ready to send
 *                  E_NOT_OK: If the driver can't send data right now
 */
Std_ReturnType HUart_Send(uint8_t *data, uint16_t length, hUartAppNotify_t notify)
{
    Std_ReturnType error = E_NOT_OK;
    hUartPacket_t pack;
    /* If the current Uart module is initialized */
    if(HUART_INITIALIZED == isInitialized[HUart_module])
    {
        pack.data = data;
        pack.len = length;
        pack.appNotify = notify;
        error = Queue_Enqueue(&(HUart_txQueue[HUart_module]), (uint8_t*)(&pack));
        Uart_Send(pack.data, pack.len, HUart_module);
    }
    return error;
}
/**
 * @brief Receives data through the UART
 *
 * @param data The buffer to receive data in
 * @param length the length of the data in bytes
 * @param notify The application notification
 * @return Std_ReturnType A Status
 *                  E_OK: If the driver is ready to receive
 *                  E_NOT_OK: If the driver can't receive data right now
 */
Std_ReturnType HUart_Receive(uint8_t *data, uint16_t length, hUartAppNotify_t notify)
{
    Std_ReturnType error = E_NOT_OK;
    hUartPacket_t pack;
    /* If the current Uart module is initialized */
    if(HUART_INITIALIZED == isInitialized[HUart_module])
    {
        pack.data = data;
        pack.len = length;
        pack.appNotify = notify;
        error = Queue_Enqueue(&(HUart_rxQueue[HUart_module]), (uint8_t*)(&pack));
        Uart_Receive(pack.data, pack.len, HUart_module);
    }
    return error;
}
/**
 * @brief The TX Callback function sent to the UART driver
 * 
 * @param module The number of the UART module
 *                  @arg HUART_MODULE_1
 *                  @arg HUART_MODULE_2
 *                  @arg HUART_MODULE_3
 */
static void HUart_TxCallBack(uint8_t module)
{
    hUartPacket_t packet;
    /* If the first packet in the queue is valid */
    if(E_OK == Queue_GetFront(&(HUart_txQueue[module]), (uint8_t*)(&packet)))
    {
      if(packet.appNotify)
      {
        packet.appNotify();
      }
      /* Pop the packet from the queue */
      Queue_Dequeue(&(HUart_txQueue[module]), (uint8_t*)(&packet));
    }
    /* If there were any other packets in the queue */
    if(E_OK == Queue_GetFront(&(HUart_txQueue[module]), (uint8_t*)(&packet)))
    {
        Uart_Send(packet.data, packet.len, module);
    }
}
/**
 * @brief The RX Callback function sent to the UART driver
 * 
 * @param module The number of the UART module
 *                  @arg HUART_MODULE_1
 *                  @arg HUART_MODULE_2
 *                  @arg HUART_MODULE_3
 */
static void HUart_RxCallBack(uint8_t module)
{
    hUartPacket_t packet;
    /* If the first packet in the queue is valid */
    if(E_OK == Queue_GetFront(&(HUart_rxQueue[module]), (uint8_t*)(&packet)))
    {
      if(packet.appNotify)
      {
        packet.appNotify();
      }
      /* Pop the packet from the queue */
      Queue_Dequeue(&(HUart_rxQueue[module]), (uint8_t*)(&packet));
    }
    /* If there were any other packets in the queue */
    if(E_OK == Queue_GetFront(&(HUart_rxQueue[module]), (uint8_t*)(&packet)))
    {
        Uart_Receive(packet.data, packet.len, module);
    }
}
