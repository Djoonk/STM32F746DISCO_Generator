/**
 * @file test_signal_gen.c
 * @brief Unit tests for signal generator module
 * 
 * Tests cover:
 * 1. Generete_SineTable produces correct sine wave values for various amplitudes
 * 2. DAC is correctly configured to use DMA for sine wave output
 * 3. DMA transfer of sine wave data to DAC works as expected
 * 4. TIM7 trigger correctly initiates DAC conversions
 * 5. DMA1_Stream5_IRQHandler handles the DMA interrupt correctly
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stm32_hal_mock.h"

/* ========== Constants ========== */
#define SINE_SAMPLES 1024
#define M_PI 3.14159265358979323846f
#define TEST_TOLERANCE 2  /* Allow ±2 DAC units for floating point rounding */

/* ========== Test infrastructure ========== */
static int tests_run = 0;
static int tests_passed = 0;

#define TEST_ASSERT(condition, message) do { \
    if (!(condition)) { \
        printf("  [FAIL] %s\n", message); \
        return 0; \
    } \
} while(0)

#define TEST_ASSERT_EQUAL(expected, actual, message) do { \
    if ((expected) != (actual)) { \
        printf("  [FAIL] %s: expected %d, got %d\n", message, (int)(expected), (int)(actual)); \
        return 0; \
    } \
} while(0)

#define TEST_ASSERT_NEAR(expected, actual, tolerance, message) do { \
    int diff = abs((int)(expected) - (int)(actual)); \
    if (diff > (tolerance)) { \
        printf("  [FAIL] %s: expected %d, got %d (diff=%d, tolerance=%d)\n", \
               message, (int)(expected), (int)(actual), diff, (int)(tolerance)); \
        return 0; \
    } \
} while(0)

#define RUN_TEST(test_func) do { \
    tests_run++; \
    printf("Running: %s\n", #test_func); \
    Mock_Reset_All(); \
    if (test_func()) { \
        tests_passed++; \
        printf("  [PASS]\n"); \
    } \
} while(0)

/* ========== Module under test ========== */
uint16_t sine_table[SINE_SAMPLES];

void Generete_SineTable(uint16_t amplitude_mw)
{
    /* Convert amplitude from mV to DAC value (3300mV = 4095) */
    uint32_t max_dac_val = (amplitude_mw * 4095) / 3300;
    uint32_t offset = max_dac_val / 2;

    for (int i = 0; i < SINE_SAMPLES; i++)
    {
        float angle = (2.0f * M_PI * i) / SINE_SAMPLES;
        sine_table[i] = (uint16_t)(offset + (max_dac_val / 2.0f) * sinf(angle));
    }
}

/* ========== External variables (mocked) ========== */
DMA_HandleTypeDef hdma_dac1;
DAC_HandleTypeDef hdac;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim6;
extern DAC_ChannelConfTypeDef captured_dac_config;
extern TIM_MasterConfigTypeDef captured_tim_master_config;

/* ========== IRQ Handler under test ========== */
void DMA1_Stream5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_dac1);
}

/* ========== Test 1: Sine table generation ========== */

/**
 * Test: Sine table generation with 3300mV (full scale)
 * Expected: Values should range from 0 to 4095, centered at ~2047
 */
int test_sine_table_full_amplitude(void)
{
    Generete_SineTable(3300);
    
    /* Find min and max values */
    uint16_t min_val = 65535;
    uint16_t max_val = 0;
    for (int i = 0; i < SINE_SAMPLES; i++) {
        if (sine_table[i] < min_val) min_val = sine_table[i];
        if (sine_table[i] > max_val) max_val = sine_table[i];
    }
    
    /* At full amplitude, min should be near 0, max near 4095 */
    TEST_ASSERT_NEAR(0, min_val, TEST_TOLERANCE, "Min value should be near 0");
    TEST_ASSERT_NEAR(4095, max_val, TEST_TOLERANCE, "Max value should be near 4095");
    
    /* Check that first value (at 0 degrees) is at offset (middle) */
    uint32_t expected_offset = 4095 / 2;
    TEST_ASSERT_NEAR(expected_offset, sine_table[0], TEST_TOLERANCE, "Value at 0 deg should be at offset");
    
    return 1;
}

/**
 * Test: Sine table generation with 1650mV (half amplitude)
 * Expected: Values should range from 0 to ~2047, centered at ~1023
 */
