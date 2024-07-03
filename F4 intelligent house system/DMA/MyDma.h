#ifndef __MyDMA_H
#define	__MyDMA_H

#include "stm32f4xx_hal.h"
#include "dma.h"
#include "Lcd.h" 
#include "spi.h"

typedef uint32_t u32;
#define LCD_DMA_CHANNEL DMA_CHANNEL_7

//void MYDMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);//配置DMA1_CHx
//void MYDMA_Config1(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
//void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);//使能DMA1_CHx
		   
#endif







