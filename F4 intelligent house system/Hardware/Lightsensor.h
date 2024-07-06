#ifndef _Lightsensor_H
#define _Lightsensor_H
#include "stm32f4xx_hal.h"
#include "adc.h"
#include <stdint.h>
#include "usart.h"
#include "tim.h"
#include "semphr.h"
#include "Lcd.h"

#define LIGHT_SENSOR_ADC_TIMEOUT 500

extern ADC_HandleTypeDef hadc1;

struct Light_Data{
	int Volt;
	int Sample;
	char data_sensor[50];
};

void LightSensor_Init(void);
int LightSensor_Read(int *pData);
void LightSensor_Test(void);
void LightSensor_MQTT_Task(void *para);

#endif
