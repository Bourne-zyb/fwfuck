
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

void SystemClock_Config(void);

uint8_t datarecv111[28];
FrameData_t frameData;

// CRC16-IBM (Modbus) 多用于通信协议
uint16_t crc16_compute(const uint8_t *data, uint16_t length) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; ++j) {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc = crc >> 1;
        }
    }
    return crc;
}

int extract_valid_frame(uint8_t *buf, size_t len, FrameData_t *outFrame)
{
	for (size_t i = 0; i <= len; ++i) {
		if (buf[i] == FRAME_HEADER_0 &&
			buf[i + 1] == FRAME_HEADER_1 &&
			buf[i + 2] == FRAME_HEADER_2) {

			uint8_t *frame_ptr = &buf[i];
			uint16_t crc_calc = crc16_compute(frame_ptr, FRAME_SIZE - 2);
			uint16_t crc_recv = frame_ptr[13] | (frame_ptr[12] << 8);

			//if (crc_calc == crc_recv) {
				// 解析 Payload（第4字节开始，共9字节）
				uint8_t *payload = &frame_ptr[3];
				for (int j = 0; j < 4; ++j) {
					outFrame->ch[j] = (payload[j * 2] << 8) | payload[j * 2 + 1];
				}
				outFrame->stickState = payload[8];
				return 1; // 成功
			//}
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
	
	//I2C_Init();
	//AW9523_Init();

	while (1)
	{
		//if ( SYS_SysTickMs - Real_Time_Task > 10 )
		{
			//HAL_Delay(10);
			ADC_RefreshValues();
		//	Real_Time_Task = SYS_SysTickMs;
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
