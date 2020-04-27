/**
 * @file Spi.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the implementation for the SPI driver
 * @version 0.1
 * @date 2020-04-22
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"
#include "Spi.h"

#define SPI_NUMBER_OF_MODULES               2

typedef struct
{
    uint32_t CR1;
    uint32_t CR2;
    uint32_t SR;
    uint32_t DR;
    uint32_t CRCPR;
    uint32_t RXCRCR;
    uint32_t TXCRCR;
    uint32_t I2SCFGR;
    uint32_t I2SPR;
} spi_t;

/**
 * @brief The Spi Data Buffer
 * 
 */
typedef struct
{
  uint8_t *ptr;   /* A pointer to the bytes of data */
  uint32_t pos;   /* The current position */
  uint32_t size;  /* The size of the data in the buffer */
  uint8_t state;  /* The state of the buffer (SPI_BUFFER_IDLE/SPI_BUFFER_BUSY) */
} dataBuffer_t;

#define SPI_BUFFER_IDLE 0
#define SPI_BUFFER_BUSY 1

/*Transmit data register
              empty*/
#define SPI_TXE_CLR 0xFFFFFFFD
/*Read data register not
              empty*/
#define SPI_RXNE_CLR 0xFFFFFFFE
/*Data value*/
#define SPI_DR_CLR 0xFFFF0000
/*TXE interrupt enable*/
#define SPI_TXEIE_CLR 0xFFFFFF7F

/*Transmit data register
              empty*/
#define SPI_TXE_GET 0x00000002
/*Read data register not
              empty*/
#define SPI_RXNE_GET 0x00000001

/*USART enable*/
#define SPI_SPE_SET 0x00000040

/*TXE interrupt enable*/
#define SPI_TXEIE_SET 0x00000080
/*RXNE interrupt enable*/
#define SPI_RXNEIE_SET 0x00000040

#define SPI_RXNEIE_CLR 0xFFFFFFBF

#define SPI_BIDIRECTION_SET   0x00004000


/**
 * @brief The Base Adresses of the SPI module
 * 
 */
const uint32_t Spi_Address[SPI_NUMBER_OF_MODULES] = {
  0x40013000,
  0x40003800
};

static volatile dataBuffer_t txBuffer[SPI_NUMBER_OF_MODULES];
static volatile dataBuffer_t rxBuffer[SPI_NUMBER_OF_MODULES];

static volatile txCb_t appTxNotify[SPI_NUMBER_OF_MODULES];
static volatile rxCb_t appRxNotify[SPI_NUMBER_OF_MODULES];


/**
 * @brief The Interrupt Handler for the SPI driver
 * 
 * @param spiModule the module number of the SPI
 *                 @arg SPI1
 *                 @arg SPI2
 */
static void SPI_IRQHandler(uint8_t spiModule)
{
  volatile spi_t* Spi = (volatile spi_t*)Spi_Address[spiModule];
  /* If the TX is Empty */
  if (SPI_TXE_GET & Spi->SR)
  {
    /* If there is still data in the buffer */
    if (txBuffer[spiModule].size != txBuffer[spiModule].pos)
    {
      Spi->DR = txBuffer[spiModule].ptr[txBuffer[spiModule].pos++];
    }
    else
    {
      txBuffer[spiModule].ptr = NULL;
      txBuffer[spiModule].size = 0;
      txBuffer[spiModule].pos = 0;
      txBuffer[spiModule].state = SPI_BUFFER_IDLE;
      Spi->CR2 &= SPI_TXEIE_CLR;
      if (appTxNotify[spiModule])
      {
        appTxNotify[spiModule]();
      }
    }
  }

  if (SPI_RXNE_GET & Spi->SR)
  {
    Spi->SR &= SPI_RXNE_CLR;
    /* If there is still data to receive */
    if (SPI_BUFFER_BUSY == rxBuffer[spiModule].state)
    {
      rxBuffer[spiModule].ptr[rxBuffer[spiModule].pos] = Spi->DR;
      rxBuffer[spiModule].pos++;
      /* If the data is received successfully */
      if (rxBuffer[spiModule].pos == rxBuffer[spiModule].size)
      {
        rxBuffer[spiModule].ptr = NULL;
        rxBuffer[spiModule].size = 0;
        rxBuffer[spiModule].pos = 0;
        rxBuffer[spiModule].state = SPI_BUFFER_IDLE;
        Spi->CR2 &= SPI_RXNEIE_CLR;
        if (appRxNotify[spiModule])
        {
          appRxNotify[spiModule]();
        }
      }
    }
  }
}


