/****************************************************************************** 
 * @file    drv_LED_RGB.c
 * @brief   LED RGB driver module implementation.
 * @author  Jason
 * @version V1.0.0 
 * @date    2025-3 
 * @copyright (c) 2025, All rights reserved. 
 ******************************************************************************/ 

/* Includes ------------------------------------------------------------------*/ 
#include "drv_LED_RGB.h"

/* Private typedef -----------------------------------------------------------*/ 

/* Private define ------------------------------------------------------------*/ 

/* Private macro -------------------------------------------------------------*/ 

/* Private variables ---------------------------------------------------------*/ 
static uint16_t pwm_dma_buf[TOTAL_SLOTS];
/* Private function prototypes -----------------------------------------------*/ 
static void set_rgb_color(uint8_t r, uint8_t g, uint8_t b, uint16_t *out_buf);
void LED_RGB_Init(void);
void LED_RGB_ConfigColor(void);
void LED_RGB_Show(void);
/* Private functions ---------------------------------------------------------*/ 



/************************ (C) COPYRIGHT Jason *****END OF FILE****/

static void set_rgb_color(uint8_t r, uint8_t g, uint8_t b, uint16_t *out_buf)
{
    uint8_t grb[3] = { g, r, b };
    for (int c = 0; c < 3; c++) {
        for (int i = 7; i >= 0; i--) {
            *out_buf++ = (grb[c] & (1 << i)) ? CCR_1 : CCR_0;
        }
    }
}

void LED_RGB_Init(void)
{
    MX_TIM1_Init();
}

void LED_RGB_ConfigColor(void)
{
    int i = 0;
    // Start RESET (keep low level)
    for (int j = 0; j < RESET_SLOTS; j++) {
        pwm_dma_buf[i++] = 0;  // Duty cycle 0%
    }

    // Write PWM waveform for the first LED
    for (int led = 0; led < 1; led++) {

       //uint8_t r = 255, g = 0, b = 50;
        uint8_t r = 0, g = 0, b = 50; // RGB for light BLUE
        set_rgb_color(r, g, b, &pwm_dma_buf[i]);
        i += BITS_PER_LED;
    }
    
    // Write PWM waveform for the remaining LEDs
    for (int led = 1; led < LED_NUM; led++) {

       // uint8_t r = 0, g = 255, b = 0;
        uint8_t r = 0, g = 10, b = 0; // RGB for light green
        set_rgb_color(r, g, b, &pwm_dma_buf[i]);
        i += BITS_PER_LED;
    }

    // End RESET (keep low level)
    for (int j = 0; j < RESET_SLOTS; j++) {
        pwm_dma_buf[i++] = 0;
    }
}

void LED_RGB_Show(void)
{
    HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_4, (uint32_t *)pwm_dma_buf, TOTAL_SLOTS);
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_4);
}
