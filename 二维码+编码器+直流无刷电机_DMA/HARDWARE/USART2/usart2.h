#ifndef __USART2_H__
#define __USART2_H__
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "string.h"
#include "stdlib.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define USART2_REC_LEN  			sizeof(DMCODEDATA)  	//定义最大接收字节数 200
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口1接收
#define USART_PORT 2 //2 or 4 --> USAR2 or UART4
#define MODE 2       //模式2为DMA接收，模式1为串口中断接收

struct DMCODEInfo
{
	char idStr[7];
	char XStr[5];
	char YStr[5];
	char angleStr[7];
	int id;
	int x;
	int y;
	float angle;
	int preID;
};


typedef struct 
{
	char s;             /*  S  */
	char code[6];				/*  004478  */
	char separator0[3]; /*  #+   */
	char x[8];					/*  0674.000  */
	char separator1[3];	/*  #+   */
	char y[8];					/*  0525.000  */
	char separator2[3];	/*  #+   */
	char extra_spcae0;	/*  special bit */
	char angle[7];			/*  314.448  */
	char e;							/*  E  */
	char duration[4];		/*  0028  */ /* 相机曝光到UDP传输的时间间隔，单位ms*/
	char extra_spcae1;  /*  special bit */
}__attribute__ ((packed)) DMCODEDATA_S; //46bit

typedef union 
{
	char data[46];
	DMCODEDATA_S DMdata;
} DMCODEDATA;

extern struct DMCODEInfo DMCodeInfo;
extern u8 recvComplete;
	  	
extern char  USART2_RX_BUF[USART2_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART2_RX_STA;         		//接收状态标记	

void USART2_SendStr(char *str,int len);
void printDMCodeInfo(void);
void uart2_init(u32 bound);
void get_data(void);
void getDMCodeInfo(void);
u8 DMA_ByteAlign(void);

#ifdef __cplusplus
}
#endif
#endif
