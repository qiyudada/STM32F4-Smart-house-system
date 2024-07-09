#include "Dth11.h"
#include "usart.h"
#include <platform_mutex.h>
#include "mqttclient.h"
#include "ATcommand.h"
#include "semphr.h"
#include "Lcd.h"

extern QueueHandle_t G_xMessageQueueToMQTT;
extern EventGroupHandle_t Event_Handle;
extern TaskHandle_t G_xDHT11_Task_Handler;
extern platform_mutex_t G_xTaskMutex;
extern int WIFI_CONECT;
extern int PING_MODE1;
void DHT11_PinCfgAsOutput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void DHT11_PinCfgAsInput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void DHT11_Start(void)
{
    DHT11_PinCfgAsOutput(); // PA7设置为输出模式
    DHT11_LOW;              // 主机拉低总线
    Delay_ms(20);           // 延迟必须大于18ms ；
    DHT11_HIGH;             // 主机拉高总线等待DHT11响应
    Delay_us(30);
}

int DHT11_Wait_Ack(void)
{
    uint8_t retry = 0;
    DHT11_PinCfgAsInput();
    // 采用while循环的方式检测响应信号
    while (DHT11_IO_IN && retry < 100) // DHT11会拉低 40us ~80us
    {
        retry++;
        Delay_us(1); // 1us
    }
    if (retry >= 100) // 判断当DHT11延迟超过80us时return 1 ， 说明响应失败
    {
        return 1;
    }
    else
        retry = 0;

    while (!DHT11_IO_IN && retry < 100) // // DHT11拉低之后会拉高 40us ~80us
    {
        retry++;
        Delay_us(1); // 1us
    }

    if (retry >= 100)
    {
        return 1;
    }
    return 0;
}

uint8_t DHT11_Read_Bit(void)
{
    uint8_t retry = 0;
    while (DHT11_IO_IN && retry < 100) // 同上采用while循环的方式去采集数据
    {
        retry++;
        Delay_us(1);
    }
    retry = 0;
    while (!DHT11_IO_IN && retry < 100)
    {
        retry++;
        Delay_us(1);
    }

    Delay_us(40); // 结束信号，延时40us
    if (DHT11_IO_IN)
        return 1; // 结束信号后，总线会被拉高 则返回1表示读取成功
    else
        return 0;
}

int DHT11_ReadByte(void)
{
    uint8_t i, dat;
    dat = 0;
    for (i = 0; i < 8; i++)
    {
        dat <<= 1; // 通过左移存储数据
        dat |= DHT11_Read_Bit();
    }
    return dat;
}

// void DHT11_Init(void)
//{
//     G_xQueueDTH11 = xQueueCreate(DTH11_QUEUE_LEN, sizeof(struct DTH11_Data));
// }

/**
 * @brief  DHT11读取数据
 * @param  temp：温度值 humi ：湿度值
 * @retval 返回值0/1 0：读取数据成功 1：读取数据失败
 */
int DHT11_Read_Data(int *temp, int *humi)
{
    int buf[5]; // 储存五位数据
    uint8_t i;
    DHT11_Start();             // 起始信号
    if (DHT11_Wait_Ack() == 0) // 响应信号
    {
        for (i = 0; i < 5; i++)
        {
            buf[i] = DHT11_ReadByte();
        }
        if (buf[0] + buf[1] + buf[2] + buf[3] == buf[4]) // 校验数据
        {
            *humi = buf[0]; // 湿度
            *temp = buf[2]; // 温度
        }
    }
    else
        return 1;

    return 0;
}

void DHT11_Test(void)
{
    int hum, temp;

    // DHT11_Init();
    DHT11_Start();
    DHT11_Wait_Ack();

    while (1)
    {
        if (DHT11_Read_Data(&temp, &hum) != 0)
        {
            printf("\n\rdht11 read err!\n\r");
            DHT11_PinCfgAsOutput();
        }
        else
        {
            printf("Tempature:%d^c\r\n", temp);
            printf("Humidity:%d\r\n", hum);

            //						LCD_PrintString(0, 0, "DHT11:");
            //            LCD_PrintString(0, 2, "Humidity Temp");
            //            len = LCD_PrintSignedVal(0, 4, hum);
            //            LCD_PutChar(len, 4, '%');
            //            LCD_PrintSignedVal(9, 4, temp);
        }
        Delay_ms(2000); /* 读取周期是2S, 不能读太频繁 */
    }
}

void DHT11_MQTT_Task(void *para)
{

    platform_mutex_lock(&G_xTaskMutex);//waitting for the mutex,prevent other task from running
    struct DTH11_Data D_Data;
    int err;

    // xSemaphoreTake(G_xSemTicks, portMAX_DELAY);
    DHT11_Start();

    DHT11_Wait_Ack();

    while (1)
    {
        xEventGroupWaitBits((EventGroupHandle_t)Event_Handle,
                            (EventBits_t)WIFI_CONECT,//provided by  MQTT_Client_Task()
                            (BaseType_t)pdFALSE,
                            (BaseType_t)pdTRUE,
                            (TickType_t)portMAX_DELAY);

        err = DHT11_Read_Data(&D_Data.Temp, &D_Data.Humid);
        if (err)
        {
            DHT11_PinCfgAsInput();
            printf("\n\rdht11 read err!\n\r");
            platform_mutex_unlock(&G_xTaskMutex);
            vTaskDelay(2000);
        }
        else
        {
            sprintf(D_Data.data_of_sensor, "\"temperature\":\"%d\",\"humidity\":\"%d\",", D_Data.Temp, D_Data.Humid);
            xQueueSend(G_xMessageQueueToMQTT, &D_Data.data_of_sensor, NULL);
            //printf("send DTH11 data successfully\r\n");
            xEventGroupSetBits(Event_Handle, PING_MODE1);//set up event group bit
            LCD_ShowIntNum(72,20,D_Data.Temp,2,BLACK,WHITE,12);
            LCD_ShowIntNum(72,32,D_Data.Humid,2,BLACK,WHITE,12);
            memset(D_Data.data_of_sensor, 0, sizeof(D_Data.data_of_sensor));
            platform_mutex_unlock(&G_xTaskMutex);
            vTaskDelay(2000);
        }
    }
}
