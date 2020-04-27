/**
 * @file Dma.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the implementation for the DMA driver
 * @version 0.1
 * @date 2020-04-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"
#include "Dma.h"

#define DMA_NUMBER_OF_CHANNELS                              7
#define DMA_CH_EN                                           1
#define DMA_CH_DIS                                          0xFFFFFFFE

#define DMA_BASE_ADDRESS                                    0x40020000

/**
 * @brief DMA Interrupt Clear Masks
 * 
 */
#define DMA_CH1_INT_CLR                                     0x00000001
#define DMA_CH2_INT_CLR                                     0x00000010
#define DMA_CH3_INT_CLR                                     0x00000100
#define DMA_CH4_INT_CLR                                     0x00001000
#define DMA_CH5_INT_CLR                                     0x00010000
#define DMA_CH6_INT_CLR                                     0x00100000
#define DMA_CH7_INT_CLR                                     0x01000000

/**
 * @brief DMA Configurations memory to memory
 * 
 */
#define DMA_MEM_2_MEM_ON                    0x4000
#define DMA_MEM_2_MEM_OFF                   0x0000



#define DMA                                               ((volatile dma_t*)(DMA_BASE_ADDRESS))

/**
 * @brief The DMA Peripheral Channel Registers
 * 
 */
typedef struct
{
    uint32_t CCR;       /*The Channel Configuration Register*/
    uint32_t CNDT;      /*The Number Of Data Blocks To Transfer*/
    uint32_t CPAR;      /*The Peripheral Address*/
    uint32_t CMAR;      /*The Memory Address*/
    uint32_t RESERVED;
}dmaChannel_t;

/**
 * @brief The DMA prephiral registers
 * 
 */
typedef struct
{
    uint32_t ISR;                                           /*The Interrupt Status Register*/
    uint32_t IFCR;                                          /*The Interrupt Flag Clear Register*/
    dmaChannel_t CH[DMA_NUMBER_OF_CHANNELS];        /*The Channels Configurations*/
}dma_t;


static volatile dmaCb_t Dma_callBack[DMA_NUMBER_OF_CHANNELS];

/**
 * @brief Configures A DMA Prephiral Channel
 * 
 * @param cfg the configurations for the DMA channel
 * @return Std_ReturnType a status 
 *                  E_OK If the function was executed successfully
 *                  E_NOT_OK If the function was not executed successfully
 */
Std_ReturnType Dma_ConfigurePrephChannel(dmaPrephCfg_t* cfg)
{
    /* Disable The Channel */
    DMA->CH[cfg->channel].CCR &= DMA_CH_DIS;
    DMA->CH[cfg->channel].CCR = cfg->interrupt | cfg->direction | cfg->circular | cfg->prephInc | cfg->memInc | cfg->prephSize | cfg->memSize | cfg->priority | DMA_MEM_2_MEM_OFF;
    return E_OK;
}

/**
 * @brief Configures A DMA Memory To Memory Channel
 * 
 * @param cfg the configurations for the DMA channel
 * @return Std_ReturnType a status 
 *                  E_OK If the function was executed successfully
 *                  E_NOT_OK If the function was not executed successfully
 */
Std_ReturnType Dma_ConfigureMem2MemChannel(dmaMem2MemCfg_t* cfg)
{
    /* Disable The Channel */
    DMA->CH[cfg->channel].CCR &= DMA_CH_DIS;
    DMA->CH[cfg->channel].CCR = cfg->interrupt | DMA_READ_FROM_PREPH | DMA_CIRCULAR_MODE_OFF | cfg->srcInc | DMA_MEM_INC_ON | cfg->size | (cfg->size)<<2 | cfg->priority | DMA_MEM_2_MEM_ON;
    return E_OK;
}

/**
 * @brief Transfers Data Using A Certain DMA Channel
 * 
 * @param channelNumber The DMA Channel Number
 *                  @arg DMA_CH_x
 * @param src The Address Of The Source
 * @param dest The Address Of The Destinaion
 * @param nBlocks The Number Of Blocks To Transfer
 * @return Std_ReturnType 
 *                  E_OK If the function was executed successfully
 *                  E_NOT_OK If the function was not executed successfully
 */
