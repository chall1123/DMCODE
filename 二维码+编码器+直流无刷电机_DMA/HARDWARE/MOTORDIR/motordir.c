#include "motordir.h"
#include "motor_pwm.h"
#include "wifi.h"

int leaveDir[3][4]={{0,90,180,270},{180,270,0,90},{270,0,90,180}};//根据agv到达的方向1，2，3，4转弯控制指令，得到agv出二维码的角度
static u8 stateFlag = 1;//状态标志位保证每个函数只被执行一次，提高执行效率
static u8 turnFlag = 0;
void(*TurnRound[])()={TurnLeft,TurnRight,TurnRight};//函数指针，与数组一样地使用，可以与方向一起结合
double deviation=0.0;
u16 lineSurplus=1;

//初始化PF9和PF10为输出口.并使能这两个口的时钟		    
//LED IO初始化
void MOTORGPIO_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOI, ENABLE);//使能GPIO时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIO时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		   //IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化motor_R1
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;//|GPIO_Pin_7;				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		   //IO口速度为50MHz
	GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化motor_R2	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;//|GPIO_Pin_11;				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		   //IO口速度为50MHz
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
	
	GPIO_SetBits(GPIOE,GPIO_Pin_14);   // 电机1使能信号电平设置，1为低电平使能，0为高电平使能 // 电机2使能信号电平设置，1为低电平使能，0为高电平使能
    GPIO_SetBits(GPIOE,GPIO_Pin_15);   //说明书中为刹车引脚

	//GPIOG3初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//左电机方向
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化GPIO
	GPIO_SetBits(GPIOG,GPIO_Pin_3);
	
	TIM3_PWM_Init(2000-1,163-1);	 //不分频。PWM频率=72000000/900/8=40Khz

	STEPMOTOR_L_DISABLE();              
	STEPMOTOR_R_DISABLE();
}

int getDir()
{
	return ((int)DMCodeInfo.angle/90+(int)DMCodeInfo.angle%90/45)%4+1;
}

void goForward(void)
{
	if(stateFlag==1)
	{
		//printf("goForward\r\n");
		stateFlag=0;
		TIM_Cmd(TIM3, DISABLE); 
		STEPMOTOR_DIR_L_FORWARD();
		STEPMOTOR_DIR_R_FORWARD();
		STEPMOTOR_L_ENABLE();
		STEPMOTOR_R_ENABLE();
		TIM_Cmd(TIM3, ENABLE); 
	}
	
}
void TurnLeft()
{
	if(stateFlag==0 || stateFlag!=turnFlag)
	{
		//printf("rotate left");
		stateFlag=1;
		turnFlag=!turnFlag;
		TIM_Cmd(TIM3, DISABLE); 
		//STEPMOTOR_L_DISABLE();
		//STEPMOTOR_R_DISABLE();
		STEPMOTOR_DIR_L_BACKWARD();
		STEPMOTOR_DIR_R_FORWARD();
		STEPMOTOR_L_ENABLE();
		STEPMOTOR_R_ENABLE();
		TIM_Cmd(TIM3, ENABLE);
	}

}

void TurnRight()
{
	if(stateFlag==0 || stateFlag!=turnFlag)
	{
		//printf(" rotate right\r\n");
		stateFlag=1;
		turnFlag=!turnFlag;
		TIM_Cmd(TIM3, DISABLE); 
		//STEPMOTOR_L_DISABLE();
		//STEPMOTOR_R_DISABLE();
		STEPMOTOR_DIR_R_BACKWARD();
		STEPMOTOR_DIR_L_FORWARD();
		STEPMOTOR_L_ENABLE();
		STEPMOTOR_R_ENABLE();
		TIM_Cmd(TIM3, ENABLE);
	}

}
void Stop()
{
	TIM_Cmd(TIM3, DISABLE);  
	STEPMOTOR_L_DISABLE();
	STEPMOTOR_R_DISABLE();
}

void backward()
{
	STEPMOTOR_DIR_L_BACKWARD();
	STEPMOTOR_DIR_R_BACKWARD();
	STEPMOTOR_L_ENABLE();
	STEPMOTOR_R_ENABLE();
}

