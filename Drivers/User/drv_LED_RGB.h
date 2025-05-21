/******************************************************************************
 * @file    drv_LED_RGB.h
 * @brief   Driver header for RGB LED control
 * @author  Jason
 * @version V1.0.0
 * @date    2025-3
 * @copyright (c) 2025, All rights reserved.
 ******************************************************************************/

#ifndef __DRV_LED_RGB_H
#define __DRV_LED_RGB_H

/* Private Includes ----------------------------------------------------------*/
#include "main.h"
#include "tim.h"
/* Exported constants --------------------------------------------------------*/
#define LED_NUM        5
#define BITS_PER_LED   24
#define RESET_SLOTS    100   // Ensure > 80us low level
#define PWM_ARR   57     // Period 0.89 us
#define CCR_0     19     // Represents bit 0 (high level 0.3 us)
#define CCR_1     38     // Represents bit 1 (high level 0.6 us)
/* Exported types ------------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
// Final send buffer: 5 LEDs ¡Á 24bit + start Reset + end Reset
#define TOTAL_SLOTS    (RESET_SLOTS + LED_NUM * BITS_PER_LED + RESET_SLOTS)
/* Exported variables --------------------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/
void LED_RGB_Init(void);
void LED_RGB_ConfigColor(void);
void LED_RGB_Show(void);
#endif /* __DRV_LED_RGB_H */
