/*头文件生声明区*/
#include <REGX52.H>
#include "Key.h"
#include "Seg.h"


/*变量声明区*/
unsigned char Key_SlowDown;
unsigned char Key_Val,Key_Up,Key_Down,Key_Old;
unsigned char Seg_SlowDown;
unsigned char Seg_Pos;//数码管扫描变量
unsigned char Seg_Buf[] = {10,10,10,10,10,10} ;//数码管显示数据存放变量
unsigned int Timer_1000ms;
unsigned char Time_Count = 30;
unsigned char Seg_Mode = 0; //显示界面模式 0为显示 1为设置
unsigned char System_Flag ; //1为开始 2为复位 3为切换 4为设置
unsigned char Set_Date[] = {15,30,60};//设置参数
unsigned char Set_Date_Index = 1;
unsigned int Seg_Flicker500ms; 
bit Seg_FlickerMode;


/*按键处理函数*/
void Key_Proc()
{
	if(Key_SlowDown) return;
	Key_SlowDown = 1; //按键减速
	
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Val^Key_Old);//检测下降
	Key_Up = ~Key_Val & (Key_Val^Key_Old);//检测上升
	Key_Old = Key_Val;
	
	switch(Key_Down)
	{
		case 1://1按下 开始
				if(Seg_Mode == 0) System_Flag = 1; 
		break;
		case 2://2按下 复位
				if(Seg_Mode == 0) Time_Count = Set_Date[Set_Date_Index];
		break;
		case 3://3按下 切换
				if(Seg_Mode == 1) Time_Count = Set_Date[Set_Date_Index]; //保存设置的参数
				Seg_Mode ^= 1 ;//即Seg_Mode = Seg_Mode^1 
		break;
		case 4://4按下 设置
				if(Seg_Mode == 1)
				{
					if(++Set_Date_Index == 3) Set_Date_Index =0;//设置参数		
				}
	}
}

/*信息处理函数*/



void Seg_Proc()
{
	if(Seg_SlowDown) return;
	Seg_SlowDown = 1;//数码管减速
	
	Seg_Buf[0] = Seg_Mode + 1;
	if(Seg_Mode == 0)//系统在显示界面
	{
		Seg_Buf[4] = Time_Count / 10 %10;
		Seg_Buf[5] = Time_Count % 10;
	}
	else//系统在设置界面
	{
		if(Seg_FlickerMode == 0)
		{
		Seg_Buf[4] = Set_Date[Set_Date_Index] / 10 %10;
		Seg_Buf[5] = Set_Date[Set_Date_Index] % 10;
		}
		else
		{
		Seg_Buf[4] = 10;
		Seg_Buf[5] = 10	;
		}			
	}
}

/*其他显示函数*/
void Led_Proc()
{
	if(Time_Count == 0) 
	{
		P1 = 0x00;//灯全亮
		P2_3 = 0;//蜂鸣器使能
	}
	else
	{
		P1 = 0xff;
		P2_3 = 1;
	}
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

/*定时器0中断服务函数*/
void Timer0Server() interrupt 1
{
	TL0 = 0x18;				
	TH0 = 0xFC;

	if(++Key_SlowDown == 10) Key_SlowDown = 0;
	if(++Seg_SlowDown == 500) Seg_SlowDown = 0;
	if(++Seg_Pos==6) Seg_Pos=0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos]);//数码管显示模块
	
	if(System_Flag == 1) //倒计时模块
	{
		if(++Timer_1000ms == 1000)
		{
			Timer_1000ms = 0;
			Time_Count--;
			if(Time_Count == 255) Time_Count = 0;
		}	
	}
	
	if(++Seg_Flicker500ms > 500) 
	{
		Seg_Flicker500ms = 0;
		Seg_FlickerMode ^= 1;
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











