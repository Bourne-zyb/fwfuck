#include "main.h"
#include "System.h"
#include "stm32g0xx_it.h"
#include "stdint.h"
#include "string.h"
#include "drv_adc.h"
#include "drv_uart.h"
#include "drv_hall.h"


uint16_t PinState[15];



uint16_t ADC_Value_Temp[5][5];
uint16_t ADC_Value[5];
void ADC_RefreshValues( void )
{
    static uint8_t n = 0;
    uint16_t ADC_Max = 0, ADC_Min = 0xFFFF, ADC_Value_Temp1;
    uint16_t ADC_Value_Sum;


    if ( n < 4 )
    {
        ADC_Value_Temp[0][n] = ADC_Sampling[0];
        ADC_Value_Temp[1][n] = ADC_Sampling[1];
        ADC_Value_Temp[2][n] = ADC_Sampling[2];
        ADC_Value_Temp[3][n] = ADC_Sampling[3];
        ADC_Value_Temp[4][n] = ADC_Sampling[4];
        n++;
    }
    else
    {
        n = 0;

        for ( uint8_t i = 0; i < 5; i++)
        {
            for ( uint8_t j = 0; j < 5; j++)
            {
                ADC_Value_Temp1 = ADC_Value_Temp[i][j];

                if ( ADC_Max < ADC_Value_Temp1 )
                {
                    ADC_Max = ADC_Value_Temp1;
                }
                if ( ADC_Min > ADC_Value_Temp1 )
                {
                    ADC_Min = ADC_Value_Temp1;
                }

                ADC_Value_Sum += ADC_Value_Temp1;

            }

            ADC_Value[i] = ( ADC_Value_Sum - ADC_Max- ADC_Min ) / 3;

            ADC_Max = 0;
            ADC_Min = 0xFFFF;
            ADC_Value_Sum = 0;
        }
    }

}

uint8_t Key_Count[15];
uint8_t Key_State[15];
void KeyPress_Handle( GPIO_TypeDef *GPIO, uint16_t GPIO_Pin, uint8_t Index )
{
    if ( !HAL_GPIO_ReadPin(GPIO, GPIO_Pin) )
    {
        if ( Key_Count[Index] < KEY_PRESS_TIME )
        {
            Key_Count[Index] = KEY_PRESS_TIME;
        }
        else if ( Key_Count[Index] < 2 * KEY_PRESS_TIME )
        {
            Key_Count[Index]++;
        }
        else if ( !(Key_State[Index]&0x01)  )
        {
            Key_State[Index] |= 0x01;
            Key_State[Index] ^= 0x02;

            if ( Key_State[Index]&0x10 )
            {
                if ( Key_State[Index]&0x02 )
                {
                    /* Buzzer */
                }
            }
        }

    }
    else
    {
        if ( Key_Count[Index] > KEY_PRESS_TIME )
        {
            Key_Count[Index] = KEY_PRESS_TIME;
        }
        else if ( Key_Count[Index] )
        {
            Key_Count[Index]--;
        }
        else
        {
            Key_State[Index] &= ~0x01;
            Key_State[Index] |= 0x10;
        }
    }
}


void LockKey_Handle( GPIO_TypeDef *GPIO, uint16_t GPIO_Pin, uint8_t Index )
{
    if ( !HAL_GPIO_ReadPin(GPIO, GPIO_Pin) )
    {
        if ( Key_Count[Index] < KEY_PRESS_TIME )
        {
            Key_Count[Index] = KEY_PRESS_TIME;
        }
        else if ( Key_Count[Index] < 2 * KEY_PRESS_TIME )
        {
            Key_Count[Index]++;
        }
        else
        {
            if ( 10 == Index  )
            {
                SamplingValue.LockSWL = 1;
            }
            else
            {
                SamplingValue.LockSWR = 1;
            }
        }

    }
    else
    {
        if ( Key_Count[Index] > KEY_PRESS_TIME )
        {
            Key_Count[Index] = KEY_PRESS_TIME;
        }
        else if ( Key_Count[Index] )
        {
            Key_Count[Index]--;
        }
        else
        {
            if ( 10 == Index  )
            {
                SamplingValue.LockSWL = 0;
            }
            else
            {
                SamplingValue.LockSWR = 0;
            }
        }
    }

}


