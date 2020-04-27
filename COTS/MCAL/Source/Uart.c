/**
 * @file Uart.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the implementation for the UART driver
 * @version 0.1
 * @date 2020-03-26
 *
 * @copyright Copyright (c) 2020
 *
 */
#include "Std_Types.h"
#include "Uart_Cfg.h"
#include "Uart.h"
#include "Dma.h"

#define UART_NUMBER_OF_MODULES        3

/**
 * @brief The UART Registers
 * 
 */
typedef struct
{
  uint32_t SR;    /* The UART Status Register */
  uint32_t DR;    /* The Data Status Register */
  uint32_t BRR;   /* The UART BaudRate Register */
  uint32_t CR1;   /* The UART Configuration Register 1 */
  uint32_t CR2;   /* The UART Configuration Register 2 */
  uint32_t CR3;   /* The UART Configuration Register 3 */
  uint32_t GTPR;  /* The UART Guard Time Register */
} uart_t;

/**
 * @brief The Uart Data Buffer
 * 
 */
typedef struct
{
  uint8_t *ptr;   /* A pointer to the bytes of data */
  uint32_t pos;   /* The current position */
  uint32_t size;  /* The size of the data in the buffer */
  uint8_t state;  /* The state of the buffer (UART_BUFFER_IDLE/UART_BUFFER_BUSY) */
} dataBuffer_t;

#define UART_INT_NUMBER 37

#define UART_BUFFER_IDLE 0
#define UART_BUFFER_BUSY 1

/*Transmit data register
              empty*/
#define UART_TXE_CLR 0xFFFFFF7F
/*Transmission complete*/
#define UART_TC_CLR 0xFFFFFFBF
/*Read data register not
              empty*/
#define UART_RXNE_CLR 0xFFFFFFDF
/*Parity error*/
#define UART_PE_CLR 0xFFFFFFFE
/*Data value*/
#define UART_DR_CLR 0xFFFFFE00
/*STOP bits*/
#define UART_STOP_CLR 0xFFFFCFFF
/*TXE interrupt enable*/
#define UART_TXEIE_CLR 0xFFFFFF7F
/*Parity selection*/
#define UART_PS_CLR 0xFFFFFDFF
/*Word length*/
#define	UART_M_CLR				0xFFFFEFFF

/*Transmit data register
              empty*/
#define UART_TXE_GET 0x00000080
/*Transmission complete*/
#define UART_TC_GET 0x00000040
/*Read data register not
              empty*/
#define UART_RXNE_GET 0x00000020
/*Parity error*/
#define UART_PE_GET 0x00000001

/*USART enable*/
#define UART_UE_SET 0x00002000
/*Parity control enable*/
#define UART_PCE_SET 0x00000400

/*PE interrupt enable*/
#define UART_PEIE_SET 0x00000100
/*TXE interrupt enable*/
#define UART_TXEIE_SET 0x00000080
/*Transmission complete interrupt
              enable*/
#define UART_TCIE_SET 0x00000040
/*RXNE interrupt enable*/
#define UART_RXNEIE_SET 0x00000020
/*IDLE interrupt enable*/
#define UART_IDLEIE_SET 0x00000010
/*Transmitter enable*/
#define UART_TE_SET 0x00000008
/*Receiver enable*/
#define UART_RE_SET 0x00000004
/*Word length*/
#define	UART_M_SET 0x00001000

/*DMA enable transmitter*/
#define UART_DMAT_SET       0x00000080
/*DMA enable receiver*/
#define UART_DMAR_SET       0x00000040

/*RTS enable*/
#define UART_RTSE_CLR 0xFFFFFEFF

#define UART_NO_PRESCALER 0x1

/**
 * @brief The Base Adresses of the UART module
 * 
 */
const uint32_t Uart_Address[UART_NUMBER_OF_MODULES] = {
  0x40013800,
  0x40004400,
  0x40004800
};

#ifndef UART_USE_DMA
static volatile dataBuffer_t txBuffer[UART_NUMBER_OF_MODULES];
static volatile dataBuffer_t rxBuffer[UART_NUMBER_OF_MODULES];
#endif

static volatile txCb_t appTxNotify[UART_NUMBER_OF_MODULES];
static volatile rxCb_t appRxNotify[UART_NUMBER_OF_MODULES];

