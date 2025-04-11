/* 头文件声明区 */
#include <STC15F2K60S2.H>
#include <Led.h>
#include <Seg.h>
#include <Key.h>
#include <intrins.h>
#include <ds1302.h>
#include <iic.h>



/* 变量声明区*/
unsigned char Key_Up,Key_Down,Key_Old,Key_Val;
unsigned int Key_Slow_Down,Seg_Slow_Down;
unsigned char Seg_Pos;
unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0};
unsigned char Rtc[3] = {0x23,0x59,0x50};//时间

unsigned char Sys_Mode;//0时间 1数据 2历史

float V_Data;
float R_Data;

unsigned char Index = 0;
unsigned char Index_Time[3][3];

bit Trigger ;//触发标志
bit Trigger_hold;//持续触发标志
unsigned Trigger_Time[3];
unsigned int Timer_3000ms;

void Seg_Reset()
{
	unsigned char i = 0;
  for(i = 0;i<8;i++)
		{
			Seg_Buf[i] = 10;
			Seg_Point[i] = 0;
		}
}

void Tag_Reset()
{
	unsigned char i = 0;
	unsigned char a = 0;
	for(i = 0;i<3;i++)
	{
		for(a = 0; a < 3; a++)
		{
			Index_Time[i][a] = 0;
		}
	}
}
/* 键盘处理函数 */
void Key_Proc()
{
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;
	
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Val ^ Key_Old);
	Key_Up =~Key_Val & (Key_Val ^ Key_Old);
	Key_Old = Key_Val;//持续按下
	
	switch(Key_Down)
	{
		case 4:
			Seg_Reset();
			Index = 1;
			if(++Sys_Mode > 2)
				Sys_Mode = 0;
		break;
		case 5:
			if(++Index >3)
				Index = 1;
		break;
		case 8:
			Tag_Reset();
		break;
	}
}

