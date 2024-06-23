#ifndef _DTH11_H
#define _DTH11_H
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "tim.h"

struct DTH11_Data{
	int Temp;
	int Humid;
};
	
#define DTH11_QUEUE_LEN 10
#define DTH11_Port GPIOA
#define DTH11_PIN GPIO_PIN_12

#define DHT11_HIGH     HAL_GPIO_WritePin(GPIOA, DTH11_PIN,	GPIO_PIN_SET) //输出高电平
#define DHT11_LOW      HAL_GPIO_WritePin(GPIOA, DTH11_PIN, GPIO_PIN_RESET)//输出低电平
#define DHT11_IO_IN    HAL_GPIO_ReadPin(GPIOA, DTH11_PIN)								//读取IO口电平

//QueueHandle_t xDT11_GetHandle(void);
int DTH11_Platform_Init(void);
uint8_t DHT11_Read_Bit(void);
void DHT11_Init(void);
int DHT11_Read_Data(int* temp , int* humi);
void DHT11_Test(void);
void DHT11_MQTT_Task(void *para);
void DHT11_PinCfgAsOutput(void);
void DHT11_PinCfgAsInput(void);
void DHT11_Start(void);
int DHT11_Wait_Ack(void);



#endif
