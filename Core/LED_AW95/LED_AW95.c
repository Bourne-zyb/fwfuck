/****************************************************************************** 
 * @file    LED_AW95.c 
 * @brief   LED_AW95 module implementation.
 * @author  Jason 
 * @version V1.0.0 
 * @date    2025-05
 * @copyright (c) 2025, All rights reserved. 
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/ 
#include "LED_AW95.h"
#include "i2c.h"
/* Private typedef -----------------------------------------------------------*/ 

/* Private define ------------------------------------------------------------*/ 

/* Private macro -------------------------------------------------------------*/ 

/* Private variables ---------------------------------------------------------*/ 
/**
LED1--16 一一对应DIM0--DIM15
*/
static const uint8_t LED2DIM[] = {DIM0,DIM1,DIM2,DIM3,DIM4,DIM5,DIM6,DIM7,DIM8,DIM9,DIM10,DIM11,DIM12,DIM13,DIM14,DIM15};

/* Private function prototypes -----------------------------------------------*/ 
static __INLINE HAL_StatusTypeDef Local_Hal_I2C_Read(uint8_t regaddr,uint8_t* buf,uint8_t length);
static __INLINE HAL_StatusTypeDef Local_Hal_I2C_Write(uint8_t regaddr,uint8_t* buf,uint8_t length);
int AW9523_Init(void);
int AW9523_SetBrightness(uint8_t channel,uint8_t brightness);
int AW9523_SetAllBrightness(uint8_t brightness);
int  AW9523_SoftReset(void);
void AW9523_HWReset(void);
int AW9523_Restart(void);

/* Private functions ---------------------------------------------------------*/ 


static __INLINE HAL_StatusTypeDef Local_Hal_I2C_Read(uint8_t regaddr,uint8_t* buf,uint8_t length)
{
  HAL_I2C_Mem_Read(&hi2c1, I2C_ADDRESS, regaddr, I2C_MEMADD_SIZE_8BIT, buf, length, READ_TIMOUT);
  if(HAL_I2C_GetError(&hi2c1) != HAL_OK)
  {
    return HAL_ERROR;
  }
  
  return HAL_OK;
}

static __INLINE HAL_StatusTypeDef Local_Hal_I2C_Write(uint8_t regaddr,uint8_t* buf,uint8_t length)
{ 
  HAL_I2C_Mem_Write(&hi2c1, I2C_ADDRESS, regaddr, I2C_MEMADD_SIZE_8BIT, buf, length, READ_TIMOUT);
  if (HAL_I2C_GetError(&hi2c1) != HAL_OK)
  {
    return HAL_ERROR;
  }
  
  return HAL_OK;
}

/*
芯片初始化
*/
int AW9523_Init(void)
{
  uint8_t val = 0;
  uint8_t ret = 0;
  
  /***
   * AW9523B support 3 reset mode: power on reset, hardware reset, 
   * software reset. Each reset mode can reset registers to default value
   */
  if(AW9523_SoftReset() < 0) return -1;

  //read chip id
  ret         = Local_Hal_I2C_Read(ID,&val,1);
  if(ret != HAL_OK)     return -1;
  if(val != AW9523B_ID) return -1;
  
  //Global control register config gpio mode & dim setp P0 special
  val         = 0x12;		
  ret         = Local_Hal_I2C_Write(GCR,&val,1);
  if(ret != HAL_OK) return -1;

  //led mode switch 
  //P0 to led
  val         = 0x00;
  ret         = Local_Hal_I2C_Write(LED_MODE_SWITCH_P0,&val,1);
  if(ret != HAL_OK) return -1;

  //P1 to led
  val         = 0x00;
  ret         = Local_Hal_I2C_Write(LED_MODE_SWITCH_P1,&val,1);
  if(ret != HAL_OK) return -1;
  
  //关闭电流
  AW9523_SetAllBrightness(0);

  //16脚上电默认输出状态由AD1/AD0决定,故上电后全导通
  //P0 led breakover
  val         = 0x00;
  ret         = Local_Hal_I2C_Write(OUTPUT_PORT0,&val,1);
  if(ret != HAL_OK) return -1;

  //P1 led breakover
  val         = 0x00;
  ret         = Local_Hal_I2C_Write(OUTPUT_PORT1,&val,1);
  if(ret != HAL_OK) return -1;
    
  return 0;		
}

/**
单通道独立亮度调节
channel:   0----15
brightness:0----255
*/
int AW9523_SetBrightness(uint8_t channel,uint8_t brightness)
{	
  uint8_t val = 0;
  uint8_t ret = 0;

  val         = brightness;
  ret         = Local_Hal_I2C_Write(LED2DIM[channel],&val,1);
  if(ret != HAL_OK) return -1;

  return 0;
}

/**
多通道亮度调节,统一控制
brightness:0----255
*/
int AW9523_SetAllBrightness(uint8_t brightness)
{
  uint8_t i;
  uint8_t ret = 0;
  
  for(i = DIM0;i <= DIM15;i++)
  {
     ret  = Local_Hal_I2C_Write(i,&brightness,1);
     if(ret != HAL_OK) return -1;		     	   
  }
    
  return 0;
}

//软件复位
int  AW9523_SoftReset(void)
{
  uint8_t val;
  uint8_t ret = 0;

  val         = 0x00;
  ret         = Local_Hal_I2C_Write(SOFT_RESET,&val,1);
  if(ret != HAL_OK) return -1; 

  return 0;	
}

//硬件复位
void AW9523_HWReset(void)
{
   GPIO_InitTypeDef  GPIO_InitStruct;
      
  //  LED_HWRESET_CLK_ENABLE();
            
  //  GPIO_InitStruct.Pin       = LED_HWRESET_PIN;
  //  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  //  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  //  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
      
  //  HAL_GPIO_Init(LED_HWRESET_PORT, &GPIO_InitStruct);
     
   //RSTN
   HAL_GPIO_WritePin(LED_HWRESET_PORT,LED_HWRESET_PIN,GPIO_PIN_RESET);
   HAL_Delay(1);
   HAL_GPIO_WritePin(LED_HWRESET_PORT,LED_HWRESET_PIN,GPIO_PIN_SET);
   HAL_Delay(1);
}

//防止该芯片意外复位
int AW9523_Restart(void)
{
   uint8_t val = 0xFF;
  
   //理论上GCR寄存器复位了,为判断依据
   while(Local_Hal_I2C_Read(GCR,&val,1) != HAL_OK){}
       
   if(val != 0x00) return -1;
  
   while(AW9523_Init());
       
   return 0;
}

/************************ (C) COPYRIGHT Jason *****END OF FILE****/
