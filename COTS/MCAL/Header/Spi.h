/**
 * @file Spi.h
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the user interface for the SPI driver
 * @version 0.1
 * @date 2020-04-22
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef SPI_H_
#define SPI_H_
/* The SPI Modules */
#define SPI1                                 0
#define SPI2                                 1

/* The SPI Baudrate */
#define SPI_BAUDRATE_FCPU_DIV_2              0x00000000
#define SPI_BAUDRATE_FCPU_DIV_4              0x00000008
#define SPI_BAUDRATE_FCPU_DIV_8              0x00000010
#define SPI_BAUDRATE_FCPU_DIV_16             0x00000018
#define SPI_BAUDRATE_FCPU_DIV_32             0x00000020
#define SPI_BAUDRATE_FCPU_DIV_64             0x00000028
#define SPI_BAUDRATE_FCPU_DIV_128            0x00000030
#define SPI_BAUDRATE_FCPU_DIV_264            0x00000038

/* The SPI Direction */
#define SPI_LSB_FIRST                        0x00000080
#define SPI_MSB_FIRST                        0x00000000

/* The SPI Mode */
#define SPI_MODE_MASTER                      0x00000004
#define SPI_MODE_SLAVE                       0x00000000

/* The SPI Clock Polarity */
#define SPI_CLK_POLARITY_IDLE_0              0x00000000
#define SPI_CLK_POLARITY_IDLE_1              0x00000002

/* The SPI Clock Phase */
#define SPI_CLK_PHASE_FIRST                  0x00000000
#define SPI_CLK_PHASE_SECOND                 0x00000001



typedef void (*txCb_t)(void);
typedef void (*rxCb_t)(void);

/**
 * @brief SPI configuration type
 * 
 */
typedef struct
{
    uint16_t mode;                  /* SPI_MODE_x */
    uint16_t direction;             /* SPI_x_FIRST */           /*Most*/
    uint16_t polarity;              /* SPI_CLK_POLARITY_x */
    uint16_t phase;                 /* SPI_CLK_PHASE_x */
    uint16_t baudrate;              /* SPI_BAUDRATE_FCPU_DIV_x */
}spiCfg_t;


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
extern Std_ReturnType Spi_Init(spiCfg_t* spiCfg, uint8_t spiModule);

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
extern Std_ReturnType Spi_Send(uint8_t *data, uint16_t length, uint8_t spiModule);

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
extern Std_ReturnType Spi_Receive(uint8_t *data, uint16_t length, uint8_t spiModule);

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
extern Std_ReturnType Spi_SetTxCb(txCb_t func, uint8_t spiModule);

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
extern Std_ReturnType Spi_SetRxCb(rxCb_t func, uint8_t spiModule);
#endif
