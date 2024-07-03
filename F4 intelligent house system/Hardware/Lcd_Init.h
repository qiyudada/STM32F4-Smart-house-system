#ifndef _LCD_INIT_H
#define _LCD_INIT_H

#include "stm32f4xx_hal.h"
#include "spi.h"

#define USE_HORIZONTAL 1  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1

#define LCD_W 128
#define LCD_H 160

#else
#define LCD_W 160
#define LCD_H 128
#endif

#define LCD_RES_Clr()  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_RESET)//RES
#define LCD_RES_Set()  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_SET)

#define LCD_DC_Clr()   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET)//DC
#define LCD_DC_Set()   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET)
 		     
#define LCD_CS_Clr()   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET)//CS
#define LCD_CS_Set()   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET)

#define LCD_BLK_Clr()  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_RESET)//BLK
#define LCD_BLK_Set()  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_SET)


void LCD_GPIO_Init(void);//初始化GPIO
void SPIv_WriteData(uint8_t Data);
void LCD_Writ_Bus(uint8_t data);//模拟SPI时序
void LCD_WR_DATA8(uint8_t data);//写入一个字节
void LCD_WR_DATA(uint16_t data);//写入两个字节
void LCD_WR_REG(uint8_t HAL_DATA_EEPROMEx_Lock);//写入一个指令
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);//设置坐标函数
void LCD_Init(void);//LCD初始化

#endif