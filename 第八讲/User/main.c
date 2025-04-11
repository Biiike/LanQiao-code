/* 头文件声明区 */
#include <Init.h>
#include <Led.h>
#include <Seg.h>
#include <Key.h>
#include <intrins.h>
#include <ds1302.h>
#include <onewire.h>
#include <iic.h>
#include <STC15F2K60S2.H>


/* 变量声明区*/
unsigned char Key_Up,Key_Down,Key_Old,Key_Val;
unsigned int Key_Slow_Down,Seg_Slow_Down;
unsigned char Seg_Pos;
unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0};
unsigned char Rtc[3] = {0x23,0x59,0x55};
bit Sys_Disp_Mode; //0代表电压显示 1代表电压输出
float V_Data;//电压数据
float V_Data_Output;
bit Output_Mode = 0;
bit Seg_Flag = 1;

/* 键盘处理函数 */
void Key_Proc()
{
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;
	
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Old ^ Key_Val);
	Key_Up =~Key_Val & (Key_Old ^ Key_Val);
	Key_Old = Key_Val;
	
	switch(Key_Down)
	{
		case 19:
			Sys_Disp_Mode ^= 1;
		break;
		case 18:
			Output_Mode ^= 1;
		break;
		case 17:
			Seg_Flag ^= 1;
		break;
	}
}

/* 信息处理函数 */
void Seg_Proc()
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;
	
	if(Sys_Disp_Mode == 1)
	{
		if(Output_Mode == 0)
			V_Data_Output = 2;
		else
			V_Data_Output = V_Data;
	}
	
	if(Sys_Disp_Mode == 0)
	{
		Seg_Buf[0] = 12;
		Seg_Buf[1] = 10;
		Seg_Buf[2] = 10;
		Seg_Buf[3] = 10;
		Seg_Buf[4] = 10;
		Seg_Buf[5] = (unsigned char)V_Data%10;
		Seg_Buf[6] = (unsigned int)(V_Data*100)/10%10;
		Seg_Buf[7] = (unsigned int)(V_Data*100)%10;
		Seg_Point[5] = 1;
	}
	else
	{
		Seg_Buf[0] = 13;
		Seg_Buf[1] = 10;
		Seg_Buf[2] = 10;
		Seg_Buf[3] = 10;
		Seg_Buf[4] = 10;
		Seg_Buf[5] = (unsigned char)V_Data_Output%10;
		Seg_Buf[6] = (unsigned int)(V_Data_Output*100)/10%10;
		Seg_Buf[7] = (unsigned int)(V_Data_Output*100)%10;
		Seg_Point[5] = 1;
	}
	
	
	/* 信息读取函数 */
	V_Data = Ad_Read(0x41) / 51.0;
	Da_Write(V_Data_Output*51.0);
	/* 数据处理函数 */

}
/* 其他显示函数*/
void Led_Proc()
{
	unsigned char i;
	for(i=0;i<2;i++)
		ucLed[i] = (i == Sys_Disp_Mode);
	
	if(V_Data<1.5 || (V_Data >= 2.5 && V_Data < 3.5))
		ucLed[2] = 0;
	else
		ucLed[2] = 1; 
	
	if(Output_Mode == 0)
		ucLed[3] = 1;
	else
		ucLed[3] = 0;
}

/* 中断服务函数 */
void Timer0_Server() interrupt 1
{
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;
	if(++Seg_Slow_Down == 100) Seg_Slow_Down = 0;
	if(++Seg_Pos == 8) Seg_Pos = 0;
	if(Seg_Flag)
		Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
	else
		Seg_Disp(Seg_Pos,10,0);
	
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
	Set_Rtc(Rtc);
	
	while(1)
	{
		Seg_Proc();
		Key_Proc();
		Led_Proc();		
	}
}
