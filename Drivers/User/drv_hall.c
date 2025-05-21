#include "drv_hall.h"
#include "crc16.h"
#include "drv_uart.h"

static uint8_t datarecv[28];
static HallData_t halldata;

void HALL_init(void)
{
    MX_USART1_UART_Init();
}

void HALL_getvalue_start_once(void)
{
    HAL_UART_Receive_IT(&huart1, datarecv, 28);
}

static int extract_valid_frame(uint8_t *buf, size_t len, HallData_t *outFrame)
{
    for (size_t i = 0; i <= len; ++i) {
        if (buf[i] == FRAME_HEADER_0 &&
            buf[i + 1] == FRAME_HEADER_1 &&
            buf[i + 2] == FRAME_HEADER_2) {

            uint8_t *frame_ptr = &buf[i];
            uint16_t crc_calc = Cal_crc16(frame_ptr, FRAME_SIZE - 2);
            uint16_t crc_recv = frame_ptr[12] | (frame_ptr[13] << 8);

            if (crc_calc == crc_recv) {
                // 解析 Payload（第4字节开始，共9字节）
                uint8_t *payload = &frame_ptr[3];
                for (int j = 0; j < 4; ++j) {
                    outFrame->ch[j] = (payload[j * 2] << 8) | payload[j * 2 + 1];
                }
                outFrame->stickState = payload[8];
                return 1; // 成功
            }
        }
    }
    return 0; // 没有找到合法帧
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // (Framing Error)
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_FE))
    {
        __HAL_UART_CLEAR_FEFLAG(&huart1);
        return;
    }
    // (Noise Error)
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_NE))
    {
        __HAL_UART_CLEAR_NEFLAG(&huart1);
        return;
    }
    // (Overrun Error)
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_ORE))
    {
        __HAL_UART_CLEAR_OREFLAG(&huart1);
        return;
    }
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_ORE))
    {
        __HAL_UART_CLEAR_OREFLAG(&huart1);
        return;
    }

    if (huart->Instance == USART1)
    {	
        if(1 == extract_valid_frame(datarecv, 28, &halldata)){
            if((halldata.stickState & 0x01) == 0x01){
                SamplingValue.LHallstickX = halldata.ch[0];
                SamplingValue.LHallstickY = halldata.ch[1];
            }else{
                SamplingValue.LHallstickX = FRAME_ERR;
                SamplingValue.LHallstickY = FRAME_ERR;
            }
            if((halldata.stickState & 0x10) == 0x10){
                SamplingValue.RHallstickX = halldata.ch[2];
                SamplingValue.RHallstickY = halldata.ch[3];
            }else{
                SamplingValue.RHallstickX = FRAME_ERR;
                SamplingValue.RHallstickY = FRAME_ERR;
            }
        }
			
		SEGGER_RTT_printf(0, "CH1: 0x%04X, CH2: 0x%04X \
				CH3: 0x%04X, CH4: 0x%04X,   StickState: 0x%02X\r\n", \
            (uint16_t)halldata.ch[0], (uint16_t)halldata.ch[1], \
            (uint16_t)halldata.ch[2], (uint16_t)halldata.ch[3], \
            halldata.stickState);		
    }
}

