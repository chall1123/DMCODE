#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
/*���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��84MHz/(ENCODER_TIMx_PRESCALER+1)*/

#ifdef __cplusplus
 extern "C" {
#endif
#define ENCODER_TIM_PRESCALER                 0

/*�Զ���װ��ֵ*/
#define ENCODER_TIM_PERIOD                   63999

#define WHEEL_PERI                          34.55  //���ӵ��ܳ�cm
#define ACIRCLE_ENC                          60755//������һȦ�ı����������ĸ���
#define TARGE_DISTANCE                       50 //��ά��ļ��cm

extern int counter1;//��ʱ��4��������жϵĴ���
extern int counter2;//��ʱ��5��������жϵĴ���

/*��������*/
void Encoder_GPIO_Init(void);//��ʱ�����IO�ڳ�ʼ��
void Encoder1_TIM_Init(void);//��ʱ��4��ʼ��
void Encoder2_TIM_Init(void);//��ʱ��5��ʼ��
int Encoder1_Get_CNT(void);//��ȡ��ʱ��4��CNT�Ĵ�����ֵ����ʱ��4��ǰ����ֵ��
int Encoder2_Get_CNT(void);//��ȡ��ʱ��5��CNT�Ĵ�����ֵ����ʱ��5��ǰ����ֵ��
	 
#ifdef __cplusplus
}
#endif
#endif
