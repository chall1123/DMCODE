
#include "wifi.h"



#define GET_ARRAY_LEN_OUT(array,len) {len = (sizeof(array) / sizeof(array[0]));}
#define GET_ARRAY_LEN(array) ((sizeof(array)) / (sizeof(array[0])))

__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 	//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
#ifdef USART3_RX_EN   								//���ʹ���˽���   	  
//���ڽ��ջ����� 	

u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 				//���ջ���,���USART3_MAX_RECV_LEN���ֽ�.
u16 USART3_RX_STA=0;   	 

//��USART3������ length���ȵ�����data
void U3_SendData(u8 *data,u8 length)
{
    u8 i;
    for(i=0;i<length;i++)
    {
        USART_SendData(USART3, data[i]);
        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)//�ȵ÷������
        {
        }  
    }
    USART_SendData(USART3,0x0D);//�򴮿�3���ͻ��з��ţ��������ϵͳ����
    while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
}


//����3�жϷ�����
void USART3_IRQHandler(void)
{
	u8 res;	
	static u8 yz_flag=0;
	if(yz_flag == 0)
	{
		U3_SendData((u8*)"lkagv\0",6);
		yz_flag = 1;
	}

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//���յ�����
	{	 		
			
			res =USART_ReceiveData(USART3);
			USART3_RX_BUF[USART3_RX_STA++]=res;	
			
			USART_SendData(USART3,res);
			//if(res!=NULL)
			//USART_SendData(USART3,res);
//			if((USART3_RX_STA&(1<<15))==0)//����δ���
//			{ 
//				if(USART3_RX_STA<USART3_MAX_RECV_LEN)		//�����Խ�������
//				{						
//					USART3_RX_BUF[USART3_RX_STA++]=res;		//��¼���յ���ֵ
//					if(res == '#')
//						USART3_RX_STA|=1<<15;//U3_SendData((u8 *)"123123agvgav\n",strlen(resbuf));	 //��⵽����λ����ǽ������
//				}	
//				else
//				{				
//					USART3_RX_STA|=1<<15;			//ǿ�Ʊ�ǽ������
//				}				
//			}
//			
//			if((USART3_RX_STA&(1<<15)) != 0)//���������
//			{
//				printf("%s\r\n",resbuf);
//				WIFI_control(resbuf);									
//			}
			USART_ClearITPendingBit(USART3,USART_IT_RXNE);
 }		
}  
#endif	

//��ʼ��IO ����3
//bound:������	  
void usart3_init(u32 bound)
{  
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��

 	USART_DeInit(USART3);  //��λ����3
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11����ΪUSART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10����ΪUSART3	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10; //GPIOB11��GPIOB10��ʼ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOB,&GPIO_InitStructure); //��ʼ��GPIOB11����GPIOB10
	
	USART_InitStructure.USART_BaudRate = bound;//������ 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  
	USART_Init(USART3, &USART_InitStructure); //��ʼ������3
 
	USART_Cmd(USART3, ENABLE);               //ʹ�ܴ��� 

  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�   
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	USART3_RX_STA=0;		//����
  	
}