/* 信息处理函数 */
void Seg_Proc()
{
	unsigned char i;
	
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;
	
	Read_Rtc(Rtc);
	R_Data = Ad_Read(0x01) / 51.0;	
	R_Data = Ad_Read(0x01) / 51.0;	
	V_Data = Ad_Read(0x03) / 51.0;
	V_Data = Ad_Read(0x03) / 51.0;

	if(Trigger)
	{
		Seg_Buf[0] = Seg_Buf[1] = 18;
		Seg_Buf[2] = Trigger_Time[0] / 16 % 16;
		Seg_Buf[3] = Trigger_Time[0]  % 16;
		Seg_Buf[4] = Trigger_Time[1] / 16 % 16;
		Seg_Buf[5] = Trigger_Time[1]  % 16;
		Seg_Buf[6] = Trigger_Time[2] / 16 % 16;
		Seg_Buf[7] = Trigger_Time[2]  % 16;
	}
	else
	{
		switch(Sys_Mode)
		{
			case 0:
				Seg_Buf[0] = Rtc[0] / 16 % 16;
				Seg_Buf[1] = Rtc[0]  % 16;
				Seg_Buf[3] = Rtc[1] / 16 % 16;
				Seg_Buf[4] = Rtc[1]  % 16;
				Seg_Buf[6] = Rtc[2] / 16 % 16;
				Seg_Buf[7] = Rtc[2]  % 16;
				Seg_Buf[2] = Seg_Buf[5] = 11;
			break;
			case 1:
				Seg_Buf[0] = 12;
				Seg_Buf[4] = 17;
				
				Seg_Buf[1] = (unsigned char)R_Data %10;
				Seg_Buf[2] = (unsigned int)(R_Data * 10) %10;
				Seg_Buf[3] = (unsigned int)(R_Data * 100) %10;
				Seg_Point[1] = 1;
			
				Seg_Buf[5] = (unsigned int)V_Data %10;
				Seg_Buf[6] = (unsigned int)(V_Data * 10) %10;
				Seg_Buf[7] = (unsigned int)(V_Data * 100) %10;
				Seg_Point[5] = 1;
			break;
			case 2:
				Seg_Buf[0] = 15;
				Seg_Buf[1] = Index;
				
				if(Index == 1)
				{
					if(Index_Time[0][0] == 0 && Index_Time[0][1] == 0 && Index_Time[0][2] == 0)
						Seg_Buf[2] = Seg_Buf[3] = Seg_Buf[4] = Seg_Buf[5] = Seg_Buf[6] = Seg_Buf[7] = 11;
					else
					{
						Seg_Buf[2] = Index_Time[0][0] / 16 % 16;
						Seg_Buf[3] = Index_Time[0][0]  % 16;
						Seg_Buf[4] = Index_Time[0][1] / 16 % 16;
						Seg_Buf[5] = Index_Time[0][1]  % 16;
						Seg_Buf[6] = Index_Time[0][2] / 16 % 16;
						Seg_Buf[7] = Index_Time[0][2]  % 16;
					}
				}
				if(Index == 2)
				{
					if(Index_Time[1][0] == 0 && Index_Time[1][1] == 0 && Index_Time[1][2] == 0)
						Seg_Buf[2] = Seg_Buf[3] = Seg_Buf[4] = Seg_Buf[5] = Seg_Buf[6] = Seg_Buf[7] = 11;
					else
					{
						Seg_Buf[2] = Index_Time[1][0] / 16 % 16;
						Seg_Buf[3] = Index_Time[1][0]  % 16;
						Seg_Buf[4] = Index_Time[1][1] / 16 % 16;
						Seg_Buf[5] = Index_Time[1][1]  % 16;
						Seg_Buf[6] = Index_Time[1][2] / 16 % 16;
						Seg_Buf[7] = Index_Time[1][2]  % 16;
					}
				}
				if(Index == 3)
				{
					if(Index_Time[2][0] == 0 && Index_Time[2][1] == 0 && Index_Time[2][2] == 0)
						Seg_Buf[2] = Seg_Buf[3] = Seg_Buf[4] = Seg_Buf[5] = Seg_Buf[6] = Seg_Buf[7] = 11;
					else
					{
						Seg_Buf[2] = Index_Time[2][0] / 16 % 16;
						Seg_Buf[3] = Index_Time[2][0]  % 16;
						Seg_Buf[4] = Index_Time[2][1] / 16 % 16;
						Seg_Buf[5] = Index_Time[2][1]  % 16;
						Seg_Buf[6] = Index_Time[2][2] / 16 % 16;
						Seg_Buf[7] = Index_Time[2][2]  % 16;
					}
				}
			break;
		}
	}
	
	/* 数据处理函数 */	
	if(R_Data<V_Data && Timer_3000ms == 0 && Trigger_hold == 1)//触发切换
	{
		Seg_Reset();
		Trigger ^= 1;
		Trigger_hold = 0;
		
		Trigger_Time[0] = Rtc[0];
		Trigger_Time[1] = Rtc[1];
		Trigger_Time[2] = Rtc[2];
		
		for(i = 0; i<3; i++)
		{
			Index_Time[2][i] = Index_Time[1][i];
		}
		
		for(i = 0; i<3; i++)
		{
			Index_Time[1][i] = Index_Time[0][i];
		}
		
		for(i = 0; i<3; i++)
		{
			Index_Time[0][i] = Rtc[i];
		}
	}
	
	if(V_Data < R_Data)
		Trigger_hold = 1;

}
/* 其他显示函数*/
void Led_Proc()
{
	ucLed[0] = (Sys_Mode == 0 && Trigger == 0)? 1 : 0;
	ucLed[1] = (Sys_Mode == 1 && Trigger == 0)? 1 : 0;
	ucLed[2] = (Sys_Mode == 2 && Trigger == 0)? 1 : 0;
	ucLed[7] = Trigger;
}

/* 中断服务函数 */
void Timer0_Server() interrupt 1
{
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;
	if(++Seg_Slow_Down == 100) Seg_Slow_Down = 0;
	if(++Seg_Pos == 8) Seg_Pos = 0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
	Led_Disp(Seg_Pos,ucLed[Seg_Pos]);

	if(Trigger == 1)
	{
		++Timer_3000ms;
		if(Timer_3000ms == 3000)
		{
			Timer_3000ms = 0;
			Trigger = 0;
		}
	}
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
	Timer0_Init();//定时器初始化
	Set_Rtc(Rtc);//设置时间
	
	while(1)
	{
		Seg_Proc();
		Key_Proc();
		Led_Proc();		
	}
}



