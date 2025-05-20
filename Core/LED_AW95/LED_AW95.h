/******************************************************************************
 * @file    LED_AW95.h
 * @brief   Header file for LED_AW95 module
 * @author  Jason
 * @version V1.0.0
 * @date    2025-5
 * @copyright (c) 2025, All rights reserved.
 ******************************************************************************/

#ifndef __LED_AW95_H
#define __LED_AW95_H

/* Private Includes ----------------------------------------------------------*/
#include "main.h"

/* Exported constants --------------------------------------------------------*/
#define READ_TIMOUT                     (300)
#define I2C_ADDRESS                     (0xB6)
#define AW_I2C_RETRIES                  (5)
#define AW_I2C_RETRY_DELAY              (5)
#define AW_READ_CHIPID_RETRIES          (5)
#define AW_READ_CHIPID_RETRY_DELAY      (5)

#define INPUT_PORT0                     (0x00)//R   Equal to P0 Input_Port0 P0 port input state
#define INPUT_PORT1                     (0x01)//R   Equal to P1 Input_Port1 P1 port input state
#define OUTPUT_PORT0                    (0x02)//W/R Refer to table 1 Output_Port0 P0 port output state
#define OUTPUT_PORT1                    (0x03)//W/R Refer to table 1 Output_Port1 P1 port output state
#define CONFIG_PORT0                    (0x04)// W/R 00H Config_Port0 P0 port direction configure
#define CONFIG_PORT1                    (0x05)//W/R 00H Config_Port1 P1 port direction configure
#define INT_PORT0                       (0x06)//W/R 00H Int_Port0 P0 port interrupt enable
#define INT_PORT1                       (0x07)// W/R 00H Int_Port1 P1 port interrupt enable
#define ID                              (0x10)// R 23H ID ID register (read only)
#define GCR                             (0x11)//W/R 00H CTL Global control register
#define LED_MODE_SWITCH_P0              (0x12)//W/R FFH LED Mode Switch P0 port mode configure
#define LED_MODE_SWITCH_P1              (0x13)//W/R FFH LED Mode Switch P1 port mode configure
#define DIM0                            (0x20)// W 00H DIM0 P1_0 LED current control
#define DIM1                            (0x21)// W 00H DIM1 P1_1 LED current control
#define DIM2                            (0x22)// W 00H DIM2 P1_2 LED current control
#define DIM3                            (0x23)// W 00H DIM3 P1_3 LED current control
#define DIM4                            (0x24)// W 00H DIM4 P0_0 LED current control
#define DIM5                            (0x25)// W 00H DIM5 P0_1 LED current control
#define DIM6                            (0x26)// W 00H DIM6 P0_2 LED current control
#define DIM7                            (0x27)// W 00H DIM7 P0_3 LED current control
#define DIM8                            (0x28)// W 00H DIM8 P0_4 LED current control
#define DIM9                            (0x29)// W 00H DIM9 P0_5 LED current control
#define DIM10                           (0x2A)// W 00H DIM10 P0_6 LED current control
#define DIM11                           (0x2B)// W 00H DIM11 P0_7 LED current control
#define DIM12                           (0x2C)// W 00H DIM12 P1_4 LED current control
#define DIM13                           (0x2D)// W 00H DIM13 P1_5 LED current control
#define DIM14                           (0x2E)// W 00H DIM14 P1_6 LED current control
#define DIM15                           (0x2F)// W 00H DIM15 P1_7 LED current control
#define SOFT_RESET                      (0x7F)// W 00H SW_RSTN Soft reset

#define AW9523B_ID                      (0x23)

#define LED_HWRESET_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define LED_HWRESET_PIN                 GPIO_PIN_12
#define LED_HWRESET_PORT                GPIOA

/* Exported types ------------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/
int AW9523_Init(void);
int AW9523_SetBrightness(uint8_t channel, uint8_t brightness);
int AW9523_SetAllBrightness(uint8_t brightness);
int AW9523_SoftReset(void);
void AW9523_HWReset(void);
int AW9523_Restart(void);

#endif /* __LED_AW95_H */
