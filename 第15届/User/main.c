/* 头文件声明区 */
#include <STC15F2K60S2.H>
#include <Led.h>
#include <Seg.h>
#include <Key.h>
#include <ds1302.h>
#include <iic.h>

/* 变量声明区*/
unsigned char Key_Up,Key_Down,Key_Old,Key_Val;
unsigned int Key_Slow_Down,Seg_Slow_Down;
unsigned char Seg_Pos;
unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0};
unsigned char Rtc[3] = {0x13,0x03,0x03};//时间

unsigned int Timer_1000ms;
long int freq;
long int Real_freq;

unsigned char Sys_Mode; // 0频率 1参数 2时间 3回显 
bit Para_Mode; //0 超限参数 1校准值
bit ReDisp_Mode; //0频率 1时间

unsigned int PF = 2000;//超限参数
int jiaozhun = 0;//校准值

unsigned int MaxF;
unsigned int MaxF_Old;
unsigned char MaxF_Time[3];
 
float V_Data;

unsigned char Timer_200ms;
bit Led_Flag; 
void Seg_Reset()
{
	unsigned char i = 0;
	for(i = 0;i<8;i++)
	{
		Seg_Buf[i] = 10;
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
		case 4://界面切换模式
			Seg_Reset();
			Para_Mode = 0;
			ReDisp_Mode = 0;
			if(++Sys_Mode == 4)
				Sys_Mode = 0;
		break;
		case 5://参数子界面切换
			if(Sys_Mode == 1)
			{
				Seg_Reset();
				Para_Mode ^= 1;
			}
			else if (Sys_Mode == 3)
			{
				Seg_Reset();
				ReDisp_Mode ^= 1;
			}
		break;
		case 8://加
			if(Sys_Mode == 1)
			{
				if(Para_Mode == 0)
				{
					PF = PF+1000;
					if(PF > 9000)
						PF = 9000;
				}
				else
				{
					jiaozhun = jiaozhun + 100;
					if(jiaozhun > 900)
						jiaozhun = 900;
				}
			}
		break;
		case 9:
			if(Sys_Mode == 1)
			{
				if(Para_Mode == 0)
				{
					PF = PF - 1000;
					if(PF < 1000)
						PF = 1000;
				}
				else
				{
					jiaozhun = jiaozhun - 100;
					if(jiaozhun < -900)
						jiaozhun = -900;
				}
			}		
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
		case 0:
			Seg_Buf[0] = 14;
			if(Real_freq<0)
			{
				Seg_Buf[1] = 10;
				Seg_Buf[2] = 10;
				Seg_Buf[3] = 10;
				Seg_Buf[4] = 10;
				Seg_Buf[5] = 10;
				Seg_Buf[6] = 16;
				Seg_Buf[7] = 16;
			}
			else
			{
				if(Real_freq>9999)
					Seg_Buf[3] = Real_freq /10000 %10;
				else
					Seg_Buf[3] = 10;
				if(Real_freq>999)
					Seg_Buf[4] = Real_freq /1000 %10;
				else
					Seg_Buf[4] = 10;
				if(Real_freq>99)
					Seg_Buf[5] = Real_freq /100 %10;
				else
					Seg_Buf[5] = 10;
				if(Real_freq>9)
					Seg_Buf[6] = Real_freq /10 %10;
				else
					Seg_Buf[6] = 10;
				Seg_Buf[7] = Real_freq % 10;
			}
		break;
		case 1:
			Seg_Buf[0] = 12;
			if(Para_Mode == 0)
			{
				Seg_Buf[1] = 1;
				Seg_Buf[4] = PF/1000%10;
				Seg_Buf[5] = PF/100%10;
				Seg_Buf[6] = PF/10%10;
				Seg_Buf[7] = PF%10;
			}
			else
			{
				Seg_Buf[1] = 2;
				if(jiaozhun<0)
					Seg_Buf[4] = 11;
				else
					Seg_Buf[4] = 10;
				
				if(jiaozhun != 0)
				{
					if(jiaozhun<0)
						Seg_Buf[5] = jiaozhun / -100;
					else
						Seg_Buf[5] = jiaozhun / 100;
					Seg_Buf[6] = 0;
					Seg_Buf[7] = 0;
				}
				else
				{
					Seg_Buf[5] = 10;
					Seg_Buf[6] = 10;
					Seg_Buf[7] = 0;
				}
			}
		break;
		case 2:
			Seg_Buf[0] = Rtc[0]/16%16;
			Seg_Buf[1] = Rtc[0]%16;
			Seg_Buf[2] = 11;
			Seg_Buf[3] = Rtc[1]/16%16;
			Seg_Buf[4] = Rtc[1]%16;
			Seg_Buf[5] = 11;
			Seg_Buf[6] = Rtc[2]/16%16;
			Seg_Buf[7] = Rtc[2]%16;
		break;		
		case 3:
			if(ReDisp_Mode == 0)
			{
			Seg_Buf[0] = 13;
			Seg_Buf[1] = 14;
			if(MaxF>9999)
				Seg_Buf[3] = MaxF /10000 %10;
			else
				Seg_Buf[3] = 10;
			if(MaxF>999)
				Seg_Buf[4] = MaxF /1000 %10;
			else
				Seg_Buf[4] = 10;
			if(MaxF>99)
				Seg_Buf[5] = MaxF /100 %10;
			else
				Seg_Buf[5] = 10;
			if(MaxF>9)
				Seg_Buf[6] = MaxF /10 %10;
			else
				Seg_Buf[6] = 10;
			Seg_Buf[7] = MaxF % 10;
			}
			else
			{
				Seg_Buf[0] = 13;
				Seg_Buf[1] = 15;
				Seg_Buf[2] = MaxF_Time[0]/16%16;
				Seg_Buf[3] = MaxF_Time[0]%16;
				Seg_Buf[4] = MaxF_Time[1]/16%16;
				Seg_Buf[5] = MaxF_Time[1]%16;
				Seg_Buf[6] = MaxF_Time[2]/16%16;
				Seg_Buf[7] = MaxF_Time[2]%16;
			}
		break;
	}
	
	/* 信息读取函数 */
	Read_Rtc(Rtc);//读时钟

	
	/* 数据处理函数 */
	if(Real_freq <0)
		V_Data = 0;
	if( Real_freq>=0 && Real_freq <= 500)
		V_Data = 1;
	if(Real_freq > PF)
		V_Data = 5;
	if (Real_freq >= 500 && Real_freq < PF)
		V_Data = 1 + ((4.0/(PF - 500)) * (Real_freq - 500));
	Da_Write(V_Data * 51.0);
	
	ucLed[0] = Led_Flag & (!Sys_Mode);
	ucLed[1] = (Led_Flag & (V_Data == 5) || (Real_freq <0)) ;

}
/* 其他显示函数*/
void Led_Proc()
{



}

