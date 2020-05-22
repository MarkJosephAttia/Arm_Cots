/**
 * @file Lin.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the implementation for the Lin driver
 * @version 0.1
 * @date 2020-03-26
 *
 * @copyright Copyright (c) 2020
 *
 */
#include "Std_Types.h"
#include "Uart.h"
#include "Gpio.h"
#include "Nvic.h"
#include "Rcc.h"
#include "Sched.h"
#include "Lin_Cfg.h"
#include "Lin.h"

#define LIN_INITIALIZED             0
#define LIN_NOT_INITIALIZED         1

#define LIN_HEADER_SIZE             2
#define LIN_SYNC_BYTE               0x55
#define LIN_SYNC_BYTE_IDX           0
#define LIN_PID_IDX                 1
#define LIN_MAX_RESPONSE_SIZE       9

#define LIN_VALID                   0
#define LIN_NOT_VALID               0

#define LIN_FLAG_DOWN               0
#define LIN_FLAG_UP                 1

#define GET_PID(id)     ( (((id>>0) ^ (id>>1) ^ (id>>2) ^ (id>>4) ^ 0)&1) << 6 ) |      \
                        (  ((id>>1) ^ (id>>3) ^ (id>>4) ^ (id>>5) ^ 1)    << 7 ) |      \
                        id

typedef struct
{
  const Lin_msg_t* msg;
  uint8_t* buffer;
  uint8_t pid;
  uint8_t checksum;
}Lin_msgComplete_t;

static uint8_t Lin_init = LIN_NOT_INITIALIZED;
static uint8_t Lin_uartModule = LIN_UART_MODULE;

extern const Lin_msg_t Lin_msg[LIN_NUMBER_OF_MSGS];
static volatile Lin_msgComplete_t Lin_msgC[LIN_NUMBER_OF_MSGS];

static volatile uint8_t Lin_currentMsgIndex;
static volatile uint8_t Lin_receivedMsgIndex;
static volatile uint8_t Lin_sendHeader[LIN_HEADER_SIZE] = {LIN_SYNC_BYTE};
static volatile uint8_t Lin_receiveHeader[LIN_HEADER_SIZE];

static volatile uint8_t Lin_response[LIN_MAX_RESPONSE_SIZE];
static volatile uint8_t Lin_sendFlag = LIN_FLAG_DOWN;

#if LIN_ROLE == LIN_MASTER
static void Lin_MasterTaskProcess(void);
#endif
static void Lin_SlaveTaskProcess(void);
static void Lin_BreakHandler(uint8_t uartModule);
static void Lin_HeaderReceiveHandler(uint8_t uartModule);
static void Lin_ProcessData(uint8_t uartModule);

