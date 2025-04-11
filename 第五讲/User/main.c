/* 头文件声明区 */
#include <Init.h>
#include <Led.h>
#include <Seg.h>
#include <Key.h>
#include <intrins.h>
#include <ds1302.h>
#include <string.h> //用于memcpy(Set_Dat,ucDate,9) 将Set_Dat的9为数据复制到ucDate中
#include <STC15F2K60S2.H>


/* 变量声明区*/
unsigned char Key_Up,Key_Down,Key_Old,Key_Val;
unsigned int Key_Slow_Down,Seg_Slow_Down;
unsigned char Seg_Pos;
unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0};
unsigned char ucRtc[3] = {0x23,0x59,0x55};// 时钟数据存放数组
unsigned char Seg_Disp_Mode = 0; // 0-时钟显示 1-闹钟显示 2-日期显示 3-时钟设置 4-闹钟设置 5-日期设置
unsigned char ucAlarm[9] = {0x00,0x00,0x00,0x00,0x01,0x00,0xbb,0xbb,0xbb};//0xbb显示为-
unsigned char Alarm_Index;
unsigned char ucDate[3] = {0x22,0x12,0x12};//日期数据存放数组
unsigned char Set_Dat[9];//设置数据存放数组

/* 键盘处理函数 */
void Key_Proc()
{
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;
	
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Val ^ Key_Old);
	Key_Up =~Key_Val & (Key_Val ^ Key_Old);
	Key_Old = Key_Val;
	
	switch(Key_Down)
	{
		case 14:
			if(Seg_Disp_Mode < 3)
			{
				if(++Seg_Disp_Mode == 3) Seg_Disp_Mode = 0;
			}
		break;
		case 15:
			if(Seg_Disp_Mode < 3)
			{
				Seg_Disp_Mode += 3;
				Alarm_Index = 0;
				switch(Seg_Disp_Mode)
				{
					case 3://时钟设置界面
						memcpy(Set_Dat,ucRtc,9);//把时钟数据给到设置数据
					break;
					case 4://闹钟设置界面
						memcpy(Set_Dat,ucAlarm,9);//把闹钟数据给到设置数据
					break;
					case 5://日期设置界面
						memcpy(Set_Dat,ucDate,9);
					break;
				}
			}
		}
		
}

/* 信息处理函数 */
void Seg_Proc()
{
	unsigned char i;
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;
	
	/* 信息读取函数 */
	Read_Rtc(ucRtc);//读取时钟实时数据
	Read_Date(ucDate);//读取日期实时数据
	
	/* 数据处理函数 */
	switch(Seg_Disp_Mode)
	{
		case 0 ://时钟显示
			for(i=0;i <3;i++)
				{
					Seg_Buf[3*i] = ucRtc[i] /16;
					Seg_Buf[3*i+1] = ucRtc[i] %16;
				}
				Seg_Buf[2] = Seg_Buf[5] = 11;
		break;		
		case 1://闹钟显示
			for(i=0;i<3;i++)
			{
				Seg_Buf[3*i] = ucAlarm[3*Alarm_Index+i] /16;
				Seg_Buf[3*i+1] = ucAlarm[3*Alarm_Index+i] %16;
			}
			Seg_Buf[2] = Seg_Buf[5] = 11;		
		break;
		case 2://日期显示
			for(i=0;i<3;i++)
		
				{
					Seg_Buf[3*i] = ucDate[i] /16;
					Seg_Buf[3*i+1] = ucDate[i] %16;
				}
			Seg_Buf[2] = Seg_Buf[5] = 11;
		break;
		default:
			for(i=0;i <3;i++)
				{
					Seg_Buf[3*i] = Set_Dat[i+Alarm_Index*3] /16;
					Seg_Buf[3*i+1] = Set_Dat[i+Alarm_Index*3] %16;
				}
				Seg_Buf[2] = Seg_Buf[5] = 11;	
	}
			
}
/* 其他显示函数*/
void Led_Proc()
{
	
}

/* 中断服务函数 */
void Timer0_Server() interrupt 1
{
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;
	if(++Seg_Slow_Down == 100) Seg_Slow_Down = 0;
	if(++Seg_Pos == 8) Seg_Pos = 0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
	Led_Disp(Seg_Pos,ucLed[Seg_Pos]);
}


/* 定时器初始化函数 */
void Timer0_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x18;				//设置定时初始值
	TH0 = 0xFC;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//中断定时器0打开
	EA = 1;					//总中断打开
}


/* Main */
void main()
{
	System_Init();
	Timer0_Init();
	Set_Rtc(ucRtc);
	Set_Date(ucDate);
	
	while(1)
	{
		Seg_Proc();
		Key_Proc();
		Led_Proc();		
	}
}
