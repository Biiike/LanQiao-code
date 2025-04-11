#include <REGX52.H>
#include "Key.h"
#include "Seg.h"

/*变量声明区*/
unsigned char Key_val,Key_down,Key_up,Key_old;
unsigned int Key_slowdown, Seg_slowdown;
unsigned char Seg_Pos; //数码管扫描变量
unsigned char Seg_Buf[] = {10,10,10,10,10,10}; //数码管显示数值变量 10为全灭
unsigned char Seg_Disp_Mode = 0;//0为显示 1为时钟设置 2为闹钟设置 
unsigned char Clock_Disp[3] = {23,59,55};
unsigned int Timer_1000ms = 0;
unsigned char Seg_Point[6] = {0,1,0,1,0,1};
unsigned char Clock_Set[3];
unsigned char Clock_Save[3];
unsigned int Timer_250ms = 0;
unsigned int Seg_Set_Index = 0;
bit Seg_Flag;

/*按键处理函数*/
void Key_Proc()
{
	if(Key_slowdown) return;	//缓冲函数
	Key_slowdown = 1;
	
	Key_val = Key_Read();
	Key_down = Key_val & (Key_val ^ Key_old);
	Key_up = ~Key_val & (Key_val ^ Key_old);
	Key_old = Key_val;
	
	switch(Key_down)
	{
		case 1:
			Clock_Set[0]=Clock_Disp[0];
			Clock_Set[1]=Clock_Disp[1];
			Clock_Set[2]=Clock_Disp[2];		
			Seg_Disp_Mode = 1;
		break;
		case 3:
			if(Seg_Disp_Mode == 1) if(++Seg_Set_Index == 3) Seg_Set_Index = 0;
		break;
		case 5:
			if(Seg_Disp_Mode == 1)
			{
					switch(Seg_Set_Index)
					{
						case 0:
							if(++Clock_Set[0] == 24) Clock_Set[0] = 0;
						break;
						case 1:
							if(++Clock_Set[1] == 60) Clock_Set[1] = 0;
						break;
						case 2:
							if(++Clock_Set[2] == 60) Clock_Set[2] = 0;
						break;
					}
			}
		break;
		case 6:
			if(Seg_Disp_Mode == 1)
			{
					switch(Seg_Set_Index)
					{
						case 0:
							if(--Clock_Set[0] == 0) Clock_Set[0] = 23;
						break;
						case 1:
							if(--Clock_Set[1] == 0) Clock_Set[1] = 59;
						break;
						case 2:
							if(--Clock_Set[2] == 0) Clock_Set[2] = 59;
						break;
					}
			}				
		break;	
		case 7:
			Clock_Disp[0] = Clock_Set[0];
			Clock_Disp[1] = Clock_Set[1];
			Clock_Disp[2] = Clock_Set[2];
			Seg_Disp_Mode = 0;
		break;
		case 8:
			Seg_Disp_Mode = 0;
		break;
	}
		
}

/*数码管处理函数*/
void Seg_Proc()
{
	if(Seg_slowdown) return ;
	Seg_slowdown = 1;
	
	switch(Seg_Disp_Mode)
	{
		case 0://时钟显示界面
			Seg_Buf[0] = Clock_Disp[0]/10%10; 
			Seg_Buf[1] = Clock_Disp[0]%10; 
		
			Seg_Buf[2] = Clock_Disp[1]/10%10; 
			Seg_Buf[3] = Clock_Disp[1]%10;
		
			Seg_Buf[4] = Clock_Disp[2]/10%10; 
			Seg_Buf[5] = Clock_Disp[2]%10;		
		break;
		case 1://时钟设置界面
			Seg_Buf[0] = Clock_Set[0]/10%10; 
			Seg_Buf[1] = Clock_Set[0]%10; 
		
			Seg_Buf[2] = Clock_Set[1]/10%10; 
			Seg_Buf[3] = Clock_Set[1]%10;
		
			Seg_Buf[4] = Clock_Set[2]/10%10; 
			Seg_Buf[5] = Clock_Set[2]%10;	
		
			switch(Seg_Set_Index)
			{
				case 0:
					Seg_Buf[0] = Seg_Flag ? Clock_Set[0]/10%10 : 10;
					Seg_Buf[1] = Seg_Flag ? Clock_Set[0]%10 : 10; 	
				break;
				case 1:
					Seg_Buf[2] = Seg_Flag ? Clock_Set[1]/10%10 : 10;
					Seg_Buf[3] = Seg_Flag ? Clock_Set[1]%10 : 10; 
				break;
				case 2:
					Seg_Buf[4] = Seg_Flag ? Clock_Set[2]/10%10 : 10;
					Seg_Buf[5] = Seg_Flag ? Clock_Set[2]%10 : 10;
				break;
			}
	}
}

/*Led处理函数*/
void Led_Proc()
{
	
}


/*定时器0初始化*/
void Timer0_Init(void)		//1毫秒@12.000MHz
{	
	TMOD &= 0xF0;			//设置定时器模式
	TMOD |= 0x01;			//设置定时器模式
	TL0 = 0x18;				//设置定时初始值
	TH0 = 0xFC;				//设置定时初始值
	TF0 = 0;					//清除TF0标志
	TR0 = 1;					//定时器0开始计时
	ET0 = 1;	
	EA = 1;	
}

void Timer0Server() interrupt 1
{
	TL0 = 0x18;				
	TH0 = 0xFC;

	if(++Key_slowdown == 10) Key_slowdown = 0;
	if(++Seg_slowdown == 100) Seg_slowdown = 0;
	if(++Seg_Pos == 6) Seg_Pos = 0;
	
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);//数码管显示函数调用部分
	
	if(++Timer_1000ms == 1000)//时钟计时部分
	{
		Timer_1000ms = 0;
		Clock_Disp[2]++;
		if(Clock_Disp[2] == 60)
		{
			Clock_Disp[2] = 0;
			Clock_Disp[1]++;
			if(Clock_Disp[1] == 60)
			{
				Clock_Disp[1] = 0;
				Clock_Disp[0]++;
				if(Clock_Disp[0] == 24)
				{
					Clock_Disp[0] = 0;
				}
			}
		}
	}
	
	if(++Timer_250ms == 500) // 500ms周期闪烁
	{
		Timer_250ms = 0;
		Seg_Flag ^= 1;
	}
		
		
		
}

void main()
{
	Timer0_Init();
	
	while(1)
	{	
		Key_Proc();
		Seg_Proc();
		Led_Proc();
	}
}