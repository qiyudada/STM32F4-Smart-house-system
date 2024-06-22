#ifndef _Lightsensor_H
#define _Lightsensor_H
#include "stm32f4xx_hal.h"
#include "adc.h"
#include <stdint.h>
#include "usart.h"
#include "tim.h"

#define LIGHT_SENSOR_ADC_TIMEOUT 500
extern ADC_HandleTypeDef hadc1;


void LightSensor_Init(void);
int LightSensor_Read(uint32_t *pData);
void LightSensor_Test(void);

#endif
