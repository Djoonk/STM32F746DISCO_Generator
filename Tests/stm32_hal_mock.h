/**
 * @file stm32_hal_mock.h
 * @brief Mock definitions for STM32 HAL for unit testing
 */

#ifndef STM32_HAL_MOCK_H
#define STM32_HAL_MOCK_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* HAL Status */
typedef enum {
    HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

/* DAC State */
typedef enum {
    HAL_DAC_STATE_RESET = 0x00U,
    HAL_DAC_STATE_READY = 0x01U,
    HAL_DAC_STATE_BUSY  = 0x02U,
    HAL_DAC_STATE_ERROR = 0x04U
} HAL_DAC_StateTypeDef;

/* DAC Channel definitions */
#define DAC_CHANNEL_1              0x00000000U
#define DAC1_CHANNEL_1             DAC_CHANNEL_1

/* DAC Trigger definitions */
#define DAC_TRIGGER_NONE           0x00000000U
#define DAC_TRIGGER_T7_TRGO        0x00000014U
#define DAC_TRIGGER_SOFTWARE       0x0000003CU

/* DAC Output buffer */
#define DAC_OUTPUTBUFFER_ENABLE    0x00000000U
#define DAC_OUTPUTBUFFER_DISABLE   0x00000002U

/* DAC Alignment */
#define DAC_ALIGN_12B_R            0x00000000U
#define DAC_ALIGN_12B_L            0x00000004U
#define DAC_ALIGN_8B_R             0x00000008U

/* TIM Counter Mode */
#define TIM_COUNTERMODE_UP         0x00000000U

/* TIM Auto-reload preload */
#define TIM_AUTORELOAD_PRELOAD_DISABLE 0x00000000U
#define TIM_AUTORELOAD_PRELOAD_ENABLE  0x00000080U

/* TIM Master Output Trigger */
#define TIM_TRGO_RESET             0x00000000U
#define TIM_TRGO_ENABLE            0x00000010U
#define TIM_TRGO_UPDATE            0x00000020U

/* TIM Master Slave Mode */
#define TIM_MASTERSLAVEMODE_ENABLE  0x00000080U
#define TIM_MASTERSLAVEMODE_DISABLE 0x00000000U

/* DMA definitions */
#define DMA_CIRCULAR               0x00000100U
#define DMA_NORMAL                 0x00000000U
#define DMA_MEMORY_TO_PERIPH       0x00000040U
#define DMA_PERIPH_TO_MEMORY       0x00000000U

/* Mock TIM Instance */
typedef struct {
    uint32_t dummy;
} TIM_TypeDef;

/* Mock DMA Instance */
typedef struct {
    uint32_t CR;
    uint32_t NDTR;
    uint32_t PAR;
    uint32_t M0AR;
} DMA_Stream_TypeDef;

/* Mock DAC Instance */
typedef struct {
    uint32_t CR;
    uint32_t DHR12R1;
    uint32_t DHR12R2;
} DAC_TypeDef;

/* TIM Init structure */
typedef struct {
    uint32_t Prescaler;
    uint32_t CounterMode;
    uint32_t Period;
    uint32_t AutoReloadPreload;
} TIM_Base_InitTypeDef;

/* TIM Master Config structure */
typedef struct {
    uint32_t MasterOutputTrigger;
    uint32_t MasterSlaveMode;
} TIM_MasterConfigTypeDef;

/* TIM Handle */
typedef struct {
    TIM_TypeDef *Instance;
    TIM_Base_InitTypeDef Init;
} TIM_HandleTypeDef;

/* DMA Init structure */
typedef struct {
    uint32_t Direction;
    uint32_t Mode;
    uint32_t MemInc;
    uint32_t PeriphInc;
    uint32_t PeriphDataAlignment;
    uint32_t MemDataAlignment;
    uint32_t Priority;
} DMA_InitTypeDef;

/* DMA Handle */
typedef struct {
    DMA_Stream_TypeDef *Instance;
    DMA_InitTypeDef Init;
    void (*XferCpltCallback)(struct __DMA_HandleTypeDef *hdma);
    void (*XferHalfCpltCallback)(struct __DMA_HandleTypeDef *hdma);
    void (*XferErrorCallback)(struct __DMA_HandleTypeDef *hdma);
} DMA_HandleTypeDef;

/* DAC Channel Config structure */
typedef struct {
    uint32_t DAC_Trigger;
    uint32_t DAC_OutputBuffer;
} DAC_ChannelConfTypeDef;

/* DAC Handle */
typedef struct {
    DAC_TypeDef *Instance;
    HAL_DAC_StateTypeDef State;
    DMA_HandleTypeDef *DMA_Handle1;
} DAC_HandleTypeDef;

/* ========== Mock tracking structures ========== */

typedef struct {
    bool called;
    DAC_HandleTypeDef *hdac;
    uint32_t Channel;
    uint32_t *pData;
    uint32_t Length;
    uint32_t Alignment;
    HAL_StatusTypeDef return_value;
} Mock_DAC_Start_DMA_t;

typedef struct {
    bool called;
    DAC_HandleTypeDef *hdac;
    DAC_ChannelConfTypeDef *sConfig;
    uint32_t Channel;
    HAL_StatusTypeDef return_value;
} Mock_DAC_ConfigChannel_t;

typedef struct {
    bool called;
    DAC_HandleTypeDef *hdac;
    HAL_StatusTypeDef return_value;
} Mock_DAC_Init_t;

typedef struct {
    bool called;
    TIM_HandleTypeDef *htim;
    HAL_StatusTypeDef return_value;
} Mock_TIM_Base_Init_t;

typedef struct {
    bool called;
    TIM_HandleTypeDef *htim;
    TIM_MasterConfigTypeDef *sMasterConfig;
    HAL_StatusTypeDef return_value;
} Mock_TIMEx_MasterConfig_t;

typedef struct {
    bool called;
    TIM_HandleTypeDef *htim;
    HAL_StatusTypeDef return_value;
} Mock_TIM_Base_Start_t;

typedef struct {
    bool called;
    DMA_HandleTypeDef *hdma;
} Mock_DMA_IRQHandler_t;

typedef struct {
    bool called;
    DAC_HandleTypeDef *hdac;
} Mock_DAC_IRQHandler_t;

/* Global mock state */
extern Mock_DAC_Start_DMA_t mock_dac_start_dma;
extern Mock_DAC_ConfigChannel_t mock_dac_config_channel;
extern Mock_DAC_Init_t mock_dac_init;
extern Mock_TIM_Base_Init_t mock_tim_base_init;
extern Mock_TIMEx_MasterConfig_t mock_timex_master_config;
extern Mock_TIM_Base_Start_t mock_tim_base_start;
extern Mock_DMA_IRQHandler_t mock_dma_irq_handler;
extern Mock_DAC_IRQHandler_t mock_dac_irq_handler;

/* Reset all mocks */
void Mock_Reset_All(void);

/* Mock HAL functions */
HAL_StatusTypeDef HAL_DAC_Init(DAC_HandleTypeDef *hdac);
HAL_StatusTypeDef HAL_DAC_ConfigChannel(DAC_HandleTypeDef *hdac, DAC_ChannelConfTypeDef *sConfig, uint32_t Channel);
HAL_StatusTypeDef HAL_DAC_Start_DMA(DAC_HandleTypeDef *hdac, uint32_t Channel, uint32_t *pData, uint32_t Length, uint32_t Alignment);
HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_MasterConfigSynchronization(TIM_HandleTypeDef *htim, TIM_MasterConfigTypeDef *sMasterConfig);
HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *htim);
void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma);
void HAL_DAC_IRQHandler(DAC_HandleTypeDef *hdac);
void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim);

#endif /* STM32_HAL_MOCK_H */