/**
 * @brief Initializes the SPI
 *
 * @param spiCfg The SPI Configurations
 * @param spiModule the module number of the SPI
 *                 @arg SPI1
 *                 @arg SPI2
 *
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
Std_ReturnType Spi_Init(spiCfg_t* spiCfg, uint8_t spiModule)
{
  volatile spi_t* Spi = (volatile spi_t*)Spi_Address[spiModule];
  Spi->CR1 = SPI_SPE_SET | spiCfg->direction | spiCfg->baudrate | spiCfg->mode | spiCfg->polarity | spiCfg->phase;
  /* Set the buffer states to idle */
  rxBuffer[spiModule].state = SPI_BUFFER_IDLE;
  txBuffer[spiModule].state = SPI_BUFFER_IDLE;
  /* Enable the SPI, Receiver, Transmitter and the Receiver not empty interrupt */
  return E_OK;
}

/**
 * @brief Sends data through the SPI
 *
 * @param data The data to send
 * @param length the length of the data in bytes
 * @param spiModule the module number of the SPI
 *                 @arg SPI1
 *                 @arg SPI2
 * @return Std_ReturnType A Status
 *                  E_OK: If the driver is ready to send
 *                  E_NOT_OK: If the driver can't send data right now
 */
Std_ReturnType Spi_Send(uint8_t *data, uint16_t length, uint8_t spiModule)
{
  Std_ReturnType error = E_NOT_OK;
  volatile spi_t* Spi = (volatile spi_t*)Spi_Address[spiModule];
  /*If there is valid data and length and the TX buffer is idle*/
  if (data && (length > 0) && txBuffer[spiModule].state == SPI_BUFFER_IDLE)
  {
    txBuffer[spiModule].state = SPI_BUFFER_BUSY;
    txBuffer[spiModule].ptr = data;
    txBuffer[spiModule].pos = 0;
    txBuffer[spiModule].size = length;

    Spi->DR = txBuffer[spiModule].ptr[txBuffer[spiModule].pos++];
    Spi->CR2 |= SPI_TXEIE_SET;
    error = E_OK;
  }
  return error;
}
/**
 * @brief Receives data through the SPI
 *
 * @param data The buffer to receive data in
 * @param length the length of the data in bytes
 * @param spiModule the module number of the SPI
 *                 @arg SPI1
 *                 @arg SPI2
 * @return Std_ReturnType A Status
 *                  E_OK: If the driver is ready to receive
 *                  E_NOT_OK: If the driver can't receive data right now
 */
Std_ReturnType Spi_Receive(uint8_t *data, uint16_t length, uint8_t spiModule)
{
  Std_ReturnType error = E_NOT_OK;
  volatile spi_t* Spi = (volatile spi_t*)Spi_Address[spiModule];
  /* If the RX buffer is idle */
  if (rxBuffer[spiModule].state == SPI_BUFFER_IDLE)
  {
    rxBuffer[spiModule].ptr = data;
    rxBuffer[spiModule].size = length;
    rxBuffer[spiModule].pos = 0;
    rxBuffer[spiModule].state = SPI_BUFFER_BUSY;
    Spi->CR2 = SPI_RXNEIE_SET;
    error = E_OK;
  }
  return error;
}
/**
 * @brief Sets the callback function that will be called when transmission is
 * completed
 * @param spiModule the module number of the SPI
 *                 @arg SPI1
 *                 @arg SPI2
 * @param func the callback function
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
Std_ReturnType Spi_SetTxCb(txCb_t func, uint8_t spiModule)
{
  appTxNotify[spiModule] = func;
  return E_OK;
}
/**
 * @brief Sets the callback function that will be called when receive is
 * completed
 *
 * @param func the callback function
 * @param spiModule the module number of the SPI
 *                 @arg SPI1
 *                 @arg SPI2
 *                 @arg SPI3
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
Std_ReturnType Spi_SetRxCb(rxCb_t func, uint8_t spiModule)
{
  appRxNotify[spiModule] = func;
  return E_OK;
}

/**
 * @brief The SPI 1 Handler
 * 
 */
void SPI1_IRQHandler(void)
{
  SPI_IRQHandler(SPI1);
}
/**
 * @brief The SPI 2 Handler
 * 
 */
void SPI2_IRQHandler(void)
{
  SPI_IRQHandler(SPI2);
}