/**
 * @brief Initializes the UART
 *
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
Std_ReturnType Lin_Init(void)
{
  gpio_t gpio;
  Uart_cfg_t cfgUart;
  uint8_t i;
  for(i=0; i<LIN_NUMBER_OF_MSGS; i++)
  {
    Lin_msgC[i].msg = &(Lin_msg[i]);
    Lin_msgC[i].buffer = NULL;
    Lin_msgC[i].pid = GET_PID(Lin_msgC[i].msg->id);
  }
  switch(LIN_UART_MODULE)
  {
    case LIN_UART_MODULE_1:
      Rcc_SetApb2PeriphClockState(RCC_IOPA_CLK_EN, RCC_PERIPH_CLK_ON);
      gpio.pins = GPIO_PIN_9;
      gpio.port = GPIO_PORTA;
      gpio.mode = GPIO_MODE_AF_OUTPUT_PP;
      gpio.speed = GPIO_SPEED_50_MHZ;
      Gpio_InitPins(&gpio);
      gpio.pins = GPIO_PIN_10;
      gpio.mode = GPIO_MODE_INPUT_PULL_UP;
      Gpio_InitPins(&gpio);
      Rcc_SetApb2PeriphClockState(RCC_USART1_CLK_EN, RCC_PERIPH_CLK_ON);
      Nvic_EnableInterrupt(NVIC_IRQNUM_USART1);
      break;
    case LIN_UART_MODULE_2:
      Rcc_SetApb2PeriphClockState(RCC_IOPA_CLK_EN, RCC_PERIPH_CLK_ON);
      gpio.pins = GPIO_PIN_2;
      gpio.port = GPIO_PORTA;
      gpio.mode = GPIO_MODE_AF_OUTPUT_PP;
      gpio.speed = GPIO_SPEED_50_MHZ;
      Gpio_InitPins(&gpio);
      gpio.pins = GPIO_PIN_3;
      gpio.mode = GPIO_MODE_INPUT_PULL_UP;
      Gpio_InitPins(&gpio);
      Rcc_SetApb1PeriphClockState(RCC_USART2_CLK_EN, RCC_PERIPH_CLK_ON);
      Nvic_EnableInterrupt(NVIC_IRQNUM_USART2);
      break;
    case LIN_UART_MODULE_3:
      Rcc_SetApb2PeriphClockState(RCC_IOPB_CLK_EN, RCC_PERIPH_CLK_ON);
      gpio.pins = GPIO_PIN_10;
      gpio.port = GPIO_PORTB;
      gpio.mode = GPIO_MODE_AF_OUTPUT_PP;
      gpio.speed = GPIO_SPEED_50_MHZ;
      Gpio_InitPins(&gpio);
      gpio.pins = GPIO_PIN_11;
      gpio.mode = GPIO_MODE_INPUT_PULL_UP;
      Gpio_InitPins(&gpio);
      Rcc_SetApb1PeriphClockState(RCC_USART3_CLK_EN, RCC_PERIPH_CLK_ON);
      Nvic_EnableInterrupt(NVIC_IRQNUM_USART3);   
      Lin_init = LIN_INITIALIZED;         
      break;
  }
  cfgUart.baudRate = LIN_UART_BAUDRATE;
  cfgUart.stopBits = UART_STOP_ONE_BIT;
  cfgUart.parity = UART_NO_PARITY;
  cfgUart.flowControl = UART_FLOW_CONTROL_DIS;
  cfgUart.linEn = UART_LIN_EN;
  cfgUart.uartModule = LIN_UART_MODULE;
  cfgUart.sysClk = LIN_UART_SYSTEM_CLK;
  cfgUart.interrupts = UART_INTERRUPT_TC | UART_INTERRUPT_RXNE | UART_INTERRUPT_LBD;
  Uart_Init(&cfgUart);
  Uart_SetBreakCb(Lin_BreakHandler, Lin_uartModule);
  return E_OK;
}

/**
 * @brief Sets The Buffer For A Certain Message
 * 
 * @param buffer A pointer to the buffer 
 * @param msgID The Lin Message ID You Want To Set Buffer For
 * @return Std_ReturnType 
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
Std_ReturnType Lin_SetMsgBuffer(uint8_t* buffer, uint8_t msgID)
{
  Std_ReturnType error = E_NOT_OK;
  uint8_t i;
  for(i=0; i<LIN_NUMBER_OF_MSGS;i++)
  {
    if(Lin_msgC[i].msg->id == msgID)
    {
      Lin_msgC[i].buffer = buffer;
      error = E_OK;
    }
  }
  return error;
}

/**
 * @brief The Handler Of The Uart Break Interrupt
 * 
 * @param uartModule The Uart Module
 */
static void Lin_BreakHandler(uint8_t uartModule)
{
  Uart_SetRxCb(Lin_HeaderReceiveHandler, Lin_uartModule);
  Uart_Receive((uint8_t*)Lin_receiveHeader, LIN_HEADER_SIZE, Lin_uartModule);
#if LIN_ROLE == LIN_MASTER
  Lin_sendHeader[LIN_PID_IDX] = Lin_msgC[Lin_currentMsgIndex].pid;
  Uart_Send((uint8_t*)Lin_sendHeader, LIN_HEADER_SIZE, Lin_uartModule);
#endif
}

/**
 * @brief The Handler Of The Uart Receive Interrupt When A Header Is Sent
 * 
 * @param uartModule The Uart Module
 */
