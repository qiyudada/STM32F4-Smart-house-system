#include "MyDma.h"                  // Device header

DMA_InitTypeDef DMA_InitStructure;

u16 DMA1_MEM_LEN;
//保存DMA每次数据传送的长度 	    
//DMA1的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_CHx:DMA通道CHx
//cpar:外设地址
//cmar:存储器地址
//cndtr:数据传输量 
void MYDMA_Config(u32 cpar,u32 cmar,u16 cndtr)
{
		
} 

//void MYDMA_Config1(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
//{
// 	
//} 

////开启一次DMA传输
//void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
//{ 
//	
//}	  

 

