extern Std_ReturnType Dma_TransferMem2MemData(uint8_t channelNumber, uint32_t src, uint32_t dest, uint16_t nBlocks)
{
    /* Disable The Channel */
    DMA->CH[channelNumber].CCR &= DMA_CH_DIS;
    DMA->CH[channelNumber].CPAR = src;
    DMA->CH[channelNumber].CMAR = dest;
    DMA->CH[channelNumber].CNDT = nBlocks;
    /* Enable The Channel */
    DMA->CH[channelNumber].CCR |= DMA_CH_EN;
    return E_OK;
}

/**
 * @brief Transfers Data Using A Certain DMA Channel
 * 
 * @param channelNumber The DMA Channel Number
 *                  @arg DMA_CH_x
 * @param preph The Address Of The prephiral
 * @param mem The Address Of The memory
 * @param nBlocks The Number Of Blocks To Transfer
 * @return Std_ReturnType 
 *                  E_OK If the function was executed successfully
 *                  E_NOT_OK If the function was not executed successfully
 */
extern Std_ReturnType Dma_TransferPrephData(uint8_t channelNumber, uint32_t preph, uint32_t mem, uint16_t nBlocks)
{
    /* Disable The Channel */
    DMA->CH[channelNumber].CCR &= DMA_CH_DIS;
    DMA->CH[channelNumber].CPAR = preph;
    DMA->CH[channelNumber].CMAR = mem;
    DMA->CH[channelNumber].CNDT = nBlocks;
    /* Enable The Channel */
    DMA->CH[channelNumber].CCR |= DMA_CH_EN;
    return E_OK;    
}

/**
 * @brief Sets The CallBack Function For A Certian Channel
 * 
 * @param channelNumber The DMA Channel Number
 *                  @arg DMA_CH_x
 * @param callBack The CallBack Function
 * @return Std_ReturnType 
 *                  E_OK If the function was executed successfully
 *                  E_NOT_OK If the function was not executed successfully
 */
extern Std_ReturnType Dma_SetCallBack(uint8_t channelNumber, dmaCb_t callBack)
{
    Dma_callBack[channelNumber] = callBack;
    return E_OK;
}

/**
 * @brief Channel 1 Interrupt Handler
 * 
 */
void DMA1_Channel1_IRQHandler(void)
{
    if(Dma_callBack[DMA_CH_1])
    {
        Dma_callBack[DMA_CH_1]();
    }
    DMA->IFCR = DMA_CH1_INT_CLR;
}

/**
 * @brief Channel 2 Interrupt Handler
 * 
 */
void DMA1_Channel2_IRQHandler(void)
{
    if(Dma_callBack[DMA_CH_2])
    {
        Dma_callBack[DMA_CH_2]();
    }
    DMA->IFCR = DMA_CH2_INT_CLR;
}

/**
 * @brief Channel 3 Interrupt Handler
 * 
 */
void DMA1_Channe3_IRQHandler(void)
{
    if(Dma_callBack[DMA_CH_3])
    {
        Dma_callBack[DMA_CH_3]();
    }
    DMA->IFCR = DMA_CH3_INT_CLR;
}

/**
 * @brief Channel 4 Interrupt Handler
 * 
 */
void DMA1_Channel4_IRQHandler(void)
{
    if(Dma_callBack[DMA_CH_4])
    {
        Dma_callBack[DMA_CH_4]();
    }
    DMA->IFCR = DMA_CH4_INT_CLR;
}

/**
 * @brief Channel 5 Interrupt Handler
 * 
 */
void DMA1_Channel5_IRQHandler(void)
{
    if(Dma_callBack[DMA_CH_5])
    {
        Dma_callBack[DMA_CH_5]();
    }
    DMA->IFCR = DMA_CH5_INT_CLR;
}

/**
 * @brief Channel 6 Interrupt Handler
 * 
 */
void DMA1_Channel6_IRQHandler(void)
{
    if(Dma_callBack[DMA_CH_6])
    {
        Dma_callBack[DMA_CH_6]();
    }
    DMA->IFCR = DMA_CH6_INT_CLR;
}

/**
 * @brief Channel 7 Interrupt Handler
 * 
 */
void DMA1_Channel7_IRQHandler(void)
{
    if(Dma_callBack[DMA_CH_7])
    {
        Dma_callBack[DMA_CH_7]();
    }
    DMA->IFCR = DMA_CH7_INT_CLR;
}
