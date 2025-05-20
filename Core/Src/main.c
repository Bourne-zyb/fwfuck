
/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "string.h"
#include "stm32g0xx_it.h"
#include "stm32g0xx_hal.h"
#include "drv_adc.h"
#include "drv_gpio.h"
#include "System.h"
#include "drv_uart.h"
#include "dma.h"
#include "drv_i2c.h"
#include "IO_Expander.h"
#include "main.h"

#include "SEGGER_RTT.h"
#include "tim.h"
#include "LED_AW95.h"

/* USER CODE BEGIN PTD */
#define LED_NUM        5
#define BITS_PER_LED   24
#define RESET_SLOTS    100   // 保证 > 80?s 的低电平
#define PWM_ARR   57     // 周期 0.89 ?s
#define CCR_0     19     // 表示 bit 0（高电平 0.3 ?s）
#define CCR_1     38     // 表示 bit 1（高电平 0.6 ?s）


// 最终发送的缓冲：5个LED × 24bit + 起始Reset + 结尾Reset
#define TOTAL_SLOTS    (RESET_SLOTS + LED_NUM * BITS_PER_LED + RESET_SLOTS)

uint16_t pwm_dma_buf[TOTAL_SLOTS];

void set_rgb_color(uint8_t r, uint8_t g, uint8_t b, uint16_t *out_buf)
{
    uint8_t grb[3] = { g, r, b };
    for (int c = 0; c < 3; c++) {
        for (int i = 7; i >= 0; i--) {
            *out_buf++ = (grb[c] & (1 << i)) ? CCR_1 : CCR_0;
        }
    }
}

void build_pwm_buffer_1(void)
{
    int i = 0;

    // 起始 RESET（保持低电平）
    for (int j = 0; j < RESET_SLOTS; j++) {
        pwm_dma_buf[i++] = 0;  // 占空比为 0%
    }

		// 每个 LED 写入 PWM 波形
    for (int led = 0; led < 1; led++) {

        uint8_t r = 255, g = 0, b = 0;
        set_rgb_color(r, g, b, &pwm_dma_buf[i]);
        i += BITS_PER_LED;
    }
		
    // 每个 LED 写入 PWM 波形
    for (int led = 1; led < LED_NUM; led++) {

        uint8_t r = 0, g = 255, b = 0;
        set_rgb_color(r, g, b, &pwm_dma_buf[i]);
        i += BITS_PER_LED;
    }

    // 结束 RESET（保持低电平）
    for (int j = 0; j < RESET_SLOTS; j++) {
        pwm_dma_buf[i++] = 0;
    }
}

void build_pwm_buffer(void)
{
    int i = 0;

    // 起始 RESET（保持低电平）
    for (int j = 0; j < RESET_SLOTS; j++) {
        pwm_dma_buf[i++] = 0;  // 占空比为 0%
    }

    // 每个 LED 写入 PWM 波形
    for (int led = 0; led < LED_NUM; led++) {
        // 示例：全部红色亮
        uint8_t r = 255, g = 0, b = 0;
        set_rgb_color(r, g, b, &pwm_dma_buf[i]);
        i += BITS_PER_LED;
    }

    // 结束 RESET（保持低电平）
    for (int j = 0; j < RESET_SLOTS; j++) {
        pwm_dma_buf[i++] = 0;
    }
}


void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_4);
}



void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  if(GPIO_Pin == GPIO_PIN_14)
  {
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_RESET)
		{
				SEGGER_RTT_printf(1, "LLLLLLLLLL\r\n");
				SamplingValue.Coder = 0;//Left	  
		}
		else
		{
				SEGGER_RTT_printf(1, "RRRRRRRRRRR\r\n");
				SamplingValue.Coder = 2;//Right  
		}
  }
}


void SystemClock_Config(void);

uint8_t datarecv111[28];
FrameData_t frameData;


