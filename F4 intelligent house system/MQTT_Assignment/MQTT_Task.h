#ifndef _MQTT_Task_H
#define _MQTT_Task_H

#include<stdio.h>
#include<string.h>

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "mqttclient.h"

#include "Dth11.h"
#include "Lightsensor.h"
#include "ATcommand.h"
#include "Lcd.h"

#include "queue.h"
#include "semphr.h" 
#include "event_groups.h"

extern SemaphoreHandle_t G_xSemTicks;


void DHT11_MQTT_Task(void *para);
void MQTT_Client_Task(void *para);
void MQTT_Client_Init(void *para);

#endif
