#include "usart2.h"	
#include "string.h"
#include "wifi.h"

char  USART2_RX_BUF[USART2_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
char  USART2_RX_BUF1[USART2_REC_LEN];
u16   USART2_RX_STA;         		//����״̬���	

DMCODEDATA DMCodeData;
struct DMCODEInfo DMCodeInfo;
u8     recvComplete=0;

#if (USART_PORT == 2 )
/////////////////USART 2/////////////////////////////////
void USART2_DMA_init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	//����DMAʱ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	

	//����DMA1��ͨ��  DMA1_Stream5,DMA_Channel_4 --> USART2_RXͨ��
	DMA_DeInit(DMA1_Stream5);	
	DMA_InitStructure.DMA_Channel = DMA_Channel_4; 													//DMAͨ������	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);			//�����ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)USART2_RX_BUF;						//�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;									//dma���䷽��	
	DMA_InitStructure.DMA_BufferSize = USART2_REC_LEN;													//����DMA�ڴ���ʱ�������ĳ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//����DMA���������ģʽ��һ������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;									//����DMA���ڴ����ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//���������ֳ�	
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;			//�ڴ������ֳ�
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;														//����DMA�Ĵ���ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;											//����DMA�����ȼ���	
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;   								//ָ�����FIFOģʽ��ֱ��ģʽ������ָ������ �� ��ʹ��FIFOģʽ  	
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;    		//ָ����FIFO��ֵˮƽ    	
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;   					//ָ����Burstת�������ڴ洫��     	
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  		//ָ����Burstת��������Χת��	
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);  
	
	//˫��������
	DMA_DoubleBufferModeConfig(DMA1_Stream5, (uint32_t)USART2_RX_BUF1, DMA_Memory_0);
  DMA_DoubleBufferModeCmd(DMA1_Stream5, ENABLE);
		
	//ʹ��DMA
	DMA_Cmd(DMA1_Stream5,ENABLE);	  
}

void uart2_init(u32 bound){
	
   //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIODʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
 
	//����2��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2); //GPIOD5����ΪUSART2
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); //GPIOD6����ΪUSART2
	
	//USART2�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6; //GPIOD5��GPIOD6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOD,&GPIO_InitStructure); //��ʼ��PD5��PD6

   //USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������1
	
#if (MODE==1)

	NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn; //��ʱ��7�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	//ʹ�ܴ����ж�
	USART_ITConfig(USART2,USART_IT_TC,DISABLE);
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART2,USART_IT_TXE,DISABLE);
	//USART_ITConfig(USART2,USART_IT_IDLE,ENABLE); 
#elif (MODE==2)

	NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn; //��ʱ��7�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=DISABLE;
	NVIC_Init(&NVIC_InitStructure);	

	USART_ITConfig(USART2,USART_IT_TC,DISABLE);
	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);
	USART_ITConfig(USART2,USART_IT_TXE,DISABLE);
	//����DMA
	USART2_DMA_init();
	//����DMA��ʽ����
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);
	//����DMA��ʽ����
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
	
#endif	
	//ʹ�ܴ���
	USART_ClearFlag(USART2, USART_FLAG_TC);
    USART_Cmd(USART2, ENABLE);  

	DMCodeInfo.preID=0;
	DMCodeInfo.id=0;
	DMCodeInfo.x=1111;
	DMCodeInfo.y=1111;
	DMCodeInfo.angle=720;	
}

#elif (USART_PORT == 4)
/////////////////USART 4////////////////////////

void USART2_DMA_init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	//����DMAʱ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	

	//����DMA1��ͨ��  DMA1_Stream2,DMA_Channel_4  --> USART2_RXͨ��
	DMA_DeInit(DMA1_Stream2);	
	DMA_InitStructure.DMA_Channel = DMA_Channel_4; 													//DMAͨ������	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART4->DR);			//�����ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)USART2_RX_BUF;						//�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;									//dma���䷽��	
	DMA_InitStructure.DMA_BufferSize = USART2_REC_LEN;													//����DMA�ڴ���ʱ�������ĳ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//����DMA���������ģʽ��һ������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;									//����DMA���ڴ����ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//���������ֳ�	
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;			//�ڴ������ֳ�
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;														//����DMA�Ĵ���ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;											//����DMA�����ȼ���	
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;   								//ָ�����FIFOģʽ��ֱ��ģʽ������ָ������ �� ��ʹ��FIFOģʽ  	
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;    		//ָ����FIFO��ֵˮƽ    	
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;   					//ָ����Burstת�������ڴ洫��     	
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  		//ָ����Burstת��������Χת��	
	DMA_Init(DMA1_Stream2, &DMA_InitStructure);  
	
	//ʹ��DMA
	DMA_Cmd(DMA1_Stream2,ENABLE);	  
}

