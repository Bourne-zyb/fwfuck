#ifndef __IO_EXPANDER_H__
#define __IO_EXPANDER_H__

#define AW9523_ADDR       0xB6


typedef enum
{
    AW9523_P0,
    AW9523_P1
}ENUM_EXPANDER_TYPE;


#define KEY_LED1_ON()       Expander_Gpio_Write_Bit( AW9523_P1, GPIO_PIN_0, 1 );
#define KEY_LED1_OFF()      Expander_Gpio_Write_Bit( AW9523_P1, GPIO_PIN_0, 0 );

#define KEY_LED2_ON()       Expander_Gpio_Write_Bit( AW9523_P1, GPIO_PIN_1, 1 );
#define KEY_LED2_OFF()      Expander_Gpio_Write_Bit( AW9523_P1, GPIO_PIN_1, 0 );

#define KEY_LED3_ON()       Expander_Gpio_Write_Bit( AW9523_P1, GPIO_PIN_2, 1 );
#define KEY_LED3_OFF()      Expander_Gpio_Write_Bit( AW9523_P1, GPIO_PIN_2, 0 );

#define KEY_LED4_ON()       Expander_Gpio_Write_Bit( AW9523_P1, GPIO_PIN_3, 1 );
#define KEY_LED4_OFF()      Expander_Gpio_Write_Bit( AW9523_P1, GPIO_PIN_3, 0 );

#define KEY_LED5_ON()       Expander_Gpio_Write_Bit( AW9523_P0, GPIO_PIN_0, 1 );
#define KEY_LED5_OFF()      Expander_Gpio_Write_Bit( AW9523_P0, GPIO_PIN_0, 0 );

#define KEY_LED6_ON()       Expander_Gpio_Write_Bit( AW9523_P0, GPIO_PIN_1, 1 );
#define KEY_LED6_OFF()      Expander_Gpio_Write_Bit( AW9523_P0, GPIO_PIN_1, 0 );

#define KEY_LED7_ON()       Expander_Gpio_Write_Bit( AW9523_P0, GPIO_PIN_2, 1 );
#define KEY_LED7_OFF()      Expander_Gpio_Write_Bit( AW9523_P0, GPIO_PIN_2, 0 );

#define KEY_LED8_ON()       Expander_Gpio_Write_Bit( AW9523_P0, GPIO_PIN_3, 1 );
#define KEY_LED8_OFF()      Expander_Gpio_Write_Bit( AW9523_P0, GPIO_PIN_3, 0 );

#define KEY_LED9_ON()       Expander_Gpio_Write_Bit( AW9523_P0, GPIO_PIN_4, 1 );
#define KEY_LED9_OFF()      Expander_Gpio_Write_Bit( AW9523_P0, GPIO_PIN_4, 0 );

#define KEY_LED10_ON()       Expander_Gpio_Write_Bit( AW9523_P0, GPIO_PIN_5, 1 );
#define KEY_LED10_OFF()      Expander_Gpio_Write_Bit( AW9523_P0, GPIO_PIN_5, 0 );

void AW9523_Init_not( void );


#endif