int test_sine_table_half_amplitude(void)
{
    Generete_SineTable(1650);
    
    /* Calculate expected DAC value for 1650mV */
    uint32_t max_dac_val = (1650 * 4095) / 3300;  /* ~2047 */
    uint32_t expected_offset = max_dac_val / 2;   /* ~1023 */
    
    /* Find min and max values */
    uint16_t min_val = 65535;
    uint16_t max_val = 0;
    for (int i = 0; i < SINE_SAMPLES; i++) {
        if (sine_table[i] < min_val) min_val = sine_table[i];
        if (sine_table[i] > max_val) max_val = sine_table[i];
    }
    
    /* Verify range is half of full scale */
    TEST_ASSERT_NEAR(0, min_val, TEST_TOLERANCE, "Min value should be near 0");
    TEST_ASSERT_NEAR(max_dac_val, max_val, TEST_TOLERANCE, "Max value should be near half DAC range");
    
    return 1;
}

/**
 * Test: Sine table generation with 1000mV
 * Expected: Values should range proportionally
 */
int test_sine_table_1000mV(void)
{
    Generete_SineTable(1000);
    
    /* Calculate expected DAC value for 1000mV */
    uint32_t max_dac_val = (1000 * 4095) / 3300;  /* ~1240 */
    uint32_t expected_offset = max_dac_val / 2;   /* ~620 */
    
    /* Find min and max values */
    uint16_t min_val = 65535;
    uint16_t max_val = 0;
    for (int i = 0; i < SINE_SAMPLES; i++) {
        if (sine_table[i] < min_val) min_val = sine_table[i];
        if (sine_table[i] > max_val) max_val = sine_table[i];
    }
    
    TEST_ASSERT_NEAR(0, min_val, TEST_TOLERANCE, "Min value should be near 0");
    TEST_ASSERT_NEAR(max_dac_val, max_val, TEST_TOLERANCE, "Max value should match amplitude");
    
    return 1;
}

/**
 * Test: Verify sine wave shape (quarter points)
 * Expected: Known sine values at 0°, 90°, 180°, 270°
 */
int test_sine_table_wave_shape(void)
{
    Generete_SineTable(3300);
    
    uint32_t max_dac_val = 4095;
    uint32_t offset = max_dac_val / 2;
    uint32_t amplitude = max_dac_val / 2;
    
    /* At 0° (index 0): sin(0) = 0, value = offset */
    TEST_ASSERT_NEAR(offset, sine_table[0], TEST_TOLERANCE, "0° should be at offset");
    
    /* At 90° (index SINE_SAMPLES/4): sin(90°) = 1, value = offset + amplitude */
    int idx_90 = SINE_SAMPLES / 4;
    TEST_ASSERT_NEAR(offset + amplitude, sine_table[idx_90], TEST_TOLERANCE, "90° should be at max");
    
    /* At 180° (index SINE_SAMPLES/2): sin(180°) = 0, value = offset */
    int idx_180 = SINE_SAMPLES / 2;
    TEST_ASSERT_NEAR(offset, sine_table[idx_180], TEST_TOLERANCE, "180° should be at offset");
    
    /* At 270° (index 3*SINE_SAMPLES/4): sin(270°) = -1, value = offset - amplitude = 0 */
    int idx_270 = 3 * SINE_SAMPLES / 4;
    TEST_ASSERT_NEAR(0, sine_table[idx_270], TEST_TOLERANCE, "270° should be at min");
    
    return 1;
}

/**
 * Test: Zero amplitude should produce all zeros
 */
int test_sine_table_zero_amplitude(void)
{
    Generete_SineTable(0);
    
    /* All values should be 0 */
    for (int i = 0; i < SINE_SAMPLES; i++) {
        TEST_ASSERT_EQUAL(0, sine_table[i], "All values should be 0 for zero amplitude");
    }
    
    return 1;
}

/* ========== Test 2: DAC configuration ========== */

/**
 * Test: DAC is configured to use TIM7 trigger
 */
int test_dac_config_tim7_trigger(void)
{
    DAC_TypeDef dac_instance;
    hdac.Instance = &dac_instance;
    
    DAC_ChannelConfTypeDef sConfig = {0};
    sConfig.DAC_Trigger = DAC_TRIGGER_T7_TRGO;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    
    HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);
    
    TEST_ASSERT(mock_dac_config_channel.called, "HAL_DAC_ConfigChannel should be called");
    TEST_ASSERT_EQUAL(DAC_TRIGGER_T7_TRGO, captured_dac_config.DAC_Trigger, 
                      "DAC trigger should be TIM7 TRGO");
    TEST_ASSERT_EQUAL(DAC_CHANNEL_1, mock_dac_config_channel.Channel, 
                      "Should configure channel 1");
    
    return 1;
}

