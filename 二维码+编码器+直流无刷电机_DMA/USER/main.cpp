#include "main.h"


int main(void)
{ 
	
	u8 StartUp=0;
	u8 ret=0;
//	u16 times=0;  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	KEY_Init();
	delay_init(168);		//��ʱ��ʼ�� 
	uart_init(9600);	  //����1��ʼ��������Ϊ115200
	uart2_init(115200);   //����2��ʼ�������ն�ά����Ϣ
	//usart3_init(115200);
	#if (MODE==2)//������usart2.h��ͷ�ļ����޸�DM������շ�ʽ
	ret = DMA_ByteAlign();	//�ȴ���ά�����ݲ�DMA����ͬ��
	if(ret != DISABLE) 
		printf("\r\n[USART2]:DMA ByteAlign Sucess!\r\n");
	#endif
	
	TIM1_Int_Init(50*2-1,8400-1);	//��ʱ��ʱ��84M����Ƶϵ��8400������168M/8400=10Khz�ļ���Ƶ�ʣ�10ms    
	MOTORGPIO_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�  
	Encoder_GPIO_Init();      
    Encoder1_TIM_Init();
    Encoder2_TIM_Init();
	TIM_Cmd(TIM7,ENABLE);  //ʹ��TIM7  
	Current_SysCoreClock = SYS_GetCoreClockFreq();
	if(SystemCoreClock != Current_SysCoreClock)
		printf(" SysCoreClock = %d, expect %d.\r\n", Current_SysCoreClock, SystemCoreClock);

	while(DMCodeInfo.id==0)
	{
		printDMCodeInfo();
	}
	for(int i=0;i<len;i++)
	{
		if(id[i] == DMCodeInfo.id)
		{
			curIndex=i;
			break;
		}	
	}
	deviation=-(double)DMCodeInfo.y*0.0156;
	timecount=0;
	goFowardFlag=0;
	TIM4->CNT=32000;
	TIM5->CNT=32000;
	while(1)
	{
		//delay_ms(1000);
		//printf("%s\r\n",USART3_RX_BUF);
		//USART3_RX_STA=0;
		// WIFI_control();
//		TIM_SetCompare1(TIM3,400);
//		TIM_SetCompare2(TIM3,400);
//		cnt +=Encoder1_Get_CNT();
//		if(cnt<=360*ROT_1_ENC)
//			TurnLeft();
//		else
//			Stop();
//		if(goFowardFlag)
//		printf("%d\r\n",wheel_speed);
		//if(goFowardFlag)
		//printf("speed=%d\r\n",wheel_speed);
		//SendData(sendBuf,sizeof(sendBuf));//���͸�������
		//printf("%s\r\n",sendBuf);
		//delay_ms(500);
		
		//goForward();
		//goForward();
		if(KEY_Scan(1))
		{
			printf("push down\r\n");
			StartUp=!StartUp;
			deviation=-(double)DMCodeInfo.y*0.0156;
			timecount=0;
			goFowardFlag=0;
			TIM4->CNT=32000;
			TIM5->CNT=32000;
			//curIndex=0;
		}
		if(StartUp)
		{
			//TurnRight();
			//USART_SendData(USART2,'S');
			//while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			//getDMCodeInfo();
			//delay_ms(100);
//			if(goFowardFlag)
//			printDMCodeInfo();
		}
		//if(goFowardFlag)
			//printDMCodeInfo();
	}
}

