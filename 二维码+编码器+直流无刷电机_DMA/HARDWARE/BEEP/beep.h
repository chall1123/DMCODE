#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//��������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

//LED�˿ڶ���

#ifdef __cplusplus
 extern "C" {
#endif
	 
	 
#define BEEP PFout(8)	// ����������IO 

void BEEP_Init(void);//��ʼ��	


#ifdef __cplusplus
 }
#endif	 
#endif

















