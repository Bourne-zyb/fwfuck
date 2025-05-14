#include "main.h"
#include "drv_hall.h"

STRUCT_UART_RECEIVE Hall_Receive;
uint8_t Hall_ReceiverBuffer[HALL_BUFFER_SIZE];
STRUCT_HS_PROTOCOL HallSticks_Protocol;
STRUCT_HALL_STICKS HallSticks;

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void Hall_UART1_Init( void )
{
    /**USART1 GPIO Configuration
    PB6     ------> USART1_TX
    PB7     ------> USART1_RX
    */
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_USART1_FORCE_RESET();
    __HAL_RCC_USART1_RELEASE_RESET();

    GPIO_InitStruct.Pin = GPIO_PIN_6 |GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF0_USART1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    USART1->CR2 =  0;
    USART1->CR3 |= USART_CR3_ONEBIT| USART_CR3_OVRDIS;
    USART1->BRR=(SYSCLK+HALL_BAUD_RATE/2)/HALL_BAUD_RATE;
    // Initialize parameters for DMAMUX Channel
	DMAMUX1_Channel5->CCR =50; //USART1_RX
    // Disable RX DMA
    USART1->CR3 &= ~USART_CR3_DMAR;
    // Set up the RX DMA
    // Disable DMA
    DMA1_Channel6->CCR &= ~DMA_CCR_EN;
    DMA1_Channel6->CPAR = (uint32_t)&USART1->RDR;
    DMA1_Channel6->CMAR = (uint32_t)Hall_ReceiverBuffer;
    DMA1_Channel6->CNDTR = HALL_BUFFER_SIZE;
    Hall_Receive.BufferSize = HALL_BUFFER_SIZE;
    Hall_Receive.DMA_LastValue = HALL_BUFFER_SIZE;

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
    DMA1_Channel6->CCR=DMA_CCR_EN | DMA_CCR_CIRC | DMA_CCR_MINC | DMA_CCR_PL_1;//
	// Enable RX DMA
    USART1->CR3 |= USART_CR3_DMAR;

    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
    // Enable the USART
    USART1->CR1|=USART_CR1_UE;
}

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void Hall_TransmitData( uint8_t *pData, uint16_t Len )
{
    // Disable TX DMA
    //USART1->CR1 &= ~USART_CR1_TE;
	USART1->CR3 &= ~USART_CR3_DMAT;
	// Set up the TX DMA
    // Initialize parameters for DMAMUX Channel
    DMAMUX1_Channel6->CCR = 51; //USART1_TX
	// Disable DMA
	DMA1_Channel7->CCR&=~DMA_CCR_EN;
	DMA1_Channel7->CPAR=(uint32_t)&USART1->TDR;
	DMA1_Channel7->CMAR=(uint32_t)pData;
	DMA1_Channel7->CNDTR=Len;
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
	DMA1_Channel7->CCR=DMA_CCR_EN | DMA_CCR_DIR | DMA_CCR_MINC;

    USART1->CR3 |= USART_CR3_DMAT;
}

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
uint16_t Hall_GetReceiveLen( void )
{
    uint32_t n;
    uint32_t k;

    k = DMA1_Channel6->CNDTR;
    if( Hall_Receive.DMA_LastValue >= k )
    {
        n = Hall_Receive.DMA_LastValue - k;
    }
    else
    {
        n = Hall_Receive.BufferSize + Hall_Receive.DMA_LastValue - k;
    }
    Hall_Receive.DMA_LastValue = k;

    return( n );
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
BOOL HS_UART_IsTransmissionIdle( void )
{
    if( 0xC0 == (0xC0 & USART1->ISR))
    {
        return 1;
    }
    return 0;
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void HS_UART_TransmitData( uint8_t* pData, uint16_t Length )
{
    Hall_TransmitData( pData, Length );
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
uint16_t HS_UART_GetReceiveDataLength( void )
{
    return( Hall_GetReceiveLen() );
}


/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void HallSticksInit( void )
{
    HallSticks_Protocol.Fault.TX = 0;
    HallSticks_Protocol.Fault.RX = 0;
    HallSticks_Protocol.Fault.Enable = 1;
    HallSticks_Protocol.Fault.Duration = 0;
    HallSticks_Protocol.Fault.Detection = 0;
    HallSticks_Protocol.Fault.Timeout = HS_PACKET_TIMEOUT * 100;
    HallSticks_Protocol.Fault.pDisableFault = 0;
    HallSticks_Protocol.TX.TryNumber = 10;
    HallSticks_Protocol.TX.TryCycle = HS_PACKET_TIMEOUT;
    HallSticks_Protocol.ProtocolTable.pItemHandler = 0;
    HallSticks_Protocol.ProtocolTable.NumOfItems = 0;
    HallSticks_Protocol.Timeout = HS_PACKET_TIMEOUT;
    Hall_UART1_Init();
}

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
int16_t GetHallSticksChannelRealValue( uint8_t Index )
{
    int Value;
    Value = HallSticks.Channels[Index].RealValue;
    return( Value );
}

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
static void HallSticksRefreshChannels( uint8_t* pData, uint16_t Length )
{
    int n = 0;
    int Value;

    if( Length >= 9 )
    {
        int Valid;

        Valid = pData[8];
        if( 0x11 == ( Valid & 0x11 ) )
        {
            if( HS_STATUS_RANGE_READY == HallSticks.Status )
            {
                HallSticks.Status = HS_STATUS_NORMAL;
            }
            else if( HS_STATUS_RESET == HallSticks.Status )
            {
                HallSticks.Status = HS_STATUS_QUERY_RANGE;
            }
        }

        if( Valid & 0x01 )
        {
            n = 0;

            Value = pData[n++];
            Value |= pData[n++] << 8;
            HallSticks.Channels[3].RealValue = Value;
            Value = pData[n++];
            Value |= pData[n++] << 8;
            HallSticks.Channels[2].RealValue = Value;
        }
        else
        {
            HallSticks.Status = HS_STATUS_RESET;
        }

        if( Valid & 0x10 )
        {
            n = 4;
            Value = pData[n++];
            Value |= pData[n++] << 8;
            HallSticks.Channels[1].RealValue = Value;
            Value = pData[n++];
            Value |= pData[n++] << 8;
            HallSticks.Channels[0].RealValue = Value;
        }
        else
        {
            HallSticks.Status = HS_STATUS_RESET;
        }
    }
}

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
static const unsigned char tHallSticksMapping[] =
{
    3, 2, 1, 0
};
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
static void HallSticksRefreshChannelsRange( uint8_t* pData, uint16_t Length )
{
    int i;
    int n = 0;
    int Value;

    if( Length >= 25 )
    {
        for( i = 0; i < GET_ARRAY_SIZE( HallSticks.Channels ); i++ )
        {
            int Index;

            Index = tHallSticksMapping[i];
            Value = pData[n++];
            Value |= pData[n++] << 8;
            HallSticks.Channels[Index].MinValue = Value;
            Value = pData[n++];
            Value |= pData[n++] << 8;
            HallSticks.Channels[Index].MidValue = Value;
            Value = pData[n++];
            Value |= pData[n++] << 8;
            HallSticks.Channels[Index].MaxValue = Value;
            HallSticks.Status = HS_STATUS_RANGE_READY;
        }
    }
}

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void HallSticksReceiverPacketHandler( STRUCT_HS_PROTOCOL *pProtocol )
{
    if( !pProtocol )
    {
        return;
    }

    if( HS_PACKET_ADDR_RC == pProtocol->RX.Packet.RX )
    {
        if( HS_PTCL_ID_CH == pProtocol->RX.Packet.ID )
        {
            HallSticksRefreshChannels( pProtocol->RX.Packet.Payload, pProtocol->RX.Packet.PayloadLength );
        }
        else if( HS_PTCL_ID_RESPONSE == pProtocol->RX.Packet.ID )
        {
            pProtocol->Fault.TX = 0;
            pProtocol->Fault.Detection = 0;
            pProtocol->TX.TryCount = 0;
            pProtocol->TX.TryInterval = 0;
            HallSticksRefreshChannelsRange( pProtocol->RX.Packet.Payload, pProtocol->RX.Packet.PayloadLength );
        }
    }
}

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
static BOOL HS_ID_IsValid( unsigned char ID )
{
    if( ( HS_PTCL_ID_CH == ID ) || ( HS_PTCL_ID_CMD == ID ) || ( HS_PTCL_ID_RESPONSE == ID ) )
    {
        return( TRUE );
    }
    else
    {
        return( FALSE );
    }
}

/*==================================================================================================
name:
function:
input:
output:
==================================================================================================*/
BOOL HallSticksBuildMessage( uint8_t Mode, uint8_t ID, uint8_t* pData, uint32_t Length )
{
    STRUCT_HS_QUEUE  *pQueue;
    STRUCT_HS_MESSAGE *pMsg;

    //ENTER_CRITICAL();
    pQueue = &HallSticks_Protocol.TX.Queue;
    pMsg = &pQueue->Messages[pQueue->Top++];
    if( pQueue->Top >= GET_ARRAY_SIZE( pQueue->Messages ) )
    {
        pQueue->Top = 0;
    }

    pMsg->Mode = Mode;
    pMsg->Packet.ID = ID;
    pMsg->Packet.TX = HS_PACKET_ADDR_RC;
    pMsg->Packet.RX = HS_PACKET_ADDR_HS;
    pMsg->Packet.Sign = HS_PACKET_SIGN;
    pMsg->Packet.PayloadLength = Length;
    if( pData && ( Length > 0 ) )
    {
        int i;

        for( i = 0; ( i < sizeof( pMsg->Packet.Payload )  ) && ( i < Length ); i++ )
        {
            pMsg->Packet.Payload[i] = *pData++;
        }
    }
    //EXIT_CRITICAL();
    return( TRUE );
}

/*==================================================================================================
name:
function:
input:
output:
==================================================================================================*/
void ResetHallSticks( void )
{
    uint8_t Data;

    Data = 1;
    HallSticksBuildMessage( HS_MSG_DIRECT, HS_PTCL_ID_CMD,  &Data, sizeof( Data ) );
}

/*==================================================================================================
name:
function:
input:
output:
==================================================================================================*/
void QueryHallSticksChannelRange( void )
{
    uint8_t Data;

    Data = 0;
    HallSticksBuildMessage( HS_MSG_DIRECT, HS_PTCL_ID_CMD,  &Data, sizeof( Data ) );
}

/*==================================================================================================
name:
function:
input:
output:
==================================================================================================*/
static STRUCT_HS_MESSAGE *GetHallSticksMessage( STRUCT_HS_PROTOCOL *pProtocol )
{
    int i;
    STRUCT_HS_MESSAGE  *pMsg;

    if( pProtocol && ( 0 == pProtocol->TX.TryCount ) )
    {
        if( pProtocol->TX.Queue.Index >= GET_ARRAY_SIZE( pProtocol->TX.Queue.Messages ) )
        {
            pProtocol->TX.Queue.Index = 0;
        }

        for( i = 0; i < GET_ARRAY_SIZE( pProtocol->TX.Queue.Messages ); i++ )
        {
            pMsg = &pProtocol->TX.Queue.Messages[pProtocol->TX.Queue.Index];
            if( pProtocol->TX.Queue.Index == pProtocol->TX.Queue.Top )
            {
                if( HS_MSG_NONE == pMsg->Mode )
                {
                    return( 0 );
                }
            }

            pProtocol->TX.Queue.Index++;
            if( pProtocol->TX.Queue.Index >= GET_ARRAY_SIZE( pProtocol->TX.Queue.Messages ) )
            {
                pProtocol->TX.Queue.Index = 0;
            }

            if( HS_MSG_NONE != pMsg->Mode )
            {
                return( pMsg );
            }
        }
    }

    return( 0 );
}

/*==================================================================================================
name:
function:
input:
output:
==================================================================================================*/
void HallSticksProtocolLayer( void )
{
    int Index;
    int Length;
    STRUCT_HS_PROTOCOL *pProtocol;

    pProtocol = &HallSticks_Protocol;
    if( pProtocol->Fault.Detection )
    {
        if( pProtocol->Fault.Duration >= pProtocol->Fault.Timeout )
        {
            pProtocol->Fault.TX = 1;
            pProtocol->Fault.RX = 1;
            pProtocol->Fault.Duration = 0;
            pProtocol->Fault.Detection = 0;
        }
        else
        {
            pProtocol->Fault.Duration++;
        }
    }
    else
    {
        pProtocol->Fault.Duration = 0;
    }
    /*---------------------------------------------------
                    RX frame process
    ---------------------------------------------------*/
    if( pProtocol->RX.Receiving )
    {
        if( pProtocol->RX.Duration >= pProtocol->Timeout )
        {
            pProtocol->RX.Receiving = 0;
        }
        else
        {
            pProtocol->RX.Duration++;
        }
    }
    else
    {
        pProtocol->RX.Duration = 0;
    }

    Length = HS_UART_GetReceiveDataLength();
    if( Length > 0 )
    {
        pProtocol->Fault.RX = 0;
        pProtocol->RX.IdleTimeout = 0;
        pProtocol->RX.Decode.DataLength += Length;
        if( pProtocol->RX.Decode.DataLength > sizeof( Hall_ReceiverBuffer ) )
        {
            pProtocol->RX.Decode.DataLength = 0;
            pProtocol->RX.Decode.LastPosition = 0;
            pProtocol->RX.Decode.State = HS_DECODE_IDLE;
        }
    }
    else
    {
        if( pProtocol->Fault.Enable )
        {
            if( pProtocol->RX.IdleTimeout >= pProtocol->Fault.Timeout )
            {
                pProtocol->Fault.RX = 1;
            }
            else
            {
                pProtocol->RX.IdleTimeout++;
            }
        }
        else
        {
            pProtocol->RX.IdleTimeout = 0;
        }
    }

    while( pProtocol->RX.Decode.DataLength > 0 )
    {
        if( HS_DECODE_IDLE == pProtocol->RX.Decode.State )
        {
            if( ( uint8_t )HS_PACKET_SIGN == Hall_ReceiverBuffer[pProtocol->RX.Decode.LastPosition] )
            {
                pProtocol->RX.Decode.State = HS_DECODE_ID;
            }
            else
            {
                pProtocol->RX.Decode.DataLength--;
                pProtocol->RX.Decode.LastPosition++;
                if( pProtocol->RX.Decode.LastPosition >= sizeof( Hall_ReceiverBuffer ) )
                {
                    pProtocol->RX.Decode.LastPosition = 0;
                }
            }
        }
        else if( HS_DECODE_ID == pProtocol->RX.Decode.State )
        {
            if( pProtocol->RX.Decode.DataLength >= 2 )
            {
                int Code;

                Index = pProtocol->RX.Decode.LastPosition + 1;
                if( Index >= sizeof( Hall_ReceiverBuffer ) )
                {
                    Index = 0;
                }

                Code = Hall_ReceiverBuffer[Index] & 0x0f;
                if( \
                    ( ((HS_PACKET_ADDR_HS|(HS_PACKET_ADDR_RC << 2)) == Code) \
                   || ((HS_PACKET_ADDR_HS|( HS_PACKET_ADDR_COMPUTER) << 2) == Code) ) \
                   && (HS_ID_IsValid( Hall_ReceiverBuffer[Index] >> 4)) \
                  )
                {
                    pProtocol->RX.Decode.State = HS_DECODE_CRC;
                }
                else
                {
                    pProtocol->RX.Decode.DataLength--;
                    pProtocol->RX.Decode.LastPosition = Index;
                    pProtocol->RX.Decode.State = HS_DECODE_IDLE;
                }
            }
            else
            {
                break;
            }
        }
        else if( HS_DECODE_CRC == pProtocol->RX.Decode.State )
        {
            /*------------------------------------------------------------------
               total length = header length + payload length + crc length
            ------------------------------------------------------------------*/
            if( pProtocol->RX.Decode.DataLength >= 5 )
            {
                int DataLength;

                Index = pProtocol->RX.Decode.LastPosition + 2;
                Index %= sizeof( Hall_ReceiverBuffer );
                DataLength = Hall_ReceiverBuffer[Index] + 5;
                if( pProtocol->RX.Decode.DataLength >= DataLength )
                {
                    int Position;
                    int CheckCode;
                    unsigned char *pDest;

                    pDest = ( unsigned char * )&HallSticks_Protocol.RX.Packet;
                    pProtocol->RX.Decode.State = HS_DECODE_IDLE;
                    Position = pProtocol->RX.Decode.LastPosition;
                    for( Index = 0; Index < DataLength; Index++ )
                    {
                        *pDest++ = Hall_ReceiverBuffer[Position++];
                        if( Position >= sizeof( Hall_ReceiverBuffer ) )
                        {
                            Position = 0;
                        }
                    }

                    Position = pProtocol->RX.Packet.PayloadLength;
                    CheckCode = pProtocol->RX.Packet.Payload[Position]|( pProtocol->RX.Packet.Payload[Position + 1] << 8 );
                    if( CheckCode == Cal_crc16( ( unsigned char * )&pProtocol->RX.Packet, DataLength - 2 ) )
                    {
                        HallSticksReceiverPacketHandler( pProtocol );
                        pProtocol->RX.Decode.DataLength -= DataLength;
                        pProtocol->RX.Decode.LastPosition += DataLength;
                        pProtocol->RX.Decode.LastPosition %= sizeof( Hall_ReceiverBuffer );
                    }
                    else
                    {
                        pProtocol->RX.Decode.DataLength--;
                        pProtocol->RX.Decode.LastPosition++;
                        if(pProtocol->RX.Decode.LastPosition >= sizeof( Hall_ReceiverBuffer ) )
                        {
                            pProtocol->RX.Decode.LastPosition = 0;
                        }
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    /*---------------------------------------------------
                    TX frame process
    ---------------------------------------------------*/
    {
        if( pProtocol->TX.Transmitting )
        {
            /*the current buffer tx end*/
            if( HS_UART_IsTransmissionIdle() )
            {
                pProtocol->TX.Duration = 0;
                pProtocol->TX.Transmitting = 0;
            }
            else if( pProtocol->TX.Duration > 0 )
            {
                pProtocol->TX.Duration--;
                if( 0 == pProtocol->TX.Duration )
                {
                    pProtocol->TX.Transmitting = 0;
                }
            }
        }

        if( !pProtocol->TX.Transmitting )
        {
            STRUCT_HS_MESSAGE *pMsg;

            pMsg = GetHallSticksMessage( pProtocol );
            if( pMsg )
            {
                pProtocol->TX.TotalLength = 0;
                pProtocol->TX.Transmitting = 1;
                if( pProtocol->Fault.Enable )
                {
                    pProtocol->Fault.Detection = 1;
                }
                else
                {
                    pProtocol->Fault.Detection = 0;
                }

                if( HS_MSG_NONE != pMsg->Mode )
                {
                    int i;
                    uint8_t* pSrc;
                    uint8_t* pDest;

                    pSrc = ( uint8_t* )&pMsg->Packet;
                    pDest = ( uint8_t* )&pProtocol->TX.Buffer;
                    pProtocol->TX.TryCount = pProtocol->TX.TryNumber;
                    if( HS_MSG_BUILD == pMsg->Mode )
                    {
                        pProtocol->TX.TotalLength = 0;
                    }
                    else
                    {
                        pProtocol->TX.TotalLength = pMsg->Packet.PayloadLength + 3;
                        for( i = 0; i < pProtocol->TX.TotalLength; i++ )
                        {
                            *pDest++ = *pSrc++;
                        }
                    }

                    i = Cal_crc16( ( uint8_t* )&pProtocol->TX.Buffer, pProtocol->TX.TotalLength );
                    *pDest++ = i;
                    *pDest++ = i >> 8;
                    pProtocol->TX.TotalLength += 2;
                    pMsg->Mode = HS_MSG_NONE;
                    if( pProtocol->Fault.Enable )
                    {
                        pProtocol->Fault.Detection = 1;
                    }
                    else
                    {
                        pProtocol->Fault.Detection = 0;
                    }
                }
            }
        }

        if( 0 == pProtocol->TX.TryInterval )
        {
            if( ( 0 == pProtocol->TX.Duration ) && ( pProtocol->TX.TryCount > 0 ) )
            {
                if( pProtocol->TX.TotalLength > 0 )
                {
                    pProtocol->TX.TryCount--;
                    pProtocol->TX.TryInterval = pProtocol->TX.TryCycle;
                    pProtocol->TX.Transmitting = 1;
                    pProtocol->TX.Duration = pProtocol->Timeout;
                    HS_UART_TransmitData( ( uint8_t* )&pProtocol->TX.Buffer, pProtocol->TX.TotalLength );
                }
                else
                {
                    pProtocol->TX.Duration = 0;
                    pProtocol->TX.TryCount = 0;
                    pProtocol->TX.TryInterval = 0;
                    pProtocol->TX.Transmitting = 0;
                }
            }
        }
        else
        {
            pProtocol->TX.TryInterval--;
        }
    }
}


void HallSticksTask( void )
{
    static unsigned char HallSticksRetryInterval;

    if( HS_STATUS_INIT == HallSticks.Status )
    {
        HallSticksInit();
        ResetHallSticks();
        HallSticksRetryInterval = 0;
        HallSticks.Status = HS_STATUS_RESET;
        return;
    }
    else if( ( HS_STATUS_RESET == HallSticks.Status ) || ( HS_STATUS_QUERY_RANGE == HallSticks.Status ) )
    {
        if( HallSticksRetryInterval >= ( 100 * 1000UL / 2000 ) )
        {
            HallSticksRetryInterval = 0;
            if( HS_STATUS_RESET == HallSticks.Status )
            {
                ResetHallSticks();
            }
            else if( HS_STATUS_QUERY_RANGE == HallSticks.Status )
            {
                QueryHallSticksChannelRange();
            }
        }
        else
        {
            HallSticksRetryInterval++;
        }
    }

    HallSticksProtocolLayer();
}

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
int16_t GetHallStickMaxValue( uint8_t Index )
{
    if( IS_STICK_READY() )
    {
        return( HallSticks.Channels[Index].MaxValue );
    }
    else
    {
        return( STICK_HIGH_SIDE_THR );
    }
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
int16_t GetHallStickMinValue( uint8_t Index )
{
    if( IS_STICK_READY() )
    {
        return( HallSticks.Channels[Index].MinValue );
    }
    else
    {
        return( STICK_LOW_SIDE_THR );
    }
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
int16_t GetHallStickMidValue( uint8_t Index )
{
    if( IS_STICK_READY() )
    {
        return( HallSticks.Channels[Index].MidValue );
    }
    else
    {
        return( STICK_MID_VALUE );
    }
}

