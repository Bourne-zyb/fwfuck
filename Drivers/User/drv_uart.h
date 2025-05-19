#ifndef __DRV_UART_H__
#define __DRV_UART_H__

#define UASRT_START_FLAG            0x5A
#define UASRT_END_FLAG              0xAF
#define UASRT_BUFFER_SIZE           0x18

#define USART_CMD_SEND_SAMPLING            0xA0
#define USART_CMD_FIRMWARE_UPDATE          0XA1
#define USART_CMD_FACTORY_LIGHTTEST        0xA2
#define USART_CMD_FACTORY_BUZZERTEST       0xA3

typedef struct __attribute__((packed))
{
    int16_t LHallstickX;       /* 左霍尔摇杆X轴 */
    int16_t LHallstickY;       /* 左霍尔摇杆Y轴 */
    int16_t RHallstickX;       /* 右霍尔摇杆X轴 */
    int16_t RHallstickY;       /* 右霍尔摇杆Y轴 */
    uint16_t JoystickX;         /* 四向小摇杆X轴 */
    uint16_t JoystickY;         /* 四向小摇杆Y轴 */
    uint16_t RotaryVR1;         /* 自回中拨轮键VR1 */
    uint16_t RotaryVR2;         /* 自回中拨轮键VR2 */
    uint8_t  Fivestick:3;       /* 五向开关 */
    uint8_t  JoystickC:1;       /* 四向小摇杆中间按下 */
    uint8_t  ToggleSW:2;        /* 三档拨杆开关 */
    uint8_t  ShipSW:2;          /* 三档船型开关 */
    uint8_t  Coder:2;           /* 编码器 */
    uint8_t  LockSWL:1;         /* 自锁开关S3 */
    uint8_t  LockSWR:1;         /* 自锁开关S4 */
    uint8_t  SW1:1;             /* 自复位按钮K1 */
    uint8_t  SW2:1;             /* 自复位按钮K2 */
    uint8_t  SW3:1;             /* 自复位按钮K3 */
    uint8_t  SW4:1;             /* 自复位按钮K4 */
    uint8_t  SW5:1;             /* 自复位按钮K5 */
    uint8_t  SW6:1;             /* 自复位按钮K6 */
    uint8_t  SW7:1;             /* 自复位按钮K7 */
    uint8_t  SW8:1;             /* 自复位按钮K8 */
    uint8_t  SW9:1;             /* 自复位按钮K9 */
    uint8_t  SW10:1;            /* 自复位按钮K10 */
    uint8_t  Reserve:2;
}sSampling;


typedef struct __attribute__((packed))
{
    uint8_t    StartFlag;
    uint8_t    Command;
    uint8_t    PackLength;
    sSampling  Data;
    uint8_t    Checksum;
    uint8_t    EndFlag;
}sUartPack;


extern sUartPack   Uart2TxPacket;
extern sSampling   SamplingValue;
extern uint8_t Usart2_Rx_Data_buff[UASRT_BUFFER_SIZE];
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;

void UART2_Init(  uint8_t *pBuffer, uint16_t Len  );
void Usart2_TxBackground( void );
void Usart2_RxBackground( void );
void MX_USART1_UART_Init(void);

#endif
