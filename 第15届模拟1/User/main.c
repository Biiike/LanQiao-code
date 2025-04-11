#include <iic.h>
#include <Key.h>
#include <Led.h>
#include <Seg.h>
#include <ds1302.h>

unsigned char ucRtc[3] = {0x02,0x09,0x59};
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0};
unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};
unsigned char Seg_Pos;
unsigned char Key_SlowDown;
unsigned int Seg_SlowDown;
unsigned char Key_Val,Key_Down,Key_Up,Key_Old;

unsigned char Sys_Mode;//0 时间 1 输入 2 记录
unsigned int Input_Dat = 0;
unsigned char Input_Val;
unsigned char Trigger;
unsigned char Trigger_Max;
unsigned char Dat_Time[2] = {0x00,0x00};
unsigned char Dat_0,Dat_1;
unsigned char Dat_2,Dat_3;
unsigned int Val_new,Val_old;

void Seg_Reset()
{
	unsigned char i;
	for(i = 0; i <8 ;i++)
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
	
	Trigger = 0;
	if(Key_Down == 99)
	{
		Trigger = 1;
		Input_Val = 0;
	}
	if(1<=Key_Down  && Key_Down <=9)
	{		
		Trigger = 1;
		Input_Val = Key_Val;
	}
	if(Trigger == 1 && Trigger_Max < 4)
	{
		Trigger_Max++;	
		Input_Dat = Input_Dat * 10 + Input_Val ;
		if(Trigger_Max == 1)
		{
			Dat_Time[0] = ucRtc[0];
			Dat_Time[1] = ucRtc[1];
		}
		if(Trigger_Max == 4)
		{
			Val_old = Val_new;
			Val_new = Input_Dat;

		}
	}
	
	switch(Key_Down)
	{
		case 11:
			if(++Sys_Mode > 2 ) Sys_Mode = 0;
			Input_Dat = 0;
			Trigger_Max = 0;
			Seg_Reset();
		break;
		case 12:
			if(Sys_Mode == 1)
			{
				Input_Dat = 0;
				Trigger_Max = 0;
			}
		break;
	}
	
	
}

void Seg_Proc()
{
	if(Seg_SlowDown < 400) return;
	Seg_SlowDown = 0;
	
	
	Read_Rtc(ucRtc);
	
	switch(Sys_Mode)
	{
		case 0:
			Seg_Buf[0]=ucRtc[0] / 16 % 16;
			Seg_Buf[1]=ucRtc[0]  % 16;
			Seg_Buf[3]=ucRtc[1] / 16 % 16;
			Seg_Buf[4]=ucRtc[1] % 16;
			Seg_Buf[6]=ucRtc[2] / 16 % 16;
			Seg_Buf[7]=ucRtc[2] % 16;
			Seg_Buf[2] = Seg_Buf[5] = 14;
		break;
		case 1:
			Seg_Buf[0]=15;
			if(Input_Dat == 0) 
			{
				Seg_Buf[4] = Seg_Buf[5] = Seg_Buf[6] = Seg_Buf[7] = 10;
			}
			else
			{
				if(Input_Dat>999)
					Seg_Buf[4] = Input_Dat /1000 %10;
				if(Input_Dat>99)
					Seg_Buf[5] = Input_Dat /100 %10;
				if(Input_Dat>9)
					Seg_Buf[6] = Input_Dat /10 %10;
				Seg_Buf[7] = Input_Dat % 10;
			}				
		break;
		case 2:
			Seg_Buf[0] = 13;
			Seg_Buf[3]=Dat_Time[0] / 16 % 16;
			Seg_Buf[4]=Dat_Time[0]  % 16;
			Seg_Buf[6]=Dat_Time[1] / 16 % 16;
			Seg_Buf[7]=Dat_Time[1] % 16;
			Seg_Buf[5] = 14;
		
			Dat_0 = Seg_Buf[3] * 10 + Seg_Buf[4];
			Dat_1 = Seg_Buf[6] * 10 + Seg_Buf[7];
			EEPROM_Write(&Dat_0, 0,1);
			EEPROM_Write(&Dat_1, 1,1);
		
			Dat_2 = (unsigned char)(Input_Dat >> 8);
			Dat_3 = (unsigned char)(Input_Dat & 0x00ff);
			EEPROM_Write(&Dat_2, 2,1);
			EEPROM_Write(&Dat_3, 3,1);
		break;
	}
}

void Led_Proc()
{
	ucLed[0] = (Sys_Mode == 0)? 1:0;
	ucLed[1] = (Sys_Mode == 1)? 1:0;
	ucLed[2] = (Sys_Mode == 2)? 1:0;
	ucLed[3] = (Val_old<Val_new)? 1 : 0;
}

void Timer0_Isr(void) interrupt 1
{
	Key_SlowDown++;
	Seg_SlowDown++;
	if(++Seg_Pos > 8) Seg_Pos = 0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
	Led_Disp(ucLed);

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
	Write_Rtc(ucRtc);
	while(1)
	{
		Key_Proc();
		Seg_Proc();
		Led_Proc();
	}
}