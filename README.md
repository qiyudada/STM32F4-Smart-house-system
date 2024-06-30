Some bug record:
>2024/6/28
1.Programme was stuck in Hay_delay() ,because the control system of freertos is based on  sysTick timer. 
  Solution: change sysTick base on TIM4 or don't use it or rewrite fuction
2.Programme die in prvTaskexiterror() , before call for xQueueGenericSend()
  Solution: forget to create the queue of some Task queue handle
3.DTH11 init is failed,because of TIM7 is stuck in get count value function
  Solution:change delay base on TIM4 ,and verify the ms/ns accuracy

--------------------------------------------------------------------------
>2024/6/30
1. use vTaskdelete to kill the void MQTT_Client_Init(void *Param),result in MQTT break connection
   Solution: substitute vTaskdelete() for vTaskSuspend();
2.osPriority setted by hal's freertos doen't match the original priority;
  Solution: substitute #include "cmsis_os.h" for #include "cmsis_os2.h",use the (osPriority_t) to relieve the limitation of priority is less.
3.Programme through three times start to stuck in vportfree(),consider it maybe stack and heap is overflow,so expand the capacity.but helpless.
  Solution: after check the process,I find myself create and delete taskhandle over and over again ,so change the method and add a judge to determine handle or not
  code :
    if (G_xMQTT_Client_Task != NULL)  
    {
        vTaskDelete(G_xMQTT_Client_Task);
        G_xMQTT_Client_Task = NULL;
        printf("G_xMQTT_Client_Task task deleted\r\n");
    }
   
 

