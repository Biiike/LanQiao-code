#include <STC15F2K60S2.H>
#include <iic.h>
#include <Led.h>
#include <Seg.h>
#include <Key.h>
#include <ultrasound.h>

unsigned char Key_SlowDown;
unsigned int Seg_SlowDown;

unsigned char Seg_Pos;
unsigned char Seg_Buf[8]={10,10,10,10,10,10,10,10};
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0,};
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0};
unsigned char Key_Val,Key_Down,Key_Up,Key_Old;

unsigned char Sys_Mode = 0;//0 测距 1参数 2记录
bit Change_Mode = 0;

unsigned int Distance;
unsigned int Max_Dat = 60,Min_Dat = 10;
unsigned int V_Dat;
unsigned char Trigger_zhuan;

bit Trigger_normal;
bit Trigger_led;
unsigned char warn;

unsigned char Timer_100ms;
void Seg_Reset()
{
	unsigned char i;
	for(i = 0; i < 8 ; i++)
	{
		Seg_Buf[i] = 10;
		Seg_Point[i] = 0;
	}
}

void Key_Proc()
{
	unsigned char i;
	unsigned char a;
	if(Key_SlowDown < 10) return;
	Key_SlowDown = 0;
		
	Distance = Ut_Wave_Data();
	V_Dat = Ad_Read(0x03);
	
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Old ^ Key_Val);
	Key_Up = ~Key_Val & (Key_Old ^ Key_Val);
	Key_Old = Key_Val;
	
	if(Trigger_zhuan == 1)
	{
		for(i = 0; i<5 ;i++)
		{
			if(51*i < V_Dat && V_Dat < 51* (i+1))
				Max_Dat = 50 + (i*10);
		}
	}
	
 if (Trigger_zhuan == 2)
	{
		for(a = 0; a<5 ;a++)
		{
			if(51*a < V_Dat && V_Dat < 51* (a+1))
				Min_Dat = 0 + (a*10);
		}	
	}


	switch(Key_Down)
	{
		case 4:
			if(++Sys_Mode > 2) Sys_Mode = 0;
			Seg_Reset();
		break;
		case 5:
			Change_Mode ^= 1;
			if(Sys_Mode == 2)
				warn = 0;
		break;
		case 9:
			if(Sys_Mode == 1)
			{
				if(Change_Mode == 0)
				{
					Trigger_zhuan = 0;
					Max_Dat = Max_Dat + 10;
					if(Max_Dat > 90) Max_Dat = 50;
				}
				else
					Trigger_zhuan = 1;
			}
		break;
		case 8:
			if(Sys_Mode == 1)
			{
				if(Change_Mode == 0)
				{
					Trigger_zhuan = 0;
					Min_Dat = Min_Dat + 10;
					if(Min_Dat > 40) Min_Dat = 0;
				}
				else
					Trigger_zhuan = 2;
			}
		break;
	}
}

void Seg_Proc()
{
	if(Seg_SlowDown < 400) return;
	Seg_SlowDown = 0;
	
	if(Distance >= Min_Dat && Distance <= Max_Dat)
		Trigger_normal = 1;
	
	if((Distance > Max_Dat || Distance < Min_Dat) && Trigger_normal == 1)
	{
		warn++;
		Trigger_normal = 0;
	}
	
	
	switch(Sys_Mode)
	{
		case 0:
			Seg_Buf[0] = 11;
			if(Distance > 99)
				Seg_Buf[5] = Distance / 100 % 10;
			else
				Seg_Buf[5] = 10;
			if(Distance>9)
				Seg_Buf[6] = Distance / 10 %10;
			else
				Seg_Buf[6] = 10;
			Seg_Buf[7] = Distance %10;
		break;
		case 1:
			Seg_Buf[0] = 12;
			if(Change_Mode)
				Seg_Buf[1] = 2;
			else
			  Seg_Buf[1] = 1;
			Seg_Buf[2] = 10;
			Seg_Buf[3] = Min_Dat / 10;
			Seg_Buf[4] = Min_Dat % 10;
			Seg_Buf[5] = 14;
			Seg_Buf[6] = Max_Dat / 10;
			Seg_Buf[7] = Max_Dat % 10;
		break;
		case 2:
			Seg_Buf[0] = 13;
			if(warn > 9) 
				Seg_Buf[7] = 14;
			else
				Seg_Buf[7] = warn;
		break;
	}
}

void Led_Proc()
{
	ucLed[0] = (Sys_Mode == 0)? 1:0;
	ucLed[1] = (Sys_Mode == 1)? 1:0;
	ucLed[2] = (Sys_Mode == 2)? 1:0;
	ucLed[7] = (Min_Dat <= Distance && Max_Dat >= Distance)?1:Trigger_led;
	
}



void Timer0_Isr(void) interrupt 1
{
	++Key_SlowDown;
	++Seg_SlowDown;
	
	if(++Seg_Pos>8) Seg_Pos = 0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
	Led_Disp(Seg_Pos, ucLed[Seg_Pos]);
	
	if(++Timer_100ms == 100)
	{
		Timer_100ms = 0;
		Trigger_led ^= 1;
	}
}

void Timer0_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x18;				//设置定时初始值
	TH0 = 0xFC;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//使能定时器0中断
	EA = 1;
}


void main()
{
	P0 = 0xff;
	P2 = P2 & 0x1f | 0x80;
	P2 &= 0x1f;
	
	Timer0_Init();
	while(1)
	{
		Led_Proc();
		Key_Proc();
		Seg_Proc();
	}
}