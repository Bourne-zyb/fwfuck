#ifndef __DRV_ADC_H__
#define __DRV_ADC_H__

#include "main.h"

//#define FIVESTICK_UP_ADC       	0x0816
//#define FIVESTICK_DOWN_ADC     	0x0ac0
//#define FIVESTICK_L_ADC       	0x056d
//#define FIVESTICK_R_ADC     		0x02bb
//#define FIVESTICK_CENTER_ADC    0x0d5c

#define FIVESTICK_UP_ADC       	0x080b
#define FIVESTICK_DOWN_ADC     	0x0aaa
#define FIVESTICK_L_ADC       	0x0561
#define FIVESTICK_R_ADC     		0x02bb
#define FIVESTICK_CENTER_ADC    0x0d5c

#define DEADZONE                100

extern ADC_HandleTypeDef hadc;


void MX_ADC1_Init( void );

#endif
