#include "main.h"

ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;

/* ADC1 init function */
void MX_ADC1_Init( void )
{
    ADC_ChannelConfTypeDef sConfig = {0};

    hadc.Instance = ADC1;
    hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV6;
    hadc.Init.Resolution = ADC_RESOLUTION_12B;              // 12bit
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;              // 右对齐
    hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;               // 扫描模式
    hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc.Init.LowPowerAutoWait = DISABLE;
    hadc.Init.LowPowerAutoPowerOff = DISABLE;
    hadc.Init.ContinuousConvMode = ENABLE;                  //连续模式
    hadc.Init.NbrOfConversion = 5;                          //通道数
    hadc.Init.DiscontinuousConvMode = DISABLE;
    hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc.Init.DMAContinuousRequests = ENABLE;
    hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    hadc.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_1CYCLE_5;
    hadc.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
    if (HAL_ADC_Init(&hadc) != HAL_OK)
    {
        Error_Handler();
    }

    //Configure Regular Channel4
    sConfig.Channel = ADC_CHANNEL_4;                        // ADC通道选择，与具体IO相对应
    sConfig.Rank = ADC_REGULAR_RANK_1;                      // 采样序列配置，这里表示第一次采样的通道
    sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;       // 采样周期
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    //Configure Regular Channel5
    sConfig.Channel = ADC_CHANNEL_5;
    sConfig.Rank = ADC_REGULAR_RANK_2;
    sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    //Configure Regular Channel8
    sConfig.Channel = ADC_CHANNEL_8;
    sConfig.Rank = ADC_REGULAR_RANK_3;
    sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    //Configure Regular Channel9
    sConfig.Channel = ADC_CHANNEL_9;
    sConfig.Rank = ADC_REGULAR_RANK_4;
    sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    //Configure Regular Channel10
    sConfig.Channel = ADC_CHANNEL_10;
    sConfig.Rank = ADC_REGULAR_RANK_5;
    sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

}


void HAL_ADC_MspInit( ADC_HandleTypeDef* adcHandle )
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if ( adcHandle->Instance == ADC1 )
    {
        /* ADC1 clock enable */
        __HAL_RCC_ADC_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /**ADC1 GPIO Configuration
        PA4     ------> ADC1_IN4(自回中波轮左)
        PA5     ------> ADC1_IN5(自回中波轮右)
        PB0     ------> ADC1_IN8（四向小摇杆X）
        PB1     ------> ADC1_IN9（四向小摇杆Y）
        PB2     ------> ADC1_IN10（五向按键）
        */
        GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* ADC1 DMA Init */
        hdma_adc.Instance = DMA1_Channel1;
        hdma_adc.Init.Request = DMA_REQUEST_ADC1;
        hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
        hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_adc.Init.Mode = DMA_CIRCULAR;
        hdma_adc.Init.Priority = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&hdma_adc) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc);
    }

}

void HAL_ADC_MspDeInit( ADC_HandleTypeDef* adcHandle )
{
    /* Peripheral clock disable */
    __HAL_RCC_ADC_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PA4     ------> ADC1_IN4
    PA5     ------> ADC1_IN5
    PB0     ------> ADC1_IN8
    PB1     ------> ADC1_IN9
    PB2     ------> ADC1_IN10
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4|GPIO_PIN_5);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);

    /* ADC1 DMA DeInit */
    HAL_DMA_DeInit(adcHandle->DMA_Handle);
}
