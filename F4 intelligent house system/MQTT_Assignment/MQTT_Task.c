#include "MQTT_Task.h"

static QueueSetHandle_t G_xQueueSetInput; /* 输入设备的队列集 */
static QueueHandle_t G_xQueue_DTH11;
static QueueHandle_t G_xQueuePlatform; /* Process data platform */

static int Tempature;
static int Humidity;

/*return handle of DTH11 queue*/
QueueHandle_t xDT11_GetHandle(void)
{
	return G_xQueue_DTH11;
}

void DHT11_MQTT_Task(void *para)
{
	struct DTH11_Data MQTT_Data;
	int err;
		
    DHT11_Init();
		Delay_ms(50);
    DHT11_Start();
		Delay_ms(10);
    DHT11_Wait_Ack();
    
//    for(int i=0;i<5;i++){
		while(1){
		err=DHT11_Read_Data(&MQTT_Data.Temp,&MQTT_Data.Humid);
		
		if(err)
		{
			DHT11_Init();
			printf("\n\rdht11 read err!\n\r");	 
		}
    else{    
			xQueueSend(G_xQueue_DTH11, &MQTT_Data, NULL);	
			vTaskDelay(1000);
		}
	}
		
		
}

/*Process DTH11 data*/
void ProcessDTH11Data(void)
{
    struct DTH11_Data mdata;
    struct MQTT_Data idata;

    /* read DTH11 data */
    xQueueReceive(G_xQueue_DTH11, &mdata, 0);

    idata.MQTT_Value1 = mdata.Temp;
    idata.MQTT_Value2 = mdata.Humid;
    xQueueSend(G_xQueuePlatform, &idata, 0);
}

/*Process Hardware data*/
static void InputTask(void *params)
{
    QueueSetMemberHandle_t xQueueHandle;

    while (1)
    {
        /* 读队列集, 得到有数据的队列句柄 */
        xQueueHandle = xQueueSelectFromSet(G_xQueueSetInput, portMAX_DELAY);

        if (xQueueHandle)
        {
            /* 读队列句柄得到数据,处理数据 */
            if (xQueueHandle == G_xQueue_DTH11)
            {
                ProcessDTH11Data();
            }
        }
    }
}

static void MQTT_Platform(void *para)
{
    struct MQTT_Data Udata;
    while (1)
    {
        xQueueReceive(G_xQueuePlatform, &Udata, 2000);
        Tempature = Udata.MQTT_Value1;
        Humidity = Udata.MQTT_Value2;
    }
}
static void topic1_handler(void *client, message_data_t *msg)
{
    (void)client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\r\ntopic: %s\r\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char *)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
}

void MQTT_Client_Task()
{

    int err;
    char buf[60];
	
    mqtt_client_t *client = NULL;
    mqtt_message_t msg;

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

    if (0 != mqtt_connect(client)) /*if connection is failed*/
    {
        printf("mqtt_connect err\r\n");
        vTaskDelete(NULL);
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

    mqtt_publish(client, "mcu_test", &msg);

    err = mqtt_subscribe(client, "mcu_test", QOS0, NULL);
    printf("subscribe err = %d\r\n", err);

    
		G_xQueue_DTH11= xQueueCreate(10,sizeof(struct MQTT_Data));
    G_xQueuePlatform = xQueueCreate(10, sizeof(struct MQTT_Data));
    G_xQueueSetInput = xQueueCreateSet(DTH11_QUEUE_LEN);

    G_xQueue_DTH11 = xDT11_GetHandle();

    xQueueAddToSet(G_xQueue_DTH11, G_xQueueSetInput);

		
		
    xTaskCreate(DHT11_MQTT_Task, "DTH11_Task", 128, NULL, osPriorityNormal, NULL);
	
    xTaskCreate(InputTask, "InputTask", 128, NULL, osPriorityNormal, NULL);
	
    xTaskCreate(MQTT_Platform, "MQTT_Platform", 128, NULL, osPriorityNormal, NULL);

		
    while (1)
    {		
				
				msg.payload = buf;
				msg.qos = 0;
        sprintf(buf, "Tempature: %d,Humidity:%d", Tempature, Humidity);
        msg.payloadlen = strlen(msg.payload);
        mqtt_publish(client, "mcu_test", &msg);
				vTaskDelay(5000);		
    }
}