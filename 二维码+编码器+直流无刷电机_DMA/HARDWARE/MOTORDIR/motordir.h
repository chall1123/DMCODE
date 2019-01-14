#ifndef __MOTORDIR_H__
#define __MOTORDIR_H__
#include "tim1.h"
#include "usart2.h"
#include "math.h"
#ifdef __cplusplus
 extern "C" {
#endif

#define STEPMOTOR_L_DIR_PORT                  GPIOE                            // 左电机方向
#define STEPMOTOR_L_DIR_PIN                   GPIO_Pin_0                      // 左电机方向
#define STEPMOTOR_L_ENA_PORT                  GPIOC                            // 左电机使能，低电平有效
#define STEPMOTOR_L_ENA_PIN                   GPIO_Pin_0                       //左电机使能，低电平有效

#define STEPMOTOR_R_DIR_PORT                  GPIOE                            // 右电机方向
#define STEPMOTOR_R_DIR_PIN                   GPIO_Pin_1                      // 右电机方向
#define STEPMOTOR_R_ENA_PORT                  GPIOC                           // 右电机使能，低电平有效
#define STEPMOTOR_R_ENA_PIN                   GPIO_Pin_1                       //右电机使能，低电平有效

#define STEPMOTOR_DIR_L_FORWARD()             GPIO_SetBits(STEPMOTOR_L_DIR_PORT,STEPMOTOR_L_DIR_PIN);
#define STEPMOTOR_DIR_R_FORWARD()             GPIO_SetBits(STEPMOTOR_R_DIR_PORT,STEPMOTOR_R_DIR_PIN);

#define STEPMOTOR_DIR_L_BACKWARD()            GPIO_ResetBits(STEPMOTOR_L_DIR_PORT,STEPMOTOR_L_DIR_PIN);
#define STEPMOTOR_DIR_R_BACKWARD()            GPIO_ResetBits(STEPMOTOR_R_DIR_PORT,STEPMOTOR_R_DIR_PIN);

#define STEPMOTOR_L_ENABLE()	                GPIO_ResetBits(STEPMOTOR_L_ENA_PORT,STEPMOTOR_L_ENA_PIN);
#define STEPMOTOR_R_ENABLE()	                GPIO_ResetBits(STEPMOTOR_R_ENA_PORT,STEPMOTOR_R_ENA_PIN);
#define STEPMOTOR_L_DISABLE()	                GPIO_SetBits(STEPMOTOR_L_ENA_PORT,STEPMOTOR_L_ENA_PIN);
#define STEPMOTOR_R_DISABLE()	                GPIO_SetBits(STEPMOTOR_R_ENA_PORT,STEPMOTOR_R_ENA_PIN);

#define LEFT 0
#define RIGHT 1
#define TOWARD 2

#define ROT_1_ENC             492
#define ROT_90_ENC            (90*(ROT_1_ENC))
#define ROT_450_ENC           (450*(ROT_1_ENC))
#define ENC_AROUND_CNT        60755
#define WHEEL_PERIMETER       34.55f     //cm
#define ROT_1_ARC             (((float)492/ENC_AROUND_CNT)*WHEEL_PERIMETER)
#define ROT_90_ARC            (90*(ROT_1_ARC))
#define ROT_450_ARC           (450*(ROT_1_ARC))
#define DMDISTANCE             50.00f
#define SPEED_DOWN_DISTANCE_ENC ENC_AROUND_CNT/3

extern double deviation;

u8 fixedDistance(double targetDistance,u8 update);
void MOTORGPIO_Init(void);
void TurnLeft(void);
void TurnRight(void);
void goForward(void);
void backward(void);
void Stop(void);
int getDir(void);
void rotate(int LR);
float SpeedSmoother(float distance_err, float max_distance_err);
float Angle_Err_Limit(float arrive, float leave);

#ifdef __cplusplus
}
#endif
#endif
