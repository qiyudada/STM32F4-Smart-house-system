/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_ir_receiver.c
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2023.08.04      v01         百问科技      创建文件
 *--------------------------------------------------
*/

#include "driver_ultrasonic_sr04.h"
#include "driver_lcd.h"
#include "driver_timer.h"
#include "stm32f4xx_hal.h"
#include "tim.h"
#include "adc.h"

#define SR04_TRIG_GPIO_GROUP GPIOB
#define SR04_TRIG_GPIO_PIN   GPIO_PIN_9

#define SR04_ECHO_GPIO_GROUP GPIOB
#define SR04_ECHO_GPIO_PIN   GPIO_PIN_8

#define SR04_TIMEOUT   20000 // us


/**********************************************************************
 * 函数名称： SR04_Init
 * 功能描述： 超声测距模块的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void SR04_Init(void)
{
    /* Trig PB9, Echo PB8 在MX_GPIO_Init里被初始化为输出/输入引脚了 */
}

/**********************************************************************
 * 函数名称： SR04_Read
 * 功能描述： 超声测距模块读取距离函数
 * 输入参数： 无
 * 输出参数： pData - 用来保存距离(单位cm)
 * 返 回 值： 0 - 成功, (-1) - 失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
int SR04_Read(uint32_t *pData)
{
    uint32_t timeout = SR04_TIMEOUT;
    uint64_t time;
    
    /* 发送Trig信号 */
    HAL_GPIO_WritePin(SR04_TRIG_GPIO_GROUP, SR04_TRIG_GPIO_PIN, GPIO_PIN_SET);
    udelay(20);
    HAL_GPIO_WritePin(SR04_TRIG_GPIO_GROUP, SR04_TRIG_GPIO_PIN, GPIO_PIN_RESET);
    
    /* 上升沿记录起始时间 */
    while (timeout-- && (HAL_GPIO_ReadPin(SR04_ECHO_GPIO_GROUP, SR04_ECHO_GPIO_PIN) == GPIO_PIN_RESET))
    {
        udelay(1);
    }
    if (!timeout)
    {
        return -1;
    }
    time = system_get_ns();

    /* 下降沿记录结束时间, 并计算时间差, 计算距离 */
    timeout = SR04_TIMEOUT;
    while (timeout-- && (HAL_GPIO_ReadPin(SR04_ECHO_GPIO_GROUP, SR04_ECHO_GPIO_PIN) == GPIO_PIN_SET))
    {
        udelay(1);
    }
    if (!timeout)
    {
        return -1;
    }
    
    time  = system_get_ns() - time;
    
    *pData =  time * 17 / 1000000; // time /1000000000 * 340 * 100 / 2
    return 0;
}


/**********************************************************************
 * 函数名称： SR04_Test
 * 功能描述： 超声测距模块测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/04        V1.0     韦东山       创建
 ***********************************************************************/
void SR04_Test(void)
{
    uint32_t distance;
    int len;
    
    SR04_Init();

    while (1)
    {
        LCD_PrintString(0, 0, "SR04 Distance: ");

        if (!SR04_Read(&distance))
        {
            LCD_ClearLine(0, 2);
            len = LCD_PrintSignedVal(0, 2, distance);
            LCD_PrintString(len, 2, "cm");
        }
        else
        {
            LCD_ClearLine(0, 2);
            LCD_PrintString(0, 2, "err");
         }
    }
}


