#include "main.h"
#include "System.h"
#include "drv_uart.h"



sUartPack   Uart2TxPacket;
sSampling SamplingValue;

uint8_t Usart2_Rx_Data_buff[UASRT_BUFFER_SIZE];
uint8_t Usart2_RxBufferTemp[UASRT_BUFFER_SIZE];

/*==================================================================================================
name:
funciton:
input;
output:
==================================================================================================*/
void USART2_IRQHandler( void )
{
    if ( USART2->ISR & USART_ISR_IDLE  )
	{
		// Clear the IDLE flag
		USART2->ICR = USART_ICR_IDLECF;

        Usart2_RxBackground();
	}
}

void UART2_Init(  uint8_t *pBuffer, uint16_t Len  )
{
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_USART2_FORCE_RESET();
    __HAL_RCC_USART2_RELEASE_RESET();

    GPIO_InitStruct.Pin = GPIO_PIN_2 |GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART2->CR2 =  0;
    USART2->CR3 |= USART_CR3_ONEBIT| USART_CR3_OVRDIS;
    USART2->BRR=(SYSCLK+115200/2)/115200;//Todo：看效果后续换1.5M波特率
    // Initialize parameters for DMAMUX Channel
	DMAMUX1_Channel3->CCR =52; //USART2_RX
    // Disable RX DMA
    USART2->CR3 &= ~USART_CR3_DMAR;
    // Set up the RX DMA
    // Disable DMA
    DMA1_Channel4->CCR &= ~DMA_CCR_EN;
    DMA1_Channel4->CPAR = (uint32_t)&USART2->RDR;
    DMA1_Channel4->CMAR = (uint32_t)pBuffer;
    DMA1_Channel4->CNDTR = Len;

    // EN=1 DMA channel enabled
    // TCIE=0 Transfer complete interrupt disabled
    // HTIE=0 Half transfer interrupt disabled
    // TEIE=0 Transfer error interrupt disabled
    // DIR=0 Read from peripheral
    // CIRC=1 Circular mode enabled
    // PINC=0 Peripheral increment mode disabled
    // MINC=1 Memory increment mode enabled
    // PSIZE=0 8-bit peripheral size
    // MSIZE=0 8-bit memory size
    // PL=10  high priority
    // MEM2MEM=0 Memory to memory mode disabled
    DMA1_Channel4->CCR=DMA_CCR_EN | DMA_CCR_CIRC | DMA_CCR_MINC | DMA_CCR_PL_1;//
	// Enable RX DMA
    USART2->CR3 |= USART_CR3_DMAR;

    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_IDLEIE;
    // Enable the USART
    USART2->CR1|=USART_CR1_UE;

    // Clear the transmission complete and the idle flags
    USART2->ICR = USART_ICR_IDLECF;
	// Clear the USART IRQ pending flag
	NVIC_ClearPendingIRQ( USART2_IRQn );
	// Set the USART3 IRQ to realtime priority
	NVIC_SetPriority( USART2_IRQn, 1 );
	// Enable the USART3 IRQ
	NVIC_EnableIRQ( USART2_IRQn );
}


void UART2_Transmit( uint8_t *pData, uint16_t Len )
{
//    if( !len || !is_uart5_init ) return;
    // Disable TX DMA
	USART2->CR3 &= ~USART_CR3_DMAT;
	// Set up the TX DMA
    // Initialize parameters for DMAMUX Channel
    DMAMUX1_Channel4->CCR = 53; //USART2_TX
	// Disable DMA
	DMA1_Channel5->CCR   &= ~DMA_CCR_EN;
	DMA1_Channel5->CPAR  =  (uint32_t)&USART2->TDR;
	DMA1_Channel5->CMAR  =  (uint32_t)pData;
	DMA1_Channel5->CNDTR =  Len;
	// EN=1 DMA channel enabled
	// TCIE=0 Transfer complete interrupt disabled
	// HTIE=0 Half transfer interrupt disabled
	// TEIE=0 Transfer error interrupt disabled
	// DIR=1 Read from memory
	// CIRC=0 Circular mode disabled
	// PINC=0 Peripheral increment mode disabled
	// MINC=1 Memory increment mode enabled
	// PSIZE=0 8-bit peripheral size
	// MSIZE=0 8-bit memory size
	// PL=0 Low priority
	// MEM2MEM=0 Memory to memory mode disabled
	DMA1_Channel5->CCR = DMA_CCR_EN | DMA_CCR_DIR | DMA_CCR_MINC;

    USART2->CR3 |= USART_CR3_DMAT;
}


void Usart2_TxBackground( void )
{
    Uart2TxPacket.StartFlag  = UASRT_START_FLAG;
    Uart2TxPacket.Command    = USART_CMD_SEND_SAMPLING;
    Uart2TxPacket.PackLength = sizeof(sSampling);
    Uart2TxPacket.EndFlag    = UASRT_END_FLAG;

    memcpy( &Uart2TxPacket.Data, &SamplingValue, sizeof(sSampling) );
    Uart2TxPacket.Checksum = CRC8( &Uart2TxPacket.Command, sizeof(sUartPack) - 3 );

//    char hex_str[sizeof(Uart2TxPacket) * 3 + 1] = {0};
//    for (size_t i = 0; i < sizeof(Uart2TxPacket); i++) {
//      sprintf(&hex_str[i * 3], "%02X ", ((uint8_t*)&Uart2TxPacket)[i]);
//    }
//    SEGGER_RTT_printf(0, "Send Data: %s\n", hex_str);

    UART2_Transmit((unsigned char*)&Uart2TxPacket, sizeof(sUartPack));
	
		SamplingValue.Coder = 1;
}


void Usart2_RxBackground( void )
{
    if ( UASRT_START_FLAG == Usart2_Rx_Data_buff[0] )
    {
        if ( UASRT_END_FLAG == Usart2_Rx_Data_buff[23] )
        {
            uint8_t TempCheckSum = CRC8( &Uart2TxPacket.Command, sizeof(sUartPack) - 3 );
            if ( TempCheckSum == Usart2_Rx_Data_buff[22] )
            {
                memcpy( Usart2_RxBufferTemp, Usart2_Rx_Data_buff, sizeof(Usart2_Rx_Data_buff) );

                if ( USART_CMD_FIRMWARE_UPDATE == Usart2_RxBufferTemp[1] )
                {
                    /* code */
                }
                else if ( USART_CMD_FACTORY_LIGHTTEST == Usart2_RxBufferTemp[1] )
                {
                    /* code */
                }
                else if ( USART_CMD_FACTORY_BUZZERTEST == Usart2_RxBufferTemp[1] )
                {
                    /* code */
                }


            }
        }
    }
}

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 921600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PB6     ------> USART1_TX
    PB7     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF0_USART1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}
