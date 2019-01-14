#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
/*定义定时器预分频，定时器实际时钟频率为：84MHz/(ENCODER_TIMx_PRESCALER+1)*/

#ifdef __cplusplus
 extern "C" {
#endif
#define ENCODER_TIM_PRESCALER                 0

/*自动重装载值*/
#define ENCODER_TIM_PERIOD                   63999

#define WHEEL_PERI                          34.55  //轮子的周长cm
#define ACIRCLE_ENC                          60755//轮子走一圈的编码器计数的个数
#define TARGE_DISTANCE                       50 //二维码的间距cm

extern int counter1;//定时器4发生溢出中断的次数
extern int counter2;//定时器5发生溢出中断的次数

/*函数申明*/
void Encoder_GPIO_Init(void);//定时器相关IO口初始化
void Encoder1_TIM_Init(void);//定时器4初始化
void Encoder2_TIM_Init(void);//定时器5初始化
int Encoder1_Get_CNT(void);//获取定时器4的CNT寄存器的值（定时器4当前计数值）
int Encoder2_Get_CNT(void);//获取定时器5的CNT寄存器的值（定时器5当前计数值）
	 
#ifdef __cplusplus
}
#endif
#endif
