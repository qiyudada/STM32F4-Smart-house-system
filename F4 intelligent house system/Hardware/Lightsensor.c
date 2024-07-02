#include "Lightsensor.h"

static ADC_HandleTypeDef *g_HADC_LightSensor = &hadc1;
extern EventGroupHandle_t Event_Handle;
extern QueueHandle_t G_xMessageQueueToMQTT;
extern TaskHandle_t G_xLightSensor_Task_Handler;
extern SemaphoreHandle_t G_xTaskMutex;
extern int WIFI_CONECT;
extern int PING_MODE2;
/*function: LightSensor_Init
parameter: none
return: none
*/
void LightSensor_Init(void)
{
    /* PA1用作ADC(ADC1_IN1) */
    HAL_ADC_Start_IT(&hadc1);
}
/*function:LightSensor_Read
parameter: pData
return: 0 success, -1 fail
*/
int LightSensor_Read(int *pData)
{
    HAL_ADC_Start(g_HADC_LightSensor);
    if (HAL_OK == HAL_ADC_PollForConversion(g_HADC_LightSensor, LIGHT_SENSOR_ADC_TIMEOUT))
    {
        *pData = HAL_ADC_GetValue(g_HADC_LightSensor);
        return 0;
    }
    else
    {
        return -1;
    }
}
/*function:LightSensor_Test
parameter: none
return: none
*/
void LightSensor_Test(void)
{
    uint32_t ADC_Sample, ADC_Volt;

    LightSensor_Init();

    while (1)
    {

        // LCD_PrintString(0, 0, "Light Sensor: ");

        if (!LightSensor_Read(&ADC_Sample))
        {
            ADC_Volt = ADC_Sample * 330 / 4096;
            printf("Light Sensor Voltage: %d.%d%d\r\n", ADC_Volt / 100, (ADC_Volt / 10) % 10, ADC_Volt % 10);
            printf("Light Sensor Sample: %d\r\n", ADC_Sample);
            // LCD_PrintSignedVal(0, 2, ADC_Sample);
        }
        Delay_ms(2000);
    }
}

void LightSensor_MQTT_Task(void *para)
{

    xSemaphoreTake(G_xTaskMutex, portMAX_DELAY);
    struct Light_Data L_Data;

    LightSensor_Init();

    while (1)
    {
        xEventGroupWaitBits((EventGroupHandle_t)Event_Handle,
                            (EventBits_t)WIFI_CONECT,
                            (BaseType_t)pdFALSE,
                            (BaseType_t)pdTRUE,
                            (TickType_t)portMAX_DELAY);

        if (!LightSensor_Read(&L_Data.Sample))
        {
            L_Data.Volt = L_Data.Sample * 330 / 4096;
            sprintf(L_Data.data_sensor, "\"Light\":%d,\"Volt\":\"%d.%d%d\",", L_Data.Sample, L_Data.Volt / 100, (L_Data.Volt / 10) % 10, L_Data.Volt % 10);
            xQueueSend(G_xMessageQueueToMQTT, &L_Data.data_sensor, NULL);
            printf("send Lightsensor data successfully\r\n");
            xEventGroupSetBits(Event_Handle, PING_MODE2);
            xSemaphoreGive(G_xTaskMutex);
            vTaskDelay(2000);
        }
        else
        {
            printf("Light Sensor Read Fail\r\n");
            xSemaphoreGive(G_xTaskMutex);
            vTaskDelay(2000);
        }
    }
}