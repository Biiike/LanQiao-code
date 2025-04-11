/* 头文件声明区 */
#include <STC15F2K60S2.H>
#include <Init.h>
#include <Led.h>
#include <Seg.h>
#include <Key.h>
#include <intrins.h>
#include <ds1302.h>
#include <onewire.h>
#include <iic.h>



/* 变量声明区*/
unsigned char Key_Up,Key_Down,Key_Old,Key_Val;
unsigned int Key_Slow_Down,Seg_Slow_Down;
unsigned char Seg_Pos;
unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0};
unsigned char Rtc[3] = {0x23,0x59,0x55};//时间

/* 键盘处理函数 */
void Key_Proc()
{
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;
	
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Val ^ Key_Old);
	Key_Up =~Key_Val & (Key_Val ^ Key_Old);
	Key_Old = Key_Val;//持续按下
}

/* 信息处理函数 */
void Seg_Proc()
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;
	
	/* 信息读取函数 */
//	Read_Rtc(Rtc);读时钟

	
	/* 数据处理函数 */

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