#ifdef UART_USE_DMA
const volatile uint8_t Uart_DmaTxChannelNumber[UART_NUMBER_OF_MODULES] =
{
  DMA_CH_4,
  DMA_CH_7,
  DMA_CH_2
};
const volatile uint8_t Uart_DmaRxChannelNumber[UART_NUMBER_OF_MODULES] =
{
  DMA_CH_5,
  DMA_CH_6,
  DMA_CH_3
};
#endif

/**
 * @brief The Interrupt Handler for the UART driver
 * 
 * @param uartModule the module number of the UART
 *                 @arg UART1
 *                 @arg UART2
 *                 @arg UART3
 */
static void UART_IRQHandler(uint8_t uartModule)
{
  volatile uart_t* Uart = (volatile uart_t*)Uart_Address[uartModule];
#ifndef UART_USE_DMA
  /* If the TX is Empty */
  if (UART_TXE_GET & Uart->SR)
  {
    /* If there is still data in the buffer */
    if (txBuffer[uartModule].size != txBuffer[uartModule].pos)
    {
      Uart->DR = txBuffer[uartModule].ptr[txBuffer[uartModule].pos++];
    }
    else
    {
      txBuffer[uartModule].ptr = NULL;
      txBuffer[uartModule].size = 0;
      txBuffer[uartModule].pos = 0;
      txBuffer[uartModule].state = UART_BUFFER_IDLE;
      Uart->CR1 &= UART_TXEIE_CLR;
      if (appTxNotify[uartModule])
      {
        appTxNotify[uartModule](uartModule);
      }
    }
  }

  if (UART_RXNE_GET & Uart->SR)
  {
    Uart->SR &= UART_RXNE_CLR;
    /* If there is still data to receive */
    if (UART_BUFFER_BUSY == rxBuffer[uartModule].state)
    {
      rxBuffer[uartModule].ptr[rxBuffer[uartModule].pos] = Uart->DR;
      rxBuffer[uartModule].pos++;
      /* If the data is received successfully */
      if (rxBuffer[uartModule].pos == rxBuffer[uartModule].size)
      {
        rxBuffer[uartModule].ptr = NULL;
        rxBuffer[uartModule].size = 0;
        rxBuffer[uartModule].pos = 0;
        rxBuffer[uartModule].state = UART_BUFFER_IDLE;
        if (appRxNotify[uartModule])
        {
          appRxNotify[uartModule](uartModule);
        }
      }
    }
  }
#else
  /* If the Transmittion is Complete */
  if (UART_TC_GET & Uart->SR)
  {
    Uart->SR &= UART_TC_CLR;
    txBuffer[uartModule].state = UART_BUFFER_IDLE;
    if (appTxNotify[uartModule])
    {
      appTxNotify[uartModule](uartModule);
    }
  }
  else
  {
    rxBuffer[uartModule].state = UART_BUFFER_IDLE;
    if (appRxNotify[uartModule])
    {
      appRxNotify[uartModule](uartModule);
    }
  }
#endif
}


/**
 * @brief Initializes the UART
 *
 * @param baudRate the baud rate of the UART (uint32_t)
 * @param stopBits The number of the stop bits
 *                 @arg UART_ONE_STOP_BIT
 *                 @arg UART_TWO_STOP_BITS
 * @param parity The parity of the transmission
 *                 @arg UART_ODD_PARITY
 *                 @arg UART_EVEN_PARITY
 *                 @arg UART_NO_PARITY
 * @param flowControl the flow control
 *                 @arg UART_FLOW_CONTROL_EN
 *                 @arg UART_FLOW_CONTROL_DIS
 * @param sysClk the clock of the system
 * @param uartModule the module number of the UART
 *                 @arg UART1
 *                 @arg UART2
 *                 @arg UART3
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
Std_ReturnType Uart_Init(uint32_t baudRate, uint32_t stopBits, uint32_t parity, uint32_t flowControl, uint32_t sysClk, uint8_t uartModule)
{
#ifndef UART_USE_DMA
  volatile uart_t* Uart = (volatile uart_t*)Uart_Address[uartModule];
  f64 tmpBaudRate = ((f64)sysClk / ((f64)baudRate * 16.0));
  /* Clear Baudrate Register and set the mantissa */
  Uart->BRR = ((uint32_t)(tmpBaudRate)) << 4;
  /* Setting the fraction */
  Uart->BRR |= (uint32_t)((tmpBaudRate - (f64)((uint32_t)tmpBaudRate))*16.0);
  /* Setting the parity bit */
  if (UART_NO_PARITY == parity)
  {
    Uart->CR1 &= UART_M_CLR;
    Uart->CR1 &= UART_NO_PARITY;
  }
  else
  {
    Uart->CR1 |= UART_M_SET;
    Uart->CR1 |= UART_PCE_SET;
    Uart->CR1 &= UART_PS_CLR;
    Uart->CR1 |= parity;
  }
  /* Setting the stop bit */
  Uart->CR2 &= UART_STOP_CLR;
  Uart->CR2 |= stopBits;
  /* Set the hardware flowcontrol */
  Uart->CR3 &= UART_RTSE_CLR;
  Uart->CR3 |= flowControl;
  Uart->GTPR |= UART_NO_PRESCALER;
  /* Set the buffer states to idle */
  rxBuffer[uartModule].state = UART_BUFFER_IDLE;
  txBuffer[uartModule].state = UART_BUFFER_IDLE;
  /* Enable the UART, Receiver, Transmitter and the Receiver not empty interrupt */
  Uart->CR1 |= UART_UE_SET | UART_RXNEIE_SET | UART_TE_SET | UART_RE_SET;
