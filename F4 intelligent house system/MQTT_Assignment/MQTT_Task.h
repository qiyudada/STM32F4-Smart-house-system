#ifndef _MQTT_Task_H
#define _MQTT_Task_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "mqttclient.h"

#include "Dth11.h"
#include "Lightsensor.h"
#include "ATcommand.h"

#include "queue.h"
#include "semphr.h" 
#include "event_groups.h"

extern SemaphoreHandle_t G_xSemTicks;

struct MQTT_Data{
    int MQTT_Value1;
    int MQTT_Value2;/* data */
		int MQTT_Value3;
		int MQTT_Value4;
};

void DHT11_MQTT_Task(void *para);
void MQTT_Client_Task(void *para);
void MQTT_Client_Init(void *para);
#endif
