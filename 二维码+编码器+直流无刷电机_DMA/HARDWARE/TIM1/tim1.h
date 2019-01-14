#ifndef __TIM1_H__
#define __TIM1_H__



#include "sys.h"
#include "usart.h"
#include "encoder.h"

#ifdef __cplusplus
 extern "C" {
#endif
	 
extern float preLeaveAngle;
extern int trackIndex;
extern int multiDistance ;
extern int  id[];
extern int stopId[];
extern int len;
extern int angle[];
extern  char sendBuf[1024];
extern int timecount;
extern int recvFlag;
extern double right_wheel_speed;
extern double left_wheel_speed;
extern u8 goFowardFlag;
extern long long int curIndex;
void TIM1_Int_Init(u16 arr,u16 psc);
int MultiDMDistance(void);
u8 isUpdate(void);

	 
#ifdef __cplusplus
}
#endif	 
#endif

