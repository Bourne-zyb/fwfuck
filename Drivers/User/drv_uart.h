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
    int16_t LHallstickX;       /* �����ҡ��X�� */
    int16_t LHallstickY;       /* �����ҡ��Y�� */
    int16_t RHallstickX;       /* �һ���ҡ��X�� */
    int16_t RHallstickY;       /* �һ���ҡ��Y�� */
    uint16_t JoystickX;         /* ����Сҡ��X�� */
    uint16_t JoystickY;         /* ����Сҡ��Y�� */
    uint16_t RotaryVR1;         /* �Ի��в��ּ�VR1 */
    uint16_t RotaryVR2;         /* �Ի��в��ּ�VR2 */
    uint8_t  Fivestick:3;       /* ���򿪹� */
    uint8_t  JoystickC:1;       /* ����Сҡ���м䰴�� */
    uint8_t  ToggleSW:2;        /* �������˿��� */
    uint8_t  ShipSW:2;          /* �������Ϳ��� */
    uint8_t  Coder:2;           /* ������ */
    uint8_t  LockSWL:1;         /* ��������S3 */
    uint8_t  LockSWR:1;         /* ��������S4 */
    uint8_t  SW1:1;             /* �Ը�λ��ťK1 */
    uint8_t  SW2:1;             /* �Ը�λ��ťK2 */
    uint8_t  SW3:1;             /* �Ը�λ��ťK3 */
    uint8_t  SW4:1;             /* �Ը�λ��ťK4 */
    uint8_t  SW5:1;             /* �Ը�λ��ťK5 */
    uint8_t  SW6:1;             /* �Ը�λ��ťK6 */
    uint8_t  SW7:1;             /* �Ը�λ��ťK7 */
    uint8_t  SW8:1;             /* �Ը�λ��ťK8 */
    uint8_t  SW9:1;             /* �Ը�λ��ťK9 */
    uint8_t  SW10:1;            /* �Ը�λ��ťK10 */
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