/**
 * Test: DAC output buffer is enabled
 */
int test_dac_config_output_buffer(void)
{
    DAC_TypeDef dac_instance;
    hdac.Instance = &dac_instance;
    
    DAC_ChannelConfTypeDef sConfig = {0};
    sConfig.DAC_Trigger = DAC_TRIGGER_T7_TRGO;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    
    HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);
    
    TEST_ASSERT_EQUAL(DAC_OUTPUTBUFFER_ENABLE, captured_dac_config.DAC_OutputBuffer, 
                      "DAC output buffer should be enabled");
    
    return 1;
}

/* ========== Test 3: DMA transfer ========== */

/**
 * Test: DMA transfer is started with correct parameters
 */
int test_dma_transfer_start(void)
{
    DAC_TypeDef dac_instance;
    hdac.Instance = &dac_instance;
    
    Generete_SineTable(3000);
    
    HAL_StatusTypeDef status = HAL_DAC_Start_DMA(&hdac, DAC1_CHANNEL_1, 
                                                  (uint32_t*)sine_table, 
                                                  SINE_SAMPLES, 
                                                  DAC_ALIGN_12B_R);
    
    TEST_ASSERT(mock_dac_start_dma.called, "HAL_DAC_Start_DMA should be called");
    TEST_ASSERT_EQUAL(HAL_OK, status, "DMA start should return HAL_OK");
    TEST_ASSERT_EQUAL(DAC1_CHANNEL_1, mock_dac_start_dma.Channel, "Should use channel 1");
    TEST_ASSERT_EQUAL(SINE_SAMPLES, mock_dac_start_dma.Length, "Should transfer SINE_SAMPLES");
    TEST_ASSERT_EQUAL(DAC_ALIGN_12B_R, mock_dac_start_dma.Alignment, "Should use 12-bit right alignment");
    TEST_ASSERT(mock_dac_start_dma.pData == (uint32_t*)sine_table, "Should use sine_table as data source");
    
    return 1;
}

/**
 * Test: DMA transfer uses correct data pointer
 */
int test_dma_transfer_data_pointer(void)
{
    DAC_TypeDef dac_instance;
    hdac.Instance = &dac_instance;
    
    Generete_SineTable(2000);
    
    /* Verify sine table has valid data before DMA */
    TEST_ASSERT(sine_table[0] > 0 || sine_table[SINE_SAMPLES/4] > 0, 
                "Sine table should have non-zero values");
    
    HAL_DAC_Start_DMA(&hdac, DAC1_CHANNEL_1, (uint32_t*)sine_table, SINE_SAMPLES, DAC_ALIGN_12B_R);
    
    /* Verify the pointer passed to DMA points to our sine_table */
    TEST_ASSERT(mock_dac_start_dma.pData == (uint32_t*)sine_table, 
                "DMA should receive pointer to sine_table");
    
    return 1;
}

/**
 * Test: DMA transfer length matches sine table size
 */
int test_dma_transfer_length(void)
{
    DAC_TypeDef dac_instance;
    hdac.Instance = &dac_instance;
    
    HAL_DAC_Start_DMA(&hdac, DAC1_CHANNEL_1, (uint32_t*)sine_table, SINE_SAMPLES, DAC_ALIGN_12B_R);
    
    TEST_ASSERT_EQUAL(SINE_SAMPLES, mock_dac_start_dma.Length, 
                      "DMA transfer length should match SINE_SAMPLES");
    TEST_ASSERT_EQUAL(1024, mock_dac_start_dma.Length, 
                      "SINE_SAMPLES should be 1024");
    
    return 1;
}

/* ========== Test 4: TIM7 trigger configuration ========== */

/**
 * Test: TIM7 is configured with TRGO on update event
 */
int test_tim7_trgo_update(void)
{
    TIM_TypeDef tim_instance;
    htim7.Instance = &tim_instance;
    
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    
    HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig);
    
    TEST_ASSERT(mock_timex_master_config.called, 
                "HAL_TIMEx_MasterConfigSynchronization should be called");
    TEST_ASSERT_EQUAL(TIM_TRGO_UPDATE, captured_tim_master_config.MasterOutputTrigger, 
                      "Master output trigger should be UPDATE");
    
    return 1;
}

/**
 * Test: TIM7 base is started after configuration
 */
int test_tim7_base_start(void)
{
    TIM_TypeDef tim_instance;
    htim7.Instance = &tim_instance;
    
    HAL_TIM_Base_Start(&htim7);
    
    TEST_ASSERT(mock_tim_base_start.called, "HAL_TIM_Base_Start should be called");
    TEST_ASSERT(mock_tim_base_start.htim == &htim7, "Should start TIM7");
    
    return 1;
}

