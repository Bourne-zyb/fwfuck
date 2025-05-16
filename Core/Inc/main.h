/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"
#include <stdint.h>
#include <string.h>
#include "crc8.h"
#include "crc16.h"

#include "SEGGER_RTT.h"
	
#define BOOL  unsigned char
#define TRUE  1
#define FALSE 0
#define NULL  0


#define SYSCLK  64000000
#define SYSTICK (SYSCLK/8)

extern uint16_t ADC_Sampling[5];

void Error_Handler(void);


#define FRAME_HEADER_0   0x55
#define FRAME_HEADER_1   0xC4
#define FRAME_HEADER_2   0x09
#define FRAME_SIZE       14
#define PAYLOAD_SIZE     9
#define FRAME_ERR       0xff

typedef struct {
    int16_t ch[4];       // CH1 ~ CH4
    uint8_t stickState;  // Stick ״̬
} FrameData_t;

extern uint8_t datarecv111[28];
extern  FrameData_t frameData;
int extract_valid_frame(uint8_t *buf, size_t len, FrameData_t *outFrame);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
