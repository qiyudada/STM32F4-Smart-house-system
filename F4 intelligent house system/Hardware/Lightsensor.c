#include "Lightsensor.h"

extern SemaphoreHandle_t G_xSemTicks;
static ADC_HandleTypeDef *g_HADC_LightSensor = &hadc1;
// extern QueueHandle_t G_xQueue_LightSensor;
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
    uint32_t ADC_Sample,ADC_Volt;
    
    LightSensor_Init();

    while (1)
    {
        
        //LCD_PrintString(0, 0, "Light Sensor: ");

        if (!LightSensor_Read(&ADC_Sample))
        {
            ADC_Volt = ADC_Sample * 330/4096;
            printf("Light Sensor Voltage: %d.%d%d\r\n", ADC_Volt/100,(ADC_Volt/10)%10,ADC_Volt%10);
            printf("Light Sensor Sample: %d\r\n", ADC_Sample);
            //LCD_PrintSignedVal(0, 2, ADC_Sample);
        }
        Delay_ms(2000);
    }
}

//void LightSensor_MQTT_Task(void* para)
//{
//	//xSemaphoreTake(G_xSemTicks, portMAX_DELAY);
//    struct Light_Data L_Data;

//    LightSensor_Init();

//    while (1)
//    {

//        if (!LightSensor_Read(&L_Data.Sample))
//        {
//            L_Data.Volt = L_Data.Sample * 330/4096;
//            xQueueSend(G_xQueue_LightSensor, &L_Data, NULL);
//            vTaskDelay(2000);
//        }
//        else 
//        {
//            printf("Light Sensor Read Fail\r\n");
//        }
//    }
//}