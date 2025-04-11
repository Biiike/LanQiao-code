#include <STC15F2K60S2.H>
#include <Init.h>
#include <Key.h>
#include <Seg.h>
#include <Led.h>
#include <iic.h>

unsigned char Key_SlowDown;
unsigned int Seg_SlowDown;

unsigned char Key_Val,Key_Down,Key_Up,Key_Old;

unsigned char Seg_Pos;
unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0,};

unsigned char Sys_Mode;//1湿度 2参数 3时间
bit Change_Mode = 0;//0停止 1启动

float V_Data;
unsigned char W_Data;
unsigned char canshu = 50;
unsigned char time = 3;
unsigned int Timer_time;
unsigned int Timer_Trigger;
bit Trigger;
unsigned char temp;
void Seg_Reset()
{
	unsigned char i;
	for(i = 0;i<8 ;i++)
	{
		Seg_Buf[i] = 10;
		Seg_Point[i] = 0;
	}
}

void Key_Proc()
{
	if(Key_SlowDown < 10) return;
	Key_SlowDown = 0;	
	
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Old ^ Key_Val);
	Key_Up = ~Key_Val & (Key_Old ^ Key_Val);
	Key_Old = Key_Val;
	
	Timer_time = time * 1000;
	
	switch(Key_Down)
	{
		case 4:
			Seg_Reset();
			if(++Sys_Mode > 2) Sys_Mode = 0;
		break;
		case 8:
			if(Sys_Mode == 1)
			{
				canshu = canshu - 5;
				if(canshu < 30)
						canshu = 90;
			}
			else if (Sys_Mode == 2)
			{
				time = time - 1;
				if(time <1)
					time = 10;

			}
					
		break;
		case 9:
			if(Sys_Mode == 1)
			{
				canshu = canshu + 5;
				if(canshu > 90)
						canshu = 30;
			}
			else if (Sys_Mode == 2)
			{
				time = time + 1;
				if(time >10)
					time = 1;
			}		
			
		break;
	  case 5:
			Change_Mode ^= 1;
			if(Sys_Mode == 2)
				warn = 0;
		break;
	}
}

void Seg_Proc()
{

	if(Seg_SlowDown < 300) return;
	Seg_SlowDown = 0;
	
	switch(Sys_Mode)
	{
		case 0:
			Seg_Buf[0] = 11;
			if(W_Data < 10)
			{
				Seg_Buf[6] = 10;
				Seg_Buf[7] = W_Data;
			}
			else
			{
				Seg_Buf[6] = W_Data / 10 % 10 ;
				Seg_Buf[7] = W_Data  % 10 ;
			}
		break;
		case 1:
			Seg_Buf[0] = 12;
			if(canshu < 10)
			{
				Seg_Buf[6] = 10;
				Seg_Buf[7] = canshu;
			}
			else
			{
				Seg_Buf[6] = canshu / 10 % 10 ;
				Seg_Buf[7] = canshu  % 10 ;
			}		
		break;		
		case 2:
			Seg_Buf[0] = 13;
			if(time < 10)
			{
				Seg_Buf[6] = 10;
				Seg_Buf[7] = time;
			}
			else
			{
				Seg_Buf[6] = time / 10 % 10 ;
				Seg_Buf[7] = time  % 10 ;
			}			
			break;
	}
	
	if(Change_Mode)
	{

		if(W_Data < canshu )
		{
			Trigger = 1;
			P0 = 0x10;//蜂鸣器和继电器
			temp = (P2 & 0x1f) | 0xa0;
			P2 = temp;
			temp = P2 & 0x1f;
			P2 = temp;
		}
	}
	else
	{
		P0 = 0x00;//蜂鸣器和继电器
		temp = (P2 & 0x1f) | 0xa0;
		P2 = temp;
		temp = P2 & 0x1f;
		P2 = temp;
	}
	
	V_Data = Ad_Read(0x03) / 51.0;
	if(V_Data <= 1) W_Data = 10;
	else if (V_Data >= 4) W_Data = 90;
	else W_Data = 10 + ((80 / 3) * (V_Data - 1));
	

}

void Led_Proc()
{

	ucLed[0] = (Sys_Mode == 0)? 1 : 0;
	ucLed[1] = (Sys_Mode == 1)? 1 : 0;
	ucLed[2] = (Sys_Mode == 2)? 1 : 0;
  ucLed[7] = Change_Mode == 0 ? 0 : 1;	
}

void Timer0_Isr(void) interrupt 1
{
	Key_SlowDown++;
	Seg_SlowDown++;
	
	if(++Seg_Pos == 8) Seg_Pos = 0;
	Seg_Disp(Seg_Buf[Seg_Pos],Seg_Pos,Seg_Point[Seg_Pos]);
	Led_Disp(Seg_Pos,ucLed[Seg_Pos]);
	
	if(Trigger == 1)
	{
		if(++Timer_Trigger == Timer_time)
		{
			Timer_Trigger = 0;
//			Trigger = 0;
			P0 = 0x00;//蜂鸣器和继电器
			temp = (P2 & 0x1f) | 0xa0;
			P2 = temp;
			temp = P2 & 0x1f;
			P2 = temp;
		}
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

void main ()
{
	Sys_Init();
	Timer0_Init();
	while(1)
	{
		Seg_Proc();
		Key_Proc();
		Led_Proc();
	}
}