float SpeedSmoother(float distance_err, float max_distance_err)//速度平滑函数，加速、匀速、减速
{
	float dis_err=0;//for exp()
	float speed=1.0f;//max speed
	static float threshold = 0.05f;//5%     平滑距离，加减速距离
	static float arrive_threashold = 0.01f;//1cm
	
	if (distance_err<max_distance_err && distance_err>(max_distance_err - threshold))//speed up
	{
		dis_err = 1.0f - ((max_distance_err - distance_err) / threshold);//???
		speed = exp(dis_err * (-2.3));
		return speed;
	}

	else if (distance_err < threshold)//speed down
	{
		if (distance_err < arrive_threashold)
			return 0.0f;//
		dis_err = 1.0f - (distance_err / threshold);
		speed = exp(dis_err * (-2.3f));
		return speed;
	}
	else
		return 1.0f;//max speed in middle distance err
}


/*
fixedDistance行走固定距离 单位：cm
到达目标距离返回1，否则返回0
速度变化过程：加速、匀速、减速
*/

u8 fixedDistance(double targetDistance ,u8 update)
{
	static long long  readEnc = 0 ;
	static long long  readEnc2 = 0 ;
	static long long  targetEnc = 0;
//	static long long remberPath = 99999999;
	static u8 isUpdate = 1;
//	u8 acc=1;
	if(targetEnc==0)
	{
		targetEnc =(u32)((targetDistance+deviation)/WHEEL_PERIMETER*ENC_AROUND_CNT);
		//printf("%f  %d,%d,%lld    curIndex=%lld   \r\n",targetDistance,id[curIndex],DMCodeInfo.id,targetEnc-readEnc,curIndex);
		//printDMCodeInfo();
		//printf("ditance=%f cm\r\n",targetDistance+deviation);
		//printf("ditance=%f  ",targetDistance+deviation);
	}
	if(update && isUpdate && curIndex-trackIndex>=1 )
	{
		//printf("targetEnc=%lld,readEnc=%lld\r\n",targetEnc,readEnc);
		isUpdate = 0;
		targetEnc = targetEnc-DMDISTANCE*(curIndex-trackIndex-1)/WHEEL_PERIMETER*ENC_AROUND_CNT;
		lineSurplus = curIndex-trackIndex;
		curIndex=trackIndex+1;
		//printf("targetEnc=%lld,readEnc=%lld\r\n",targetEnc,readEnc);
		//printf("%f  %d,%d,%lld    curIndex=%lld   \r\n",targetDistance,id[curIndex],DMCodeInfo.id,targetEnc-readEnc,curIndex);
	}
//	if(wheel_speed<NORMAL_SPEED && readEnc< remberPath)
//	{
//		wheel_speed += SPEED_RANGE;
//	}
//	 if(wheel_speed>=NORMAL_SPEED && acc==1)
//	{
//		acc=0;
//		remberPath=readEnc;
//	}
//	if(acc==0&&targetEnc-readEnc<remberPath)
//		wheel_speed = wheel_speed >WHEEL_MIN_SPEED? wheel_speed -SPEED_RANGE:WHEEL_MIN_SPEED;
//	printf("%d\r\n",wheel_speed);
//	else if(update==2 && isUpdate)
//	{
//		isUpdate = 0;
//		targetEnc =  readEnc+ (-DMCodeInfo.y*0.0156)/2/WHEEL_PERIMETER*ENC_AROUND_CNT;
//		lineSurplus = curIndex-trackIndex-1;
//		curIndex=trackIndex;
//	}
	//printf("实际直线行走距离 cm\r\n",targetDistance+deviation);		
	//printf("%f ",targetDistance+deviation);	
	//printf("readEnc=%lld targetEnc=%lld remberPath=%lld\r\n", readEnc,targetEnc,remberPath);
	
	readEnc = readEnc+ Encoder1_Get_CNT();
	readEnc2 = readEnc2+ Encoder2_Get_CNT();
	//printf("tim4=%lld,tim5=%lld\r\n",readEnc,readEnc2);
	float dis=(float)(targetDistance+deviation)/100.0f;   //目标距离加上误差修正距离转化为m
	float div = (float)(readEnc)/targetEnc*dis;           //当前已走过的距离
	if(targetEnc-readEnc<SPEED_DOWN_DISTANCE_ENC )
		wheel_speed =  wheel_speed>WHEEL_MIN_SPEED ? wheel_speed - SPEED_RANGE:WHEEL_MIN_SPEED;
	else
		wheel_speed = readEnc<=targetEnc? NORMAL_SPEED*SpeedSmoother(div,dis)+WHEEL_MIN_SPEED : WHEEL_MIN_SPEED;   //速度变换


	//if(div/dis<0.95 || readEnc> targetEnc)
		//wheel_speed = readEnc<=targetEnc? NORMAL_SPEED*SpeedSmoother(div,dis)+WHEEL_MIN_SPEED : WHEEL_MIN_SPEED;   //速度变换
	//else
		//wheel_speed = wheel_speed>100?wheel_speed-50:100;
	//printf("%f  %d,%d,%lld    curIndex=%lld   \r\n",targetDistance,id[curIndex],DMCodeInfo.id,targetEnc-readEnc,curIndex);
	if(readEnc>1.5*targetEnc)
		Stop();
	if((id[curIndex]==DMCodeInfo.id && targetEnc-readEnc<=0 && DMCodeInfo.y>-50 )||targetDistance==0)   //编码器累加到目标值  到达目标距离
	{
		//wheel_speed=WHEEL_MIN_SPEED;
		Encoder1_Get_CNT();
		Encoder2_Get_CNT();
		readEnc = 0;
		readEnc2 = 0;
		targetEnc = 0;
		isUpdate = 1;
//		acc=1;
//		remberPath=9999999;
		//printf("%lld\r\n\r\n\r\n",targetEnc);
		return 1;
	}
	else
		return 0;	
}