/* 中断服务函数 */
void Timer1_Server() interrupt 3
{
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;
	if(++Seg_Slow_Down == 100) Seg_Slow_Down = 0;
	if(++Seg_Pos == 8) Seg_Pos = 0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
	Led_Disp(Seg_Pos,ucLed[Seg_Pos]);
	
	if(++Timer_1000ms == 1000)
	{
		Timer_1000ms = 0;
		freq = (TH0 << 8 | TL0) ;
		Real_freq = freq + jiaozhun;
		TH0 = TL0 = 0; 
	}
	
	if(Real_freq > MaxF)
	{
			MaxF = Real_freq;
			MaxF_Time[0] = Rtc[0];  
			MaxF_Time[1] = Rtc[1]; 
			MaxF_Time[2] = Rtc[2];			
	}
		
	if(++Timer_200ms == 200)
	{
		Timer_200ms = 0;
		Led_Flag ^=1;
	}
}


void Timer1_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0xBF;			//定时器时钟12T模式
	TMOD &= 0x0F;			//设置定时器模式
	TL1 = 0x18;				//设置定时初始值
	TH1 = 0xFC;				//设置定时初始值
	TF1 = 0;				//清除TF1标志
	ET1 = 1;				//使能定时器1中断
	TR1 = 1;				//定时器1开始计时
	EA = 1;

}




//频率计数函数
void Timer0_Init(void)		//100微秒@12.000MHz
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TMOD |= 0x05;			//设置定时器模式
	TL0 = 0;				//设置定时初始值
	TH0 = 0;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
}



/* Main */
void main()
{
	Timer0_Init();//计数器初始化
	Timer1_Init();//定时器
	Set_Rtc(Rtc);//设置时间
	
	while(1)
	{
		Seg_Proc();
		Key_Proc();
		Led_Proc();		
	}
}



