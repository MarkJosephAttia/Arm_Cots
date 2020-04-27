/**
 * @file Nokia.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the implementation for the Nokia 5110 LCD driver
 * @version 0.1
 * @date 2020-04-22
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"
#include "Gpio.h"
#include "Rcc.h"
#include "HRcc.h"
#include "Spi.h"
#include "Nvic.h"
#include "Nokia.h"
#include "Sched.h"

#define NOKIA_NOT_INITIALIZED                   0
#define NOKIA_INITIALIZED                       1

#define NOKIA_NUMBER_OF_INIT_BYTES              6

#define NOKIA_MSK_X                             0x80
#define NOKIA_MSK_Y                             0x40

#define NOKIA_POS_SIZE                          0x02

typedef enum
{
    idle_p,
    init_p,
    write_p
}nokiaProcess_t;

typedef struct
{
    uint8_t* data;
    uint16_t size;
    uint16_t pos;
}nokiaBuffer_t;

typedef struct
{
    uint32_t dPin;
    uint32_t dPort;
    uint32_t clkPin;
    uint32_t clkPort;
}nokiaSpi_t;

extern const nokia_t Nokia_nokia;
static volatile nokiaProcess_t Nokia_process  = idle_p;
static volatile nokiaBuffer_t Nokia_buffer;
static volatile uint8_t Nokia_isInitialized = NOKIA_NOT_INITIALIZED;
static const uint8_t Nokia_initSeq[NOKIA_NUMBER_OF_INIT_BYTES] = {0x21, 0x06, 0x13, 0xBE, 0x20, 0x0C};
static volatile nokiaCb_t Nokia_cb = NULL;

/**
 * @brief The Nokia LCD initialization
 * 
 * @return Std_ReturnType 
 *              E_OK : If the initialization started successfully
 *              E_NOT_OK : If the initialization is not able to start right now
 */
Std_ReturnType Nokia_Init(void)
{
	gpio_t gpio;
    nokiaSpi_t Nokia_spi = {
        .dPin = GPIO_PIN_7,
        .dPort = GPIO_PORTA,
        .clkPin = GPIO_PIN_5,
        .clkPort = GPIO_PORTA
    };
	Std_ReturnType error = E_NOT_OK;
    spiCfg_t spiCfg = {
        .mode = SPI_MODE_MASTER,
        .direction = SPI_MSB_FIRST,
        .polarity = SPI_CLK_POLARITY_IDLE_0,
        .phase = SPI_CLK_PHASE_SECOND,
        .baudrate = SPI_BAUDRATE_FCPU_DIV_16
    };
	/* If there were no processes execution at the moment */
	if(idle_p == Nokia_process)
	{
		/* Setup the GPIO pins and their clock */
		gpio.mode = GPIO_MODE_GP_OUTPUT_PP;
		gpio.speed = GPIO_SPEED_50_MHZ;
		gpio.pins = Nokia_nokia.rstPin;
		gpio.port = Nokia_nokia.rstPort;
		HRcc_EnPortClock(Nokia_nokia.rstPort);
		Gpio_InitPins(&gpio);
		gpio.pins = Nokia_nokia.cePin;
		gpio.port = Nokia_nokia.cePort;
		HRcc_EnPortClock(Nokia_nokia.cePort);
		Gpio_InitPins(&gpio);
		gpio.pins = Nokia_nokia.dcPin;
		gpio.port = Nokia_nokia.dcPort;
		HRcc_EnPortClock(Nokia_nokia.dcPort);
		Gpio_InitPins(&gpio);
        
        switch(Nokia_nokia.spiModule)
        {
            case SPI1:
                Rcc_SetApb2PeriphClockState(RCC_IOPA_CLK_EN, RCC_PERIPH_CLK_ON);
                Nokia_spi.dPin = GPIO_PIN_7;
                Nokia_spi.dPort = GPIO_PORTA;
                Nokia_spi.clkPin = GPIO_PIN_5;
                Nokia_spi.clkPort = GPIO_PORTA;
                Rcc_SetApb2PeriphClockState(RCC_SPI1_CLK_EN, RCC_PERIPH_CLK_ON);
                Nvic_EnableInterrupt(NVIC_IRQNUM_SPI1);
                break;
            case SPI2:
                Rcc_SetApb2PeriphClockState(RCC_IOPB_CLK_EN, RCC_PERIPH_CLK_ON);
                Nokia_spi.dPin = GPIO_PIN_15;
                Nokia_spi.dPort = GPIO_PORTB;
                Nokia_spi.clkPin = GPIO_PIN_13;
                Nokia_spi.clkPort = GPIO_PORTB;
                Rcc_SetApb1PeriphClockState(RCC_SPI2_CLK_EN, RCC_PERIPH_CLK_ON);
                Nvic_EnableInterrupt(NVIC_IRQNUM_SPI2);
                break;
        }

        Spi_SetTxCb(NULL, Nokia_nokia.spiModule);

        gpio.mode = GPIO_MODE_AF_OUTPUT_PP;
		gpio.pins = Nokia_spi.dPin;
		gpio.port = Nokia_spi.dPort;
        HRcc_EnPortClock(Nokia_spi.dPort);
		Gpio_InitPins(&gpio);
        gpio.pins = Nokia_spi.clkPin;
		gpio.port = Nokia_spi.clkPort;
        HRcc_EnPortClock(Nokia_spi.clkPort);
		Gpio_InitPins(&gpio);


		Gpio_WritePin(Nokia_nokia.cePort, Nokia_nokia.cePin, GPIO_PIN_RESET);

		Spi_Init(&spiCfg, Nokia_nokia.spiModule);

		Nokia_process  = init_p;
		error = E_OK;
	}
	return error;
}

