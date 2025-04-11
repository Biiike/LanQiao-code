/* 头文件声明区 */
#include <STC15F2K60S2.H>
#include <Init.h>
#include <Led.h>
#include <Seg.h>
#include <Key.h>
#include <intrins.h>
#include <ds1302.h>
#include <iic.h>
#include <onewire.h>



/* 变量声明区*/
unsigned char Key_Up,Key_Down,Key_Old,Key_Val;
unsigned int Key_Slow_Down,Seg_Slow_Down;
unsigned char Seg_Pos;
unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};
unsigned char UcLed[8] = {0,0,0,0,0,0,0,0};
unsigned char Rtc[3] = {0x23,0x59,0x55};//时间

unsigned char Sys_Mode = 0;//0-温度 1-参数 2-DAC
float T;
unsigned char T_Data_Disp = 25;
unsigned char T_Data = 25;//参数默认值为25；
float V_Data;
float V_Data_Disp;

unsigned char Mode;

/* 键盘处理函数 */
void Key_Proc()
{
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;
	
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Old ^ Key_Val);
	Key_Up =~Key_Val & (Key_Old ^ Key_Val);
	Key_Old = Key_Val;//持续按下
	
	

	switch(Key_Down)
	{
		case 4:
			if(++Sys_Mode >2) Sys_Mode = 0;
			if(Sys_Mode != 1)	T_Data = T_Data_Disp;
		break;
		case 8:
			if(Sys_Mode == 1)
			{
				if(--T_Data_Disp>100) T_Data_Disp = 0;
			}
		break;
		case 9:
			if(Sys_Mode == 1)
			{
				if(++T_Data_Disp==255) T_Data_Disp = 100;
			}
		break;
		case 5:
		Mode ^= 1;
		break;
	}
}

/* 信息处理函数 */
void Seg_Proc()
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;
	
	switch(Sys_Mode)
	{
		case 0://温度界面
			Seg_Buf[0] = 12;
			Seg_Buf[1] = 16;
			Seg_Buf[2] = 16;
			Seg_Buf[3] = 16;
			Seg_Buf[4] = (unsigned char) T /10 % 10;
			Seg_Buf[5] = (unsigned char) T % 10;
			Seg_Buf[6] = (int)(T* 10)% 10;
			Seg_Buf[7] = (int)(T* 100)% 10;
			Seg_Point[5] = 1;
		break;
		case 1://参数设置
			Seg_Buf[0] = 17;
			Seg_Buf[1] = 16;
			Seg_Buf[2] = 16;
			Seg_Buf[3] = 16;
			Seg_Buf[4] = 16;
			Seg_Buf[5] = 16;
			Seg_Buf[6] = (T_Data_Disp /10) % 10;
			Seg_Buf[7] = T_Data_Disp % 10;
			Seg_Point[5] = 0;
		break;
		case 2://输出界面
			if(Mode == 0)//模式一
			{
				if(T<T_Data)
					V_Data = 0;
				else if (T>T_Data)
					V_Data = 5;
			}
			else//模式二
			{
				if(T<20)
					V_Data = 1;
				else if (T>20 && T<40)
					V_Data = 1+((T-20)*0.15);
				else if (T>40)
					V_Data = 4;
			}
					
			Seg_Buf[0] = 10;
			Seg_Buf[1] = 16;
			Seg_Buf[2] = 16;
			Seg_Buf[3] = 16;
			Seg_Buf[4] = 16;
			Seg_Buf[5] = (unsigned char)V_Data%10;
			Seg_Buf[6] = (unsigned int)(V_Data*100)/10 % 10;
			Seg_Buf[7] = (unsigned int)(V_Data*100) % 10;
			Seg_Point[5] = 1;

		break;
	}
	
	/* 信息读取函数 */
	Da_Write(V_Data * 50.1);
	T = Read_T();
	
	/* 数据处理函数 */

}
/* 其他显示函数*/
void Led_Proc()
{

	UcLed[0] = !Mode;
	
		if(Sys_Mode == 0)
			UcLed[1] = 1;
		else
			UcLed[1] = 0;

		if(Sys_Mode == 1)
			UcLed[2] = 1;
		else
			UcLed[2] = 0;

		if(Sys_Mode == 2)
			UcLed[3] = 1;
		else
			UcLed[3] = 0;
	
}

/* 中断服务函数 */
void Timer0_Server() interrupt 1
{
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;
	if(++Seg_Slow_Down == 100) Seg_Slow_Down = 0;
	if(++Seg_Pos == 8) Seg_Pos = 0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
	Led_Disp(Seg_Pos,UcLed[Seg_Pos]);

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
	System_Init();//系统初始化（关闭灯，蜂鸣器）
	Timer0_Init();//定时器初始化
//	Set_Rtc(Rtc);设置时间
	
	while(1)
	{
		Seg_Proc();
		Key_Proc();
		Led_Proc();		
	}
}