#else
  volatile uart_t* Uart = (volatile uart_t*)Uart_Address[uartModule];
  f64 tmpBaudRate = ((f64)sysClk / ((f64)baudRate * 16.0));
  /* Tx Configurations */
  dmaPrephCfg_t cfg = 
  {
    .channel = Uart_DmaTxChannelNumber[uartModule],
    .interrupt = DMA_INT_NO_INT,
    .direction = DMA_READ_FROM_MEM,
    .circular = DMA_CIRCULAR_MODE_OFF,
    .prephInc = DMA_PREPH_INC_OFF,
    .memInc = DMA_MEM_INC_ON,
    .prephSize = DMA_PREPH_8_BIT,
    .memSize = DMA_MEM_8_BIT,
    .priority = DMA_PRIORITY_HIGH
  };
  Dma_ConfigurePrephChannel(&cfg);
  /* Rx Configure */
  cfg.channel = Uart_DmaRxChannelNumber[uartModule];
  cfg.interrupt = DMA_INT_TRANSFER_COMPLETE;
  cfg.direction = DMA_READ_FROM_PREPH;
  Dma_ConfigurePrephChannel(&cfg);
  switch(uartModule)
  {
    case UART1:
      Dma_SetCallBack(Uart_DmaRxChannelNumber[uartModule], USART1_DMA_IRQHandler);
      break;
    case UART2:
      Dma_SetCallBack(Uart_DmaRxChannelNumber[uartModule], USART2_DMA_IRQHandler);
      break;
    case UART3:
      Dma_SetCallBack(Uart_DmaRxChannelNumber[uartModule], USART3_DMA_IRQHandler);
      break;
  }
  /* Clear Baudrate Register and set the mantissa */
  Uart->BRR = ((uint32_t)(tmpBaudRate)) << 4;
  /* Setting the fraction */
  Uart->BRR |= (uint32_t)((tmpBaudRate - (f64)((uint32_t)tmpBaudRate))*16.0);
  /* Setting the parity bit */
  if (UART_NO_PARITY == parity)
  {
    Uart->CR1 &= UART_M_CLR;
    Uart->CR1 &= UART_NO_PARITY;
  }
  else
  {
    Uart->CR1 |= UART_M_SET;
    Uart->CR1 |= UART_PCE_SET;
    Uart->CR1 &= UART_PS_CLR;
    Uart->CR1 |= parity;
  }
  /* Setting the stop bit */
  Uart->CR2 &= UART_STOP_CLR;
  Uart->CR2 |= stopBits;
  /* Set the hardware flowcontrol */
  Uart->CR3 &= UART_RTSE_CLR;
  Uart->CR3 |= flowControl | UART_DMAT_SET | UART_DMAR_SET;
  Uart->GTPR |= UART_NO_PRESCALER;
  /* Set the buffer states to idle */
  rxBuffer[uartModule].state = UART_BUFFER_IDLE;
  txBuffer[uartModule].state = UART_BUFFER_IDLE;
  /* Enable the UART, Receiver, Transmitter and the Receiver not empty interrupt */
  Uart->CR1 |= UART_UE_SET | UART_TCIE_SET | UART_TE_SET | UART_RE_SET;
#endif
  return E_OK;
}

/**
 * @brief Sends data through the UART
 *
 * @param data The data to send
 * @param length the length of the data in bytes
 * @param uartModule the module number of the UART
 *                 @arg UART1
 *                 @arg UART2
 *                 @arg UART3
 * @return Std_ReturnType A Status
 *                  E_OK: If the driver is ready to send
 *                  E_NOT_OK: If the driver can't send data right now
 */
