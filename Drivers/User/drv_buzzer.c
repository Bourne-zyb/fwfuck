#include "main.h"
#include "stm32g0xx_it.h"
#include "stm32g0xx_hal.h"

TIM_HandleTypeDef hbuzzer_tim;

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void Buzzer_Tim_Init( void )
{
    TIM_OC_InitTypeDef sConfigOC = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    hbuzzer_tim.Instance = TIM3;
    hbuzzer_tim.Init.Prescaler = 0;
    hbuzzer_tim.Init.CounterMode = TIM_COUNTERMODE_UP;
    hbuzzer_tim.Init.Period = (64000000 / 2700) - 1;
    hbuzzer_tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    hbuzzer_tim.Init.RepetitionCounter = 0;
    hbuzzer_tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if( HAL_TIM_Base_Init(&hbuzzer_tim) != HAL_OK )
    {
        Error_Handler();
    }
    if( HAL_TIM_PWM_Init( &hbuzzer_tim) != HAL_OK )
    {
        Error_Handler();
    }


    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;//BUZZER_TIM_PULSE;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if( HAL_TIM_PWM_ConfigChannel(&hbuzzer_tim, &sConfigOC, TIM_CHANNEL_2) != HAL_OK )
    {
        Error_Handler();
    }

    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
    HAL_GPIO_Init( GPIOC, &GPIO_InitStruct );
    HAL_TIM_PWM_Start( &hbuzzer_tim, TIM_CHANNEL_2 );
}

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void SetBuzzerFrequency( unsigned char Duty, unsigned short Frequency )
{
    uint32_t t=0;
    TIM_TypeDef *TIMx = TIM3;

	__IO uint32_t *pCCMR = &(TIMx->CCMR1);
	__IO uint32_t *pCCR = &(TIMx->CCR1);
	uint32_t BitPosition = 4;

    if( ( 0 == Duty ) || ( 0 == Frequency ) )
    {
        TIMx->CR1 &= 0xfffffffe;
        *pCCR = 0;

        t = *pCCMR;

        t &= ~( 0x07UL << BitPosition );
        t |= 0x04 << BitPosition;
        *pCCMR = t;
    }
    else
    {
        t = *pCCMR;
        t &= ~( 0x07UL << BitPosition );
        t |= 0x07 << BitPosition;
        *pCCMR = t;

        if( Duty >= 100 )
        {
            Duty = 96;
        }

        t = 2700 / Frequency - 1;
        TIMx->CR1 &= 0xfffffffe;
        TIMx->CNT = 0;
        TIMx->ARR = t;
        t = (t * (100-Duty))/100;
        *pCCR = t;
        TIMx->CR1 |= 0x01;
    }
}