/**
 * @brief Writes a string on a specific location on the lcd display
 * 
 * @param str the string to write
 * @param x the location on the x-axis
 * @param y the location on the y-axis
 * @return Std_ReturnType 
 *              E_OK : If the writing started successfully
 *              E_NOT_OK : If the write operation is not able to start right now
 */
Std_ReturnType Nokia_WriteString(uint8_t* str, uint16_t size, uint16_t x, uint16_t y)
{
	Std_ReturnType error = E_NOT_OK;
	/* If the CLCD was initialized and there where no current processes executing */
	if(idle_p == Nokia_process && NOKIA_INITIALIZED == Nokia_isInitialized)
	{
        Nokia_buffer.data = str;
        Nokia_buffer.size = size;
        Nokia_buffer.pos = ((x | NOKIA_MSK_X)<<8) | (y | NOKIA_MSK_Y);
		    Nokia_process = write_p;
		error = E_OK;
	}
	return error;
}
/**
 * @brief Sets the application notify callback function
 * 
 * @param func The function to set
 * @return Std_ReturnType 
 *              E_OK : If the function was executed successfully
 *              E_NOT_OK : If the function was not executed successfully
 */
Std_ReturnType Nokia_SetAppNotify(nokiaCb_t func)
{
    Nokia_cb = func;
    return E_OK;
}

/**
 * @brief The initialization process
 * 
 * @return Std_ReturnType 
 * 				E_OK : If executed successfully
 * 				E_NOT_OK : If it failed to execute
 */
static Std_ReturnType Nokia_InitProcess(void)
{
	static uint8_t counter;
    Gpio_WritePin(Nokia_nokia.dcPort, Nokia_nokia.dcPin, GPIO_PIN_RESET);
    switch(counter++)
    {
        case 0:
            Gpio_WritePin(Nokia_nokia.rstPort, Nokia_nokia.rstPin, GPIO_PIN_SET);
            break;
        case 2:
            Gpio_WritePin(Nokia_nokia.rstPort, Nokia_nokia.rstPin, GPIO_PIN_RESET);
            break;
        case 4:
            Gpio_WritePin(Nokia_nokia.rstPort, Nokia_nokia.rstPin, GPIO_PIN_SET);
            break;
        case 6:
            Spi_SetTxCb(NULL, Nokia_nokia.spiModule);
            Spi_Send((uint8_t*)Nokia_initSeq, NOKIA_NUMBER_OF_INIT_BYTES, Nokia_nokia.spiModule);
            counter = 0;
            Nokia_process = idle_p;
            Nokia_isInitialized = NOKIA_INITIALIZED;
            break;
    }
	return E_OK;
}

/**
 * @brief The write process
 * 
 * @return Std_ReturnType 
 * 				E_OK : If executed successfully
 * 				E_NOT_OK : If it failed to execute
 */
static Std_ReturnType Nokia_WriteProcess(void)
{
	static uint8_t counter;
    switch(counter++)
    {
        case 0:
            Spi_SetTxCb(NULL, Nokia_nokia.spiModule);
            Gpio_WritePin(Nokia_nokia.dcPort, Nokia_nokia.dcPin, GPIO_PIN_RESET);
            Spi_Send((uint8_t*)&(Nokia_buffer.pos), NOKIA_POS_SIZE, Nokia_nokia.spiModule);
            break;
        case 1:
            Spi_SetTxCb(Nokia_cb, Nokia_nokia.spiModule);
            Gpio_WritePin(Nokia_nokia.dcPort, Nokia_nokia.dcPin, GPIO_PIN_SET);
            Spi_Send(Nokia_buffer.data, Nokia_buffer.size, Nokia_nokia.spiModule);
            counter = 0;
            Nokia_process = idle_p;
            break;
    }
	return E_OK;
}

/**
 * @brief The running task that have to come every 1 milli second
 * 
 */
static void Nokia_Task(void)
{
	switch(Nokia_process)
	{
		case idle_p:
			break;
		case init_p:
			Nokia_InitProcess();
			break;
		case write_p:
			Nokia_WriteProcess();
			break;
	}
}

const task_t Nokia_task = {Nokia_Task, 2};
