#ifndef _WWDG_H
#define _WWDG_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//看门狗 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/6/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved				
//********************************************************************************
//V1.1 20140504
//增加了窗口看门狗相关函数。
////////////////////////////////////////////////////////////////////////////////// 	 
#ifdef __cplusplus
 extern "C" {
#endif

void WWDG_Init(u8 tr,u8 wr,u32 fprer);
void WWDG_IRQHandler(void);
	 

#ifdef __cplusplus
 }
#endif
#endif
