#include "main.h"
#include "drv_i2c.h"
#include "IO_Expander.h"

static volatile uint8_t AW9523_Output_Data_P0 = 0;
static volatile uint8_t AW9523_Output_Data_P1 = 0;

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void AW9523_Init_not( void )
{
    uint8_t Result;
    uint32_t Count = 0;
    uint8_t data[2]={0x00, 0x00};

    Result = IsDeviceReady( AW9523_ADDR );
    while ( Result )
    {
        HAL_Delay(20);
        Result = IsDeviceReady( AW9523_ADDR );

        Count++;
        if ( Count > 10 )
        {
            return;
        }
    }

    I2C_Write( AW9523_ADDR, 0x04, (void*)&data[0], 1 );
    I2C_Write( AW9523_ADDR, 0x05, (void*)&data[1], 1 );


    // Bit0=0  : KEY_LED5
    // Bit1=0  : KEY_LED6
    // Bit2=0  : KEY_LED7
    // Bit3=0  : KEY_LED8
    // Bit4=0  : KEY_LED9
    // Bit5=0  : KEY_LED10
    // Bit6=0  : 0
    // I2C_Write( AW9523_ADDR, 0x02, (void*)&AW9523_Output_Data_P0, 1 );


    // Bit0=0  : KEY_LED1
    // Bit1=0  : KEY_LED2
    // Bit2=0  : KEY_LED3
    // Bit3=0  : KEY_LED4
    // Bit4=0  : 0
    // Bit5=0  : 0
    // Bit6=0  : 0
    // Bit7=0  : 0
    // I2C_Write( AW9523_ADDR, 0x03, (void*)&AW9523_Output_Data_P1, 1 );

    AW9523_Output_Data_P0 = 0x3F;
    I2C_Write( AW9523_ADDR, 0x02, (void*)&AW9523_Output_Data_P0, 1 );
    AW9523_Output_Data_P1 = 0x0F;
    I2C_Write( AW9523_ADDR, 0x03, (void*)&AW9523_Output_Data_P1, 1 );
}


/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void Expander_Gpio_Write_Bit( ENUM_EXPANDER_TYPE Type, uint8_t GPIO_Pin, uint8_t Pin_State )
{
    uint16_t Temp = GPIO_Pin & 0xFF;
    volatile uint8_t *Output_Data = Type? &AW9523_Output_Data_P1 : &AW9523_Output_Data_P0;
    uint8_t Mem_Addr = Type? 0x03 : 0x02;
    uint8_t Current_State = (*Output_Data & Temp);

    if ( (Current_State && Pin_State) || (!Current_State && !Pin_State) )
    {
        return;
    }

    if ( Pin_State )
    {
        *Output_Data |= Temp;
    }
    else
    {
        *Output_Data &= ~Temp;
    }

    I2C_Write( AW9523_ADDR, Mem_Addr, (void*)Output_Data, 1 );

}


