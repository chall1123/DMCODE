#include "encoder.h"
#include "usart.h"	
/*������*/
/*���ߣ���ɽ�*/
/*2018��9��4��*/
int counter1=0;
int counter2=0;
int Encoder1_Get_CNT(void)//�������
{
	int cnt = 0;
	cnt=TIM4->CNT-32000;
	TIM4->CNT=32000;
	cnt=cnt>0?cnt:-cnt;

	return cnt;
}
int Encoder2_Get_CNT(void)//�������
{
	int cnt = 0;
	cnt=TIM5->CNT-32000;
	TIM5->CNT=32000;
	cnt=cnt>0?cnt:-cnt;
	return cnt;
}
void Encoder1_TIM_Init(void)//�������
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  ///ʹ��TIM4ʱ��
	TIM_TimeBaseInitStructure.TIM_Period =ENCODER_TIM_PERIOD; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=ENCODER_TIM_PRESCALER;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 

	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);//��ʼ��TIM4

	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //��ʱ��4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_BothEdge,TIM_ICPolarity_BothEdge);//�Ƿ��࣬�����غ��½��ؾ�����
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter=0;
	TIM_ICInit(TIM4,&TIM_ICInitStructure);
	TIM_ClearFlag(TIM4,TIM_FLAG_Update);
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	TIM_SetCounter(TIM4,32000);
	TIM_Cmd(TIM4,ENABLE); //ʹ�ܶ�ʱ��4

	//printf("TIM4_OK!\r\n");//������
	}
void Encoder2_TIM_Init(void)//�������
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  ///ʹ��TIM5ʱ��

	TIM_TimeBaseInitStructure.TIM_Period =ENCODER_TIM_PERIOD; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=ENCODER_TIM_PRESCALER;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 

	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);//��ʼ��TIM5

	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn; //��ʱ��5�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_EncoderInterfaceConfig(TIM5,TIM_EncoderMode_TI12,TIM_ICPolarity_BothEdge,TIM_ICPolarity_BothEdge);
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter=0;
	TIM_ICInit(TIM5,&TIM_ICInitStructure);
	TIM_ClearFlag(TIM5,TIM_FLAG_Update);
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE); //����ʱ��5�����ж�
	TIM_SetCounter(TIM5,32000);
	TIM_Cmd(TIM5,ENABLE); //ʹ�ܶ�ʱ��5
	//printf("TIM5_OK!\r\n");//������
}
 
void Encoder_GPIO_Init(void)//��ʼ��IO��
{
	GPIO_InitTypeDef GPIO_InitStructure;//IO�ڳ�ʼ�������ṹ��

	/*ʱ��ʹ��*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/*�˿ڸ���*/
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_TIM4);

	/*I/O�ڳ�ʼ��*/
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF;//�������ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//����
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP ;//����
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP ;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP ;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
		//printf("GPIO_OK!\r\n");//������
}
 /*�жϷ�����*/
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET)
	{
		counter1++;
		//printf("Encoder1:%d\r\n",counter1);//������
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //����жϱ�־λ
}

void TIM5_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET)
	{
		counter2++;
		//printf("Encoder2:%d\r\n",counter2);//������
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);  //����жϱ�־λ
}
