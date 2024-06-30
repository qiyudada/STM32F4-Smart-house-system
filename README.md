Some bug record:
>6/28
1.Programme was stuck in Hay_delay() ,because the control system of freertos is based on  sysTick timer. 
  Solution: change sysTick base on TIM4 or don't use it or rewrite fuction
2.Programme die in prvTaskexiterror() , before call for xQueueGenericSend()
  Solution: forget to create the queue of some Task queue handle
3.DTH11 init is failed,because of TIM7 is stuck in get count value function
  Solution:change delay base on TIM4 ,and verify the ms/ns accuracy

-----------