/* CRC16 implementation acording to CCITT standards */
static const unsigned short CRC16Table[]=
{
        0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
        0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
        0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
        0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
        0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
        0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
        0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
        0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
        0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
        0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
        0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
        0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
        0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
        0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
        0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
        0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
        0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
        0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
        0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
        0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
        0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
        0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
        0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
        0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
        0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
        0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
        0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
        0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
        0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
        0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
        0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
        0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

uint16_t Crc16(const unsigned char *pBuffer, unsigned long BufferSize)
{
        unsigned short CRC16;

        CRC16=0xFFFF;
        while (BufferSize)
        {
                CRC16=(CRC16<<8)^CRC16Table[((CRC16>>8)^*pBuffer)&0x00FF];
                pBuffer++;
                BufferSize--;
        }
        return CRC16;
}

int extract_valid_frame(uint8_t *buf, size_t len, FrameData_t *outFrame)
{
	for (size_t i = 0; i <= len; ++i) {
		if (buf[i] == FRAME_HEADER_0 &&
			buf[i + 1] == FRAME_HEADER_1 &&
			buf[i + 2] == FRAME_HEADER_2) {

			uint8_t *frame_ptr = &buf[i];
			uint16_t crc_calc = Crc16(frame_ptr, FRAME_SIZE - 2);
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



uint32_t Real_Time_Task;
uint16_t ADC_Sampling[5];


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {	
			if(1 == extract_valid_frame(datarecv111, 28, &frameData)){
            if((frameData.stickState & 0x01) == 0x01){
                SamplingValue.LHallstickX = frameData.ch[0];
                SamplingValue.LHallstickY = frameData.ch[1];
            }else{
                SamplingValue.LHallstickX = FRAME_ERR;
                SamplingValue.LHallstickY = FRAME_ERR;
            }
            if((frameData.stickState & 0x10) == 0x10){
                SamplingValue.RHallstickX = frameData.ch[2];
                SamplingValue.RHallstickY = frameData.ch[3];
            }else{
                SamplingValue.RHallstickX = FRAME_ERR;
                SamplingValue.RHallstickY = FRAME_ERR;
            }
        }
			
			SEGGER_RTT_printf(0, "CH1: 0x%04X, CH2: 0x%04X, CH3: 0x%04X, CH4: 0x%04X, StickState: 0x%02X\r\n",
          (uint16_t)frameData.ch[0], (uint16_t)frameData.ch[1], (uint16_t)frameData.ch[2], (uint16_t)frameData.ch[3], frameData.stickState);
  
        //HAL_UART_Receive_IT(&huart1, datarecv111, 28);
			
    }
}

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_ADC1_Init();
	HAL_ADCEx_Calibration_Start(&hadc);
	HAL_ADC_Start_DMA(&hadc, (uint32_t *)&ADC_Sampling, 5);
	UART2_Init(Usart2_Rx_Data_buff, sizeof(Usart2_Rx_Data_buff));
	MX_USART1_UART_Init();
	
	MX_TIM3_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
	//I2C_Init();
	//AW9523_Init();

	build_pwm_buffer();
	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_4, (uint32_t *)pwm_dma_buf, TOTAL_SLOTS);
	HAL_Delay(10);
	build_pwm_buffer_1();
	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_4, (uint32_t *)pwm_dma_buf, TOTAL_SLOTS);//
	HAL_Delay(10);
	
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 2500);
  //开启TIM3 PWM输出
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  //关闭TIM3 PWM输出
  HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
	
	
	
#if 1
  AW9523_HWReset();
  HAL_Delay(1000);
  // 初始化AW9523
  if (AW9523_Init() != 0) {
    // 初始化失败处理
    while (1);
  }
  // 点亮第3个LED，亮度为128
  for (uint8_t i = 0; i < 11; i++) {
    AW9523_SetBrightness(i, 128);
  }
  // 所有LED亮度设置为最大
  AW9523_SetAllBrightness(255);
#endif
	while (1)
	{
		//if ( SYS_SysTickMs - Real_Time_Task > 10 )
		{
			//	HAL_Delay(10);
			//	Real_Time_Task = SYS_SysTickMs;
			
			ADC_RefreshValues();
			Channel_Calculate();
			Usart2_TxBackground();
		}
	}
}



void SystemClock_Config(void)
{

	 RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