void uart2_init(u32 bound){
	
   //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	//NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //ʹ��GPIODʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//ʹ��USART2ʱ��
 
	//����4��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4); //GPIOD5����ΪUSART2
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); //GPIOD6����ΪUSART2
	
	//USART4�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOD5��GPIOD6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��PD5��PD6

   //USART4 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(UART4, &USART_InitStructure); //��ʼ������1
	
	//ʹ�ܴ����ж�
	USART_ITConfig(UART4,USART_IT_TC,DISABLE);
	USART_ITConfig(UART4,USART_IT_RXNE,DISABLE);
	USART_ITConfig(UART4,USART_IT_TXE,DISABLE);
	//USART_ITConfig(USART4,USART_IT_IDLE,ENABLE); 


	//����DMA
	USART4_DMA_init();
	//����DMA��ʽ����
	USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE);
	//����DMA��ʽ����
	USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE);
	
	
	//ʹ�ܴ���
	USART_ClearFlag(UART4, USART_FLAG_TC);
  USART_Cmd(UART4, ENABLE);  

	DMCodeInfo.preID=0;
	DMCodeInfo.id=0;
	DMCodeInfo.x=1111;
	DMCodeInfo.y=1111;
	DMCodeInfo.angle=720;	
}
#endif
/////////////////////////////////////////
#include "delay.h"
u8 DMA_ByteAlign(void)
{
	u16 tcnt=0;
	u8 ret = DISABLE;
	while(++tcnt<1500)//20ms * 50 *30 = 30s
	{
		DMA_Cmd(DMA1_Stream5,DISABLE);
		DMA_SetCurrDataCounter(DMA1_Stream5,sizeof(DMCodeData));
		DMA_Cmd(DMA1_Stream5,ENABLE);
//		while(DMA_GetITStatus(DMA1_Stream5,DMA_IT_TCIF5) != SET);//�ȴ�����
//		DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
//		while(DMA_GetITStatus(DMA1_Stream5,DMA_IT_TCIF5) != SET);
//		DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
		delay_ms(10);
		if(USART2_RX_BUF[0]=='S'&& USART2_RX_BUF1[0]=='S')
		{
			ret = ENABLE;
			return ret;
		}
	}
	return ret;
}
void get_data()
{
	char *buf=NULL,*temp=NULL;
	u8 size_Headpart = 0;
	u8 size_Tailpart = 0;
	
	if(DMA_GetITStatus(DMA1_Stream5, DMA_IT_TCIF5) == SET)
	{
		 DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
	}
	
	if(DMA_GetCurrentMemoryTarget(DMA1_Stream5)==1)//��ʱDMA�ڴ��ַ�õ����õ���USART2_RX_BUF1
	{
		buf = USART2_RX_BUF1;//�����ӣ�����������λ
		temp = strstr(USART2_RX_BUF, "S");//��ȡ��һ�黺��USART2_RX_BUF
		size_Headpart = USART2_RX_BUF+sizeof(DMCodeData)-temp;
		size_Tailpart = temp - USART2_RX_BUF;
	}
	else{
		buf = USART2_RX_BUF;
		temp = strstr(USART2_RX_BUF1, "S");
		size_Headpart = USART2_RX_BUF1+sizeof(DMCodeData)-temp;
		size_Tailpart = temp - USART2_RX_BUF1;
	}
	
	
	if(temp != NULL)
	{
		//memcpy(DMCodeData.data, temp, sizeof(DMCodeData));
		memcpy(DMCodeData.data, temp, size_Headpart);////����USART2_REC_LEN=sizeof(DMCodeData)ʱ
		memcpy(&DMCodeData.data[0]+size_Headpart, buf, size_Tailpart);//ע��������Ԫ��ָ�룬����������ָ����ָ��ƫ��
		
		DMCodeData.DMdata.separator0[0] = '\0';
		DMCodeData.DMdata.separator1[0] = '\0';
		DMCodeData.DMdata.separator2[0] = '\0';
		DMCodeData.DMdata.e = '\0';
		
		if(atoi(DMCodeData.DMdata.code)!=0)
		{
			DMCodeInfo.id = atoi(DMCodeData.DMdata.code);
			DMCodeInfo.x = 640-atoi(DMCodeData.DMdata.x);
			DMCodeInfo.y = atoi(DMCodeData.DMdata.y)-512;
			DMCodeInfo.angle = (atof(DMCodeData.DMdata.angle));
		}
		
		if(DMCodeInfo.id==0)
		{
//			printf("%s\r\n",DMCodeData.DMdata.code);
			printf("id:%d   ",DMCodeInfo.id);
			printf("x:%d   ",DMCodeInfo.x);
			printf("y:%d   ",DMCodeInfo.y);
			printf("angle:%f\r\n",DMCodeInfo.angle);
		}
		recvComplete=1;
	}	
}
void printDMCodeInfo()
{
	//static int cnt=0;
	
 // printf("cnt=%d   \n",cnt++);
//	printf("%s\r\n",USART2_RX_BUF);
//	printf("%s   ",DMCodeInfo.XStr);
//	printf("%s   ",DMCodeInfo.YStr);
//	printf("%s  ",DMCodeInfo.angleStr);
//	printf("%s   \r\n",DMCodeInfo.idStr);
	//if(DMCodeInfo.id==0)
	printf("id:%d   x:%d   y:%d   angle:%f\r\n ", DMCodeInfo.id, DMCodeInfo.x, DMCodeInfo.y,DMCodeInfo.angle);
	//char sendBuf[1024];
	//sprintf(sendBuf,"id:%d   x:%d   y:%d   angle:%f\r\n ",DMCodeInfo.id, DMCodeInfo.x, DMCodeInfo.y,DMCodeInfo.angle);
	//SendData(sendBuf,sizeof(sendBuf));//���͸�������
	//printf("%d %d %d %f ", DMCodeInfo.id, DMCodeInfo.x, DMCodeInfo.y,DMCodeInfo.angle);
//	printf("",DMCodeInfo.x);
//	printf("",DMCodeInfo.y);
//	printf("angle:%d\r\n",DMCodeInfo.angle);
				
}

