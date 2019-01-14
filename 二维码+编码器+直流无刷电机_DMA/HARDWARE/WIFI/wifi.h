
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

#define USART3_MAX_RECV_LEN		400					//最大接收缓存字节数
#define USART3_MAX_SEND_LEN		400					//最大发送缓存字节数
#define USART3_RX_EN 			1					//0,不接收;1,接收.

extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节
extern u16 USART3_RX_STA;   						//接收数据状态
extern u8 res;	

void usart3_init(u32 bound);				//串口3初始化 
void U3_SendData(u8 *data,u8 length);
u8 WIFI_control(char *rlyms);

#ifdef __cplusplus
 }
#endif

#endif