void Channel_Calculate( void )
{

    PinState[0] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4);
    {
        //霍尔mini	
        //HAL_UART_Receive_IT(&huart1, datarecv111, 28);
				if(HAL_OK == HAL_UART_Receive(&huart1, datarecv111, 28, 20)){
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
					SEGGER_RTT_printf(0, "LHX:0x%04X LHY:0x%04X RHX:0x%04X RHY:0x%04X JX:0x%04X JY:0x%04X VR1:0x%04X VR2:0x%04X\r\n",
					 SamplingValue.LHallstickX, SamplingValue.LHallstickY,
					 SamplingValue.RHallstickX, SamplingValue.RHallstickY,
					 SamplingValue.JoystickX, SamplingValue.JoystickY,
					 SamplingValue.RotaryVR1, SamplingValue.RotaryVR2
					);
				}else{
					
					// (Framing Error)
					if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_FE))
					{
							__HAL_UART_CLEAR_FEFLAG(&huart1);
					}

					// (Noise Error)
					if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_NE))
					{
							__HAL_UART_CLEAR_NEFLAG(&huart1);
					}

					// (Overrun Error)
					if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_ORE))
					{
							__HAL_UART_CLEAR_OREFLAG(&huart1);
					}
					
					if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_ORE))
					{
							__HAL_UART_CLEAR_OREFLAG(&huart1);
					}
				
					
					SEGGER_RTT_printf(0, "hall err!!! JX:0x%04X JY:0x%04X VR1:0x%04X VR2:0x%04X\r\n",
					 SamplingValue.LHallstickX, SamplingValue.LHallstickY,
					 SamplingValue.RHallstickX, SamplingValue.RHallstickY,
					 SamplingValue.JoystickX, SamplingValue.JoystickY,
					 SamplingValue.RotaryVR1, SamplingValue.RotaryVR2
					);
				}
				SEGGER_RTT_printf(0, "Five:0x%X JC:0x%X TSW:0x%X SSW:0x%X Coder:0x%X LSWL:0x%X LSWR:0x%X\r\n",
				 SamplingValue.Fivestick, SamplingValue.JoystickC,
				 SamplingValue.ToggleSW, SamplingValue.ShipSW,
				 SamplingValue.Coder, SamplingValue.LockSWL, SamplingValue.LockSWR
				);
    }



    {
        //四向
        SamplingValue.JoystickX = ADC_Value[3];
        SamplingValue.JoystickY = ADC_Value[2];
    }

    {
        //V1/V2
        SamplingValue.RotaryVR1 = ADC_Value[0];
        SamplingValue.RotaryVR2 = ADC_Value[1];
    }

    {
        //five
        if ( (ADC_Value[4] < FIVESTICK_UP_ADC + DEADZONE) && (ADC_Value[4] > FIVESTICK_UP_ADC - DEADZONE) )
        {
            SamplingValue.Fivestick = 1;
        }
        else if ( (ADC_Value[4] < FIVESTICK_DOWN_ADC + DEADZONE) && (ADC_Value[4] > FIVESTICK_DOWN_ADC - DEADZONE) )
        {
            SamplingValue.Fivestick = 2;
        }
        else if ( (ADC_Value[4] < FIVESTICK_L_ADC + DEADZONE) && (ADC_Value[4] > FIVESTICK_L_ADC - DEADZONE) )
        {
            SamplingValue.Fivestick = 3;
        }
        else if ( (ADC_Value[4] < FIVESTICK_R_ADC + DEADZONE) && (ADC_Value[4] > FIVESTICK_R_ADC - DEADZONE) )
        {
            SamplingValue.Fivestick = 4;
        }
        else if ( (ADC_Value[4] < FIVESTICK_CENTER_ADC + DEADZONE) && (ADC_Value[4] > FIVESTICK_CENTER_ADC - DEADZONE) )
        {
            SamplingValue.Fivestick = 5;
        }
        else
        {
            SamplingValue.Fivestick = 0;
        }
    }

    {
        //Joystick Center
        KeyPress_Handle( GPIOB, GPIO_PIN_4, 12 );
        SamplingValue.JoystickC = (Key_State[12] & 0x01)? 1 : 0;

    }

    {//VR3
       
//				SamplingValue.Coder = 0;//Left
//				SamplingValue.Coder = 2;//Right        
    }


    {//三档拨杆
        uint8_t ToggleSW_L;
        uint8_t ToggleSW_R;

        ToggleSW_L = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12);
        ToggleSW_R = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13);

        if ( !ToggleSW_L && ToggleSW_R )
        {
            SamplingValue.ToggleSW = 0;
        }
        else if ( ToggleSW_L && !ToggleSW_R )
        {
            SamplingValue.ToggleSW = 2;
        }
        else
        {
            SamplingValue.ToggleSW = 1;
        }
    }

    {//船型三档
        uint8_t ShipSW_L;
        uint8_t ShipSW_R;

        ShipSW_L = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);
        ShipSW_R = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);

        if ( !ShipSW_L && ShipSW_R )
        {
            SamplingValue.ShipSW = 0;
        }
        else if ( ShipSW_L && !ShipSW_R )
        {
            SamplingValue.ShipSW = 2;
        }
        else
        {
            SamplingValue.ShipSW = 1;
        }
    }


    {//SW3、SW4
        LockKey_Handle( GPIOC, GPIO_PIN_13, 10 );
        LockKey_Handle( GPIOA, GPIO_PIN_10, 11 );
    }

    {//K1~K10
        KeyPress_Handle( GPIOA, GPIO_PIN_15, 0 );
        SamplingValue.SW1 = (Key_State[0] & 0x01)? 1 : 0;

        KeyPress_Handle( GPIOD, GPIO_PIN_0, 1 );
        SamplingValue.SW2 = (Key_State[1] & 0x01)? 1 : 0;

        KeyPress_Handle( GPIOD, GPIO_PIN_1, 2 );
        SamplingValue.SW3 = (Key_State[2] & 0x01)? 1 : 0;

        KeyPress_Handle( GPIOD, GPIO_PIN_2, 3 );
        SamplingValue.SW4 = (Key_State[3] & 0x01)? 1 : 0;

        KeyPress_Handle( GPIOD, GPIO_PIN_3, 4 );
        SamplingValue.SW5 = (Key_State[4] & 0x01)? 1 : 0;

        KeyPress_Handle( GPIOB, GPIO_PIN_5, 5 );
        SamplingValue.SW6 = (Key_State[5] & 0x01)? 1 : 0;

        KeyPress_Handle( GPIOB, GPIO_PIN_3, 6 );
        SamplingValue.SW7 = (Key_State[6] & 0x01)? 1 : 0;

        KeyPress_Handle( GPIOA, GPIO_PIN_8, 7 );
        SamplingValue.SW8 = (Key_State[7] & 0x01)? 1 : 0;

        KeyPress_Handle( GPIOA, GPIO_PIN_9, 8 );
        SamplingValue.SW9 = (Key_State[8] & 0x01)? 1 : 0;

        KeyPress_Handle( GPIOC, GPIO_PIN_6, 9 );
        SamplingValue.SW10 = (Key_State[9] & 0x01)? 1 : 0;
    }

}



