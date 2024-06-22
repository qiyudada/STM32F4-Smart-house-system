#ifndef _MQTT_Task_H
#define _MQTT_Task_H
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "mqttclient.h"
#include "Dth11.h"
#include "cmsis_os.h"
#include "ATcommand.h"
struct MQTT_Data{
    int MQTT_Value1;
    int MQTT_Value2;/* data */
};

void DHT11_MQTT_Task(void *para);
void MQTT_Client_Task();

#endif
