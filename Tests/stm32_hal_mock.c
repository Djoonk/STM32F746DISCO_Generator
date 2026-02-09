/**
 * @file stm32_hal_mock.c
 * @brief Mock implementations for STM32 HAL functions
 */

#include "stm32_hal_mock.h"

/* Global mock state definitions */
Mock_DAC_Start_DMA_t mock_dac_start_dma;
Mock_DAC_ConfigChannel_t mock_dac_config_channel;
Mock_DAC_Init_t mock_dac_init;
Mock_TIM_Base_Init_t mock_tim_base_init;
Mock_TIMEx_MasterConfig_t mock_timex_master_config;
Mock_TIM_Base_Start_t mock_tim_base_start;
Mock_DMA_IRQHandler_t mock_dma_irq_handler;
Mock_DAC_IRQHandler_t mock_dac_irq_handler;

/* Captured config for verification */
DAC_ChannelConfTypeDef captured_dac_config;
TIM_MasterConfigTypeDef captured_tim_master_config;

void Mock_Reset_All(void)
{
    memset(&mock_dac_start_dma, 0, sizeof(mock_dac_start_dma));
    mock_dac_start_dma.return_value = HAL_OK;

    memset(&mock_dac_config_channel, 0, sizeof(mock_dac_config_channel));
    mock_dac_config_channel.return_value = HAL_OK;

    memset(&mock_dac_init, 0, sizeof(mock_dac_init));
    mock_dac_init.return_value = HAL_OK;

    memset(&mock_tim_base_init, 0, sizeof(mock_tim_base_init));
    mock_tim_base_init.return_value = HAL_OK;

    memset(&mock_timex_master_config, 0, sizeof(mock_timex_master_config));
    mock_timex_master_config.return_value = HAL_OK;

    memset(&mock_tim_base_start, 0, sizeof(mock_tim_base_start));
    mock_tim_base_start.return_value = HAL_OK;

    memset(&mock_dma_irq_handler, 0, sizeof(mock_dma_irq_handler));
    memset(&mock_dac_irq_handler, 0, sizeof(mock_dac_irq_handler));

    memset(&captured_dac_config, 0, sizeof(captured_dac_config));
    memset(&captured_tim_master_config, 0, sizeof(captured_tim_master_config));
}

HAL_StatusTypeDef HAL_DAC_Init(DAC_HandleTypeDef *hdac)
{
    mock_dac_init.called = true;
    mock_dac_init.hdac = hdac;
    if (hdac) {
        hdac->State = HAL_DAC_STATE_READY;
    }
    return mock_dac_init.return_value;
}

HAL_StatusTypeDef HAL_DAC_ConfigChannel(DAC_HandleTypeDef *hdac, DAC_ChannelConfTypeDef *sConfig, uint32_t Channel)
{
    mock_dac_config_channel.called = true;
    mock_dac_config_channel.hdac = hdac;
    mock_dac_config_channel.sConfig = sConfig;
    mock_dac_config_channel.Channel = Channel;
    
    /* Capture config for verification */
    if (sConfig) {
        captured_dac_config = *sConfig;
    }
    
    return mock_dac_config_channel.return_value;
}

HAL_StatusTypeDef HAL_DAC_Start_DMA(DAC_HandleTypeDef *hdac, uint32_t Channel, uint32_t *pData, uint32_t Length, uint32_t Alignment)
{
    mock_dac_start_dma.called = true;
    mock_dac_start_dma.hdac = hdac;
    mock_dac_start_dma.Channel = Channel;
    mock_dac_start_dma.pData = pData;
    mock_dac_start_dma.Length = Length;
    mock_dac_start_dma.Alignment = Alignment;
    
    if (hdac) {
        hdac->State = HAL_DAC_STATE_BUSY;
    }
    
    return mock_dac_start_dma.return_value;
}

HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim)
{
    mock_tim_base_init.called = true;
    mock_tim_base_init.htim = htim;
    return mock_tim_base_init.return_value;
}

HAL_StatusTypeDef HAL_TIMEx_MasterConfigSynchronization(TIM_HandleTypeDef *htim, TIM_MasterConfigTypeDef *sMasterConfig)
{
    mock_timex_master_config.called = true;
    mock_timex_master_config.htim = htim;
    mock_timex_master_config.sMasterConfig = sMasterConfig;
    
    /* Capture config for verification */
    if (sMasterConfig) {
        captured_tim_master_config = *sMasterConfig;
    }
    
    return mock_timex_master_config.return_value;
}

HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *htim)
{
    mock_tim_base_start.called = true;
    mock_tim_base_start.htim = htim;
    return mock_tim_base_start.return_value;
}

void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma)
{
    mock_dma_irq_handler.called = true;
    mock_dma_irq_handler.hdma = hdma;
}

void HAL_DAC_IRQHandler(DAC_HandleTypeDef *hdac)
{
    mock_dac_irq_handler.called = true;
    mock_dac_irq_handler.hdac = hdac;
}

void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim)
{
    /* Empty mock implementation */
    (void)htim;
}
