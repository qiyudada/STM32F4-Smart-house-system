#include "MQTT_Task.h"

/*Task handle*/
TaskHandle_t G_xMQTT_Client_Task;
TaskHandle_t G_xQueuePlatform; /* Process data platform */
TaskHandle_t G_xDHT11_Task_Handler;
TaskHandle_t G_xLightSensor_Task_Handler;
extern TaskHandle_t G_xMQTTClientInitHandle;
/*Semaphore handle*/
SemaphoreHandle_t G_xTaskMutex;
/*Eventgroup handle*/
EventGroupHandle_t Event_Handle = NULL;
const int WIFI_CONECT = (0x01 << 0);
const int PING_MODE1 = (0x01 << 1);
const int PING_MODE2 = (0x01 << 2);
/*Queue handle*/
QueueHandle_t G_xMessageQueueToMQTT;
const UBaseType_t MESSAGE_DATA_TX_NUM = 5;   // Message Queue Maximun numbers
const UBaseType_t MESSAGE_DATA_TX_LEN = 100; // Message Queue unit size Byte
/*MQTT Client name*/
mqtt_client_t *client = NULL;
mqtt_message_t msg;

/*function:MQTT_Platform
receive data from Model and send to MQTT*/
static void MQTT_Platform(void *para)
{

    xSemaphoreTake(G_xTaskMutex, portMAX_DELAY);
    /*send the message to MQTT servicer*/
    printf("enter MQTT_Platform successfully\r\n");
    char data_buffer[256] = {0}; // data buffer Initialization
    msg.payload = data_buffer;
    msg.qos = QOS0;

    while (1)
    {
        // waitting for ping message
        xEventGroupWaitBits((EventGroupHandle_t)Event_Handle,
                            (EventBits_t)PING_MODE1 | PING_MODE2, // add event signal in here
                            (BaseType_t)pdTRUE,                   // clear the signal when complete once upload
                            (BaseType_t)pdTRUE,
                            (TickType_t)portMAX_DELAY);
        if (xQueueReceive(G_xMessageQueueToMQTT, data_buffer + strlen(data_buffer), 10))
        {
            while (xQueueReceive(G_xMessageQueueToMQTT, data_buffer + strlen(data_buffer), 10))
            {
            }
            msg.payloadlen = strlen(msg.payload);
            mqtt_publish(client, "mcu_test", &msg);      // publish the message to mqtt server
           // LCD_ShowString(0,0,data_buffer,BLACK,WHITE,12,0);
            memset(data_buffer, 0, sizeof(data_buffer)); // reset data buffer
            xSemaphoreGive(G_xTaskMutex);
            // printf("send data to MQTT server successfully\r\n");
            vTaskDelay(1000);
        }
        else
        {
            printf("No data received\r\n");
            xSemaphoreGive(G_xTaskMutex);
            vTaskDelay(1000);
        }
    }
}
static void topic1_handler(void *client, message_data_t *msg)
{
    (void)client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\r\ntopic: %s\r\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char *)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
}
/*Gross Assigmment
 *Set up MQTT connection
 *Subscribe to topic and send messages
 */
void MQTT_Client_Init(void *Param)
{

    int err;

    memset(&msg, 0, sizeof(msg));

    mqtt_log_init();

    client = mqtt_lease();

    mqtt_set_port(client, "1883");

    //     mqtt_set_host(client, "192.168.11.141");
    mqtt_set_host(client, "47.114.187.247"); // iot.100ask.net
    mqtt_set_client_id(client, random_string(10));
    mqtt_set_user_name(client, random_string(10));
    mqtt_set_password(client, random_string(10));
    mqtt_set_clean_session(client, 1);

    if (0 != mqtt_connect(client))
    { /*if connection is failed*/
        printf("mqtt_connect err\r\n");
        if (G_xMQTTClientInitHandle != NULL)
        {
            vTaskDelete(G_xMQTTClientInitHandle);
            G_xMQTTClientInitHandle = NULL;
            printf("G_xMQTTClientInitHandle task deleted\r\n");
        }
    }

    err = mqtt_subscribe(client, "topic1", QOS0, topic1_handler); /*subscribe topic1,Only once at most*/
    if (err)
    {
        printf("mqtt_subscribe topic1 err\r\n");
    }

    err = mqtt_subscribe(client, "topic2", QOS1, NULL); /*subscribe topic2,Only once at least*/
    if (err)
    {
        printf("mqtt_subscribe topic2 err\r\n");
    }

    err = mqtt_subscribe(client, "topic3", QOS2, NULL); /*subscribe topic3, Only receive once*/
    if (err)
    {
        printf("mqtt_subscribe topic3 err\r\n");
    }
    printf("subscribe Topic successfully\r\n");

    err = mqtt_subscribe(client, "mcu_test", QOS0, NULL);
    if (err)
    {
        printf("subscribe err = %d\r\n", err);
    }
    else
    {
        printf("subscribe destination successfully\r\n");
    }
    xTaskCreate(MQTT_Client_Task, "MQTT_Client_Task", 128, NULL, osPriorityHigh2, &G_xMQTT_Client_Task);
    vTaskSuspend(G_xMQTTClientInitHandle);
}

void MQTT_Client_Task(void *Param)
{
    printf("enter MQTTClientTask successfully\r\n");
    taskENTER_CRITICAL(); // enter critical area to prevent the task from being interrupted
    /*create Eventgroup task*/
    Event_Handle = xEventGroupCreate();
    xEventGroupSetBits(Event_Handle, WIFI_CONECT);
    /*create Semaphore*/
    G_xTaskMutex = xSemaphoreCreateMutex();
    /*create Message Queue*/
    G_xMessageQueueToMQTT = xQueueCreate(MESSAGE_DATA_TX_NUM, MESSAGE_DATA_TX_LEN);

    /* create Process task */
    xTaskCreate(MQTT_Platform, "MQTT_Platform", 512, NULL, osPriorityAboveNormal, &G_xQueuePlatform);

    /* create task */
    xTaskCreate(LightSensor_MQTT_Task, "LightSensor_MQTT_Task", 128, NULL, osPriorityAboveNormal7, &G_xLightSensor_Task_Handler);
    xTaskCreate(DHT11_MQTT_Task, "DTH11_Task", 128, NULL, osPriorityHigh, &G_xDHT11_Task_Handler); // last create task(must running dth11 task firstly)

    if (G_xMQTT_Client_Task != NULL) // delete create task
    {
        vTaskDelete(G_xMQTT_Client_Task);
        G_xMQTT_Client_Task = NULL;
        printf("G_xMQTT_Client_Task task has deleted\r\n");
    }
    taskEXIT_CRITICAL();
}