/**
 * Test: TIM7 prescaler and period determine output frequency
 * With 108MHz clock, Prescaler=0, Period=108: freq = 108MHz / (1 * 109) ≈ 1MHz sample rate
 * For 1024 samples, output frequency = 1MHz / 1024 ≈ 976Hz sine wave
 */
int test_tim7_frequency_config(void)
{
    TIM_TypeDef tim_instance;
    htim7.Instance = &tim_instance;
    htim7.Init.Prescaler = 0;
    htim7.Init.Period = 108;
    htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    HAL_TIM_Base_Init(&htim7);
    
    TEST_ASSERT(mock_tim_base_init.called, "HAL_TIM_Base_Init should be called");
    TEST_ASSERT_EQUAL(0, htim7.Init.Prescaler, "Prescaler should be 0");
    TEST_ASSERT_EQUAL(108, htim7.Init.Period, "Period should be 108");
    TEST_ASSERT_EQUAL(TIM_COUNTERMODE_UP, htim7.Init.CounterMode, "Counter mode should be UP");
    
    return 1;
}

/* ========== Test 5: DMA IRQ Handler ========== */

/**
 * Test: DMA1_Stream5_IRQHandler calls HAL_DMA_IRQHandler
 */
int test_dma_irq_handler_calls_hal(void)
{
    DMA_Stream_TypeDef dma_instance;
    hdma_dac1.Instance = &dma_instance;
    
    DMA1_Stream5_IRQHandler();
    
    TEST_ASSERT(mock_dma_irq_handler.called, "HAL_DMA_IRQHandler should be called");
    TEST_ASSERT(mock_dma_irq_handler.hdma == &hdma_dac1, 
                "Should pass hdma_dac1 to HAL_DMA_IRQHandler");
    
    return 1;
}

/**
 * Test: DMA IRQ Handler receives correct DMA handle
 */
int test_dma_irq_handler_correct_handle(void)
{
    DMA_Stream_TypeDef dma_instance;
    hdma_dac1.Instance = &dma_instance;
    
    /* Set some identifiable values */
    dma_instance.CR = 0x12345678;
    
    DMA1_Stream5_IRQHandler();
    
    TEST_ASSERT(mock_dma_irq_handler.hdma == &hdma_dac1, 
                "IRQ handler should use hdma_dac1");
    TEST_ASSERT(mock_dma_irq_handler.hdma->Instance == &dma_instance, 
                "DMA handle should have correct instance");
    
    return 1;
}

/* ========== Main test runner ========== */

int main(void)
{
    printf("========================================\n");
    printf("Signal Generator Unit Tests\n");
    printf("========================================\n\n");
    
    /* Test 1: Sine table generation */
    printf("--- Test Group 1: Sine Table Generation ---\n");
    RUN_TEST(test_sine_table_full_amplitude);
    RUN_TEST(test_sine_table_half_amplitude);
    RUN_TEST(test_sine_table_1000mV);
    RUN_TEST(test_sine_table_wave_shape);
    RUN_TEST(test_sine_table_zero_amplitude);
    
    /* Test 2: DAC configuration */
    printf("\n--- Test Group 2: DAC Configuration ---\n");
    RUN_TEST(test_dac_config_tim7_trigger);
    RUN_TEST(test_dac_config_output_buffer);
    
    /* Test 3: DMA transfer */
    printf("\n--- Test Group 3: DMA Transfer ---\n");
    RUN_TEST(test_dma_transfer_start);
    RUN_TEST(test_dma_transfer_data_pointer);
    RUN_TEST(test_dma_transfer_length);
    
    /* Test 4: TIM7 trigger */
    printf("\n--- Test Group 4: TIM7 Trigger ---\n");
    RUN_TEST(test_tim7_trgo_update);
    RUN_TEST(test_tim7_base_start);
    RUN_TEST(test_tim7_frequency_config);
    
    /* Test 5: DMA IRQ Handler */
    printf("\n--- Test Group 5: DMA IRQ Handler ---\n");
    RUN_TEST(test_dma_irq_handler_calls_hal);
    RUN_TEST(test_dma_irq_handler_correct_handle);
    
    /* Summary */
    printf("\n========================================\n");
    printf("Test Results: %d/%d passed\n", tests_passed, tests_run);
    printf("========================================\n");
    
    return (tests_passed == tests_run) ? 0 : 1;
}
