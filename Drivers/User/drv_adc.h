#ifndef __DRV_ADC_H__
#define __DRV_ADC_H__

#include "main.h"

#define FIVESTICKL_UP_ADC       1353
#define FIVESTICKL_DOWN_ADC     1960
#define FIVESTICKR_UP_ADC       2680
#define FIVESTICKR_DOWN_ADC     3388
#define FIVESTICK_CENTER_ADC    375

#define DEADZONE                60

extern ADC_HandleTypeDef hadc;


void MX_ADC1_Init( void );

#endif
