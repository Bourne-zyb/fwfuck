#include "main.h"
#include "drv_i2c.h"

I2C_HandleTypeDef hi2c1;

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void I2C_Init( void )
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.Timing = 0x10707DBC;

    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure Analogue filter
     */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure Digital filter
     */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
    {
        Error_Handler();
    }
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if( i2cHandle->Instance == I2C1 )
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_8;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF6_I2C1;
        HAL_GPIO_Init( GPIOB, &GPIO_InitStruct );

        /* I2C3 clock enable */
        __HAL_RCC_I2C1_CLK_ENABLE();

    }
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{
    if( i2cHandle->Instance == I2C1 )
    {
        /* Peripheral clock disable */
        __HAL_RCC_I2C1_CLK_ENABLE();

        /**I2C1 GPIO Configuration
        PB8     ------> I2C1_SCL
        PB9     ------> I2C1_SDA
        */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8);

        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9);
    }
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
uint8_t I2C_Read( uint16_t DevAddress, uint16_t MemAddress, void *pData, uint32_t Size )
{
    HAL_StatusTypeDef Result = HAL_I2C_Mem_Read( &hi2c1, DevAddress, MemAddress, 1, pData, Size, 10 );
    return  Result;
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
uint8_t I2C_Write( uint16_t DevAddress, uint16_t MemAddress, void *pData, uint32_t Size )
{
    HAL_StatusTypeDef Result = HAL_I2C_Mem_Write( &hi2c1, DevAddress, MemAddress, 1, pData, Size, 10 );
    return  Result;
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
uint8_t IsDeviceReady( uint16_t DevAddress )
{
    HAL_StatusTypeDef Result;
    Result = HAL_I2C_IsDeviceReady( &hi2c1, DevAddress, 0xff, 0xff );
    return Result;
}






