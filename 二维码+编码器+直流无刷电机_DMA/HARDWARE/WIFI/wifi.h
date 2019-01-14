
#ifndef __WIFI_H
#define __WIFI_H	 
#include "sys.h"    

#include "delay.h"
#include "stdarg.h"	

#include <cstdio> 	 
#include "string.h"	  
#include "motordir.h"






#ifdef __cplusplus

class WiFi
{
public:
	WiFi();
	
	
};

 extern "C" {
#endif

#define USART3_MAX_RECV_LEN		400					//�����ջ����ֽ���
#define USART3_MAX_SEND_LEN		400					//����ͻ����ֽ���
#define USART3_RX_EN 			1					//0,������;1,����.

extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//���ջ���,���USART3_MAX_RECV_LEN�ֽ�
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
extern u16 USART3_RX_STA;   						//��������״̬
extern u8 res;	

void usart3_init(u32 bound);				//����3��ʼ�� 
void U3_SendData(u8 *data,u8 length);
u8 WIFI_control(char *rlyms);

#ifdef __cplusplus
 }
#endif

#endif