static void Lin_HeaderReceiveHandler(uint8_t uartModule)
{
  uint8_t valid = LIN_NOT_VALID;
  if(Lin_receiveHeader[LIN_SYNC_BYTE_IDX] == LIN_SYNC_BYTE)
  {
    for(Lin_receivedMsgIndex=0; Lin_receivedMsgIndex<LIN_NUMBER_OF_MSGS; Lin_receivedMsgIndex++)
    {
      if(Lin_msgC[Lin_receivedMsgIndex].pid == Lin_receiveHeader[LIN_PID_IDX])
      {
        valid = LIN_VALID;
        break;
      }
    }
    if(valid == LIN_VALID && Lin_msgC[Lin_receivedMsgIndex].msg->direction == LIN_RECEIVE && Lin_msgC[Lin_receivedMsgIndex].buffer)
    {
      Uart_SetRxCb(Lin_ProcessData, Lin_uartModule);
      Uart_Receive((uint8_t*)Lin_response, Lin_msgC[Lin_receivedMsgIndex].msg->size + 1, Lin_uartModule);
    }
    if(valid == LIN_VALID && Lin_msgC[Lin_receivedMsgIndex].msg->direction == LIN_SEND)
    {
      Lin_sendFlag = LIN_FLAG_UP;
    }
  }
}

/**
 * @brief The Handler Of The Uart Receive Interrupt When A Response Is Received
 * 
 * @param uartModule The Uart Module
 */
static void Lin_ProcessData(uint8_t uartModule)
{
  uint8_t i,tmpChecksum=0;
  for(i=0; i<Lin_msgC[Lin_receivedMsgIndex].msg->size; i++)
  {
    tmpChecksum ^= Lin_response[i];
  }
  if(tmpChecksum == Lin_response[Lin_msgC[Lin_receivedMsgIndex].msg->size])
  {
    for(i=0; i<Lin_msgC[Lin_receivedMsgIndex].msg->size; i++)
    {
      Lin_msgC[Lin_receivedMsgIndex].buffer[i] = Lin_response[i]; 
    }
  }
  Uart_SetRxCb(NULL, Lin_uartModule);
}

#if LIN_ROLE == LIN_MASTER
/**
 * @brief The Master Task Runnable
 * 
 */
static void Lin_MasterTaskProcess(void)
{
  static uint16_t msgItr;
  if(msgItr == 0)
  {
    Uart_SendBreak(Lin_uartModule);
  }
  if(msgItr == Lin_msgC[Lin_currentMsgIndex].msg->ticks)
  {
    Lin_currentMsgIndex++;
    if(Lin_currentMsgIndex == LIN_NUMBER_OF_MSGS)
    {
      Lin_currentMsgIndex=0;
    }
    msgItr=0;  
  }
  else
  {
    msgItr++;
  }
}
#endif

/**
 * @brief The Slave Task Runnable
 * 
 */
static void Lin_SlaveTaskProcess(void)
{
  uint8_t i,tmpChecksum=0;
  
  if(Lin_sendFlag == LIN_FLAG_UP)
  {
    if(Lin_msgC[Lin_receivedMsgIndex].buffer)
    {
      for(i=0; i<Lin_msgC[Lin_receivedMsgIndex].msg->size; i++)
      {
        tmpChecksum ^= Lin_msgC[Lin_receivedMsgIndex].buffer[i];
        Lin_response[i] = Lin_msgC[Lin_receivedMsgIndex].buffer[i];
      }
      Lin_response[i] = tmpChecksum;
      Uart_Send((uint8_t*)Lin_response, i+1, Lin_uartModule);
    }
    Lin_sendFlag = LIN_FLAG_DOWN;
  }
}


/**
 * @brief The Runnable of the Lin Task
 * 
 */
static void Lin_Runnable(void)
{
  if(Lin_init == LIN_INITIALIZED)
  {
#if LIN_ROLE == LIN_MASTER
    Lin_MasterTaskProcess();
#endif
    Lin_SlaveTaskProcess();
  }
}

const task_t Lin_task = {Lin_Runnable, 5};