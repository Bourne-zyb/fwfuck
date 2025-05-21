
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
#include "IO_Expander.h"
#include "main.h"

#include "SEGGER_RTT.h"
#include "drv_hall.h"
#include "drv_buzzer.h"
#include "drv_LED_RGB.h"
#include "drv_LED_AW95.h"


void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  if(GPIO_Pin == GPIO_PIN_14)
  {
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_RESET)
		{
				SEGGER_RTT_printf(0, "LLLLLLLLLL\r\n");
				SamplingValue.Coder = 0;//Left	  
		}
		else
		{
				SEGGER_RTT_printf(0, "RRRRRRRRRRR\r\n");
				SamplingValue.Coder = 2;//Right  
		}
  }
}


void SystemClock_Config(void);
uint16_t ADC_Sampling[5];

int main(void)
{
  uint32_t Real_Time_Task;

	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_ADC1_Init();
	HAL_ADCEx_Calibration_Start(&hadc);
	HAL_ADC_Start_DMA(&hadc, (uint32_t *)&ADC_Sampling, 5);
  UART2_Init(Usart2_Rx_Data_buff, sizeof(Usart2_Rx_Data_buff));
	
  HALL_init();
  Buzzer_Init();
  LED_RGB_Init();
  LED_AW9523_Init();

  LED_RGB_ConfigColor();
  LED_RGB_Show();

  LED_AW9523_HWReset();
  LED_AW9523_cfg();

	for (uint8_t i = 0; i < 11; i++) {
    LED_AW9523_SetBrightness(i, BRIGHTNESS_MAX / 2);
  }
  LED_AW9523_SetAllBrightness(BRIGHTNESS_MAX);

	while (1)
	{
		if ( uwTick - Real_Time_Task > 10 )
		{
			Real_Time_Task = uwTick;
			
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
