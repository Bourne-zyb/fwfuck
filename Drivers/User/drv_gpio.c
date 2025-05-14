#include "main.h"


void MX_GPIO_Init( void )
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();


    /*Configure GPIO pin Output Level */
    // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);//Buzzer

    /**ADC1 GPIO Configuration
    PA6     ------> SHIPSW_IO1
    PA7     ------> SHIPSW_IO2
    PA8     ------> K8 �������Ը�λ����
    PA9     ------> K9 �������Ը�λ����
    PA10    ------> ��������������
    PA15    ------> K1 ���������Ը�λ����
    PB3     ------> K7 �������Ը�λ����
    PB4     ------> ����ҡ���м䰴��
    PB5     ------> K6 ���������Ը�λ����
    PB12    ------> ��������
    PB13    ------> ��������
    PB14    ------> ������
    PB15    ------> ������
    PC6     ------> K10 �������Ը�λ����
    PC13    ------> ��������������
    PD0     ------> K2 ���������Ը�λ����
    PD1     ------> K3 ���������Ը�λ����
    PD2     ------> K4 ���������Ը�λ����
    PD3     ------> K5 ���������Ը�λ����
    */


    /*Configure GPIO pin : PC14 *///Buzzer
    // GPIO_InitStruct.Pin = GPIO_PIN_14;
    // GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    // GPIO_InitStruct.Pull = GPIO_NOPULL;
    // GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    // HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pins : PA6 PA7 PA8 PA9
                            PA10 PA15 */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                            |GPIO_PIN_10|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : PB11 PB12 PB13 PB14
                            PB15 PB3 PB4 PB5 */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                            |GPIO_PIN_15|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pins : PC13 PC6 */
    GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pins : PD0 PD1 PD2 PD3 */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}


