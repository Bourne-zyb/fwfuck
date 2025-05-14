
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

uint32_t Real_Time_Task;
uint16_t ADC_Sampling[5];

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
	I2C_Init();
	AW9523_Init();

	while (1)
	{
		if ( SYS_SysTickMs - Real_Time_Task > 10 )
		{
			Real_Time_Task = SYS_SysTickMs;
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
	//旭哥说Cube MX时钟频率PLLM、PLLN、PLLP、PLLR无特殊要求就随便填，只要使得系统时钟频率达到芯片可调的最大值即可
	//(这里查找参考手册System clock(SYSCLK) selection章节可以知道STM32G070系统时钟最高频率为64MHz)
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

	SYS_SysTickMs=0;

	RCC->APBENR1|=RCC_APBENR1_TIM6EN;
	// Reset TIM6
	RCC->APBRSTR1|=RCC_APBRSTR1_TIM6RST;
	RCC->APBRSTR1&=~RCC_APBRSTR1_TIM6RST;
	// CEN=0 counter disabled
	// UDIS=0 Update event enabled
	// URS=0 Update event is any event
	// OPM=0 Counter is not stopped at update event
	// ARPE=0 ARR register is not buffered
	TIM6->CR1=0;
	// UIE=1 interrupt enabled
	// Other interrupts and DMA disabled
	TIM6->DIER=TIM_DIER_UIE;
	// Prescaler=1MHz timer counter clock
	TIM6->PSC=64000000/1000000-1;
	// Generate an update event to update the prescaler
	TIM6->EGR=TIM_EGR_UG;
	// Auto reload register set to 999 (counter overflows every 1ms)
	TIM6->ARR=1000-1;
	// Enable the timer counter
	TIM6->CR1|=TIM_CR1_CEN;
	// Set the timer 6 IRQ to low priority and enable it
	NVIC_SetPriority(TIM6_IRQn,3);
	NVIC->ISER[0]=1UL<<TIM6_IRQn;
	// Trigger an event
	TIM6->EGR|=TIM_EGR_UG;

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