////左转负，右转正
float Angle_Err_Limit(float arrive, float leave)
{
	float err;
	err = arrive - leave;
	if(err>180.0f) err -= 360.0f;
	else if(err<-180.f) err += 360.0f;
	return err;
}

void rotate(int LR)
{
	static u8 first=1;
	//static char dir=0;
	static long int readEnc=0,targrtEnc=0;
	static float arriveAngle=0,angleErr=0,towardAngle=0;//实际转角等于90+arriveErr
	static float dis=0.0f,div=0.0f;
	static float tan = 0.0f;
	//printf("13");
	static int preTmp=0;
	int tmp=Encoder1_Get_CNT();
	if(abs(tmp-preTmp)<1000)
	{
		preTmp=tmp;
		readEnc = readEnc+ tmp;
	}
		
	if(first && LR!=2)//左转或者右转
	{
		//TurnRound[LR]();//函数指针 左转、右转、直走
	   arriveAngle = DMCodeInfo.angle;//DMCodeInfo.angle>315 ? DMCodeInfo.angle-360 : DMCodeInfo.angle;//到达角度-45  ----- 315度
		first=0;
		angleErr =Angle_Err_Limit(arriveAngle,angle[curIndex]) ; //fabs(leaveDir[LR][arriveDir%4]-arriveAngle)-90;   //角度误差 -10----+10度  -170---190
		dis = (float)( fabs(angleErr)*ROT_1_ARC)/100.0f;         //目标距离加上误差修正距离转化为单位m
	    targrtEnc = ( fabs(angleErr)*ROT_1_ENC);
		angleErr>0 ? TurnRight() : TurnLeft();
		
//		if( )
//			TurnRight(),dir = 1;
//		else
//			TurnLeft(),dir = -1;
		//printf("到达二维码方向 :%d    出二维码角度:%d     angleErr=%f   \r\n",arriveDir,angle[curIndex],angleErr);
		//printf("%f %d %f",arriveAngle,angle[trackIndex],angleErr);
		//printf("入二维码信息：");
		//printDMCodeInfo();
		
	}
	else if(first)//直走
	{
//		unsigned char sendBuf[1024];
		//multiDistance=MultiDMDistance();
		first = 0;
		//printDMCodeInfo();
		tan = fabs(DMCodeInfo.x*0.0156f) / (lineSurplus* DMDISTANCE);   //x方向进行校正
		
		towardAngle = atan(tan) * 180.0f/3.1416f;
		towardAngle = DMCodeInfo.x>=0? -towardAngle : towardAngle;
		//printf("x=%d,towardAngle=%f\r\n",DMCodeInfo.x,towardAngle);
		arriveAngle = DMCodeInfo.angle;
		//arriveDir = getDir();
		angleErr = Angle_Err_Limit(arriveAngle,angle[curIndex]);
		dis = (float)( (fabs(angleErr+towardAngle))*ROT_1_ARC)/100.0f;    //目标距离加上误差修正距离转化为单位m
		targrtEnc = (fabs(angleErr+towardAngle))*ROT_1_ENC;//+
		//sprintf(sendBuf,"%f,%f realSumEerr=%f\r\n",angleErr,towardAngle,angleErr+towardAngle);
		//sprintf(sendBuf,"%f,%f realSumEerr=%f\r\n",angleErr,towardAngle,angleErr+towardAngle);
		//SendData(sendBuf,sizeof(sendBuf));//发送给服务器
		printf("%f    %d    realErr=%f    %f   %d\r\n",arriveAngle,angle[trackIndex],angleErr,towardAngle,lineSurplus);
		lineSurplus = 1;
		//printf("towardAngle:%f   arriveAngle=%f   angleErr=%f   \r\n",towardAngle,arriveAngle,angleErr);
		//printf("dis=%f,angleErr=%f\r\n",dis,angleErr);
		//printf("%d %d ",arriveDir,leaveDir[LR][arriveDir%4]);
		//printf("入二维码信息：");
		//printDMCodeInfo();
		if( fabs(angleErr + towardAngle) < 1 )
			goFowardFlag=1 ;
		else
			(angleErr + towardAngle) >0 ? TurnRight():TurnLeft();
	}
	
	
//	if(preAngle != DMCodeInfo.angle)
//	{
//		float temp = Angle_Err_Limit(DMCodeInfo.angle,angle[curIndex]);
//		
//		//dis = (float)( fabs(temp)*ROT_1_ARC)/100.0f;         //目标距离加上误差修正距离转化为单位m
////	    dis = (float)( fabs(temp)*ROT_1_ARC)/100.0f + div; 	
//		long long  tempEnc = ( fabs(temp)*ROT_1_ENC);
//		if((temp>0&&dir>0)||(temp<=0&&dir<=0))
//			readEnc =targrtEnc- tempEnc;
//		preAngle = DMCodeInfo.angle;
//	}
	div = (float)(readEnc)/(fabs(angleErr +towardAngle )*ROT_1_ENC)*dis;//当前已走过的距离 +towardAngle
	//wheel_speed =  div<dis ? ROT_NORMAL_SPEED*SpeedSmoother(div,dis)+ROT_MIN_SPEED:ROT_MIN_SPEED;  //速度变换
	wheel_speed = LR==TOWARD? ROT_MIN_SPEED : div<dis ? ROT_NORMAL_SPEED*fabs(SpeedSmoother(div,dis))+ROT_MIN_SPEED:ROT_MIN_SPEED;  //速度变换
	//printf("%ld,%ld,tmp=%d\r\n",readEnc,targrtEnc,tmp);
	//if(LR==2)
		//printf("%d  ",wheel_speed);
	if(fabs(DMCodeInfo.angle-angle[curIndex] + towardAngle)<=0.5||fabs(DMCodeInfo.angle-360-angle[curIndex] + towardAngle)<=0.5)
	{
		//printf("leave angle:%f\r\n",leaveDir[LR][arriveDir%4] - towardAngle);
		//到达二维码中心点，识别到中心点
		///printf("识别出去    ");
		goFowardFlag=1;
	}
	else if((readEnc>(targrtEnc+ROT_1_ENC/3) && LR!=2)|| (readEnc>targrtEnc && LR==2))//&& DMCodeInfo.id!=DMCodeInfo.preID)
	{
		//编码器累加到目标值，
		//printf("强制出去    ");
		goFowardFlag=1;	
	}
	if(goFowardFlag==1)
	{	
		for(int i=0;i<2;i++)
		{
			if(DMCodeInfo.id ==stopId[i] && targrtEnc>ROT_90_ENC/1.5)
			{
				goFowardFlag=3;
				break;
			}
			else
				goFowardFlag=1;
		}
			
		preTmp=0;
		towardAngle=0;
        //preLeaveAngle=DMCodeInfo.angle;		
		readEnc = 0;
		//wheel_speed = WHEEL_MIN_SPEED;
		//printf("%d ",timecount*10);//10
		timecount = 0;
		//printf("出二维码信息：");
		//if(LR==2)
		printDMCodeInfo(),
		//printf("\r\n");
		//printf("\r\n\r\n\r\n");
		first=1;
		TIM4->CNT=32000;
		TIM5->CNT=32000;
		deviation=-(double)DMCodeInfo.y*0.0156; //直线距离误差修正误差修正
		//DMCodeInfo.preID=DMCodeInfo.id;
		towardAngle=0;	
//		curIndex++;
//		curIndex %= len;
	}

}
