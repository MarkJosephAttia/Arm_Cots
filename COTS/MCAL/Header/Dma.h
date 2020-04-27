/**
 * @file Dma.h
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the user interface for the DMA driver
 * @version 0.1
 * @date 2020-04-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef DMA_H
#define DMA_H

/**
 * @brief DMA Configuration for Channels
 * 
 */
#define DMA_CH_1                                0
#define DMA_CH_2                                1
#define DMA_CH_3                                2
#define DMA_CH_4                                3
#define DMA_CH_5                                4
#define DMA_CH_6                                5
#define DMA_CH_7                                6

/**
 * @brief DMA Configuration for Interrupts
 * 
 */
#define DMA_INT_NO_INT                          0x0
#define DMA_INT_TRANSFER_COMPLETE               0x2
#define DMA_INT_HALF_TRANSFER                   0x4
#define DMA_INT_TRANSFER_ERROR                  0x8

/**
 * @brief DMA Configurations for direction
 * 
 */
#define DMA_READ_FROM_PREPH                     0x00
#define DMA_READ_FROM_MEM                       0x10

/**
 * @brief DMA Configurations for circular mode
 * 
 */
#define DMA_CIRCULAR_MODE_ON                    0x20
#define DMA_CIRCULAR_MODE_OFF                   0x00

/**
 * @brief DMA Configurations for prephiral increment
 * 
 */
#define DMA_PREPH_INC_ON                    0x40
#define DMA_PREPH_INC_OFF                   0x00

/**
 * @brief DMA Configurations for source increment
 * 
 */
#define DMA_SRC_INC_ON                      0x40
#define DMA_SRC_INC_OFF                     0x00

/**
 * @brief DMA Configurations for memory increment
 * 
 */
#define DMA_MEM_INC_ON                    0x80
#define DMA_MEM_INC_OFF                   0x00

/**
 * @brief DMA Configurations for prephiral size
 * 
 */
#define DMA_PREPH_8_BIT                    0x000
#define DMA_PREPH_16_BIT                   0x100
#define DMA_PREPH_32_BIT                   0x200

/**
 * @brief DMA Configurations for memory size
 * 
 */
#define DMA_MEM_8_BIT                    0x000
#define DMA_MEM_16_BIT                   0x400
#define DMA_MEM_32_BIT                   0x800

/**
 * @brief DMA Configurations for Memory to Memory size
 * 
 */
#define DMA_8_BIT                    0x000
#define DMA_16_BIT                   0x100
#define DMA_32_BIT                   0x200

/**
 * @brief DMA Configurations for priority
 * 
 */
#define DMA_PRIORITY_LOW                    0x0000
#define DMA_PRIORITY_MEDIUM                 0x1000
#define DMA_PRIORITY_HIGH                   0x2000
#define DMA_PRIORITY_VERY_HIGH              0x3000

typedef void (*dmaCb_t)(void);

typedef struct
{
    uint8_t channel;                    /* DMA_CH_x */
    uint8_t interrupt;                  /* DMA_INT_x */
    uint8_t direction;                  /* DMA_READ_FROM_x */
    uint8_t circular;                   /* DMA_CIRCULAR_MODE_x */
    uint8_t prephInc;                   /* DMA_PREPHIRAL_INC_x */
    uint8_t memInc;                     /* DMA_PREPHIRAL_INC_x */
    uint16_t prephSize;                 /* DMA_PREPH_x_BIT */
    uint16_t memSize;                   /* DMA_MEM_x_BIT */
    uint16_t priority;                  /* DMA_PRIORITY_x */
}dmaPrephCfg_t;

typedef struct
{
    uint8_t channel;                    /* DMA_CH_x */
    uint8_t interrupt;                  /* DMA_INT_x */
    uint8_t srcInc;                     /* DMA_SRC_INC_x */
    uint16_t size;                      /* DMA_x_BIT */
    uint16_t priority;                  /* DMA_PRIORITY_x */
}dmaMem2MemCfg_t;

/**
 * @brief Configures A DMA Prephiral Channel
 * 
 * @param cfg the configurations for the DMA channel
 * @return Std_ReturnType a status 
 *                  E_OK If the function was executed successfully
 *                  E_NOT_OK If the function was not executed successfully
 */
extern Std_ReturnType Dma_ConfigurePrephChannel(dmaPrephCfg_t* cfg);

/**
 * @brief Configures A DMA Memory To Memory Channel
 * 
 * @param cfg the configurations for the DMA channel
 * @return Std_ReturnType a status 
 *                  E_OK If the function was executed successfully
 *                  E_NOT_OK If the function was not executed successfully
 */
extern Std_ReturnType Dma_ConfigureMem2MemChannel(dmaMem2MemCfg_t* cfg);

/**
 * @brief Transfers Memory To Memory Data Using A Certain DMA Channel
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
extern Std_ReturnType Dma_TransferMem2MemData(uint8_t channelNumber, uint32_t src, uint32_t dest, uint16_t nBlocks);

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
extern Std_ReturnType Dma_TransferPrephData(uint8_t channelNumber, uint32_t preph, uint32_t mem, uint16_t nBlocks);

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
extern Std_ReturnType Dma_SetCallBack(uint8_t channelNumber, dmaCb_t callBack);

#endif