/////////////////// not use /////////////////////////////


void USART2_SendStr(char *str,int len)
{
	int i;
	for(i=0;i<len;i++)
	{
		USART_SendData(USART2,str[i]);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
}



void USART2_IRQHandler(void)  
{
	char Res;
	static u8 recvStart=0;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		
		Res =USART_ReceiveData(USART2);//(USART1->DR);	//��ȡ���յ�������
		if(Res=='S')
		{
			recvStart=1;
			USART2_RX_STA=0;
		}
		if(recvStart)
		{
			USART2_RX_BUF[USART2_RX_STA++]=Res;
			if(USART2_RX_STA>1&&USART2_RX_STA<8)
				DMCodeInfo.idStr[USART2_RX_STA-2]=Res;
			else if(USART2_RX_STA>10&&USART2_RX_STA<15)
				DMCodeInfo.XStr[USART2_RX_STA-11]=Res;
			else if(USART2_RX_STA>21&&USART2_RX_STA<26)
				DMCodeInfo.YStr[USART2_RX_STA-22]=Res;
			else if(USART2_RX_STA>33&&USART2_RX_STA<40)
				DMCodeInfo.angleStr[USART2_RX_STA-34]=Res;
			else if(USART2_RX_STA>39)
			{
				recvStart=0;	
				//
				if(atoi(DMCodeInfo.idStr)!=0)
				{	
					DMCodeInfo.id = atoi(DMCodeInfo.idStr);
					DMCodeInfo.x = 640-atoi(DMCodeInfo.XStr);
					DMCodeInfo.y = atoi(DMCodeInfo.YStr)-512;
					DMCodeInfo.angle = (atof(DMCodeInfo.angleStr));		
				}
				recvComplete=1;
				//USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
			}
		}		
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);

	}
	
}

