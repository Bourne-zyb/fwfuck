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
    UART2_Transmit((unsigned char*)&Uart2TxPacket, sizeof(sUartPack));
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

