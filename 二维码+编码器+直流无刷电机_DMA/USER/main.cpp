#include "main.h"


int main(void)
{ 
	
	u8 StartUp=0;
	u8 ret=0;
//	u16 times=0;  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	KEY_Init();
	delay_init(168);		//延时初始化 
	uart_init(9600);	  //串口1初始化波特率为115200
	uart2_init(115200);   //串口2初始化，接收二维码信息
	//usart3_init(115200);
	#if (MODE==2)//可以在usart2.h的头文件中修改DM相机接收方式
	ret = DMA_ByteAlign();	//等待二维码数据并DMA数据同步
	if(ret != DISABLE) 
		printf("\r\n[USART2]:DMA ByteAlign Sucess!\r\n");
	#endif
	
	TIM1_Int_Init(50*2-1,8400-1);	//定时器时钟84M，分频系数8400，所以168M/8400=10Khz的计数频率，10ms    
	MOTORGPIO_Init();		  		//初始化与LED连接的硬件接口  
	Encoder_GPIO_Init();      
    Encoder1_TIM_Init();
    Encoder2_TIM_Init();
	TIM_Cmd(TIM7,ENABLE);  //使能TIM7  
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
		//SendData(sendBuf,sizeof(sendBuf));//发送给服务器
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
			//while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
			//getDMCodeInfo();
			//delay_ms(100);
//			if(goFowardFlag)
//			printDMCodeInfo();
		}
		//if(goFowardFlag)
			//printDMCodeInfo();
	}
}

