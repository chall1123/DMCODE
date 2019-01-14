#include "tim1.h"
#include "usart2.h"
#include "motordir.h"
#include "motor_pwm.h"
#include "wifi.h"
#include "stdio.h"

//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
//int idAndDir[16]={1,0,1,1,1,0,0,0,0,1,0,0,0,1,1,1};
//int idAndDir[48]={2,2,2,0,0,1,1,0,2,0,0,1,1,0,0,2,0,1,1,2,1,2,2,1,2,2,2,1,1,0,0,1,2,1,1,0,0,1,1,2,1,0,0,2,0,2,2,0};
int idAndDir[] = {1,2,2,2,2,1,1,2,2,2,2,0,0,2,2,2,2,0,0,2,2,2,2,1};
//int idAndDir[40]={2  ,2  ,2  ,0  ,0  ,2  ,2  ,2  ,1  ,1  ,2 ,2,2 ,0  ,0  ,2  ,2  ,2 ,1,1 ,2 ,2 ,2 ,1  ,1  ,\
									2  ,2 ,2,0 ,0 ,2 ,2  ,2  ,1  ,1  ,2  ,2  ,2  ,0  ,0};
//int id[40] =     {194,214,234,258,226,250,190,210,254,27,7,26,270,206,186,237,31,6,47,10,22,11,266,246,186,\
									237,31,6,47,27,7,26,270,206,226,250,190,210,254,230};
int id[]={59,267,54,8,16,37,251,14,13,12,35,60,56,207 ,15,16,17,33,251,14,13,12,35,60};
int stopId[] = {59,33};
int angle[]={0,0,0,0,0,270,180,180,180,180,180,270,0 ,0,0,0,0,90,180,180,180,180,180,90};
//int idAndDir[10]={2,2,2,0,0,2,2,2,0,0};
int len = sizeof(idAndDir)/sizeof(int);
u8 goFowardFlag=255;
int timecount=0;//ÿ10ms�ñ�־λ��һ	
int recvFlag=0;
long long int curIndex=0;
double right_wheel_speed=0.0;//�����ٶ�cm/s��ǰ��Ϊ��������Ϊ��
double left_wheel_speed=0.0;//�����ٶ�cm/s��ǰ��Ϊ��������Ϊ��
int trackIndex=-1;
	
void TIM1_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);  ///ʹ��TIM7ʱ��

	TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 

	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);//��ʼ��TIM7

	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE); //����ʱ��7�����ж�

	//��ʱ��7����������ʹ��
	//TIM_Cmd(TIM7,ENABLE); //ʹ�ܶ�ʱ��7

	NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn; //��ʱ��7�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}
float preLeaveAngle;
int multiDistance = 1;
u8 mutil=1;
int f=1;
int MultiDMDistance(void)
{
	int cnt=1;
	while(1)
	{
		if(idAndDir[(++curIndex)%len]==TOWARD )
			cnt++;
		else 
			break;
	}
	curIndex %= len;
	return cnt;
	
}
 char sendBuf[1024];
u8 isUpdate()
{
	
	if(DMCodeInfo.preID != DMCodeInfo.id)
	{
		DMCodeInfo.preID = DMCodeInfo.id;
		trackIndex++;
		
		if(id[trackIndex]!=DMCodeInfo.preID)
		{
			int i=curIndex>=len/2 ? len/2 : 0;
			for( ;i<len;i++)
			{
				 if(id[i]==DMCodeInfo.preID )
				 {
					trackIndex=i;break;
				 }		 
			}
		}
		//printf("trackIndex=%d,id=%d\r\n",trackIndex,DMCodeInfo.preID);
		float arriveAngle=DMCodeInfo.angle;
		float tan = fabs(DMCodeInfo.x*0.0156) / DMDISTANCE;   //x�������У��
		float towardAngle = atan(tan) * 180.0/3.1416;
		towardAngle = DMCodeInfo.x>=0? -towardAngle : towardAngle;
		float angleErr = Angle_Err_Limit(arriveAngle , angle[trackIndex]);
		printf("%f,%f testSumEerr=%f\r\n",angleErr,towardAngle,angleErr+towardAngle);
		//sprintf(sendBuf,"angleErr=%f,towardAngle=%f,%f,id=%d,x=%d,y=%d,Angle=%f;\r\n",angleErr,towardAngle,angleErr+towardAngle,DMCodeInfo.id,DMCodeInfo.x,DMCodeInfo.y,arriveAngle);
		//printf("%s\r\n",sendBuf);
		//char c[]="lklk\n";
		//SendData(sendBuf,sizeof(sendBuf));//���͸�������
		printDMCodeInfo();
		//printf("\r\n");
		if((fabs(angleErr + towardAngle)>=3 || fabs(angleErr)>3 || abs(DMCodeInfo.x)>200)&&((fabs(angleErr + towardAngle)<7)))//&&  idAndDir[trackIndex] == 2)
		{
			//printf("modify\r\n");
			return 1;
		}
		else if(fabs(angleErr + towardAngle)>=7)
			return 2;
	}
	return 0;
}

//��ʱ��7�жϷ�����
void TIM7_IRQHandler(void)
{
	
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET) //����ж�
	{	
		
		#if (MODE==2)//������usart2.h��ͷ�ļ����޸�DM������շ�ʽ
			//printf("DMA��������");
			get_data();
			//printf("�жϽ�������");
		#endif
        //get_data();
		//printDMCodeInfo();	
		switch(goFowardFlag)
		{
			case 0:timecount=0;
				rotate(idAndDir[curIndex]);
					break;
			case 1: goForward();//printf("go foward");
				    break;
			case 3:Stop();
			timecount>500?goFowardFlag=1:timecount++;
					break;
			default:Stop();
					break;
		}
		//int dir=getDir();
		u8 update=isUpdate();
		//printf("%d\r\n",update);
		//if(goFowardFlag)
		//if( (abs(DMCodeInfo.x)<15) || (abs(DMCodeInfo.y)<15  ))//||turnFlag>430)
		if(mutil==1 && goFowardFlag==1)
			mutil=0,multiDistance=MultiDMDistance();
		
		if( goFowardFlag==1 && abs(DMCodeInfo.y)<10 && curIndex!=0 ) //ʶ�𵽶�ά������ĵ�
		{
			if(DMCodeInfo.id == id[curIndex])
			
			{
				mutil=1;
				fixedDistance(0,0);
				goFowardFlag=0;
				//printf("ʶ��ת�䣬ID=%d,curIndex=%lld\r\n",DMCodeInfo.id,curIndex);
				//printf("%d ʶ��ת�� ",curIndex);
			}
			
		}
		 if(goFowardFlag==1 && fixedDistance( multiDistance*DMDISTANCE ,update))//���ߵ�Ŀ����룬������һ����ά���λ�� 
		{
			mutil=1;
			goFowardFlag=0;
			//printf("ǿ��ת�䣬ID=%d,curIndex=%lld\r\n",DMCodeInfo.id,curIndex);
			//printf("%d ǿ��ת�� ",curIndex);
		}
		
		if(timecount%100==0)//��ʱ�ﵽ1��
		{
			
		
		}
		TIM_SetCompare1(TIM3,wheel_speed);
		TIM_SetCompare2(TIM3,wheel_speed);
		//printf("%d\r\n",wheel_speed);
		if(timecount%1==0&&recvComplete)
		{
			//USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
			recvComplete=0;
		}	
	}
	TIM_ClearITPendingBit(TIM7,TIM_IT_Update);  //����жϱ�־λ
}
