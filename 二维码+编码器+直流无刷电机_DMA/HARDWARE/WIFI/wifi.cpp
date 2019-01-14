
#include "wifi.h"



#define GET_ARRAY_LEN_OUT(array,len) {len = (sizeof(array) / sizeof(array[0]));}
#define GET_ARRAY_LEN(array) ((sizeof(array)) / (sizeof(array[0])))

__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 	//发送缓冲,最大USART3_MAX_SEND_LEN字节
#ifdef USART3_RX_EN   								//如果使能了接收   	  
//串口接收缓存区 	

u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 				//接收缓冲,最大USART3_MAX_RECV_LEN个字节.
u16 USART3_RX_STA=0;   	 

//往USART3，发送 length长度的数据data
void U3_SendData(u8 *data,u8 length)
{
    u8 i;
    for(i=0;i<length;i++)
    {
        USART_SendData(USART3, data[i]);
        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)//等得发送完成
        {
        }  
    }
    USART_SendData(USART3,0x0D);//向串口3发送换行符号，方便管理系统接收
    while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
}


//串口3中断服务函数
void USART3_IRQHandler(void)
{
	u8 res;	
	static u8 yz_flag=0;
	if(yz_flag == 0)
	{
		U3_SendData((u8*)"lkagv\0",6);
		yz_flag = 1;
	}

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//接收到数据
	{	 		
			
			res =USART_ReceiveData(USART3);
			USART3_RX_BUF[USART3_RX_STA++]=res;	
			
			USART_SendData(USART3,res);
			//if(res!=NULL)
			//USART_SendData(USART3,res);
//			if((USART3_RX_STA&(1<<15))==0)//接收未完成
//			{ 
//				if(USART3_RX_STA<USART3_MAX_RECV_LEN)		//还可以接收数据
//				{						
//					USART3_RX_BUF[USART3_RX_STA++]=res;		//记录接收到的值
//					if(res == '#')
//						USART3_RX_STA|=1<<15;//U3_SendData((u8 *)"123123agvgav\n",strlen(resbuf));	 //检测到结束位，标记接收完成
//				}	
//				else
//				{				
//					USART3_RX_STA|=1<<15;			//强制标记接收完成
//				}				
//			}
//			
//			if((USART3_RX_STA&(1<<15)) != 0)//接收已完成
//			{
//				printf("%s\r\n",resbuf);
//				WIFI_control(resbuf);									
//			}
			USART_ClearITPendingBit(USART3,USART_IT_RXNE);
 }		
}  
#endif	

//初始化IO 串口3
//bound:波特率	  
void usart3_init(u32 bound)
{  
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟

 	USART_DeInit(USART3);  //复位串口3
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11复用为USART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10复用为USART3	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10; //GPIOB11和GPIOB10初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure); //初始化GPIOB11，和GPIOB10
	
	USART_InitStructure.USART_BaudRate = bound;//波特率 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  
	USART_Init(USART3, &USART_InitStructure); //初始化串口3
 
	USART_Cmd(USART3, ENABLE);               //使能串口 

  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断   
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	USART3_RX_STA=0;		//清零
  	
}