Std_ReturnType Uart_Send(uint8_t *data, uint16_t length, uint8_t uartModule)
{
  Std_ReturnType error = E_NOT_OK;
  volatile uart_t* Uart = (volatile uart_t*)Uart_Address[uartModule];
  /*If there is valid data and length and the TX buffer is idle*/
  if (data && (length > 0) && txBuffer[uartModule].state == UART_BUFFER_IDLE)
  {
#ifndef UART_USE_DMA
    txBuffer[uartModule].state = UART_BUFFER_BUSY;
    txBuffer[uartModule].ptr = data;
    txBuffer[uartModule].pos = 0;
    txBuffer[uartModule].size = length;

    Uart->DR = txBuffer[uartModule].ptr[txBuffer[uartModule].pos++];
    Uart->CR1 |= UART_TXEIE_SET;
    error = E_OK;
#else
  txBuffer[uartModule].state = UART_BUFFER_BUSY;
  Dma_TransferPrephData(Uart_DmaTxChannelNumber[uartModule],(uint32_t)(&(Uart->DR)), (uint32_t)data, length);
  error = E_OK;
#endif
  }
  return error;
}
/**
 * @brief Receives data through the UART
 *
 * @param data The buffer to receive data in
 * @param length the length of the data in bytes
 * @param uartModule the module number of the UART
 *                 @arg UART1
 *                 @arg UART2
 *                 @arg UART3
 * @return Std_ReturnType A Status
 *                  E_OK: If the driver is ready to receive
 *                  E_NOT_OK: If the driver can't receive data right now
 */
Std_ReturnType Uart_Receive(uint8_t *data, uint16_t length, uint8_t uartModule)
{
  Std_ReturnType error = E_NOT_OK;
  volatile uart_t* Uart = (volatile uart_t*)Uart_Address[uartModule];
  /* If the RX buffer is idle */
  if (rxBuffer[uartModule].state == UART_BUFFER_IDLE)
  {
#ifndef UART_USE_DMA
    rxBuffer[uartModule].ptr = data;
    rxBuffer[uartModule].size = length;
    rxBuffer[uartModule].pos = 0;
    rxBuffer[uartModule].state = UART_BUFFER_BUSY;
    error = E_OK;
#else
  rxBuffer[uartModule].state = UART_BUFFER_BUSY;
  Dma_TransferPrephData(Uart_DmaRxChannelNumber[uartModule],(uint32_t)(&(Uart->DR)), (uint32_t)data, length);
  error = E_OK;
#endif
  }
  return error;
}
/**
 * @brief Sets the callback function that will be called when transmission is
 * completed
 *
 * @param func the callback function
 * @param uartModule the module number of the UART
 *                 @arg UART1
 *                 @arg UART2
 *                 @arg UART3
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
Std_ReturnType Uart_SetTxCb(txCb_t func, uint8_t uartModule)
{
  appTxNotify[uartModule] = func;
  return E_OK;
}
/**
 * @brief Sets the callback function that will be called when receive is
 * completed
 *
 * @param func the callback function
 * @param uartModule the module number of the UART
 *                 @arg UART1
 *                 @arg UART2
 *                 @arg UART3
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
Std_ReturnType Uart_SetRxCb(rxCb_t func, uint8_t uartModule)
{
  appRxNotify[uartModule] = func;
  return E_OK;
}



/**
 * @brief The UART 1 Handler
 * 
 */
void USART1_IRQHandler(void)
{
  UART_IRQHandler(UART1);
}
/**
 * @brief The UART 2 Handler
 * 
 */
void USART2_IRQHandler(void)
{
  UART_IRQHandler(UART2);
}
/**
 * @brief The UART 3 Handler
 *
 */
void USART3_IRQHandler(void)
{
  UART_IRQHandler(UART3);
}

/**
 * @brief The UART 1 DMA Handler
 * 
 */
void USART1_DMA_IRQHandler(void)
{
  UART_IRQHandler(UART1);
}
/**
 * @brief The UART 2 DMA Handler
 * 
 */
void USART2_DMA_IRQHandler(void)
{
  UART_IRQHandler(UART2);
}
/**
 * @brief The UART 3 DMA Handler
 *
 */
void USART3_DMA_IRQHandler(void)
{
  UART_IRQHandler(UART3);
}