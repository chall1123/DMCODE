#ifndef _MOTOT_PWM_H
#define _MOTOT_PWM_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/6/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#ifdef __cplusplus
 extern "C" {
#endif
#define WHEEL_MIN_SPEED 100
#define NORMAL_SPEED 1700
#define ROT_NORMAL_SPEED 1000
#define ROT_MIN_SPEED 100
#define SPEED_RANGE (NORMAL_SPEED-WHEEL_MIN_SPEED)/40


extern int wheel_speed;

void TIM8_PWM_Init(u32 arr,u32 psc);
void TIM3_PWM_Init(u32 arr,u32 psc);
	 
#ifdef __cplusplus
 }
#endif
#endif
