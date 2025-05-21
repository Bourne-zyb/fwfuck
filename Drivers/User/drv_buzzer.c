/****************************************************************************** 
 * @file    drv_buzzer.c
 * @brief   Buzzer driver module implementation.
 * @note    ʹ�� pwm ��Ƶ�ʷ�Χ�� 200 Hz �� 10 kHz, 50% ռ�ձȷ�������������������
            ���У�2.5khz Ƶ���£�50% ռ�ձȷ������������
 * @author  Jason
 * @version V1.0.1
 * @date    2025-5
 * @copyright (c) 2025, All rights reserved.
 ******************************************************************************/ 

/* Includes ------------------------------------------------------------------*/ 
#include "drv_buzzer.h"
#include "tim.h"
/* Private typedef -----------------------------------------------------------*/ 

/* Private define ------------------------------------------------------------*/ 

/* Private macro -------------------------------------------------------------*/ 

/* Private variables ---------------------------------------------------------*/ 

/* Private function prototypes -----------------------------------------------*/ 
void Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);

/* Private functions ---------------------------------------------------------*/ 
void Buzzer_Init(void)
{
    MX_TIM3_Init(BUZZER_PERIOD);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, BUZZER_PERIOD / 2);
}

void Buzzer_On(void)
{
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
}

void Buzzer_Off(void)
{
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
}

/************************ (C) COPYRIGHT Jason *****END OF FILE****/


