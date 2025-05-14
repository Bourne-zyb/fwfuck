#ifndef __DRV_HALL__
#define __DRV_HALL__

#define HALL_BAUD_RATE              921600
#define HALL_BUFFER_SIZE            128

#define GET_HALL_STICK_REAL_VALUE( CH )              GetHallSticksChannelRealValue( CH )
#define GET_HALL_STICKS_STATUS()                     ( HallSticks.Status )
#define IS_STICK_READY()                     	( HS_STATUS_NORMAL == GET_HALL_STICKS_STATUS() )
#define GET_ARRAY_SIZE( ARRAY )              ( ( sizeof( ARRAY ) ) / ( sizeof( ARRAY[0] ) ) )

#define HS_PACKET_SIGN                    ( 0x55 )
#define HS_PACKET_ADDR_HS                 ( 0x00 )
#define HS_PACKET_ADDR_RC                 ( 0x01 )
#define HS_PACKET_ADDR_COMPUTER           ( 0x02 )
#define HS_PTCL_ID_CH                     ( 0x0C )
#define HS_PTCL_ID_CMD                    ( 0x0D )
#define HS_PTCL_ID_RESPONSE               ( 0x0E )

#define HS_TICK                           (2000)//( SYSTEM_TICK )
#define HS_PACKET_TIMEOUT                 ( 5000UL * 1 / HS_TICK )

#define STICK_MAX_VALUE                      ( 3500 )
#define STICK_MID_VALUE                      ( 0 )
#define STICK_MIN_VALUE                      ( -3500 )

#define STICK_LOW_SIDE_THR                   ( -1500 )
#define STICK_HIGH_SIDE_THR                  ( 1500 )

#define STICK_NEUTRAL_ERROR                  ( 1024 )
#define STICK_LOW_SIDE_ERROR                 ( 1024 )
#define STICK_HIGH_SIDE_ERROR                ( 1024 )


enum ENUM_HS_MSG_MODE
{
    HS_MSG_NONE,
    HS_MSG_BUILD,
    HS_MSG_DIRECT,
};

typedef enum
{
    HS_STATUS_INIT,
    HS_STATUS_RESET,
    HS_STATUS_QUERY_RANGE,
    HS_STATUS_RANGE_READY,
    HS_STATUS_NORMAL
}ENUM_HS_STATUS;

typedef enum
{
    HS_DECODE_IDLE,
    HS_DECODE_ID,
    HS_DECODE_CRC,
    HS_DECODE_PAYLOAD
}ENUM_HS_PTCL_DECODE;

typedef struct
{
    uint16_t BufferSize;
    uint16_t DMA_LastValue;
}STRUCT_UART_RECEIVE;

typedef __packed struct
{
    uint8_t Sign;
    uint8_t TX:2;
    uint8_t RX:2;
    uint8_t ID:4;
    uint8_t PayloadLength;
    uint8_t Payload[32];
}STRUCT_HS_PACKET;

typedef struct
{
    uint8_t Mode;
    STRUCT_HS_PACKET Packet;
}STRUCT_HS_MESSAGE;

typedef struct
{
    uint8_t Top;
    uint8_t Index;
    STRUCT_HS_MESSAGE Messages[3];
}STRUCT_HS_QUEUE;

typedef __packed struct
{
    uint8_t   ID;
    uint16_t  ( *pBuildPacket )( uint8_t* pRequest, uint8_t* pData, uint16_t Length );
    uint16_t  ( *pBuildResponse )( uint8_t* pRequest, uint8_t* pData, uint16_t Length );
    void ( *pSetupHandler )( uint8_t* pData, uint16_t Length );
    void ( *pResponseHandler )( uint8_t* pData, uint16_t Length );
}STRUCT_HS_PTCL_ID_HANDLER;

typedef struct
{
    uint16_t Timeout;
    struct
    {
        uint8_t    TX:1;
        uint8_t    RX:1;
        uint8_t    Enable:1;
        uint8_t    Detection:1;
        uint16_t   Timeout;
        uint16_t   Duration;
        BOOL ( *pDisableFault )( void );

    }Fault;

    struct
    {
        uint8_t  Transmitting;
        uint8_t  TryNumber;
        uint8_t  TryCount;
        uint16_t TryCycle;
        uint16_t TryInterval;
        uint16_t TotalLength;
        uint16_t Duration;
        uint8_t  Buffer[128];
        STRUCT_HS_QUEUE Queue;
    }TX;

    struct
    {
        uint8_t  Receiving;
        uint16_t Duration;
        uint16_t IdleTimeout;
        struct
        {
            uint8_t DataLength;
            uint8_t LastPosition;
            ENUM_HS_PTCL_DECODE State;
        }Decode;
        STRUCT_HS_PACKET Packet;
    }RX;

    struct
    {
        uint8_t NumOfItems;
        STRUCT_HS_PTCL_ID_HANDLER *pItemHandler;
    }ProtocolTable;
}STRUCT_HS_PROTOCOL;

typedef struct
{
    ENUM_HS_STATUS Status;
    struct
    {
        int16_t  MaxValue;
        int16_t  MidValue;
        int16_t  MinValue;
        int16_t  RealValue;
    }Channels[4];
}STRUCT_HALL_STICKS;

extern STRUCT_HALL_STICKS HallSticks;

void Hall_UART1_Init( void );
void Hall_TransmitData( uint8_t *pData, uint16_t Len );
uint16_t Hall_GetReceiveLen( void );
void HallSticksTask( void );
int16_t  GetHallStickMaxValue( uint8_t Index );
int16_t  GetHallStickMidValue( uint8_t Index );
int16_t  GetHallStickMinValue( uint8_t Index );


#endif

