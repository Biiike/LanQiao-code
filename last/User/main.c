#include <STC15F2K60S2.H>
#include <init.h>
#include <iic.h>
#include <Seg.h>
#include <Key.h>
#include <Led.h>
#include <ds1302.h>
#include <onewire.h>
#include <ultrasound.h>
#include <intrins.h>
#include <Uart.h>

unsigned char Key_Slowdown;
unsigned int Seg_Slowdown;

unsigned char Key_Val,Key_Down,Key_Up,Key_Old;
unsigned char Seg_Pos;
unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};
unsigned char Led_Buf[8] = {0,0,0,0,0,0,0,0};
unsigned char ucRtc[3] = {0x23,0x59,0x55};
unsigned char Sys_Mode = 0;
unsigned int Timer_1000ms;

unsigned char Uart_Recv[10];
unsigned char Uart_Recv_Index;
unsigned char Uart_Recv_Tick;
unsigned char Uart_Rx_Flag;
unsigned char Str[] = {1,2};

float T_Data;
float V_Data;
float R_Data;
unsigned int U_Data;
unsigned int freq;

void Seg_Reset()
{
	unsigned char i = 0;
	for(i = 0; i <8; i++)
	{
		Seg_Buf[i] = 10;
	}
}

void Key_Proc()
{
	if(Key_Slowdown<10) return;
	Key_Slowdown = 0;
	
	T_Data = Read_T();
	Read_Rtc(ucRtc);

	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Old ^ Key_Val);
	Key_Up = ~Key_Val & (Key_Old ^ Key_Val);
	Key_Old = Key_Val;
	
	
	
	switch(Key_Down)
	{
		case 4:
			if(++Sys_Mode >4) Sys_Mode = 0;
			Uart_Send("change");
			Seg_Reset();
		break;
	}
}

void Seg_Proc()
{
	if(Seg_Slowdown<400) return;
	Seg_Slowdown = 0;
	R_Data = Ad_Read(0x01) / 51.0;
	R_Data = Ad_Read(0x01) / 51.0;
	U_Data = Ut_Wava_Read();
	
	
	switch(Sys_Mode)
	{
		case 0:
			Seg_Buf[0] = (unsigned char)T_Data /10 % 10;
			Seg_Buf[1] = (unsigned char)T_Data % 10;
			Seg_Buf[2] = (unsigned int)(T_Data *10) % 10;
			Seg_Buf[7] = Uart_Recv[0];
		break;
		case 1:
			Seg_Buf[0] = ucRtc[0] /16 % 16;
			Seg_Buf[1] = ucRtc[0]  % 16;
			Seg_Buf[2] = ucRtc[1] /16 % 16;
			Seg_Buf[3] = ucRtc[1]  % 16;
			Seg_Buf[4] = ucRtc[2] /16 % 16;
			Seg_Buf[5] = ucRtc[2]  % 16;
		break;
		case 2:
			Seg_Buf[0] = freq / 10000 %10;
			Seg_Buf[1] = freq / 1000 %10;
			Seg_Buf[2] = freq / 100 %10;
			Seg_Buf[3] = freq / 10 %10;
			Seg_Buf[4] = freq %10;
			
		break;
		case 3:
			Seg_Buf[0] = (unsigned char)V_Data %10;
			Seg_Buf[1] = (unsigned int)(V_Data*100)/10%10;
			Seg_Buf[2] = (unsigned int)(V_Data*100)%10;
		
			Seg_Buf[5] = (unsigned char)R_Data %10;
			Seg_Buf[6] = (unsigned int)(R_Data*100)/10%10;
			Seg_Buf[7] = (unsigned int)(R_Data*100)%10;
		break;
		case 4:
			Seg_Buf[0] = U_Data/100 %10;
			Seg_Buf[1] = U_Data /10%10;
			Seg_Buf[2] = U_Data%10;
		break;
	}
	
	V_Data = Ad_Read(0x03) / 51.0;
	V_Data = Ad_Read(0x03) / 51.0;

}

void Led_Proc()
{
	
}

void Timer1_Isr(void) interrupt 3
{
	Key_Slowdown++;
	Seg_Slowdown++;
	if(++Seg_Pos>8) Seg_Pos = 0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
	Led_Disp(Led_Buf);
	
	if(++Timer_1000ms == 1000)
	{
		Timer_1000ms = 0;
		freq = TH0<<8|TL0;
		TH0 = TL0 = 0;
	}

		
}


void Timer0_Init(void)		//@12.000MHz
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TMOD |= 0x05;			//设置定时器模式
	TL0 = 0x00;				//设置定时初始值
	TH0 = 0x00;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时

}

void Timer1_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0xBF;			//定时器时钟12T模式
	TMOD &= 0x0F;			//设置定时器模式
	TL1 = 0x18;				//设置定时初始值
	TH1 = 0xFC;				//设置定时初始值
	TF1 = 0;				//清除TF1标志
	TR1 = 1;				//定时器1开始计时
	ET1 = 1;				//使能定时器1中断
	EA = 1;
}

void Uart1Server() interrupt 4
{
	if(RI == 1)
	{
		Uart_Recv[Uart_Recv_Index] = SBUF;
		Uart_Recv_Index++;
		RI = 0;
	}
}



void Delay750ms(void)	//@12.000MHz
{
	unsigned char data i, j, k;

	_nop_();
	_nop_();
	i = 35;
	j = 51;
	k = 182;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


void main()
{
	Timer0_Init();
	Timer1_Init();
	Uart1_Init();	
	Sys_Init();
	Read_T();
	Delay750ms();
	Read_T();
	Write_Rtc(ucRtc);
		
	while(1)
	{
		Key_Proc();
		Led_Proc();
		Seg_Proc();
	}
}