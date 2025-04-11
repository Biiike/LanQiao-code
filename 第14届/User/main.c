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
unsigned char Rtc[3] = {0x13,0x03,0x05};//时间
unsigned int Timer_1000ms,Timer_3000ms = 0 , Timer_2000ms , Timer_100ms;
unsigned int freq;
float V_Data;//电压采集
unsigned char Sys_Mode = 0;//0-时间 1-回显 2-参数 
unsigned char T_Set=30;//温度参数
float T_Data;
float W_Data;
unsigned char EchoDisp = 0;//0温度 1湿度 2时间
unsigned char MaxT,MaxW;//最大变量
float AverT,AverW;//平均变量
unsigned char Trigger_count,Trigger_time[3];
unsigned char Sys_Mode_Old;//原状态
unsigned char flag;
bit Press_flag,Led_flag,Led_error_flag,Led_L6_flag;
bit Led_Warning;
unsigned char newlight,oldlight;//亮暗检测

unsigned char NewData[2],OldData[2];

void Seg_Reset(void)//数码管重置
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		Seg_Point[i] = 0;
		Seg_Buf[i] = 10;
	}
}

/* 键盘处理函数 */
void Key_Proc()
{
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;
	
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Old ^ Key_Val);
	Key_Up =~Key_Val & (Key_Old ^ Key_Val);
	Key_Old = Key_Val;//持续按下
	
	if(EchoDisp == 2)//时间回显界面清空
	{
		if(Key_Down == 9 )
			Press_flag = 1;
		if(Timer_2000ms >=2000 && Key_Up == 9)
		{
			unsigned char q = 0;
			Press_flag = 0;
			AverT = AverW = MaxT = MaxW;
			Trigger_count = 0;
			Timer_2000ms = 0;
			for(q = 0;q<2;q++)
				Trigger_time[q]=0;
		}
	}

	
	switch(Key_Down)
	{
		case 4://界面切换
			if(flag == 0)
			{
				Seg_Reset();
				EchoDisp = 0;
				if(++Sys_Mode >2) Sys_Mode = 0;
				Sys_Mode_Old = Sys_Mode;
			}
		break;
		case 5://回显界面
			if(flag == 0)
			{
				if(Sys_Mode == 1)
				{
					Seg_Reset();
					if(++EchoDisp>2) EchoDisp = 0;
				}
			}
		break;
		case 9:
			if(flag == 0)
			{
				if(Sys_Mode == 2)
					if(--T_Set==255) T_Set = 99;
			}
		break;
		case 8:
			if(flag == 0)
			{
				if(Sys_Mode == 2)
					if(++T_Set==100) T_Set = 0;
			}
		break;
		
	}
		
}

/* 信息处理函数 */
void Seg_Proc()
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;
	
	/* 信息读取函数 */
  Read_Rtc(Rtc);//获取时钟
	
	oldlight = newlight;
	newlight = Ad_Read(0x41) /51.0;
	
	T_Data = Read_T();//获取温度
	
	if(T_Data<0)//温度合法判断
		T_Data = 0;
	else if (T_Data>99)
		T_Data = 99;
	
	/* 数据处理函数 */
	if(freq<200 || freq>2000) //湿度处理
		W_Data = 0;
	else
		W_Data = (freq-200) * 2/45 +10;
	
	if((oldlight>2.5) && (newlight<2.5) && (flag ==0))//亮暗变化
	{
		NewData[0] = T_Data;
		NewData[1] = W_Data;
		if(Trigger_count >= 2)
		{
			if((NewData[0]>OldData[0]) && (NewData[1]>OldData[1]))
				Led_L6_flag = 1;
			else
				Led_L6_flag = 0;
		}
		OldData[0] = NewData[0];
		OldData[1] = NewData[1];
		
		if(W_Data == 0)
		{
		}
		else if(++Trigger_count==100)//数据合法时count++
				Trigger_count = 99;		
		flag = 1;
		Read_Rtc(Trigger_time); //数据合法时读时间
	}
	else if (flag == 1 && Timer_3000ms>=3000)
		flag = 0;
	

	if(flag)//如果触发
	{
		Seg_Buf[0] = 18;
		Seg_Buf[1] = 10;
		Seg_Buf[2] = 10;
		Seg_Buf[3] = (unsigned char) T_Data / 10 % 10;
		Seg_Buf[4] = (unsigned char) T_Data%10;
		Seg_Buf[5] = 11;
		if(W_Data == 0)//数据不合法
		{
			Led_error_flag = 0;
			Seg_Buf[6] = 14;
			Seg_Buf[7] = 14;
		}
		else//数据合法
		{
			Led_error_flag = 1;
			Seg_Buf[6] = (unsigned char) W_Data/10%10;
			Seg_Buf[7] = (unsigned char) W_Data%10;

			if(T_Data > T_Set)//用于Led合法时亮暗辅助
				Led_flag = 1;
			else
				Led_flag = 0;
					
			MaxT = (T_Data>MaxT)?T_Data:MaxT;
			MaxW = (W_Data>MaxW)?W_Data:MaxW;
			AverW = (AverW * (Trigger_count -1 )+W_Data)/Trigger_count;
			AverT = (AverT * (Trigger_count -1 )+T_Data)/Trigger_count;
		}
		Seg_Point[6] = 0;
	}
	else
	{
		switch(Sys_Mode)
		{
			case 0://时间界面
				Seg_Buf[0] = Rtc[0] /16%16;
				Seg_Buf[1] = Rtc[0]  %16;
				Seg_Buf[2] = 11;
				Seg_Buf[3] = Rtc[1] /16 %16;
				Seg_Buf[4] = Rtc[1] %16;
				Seg_Buf[5] = 11;
				Seg_Buf[6] = Rtc[2] /16%16;
				Seg_Buf[7] = Rtc[2] %16;
				Seg_Point[6] = 0;

			break;
			case 1://回显界面
				if(Sys_Mode == 1)
				{
					if(EchoDisp == 0)//温度回显
					{
						Seg_Reset();
						Seg_Buf[0] = 15;
						if(Trigger_count != 0)
						{
							Seg_Buf[1] = 10;
							Seg_Buf[2] = MaxT/10%10;
							Seg_Buf[3] = MaxT%10;
							Seg_Buf[4] = 11;
							Seg_Buf[5] = (unsigned char)AverT/10%10;
							Seg_Buf[6] = (unsigned char)AverT%10;
							Seg_Buf[7] = (unsigned char)(AverT*10)%10;
							Seg_Point[6] = 1;							
						}
					}
					
					if(EchoDisp == 1)//湿度回显
					{
						Seg_Reset();
						Seg_Buf[0] = 16;
						if(Trigger_count != 0)
						{
							Seg_Buf[1] = 10;
							Seg_Buf[2] = MaxW/10%10;
							Seg_Buf[3] = MaxW%10;
							Seg_Buf[4] = 11;
							Seg_Buf[5] = (unsigned char)AverW/10%10;
							Seg_Buf[6] = (unsigned char)AverW%10;
							Seg_Buf[7] = (unsigned char)(AverW*10)%10;	
							Seg_Point[6] = 1;
						}
					}
						
					if(EchoDisp == 2)//时间回显
					{
						Seg_Reset();
						Seg_Point[6] = 0;
						Seg_Buf[0] = 17;
						Seg_Buf[1] = Trigger_count /10 %10;
						Seg_Buf[2] = Trigger_count%10;
						if(Trigger_count !=0)
						{
							Seg_Buf[3] = Trigger_time[0]/16%16;
							Seg_Buf[4] = Trigger_time[0]%16;
							Seg_Buf[5] = 11;
							Seg_Buf[6] = Trigger_time[1]/16%16;
							Seg_Buf[7] = Trigger_time[1]%16;
						}						
					}
				}
			break;
			case 2://参数设置
				Seg_Buf[0] = 12;
				Seg_Buf[1] = 10;
				Seg_Buf[2] = 10;
				Seg_Buf[3] = 10;
				Seg_Buf[4] = 10;
				Seg_Buf[5] = 10;
				Seg_Buf[6] = T_Set/10%10;
				Seg_Buf[7] = T_Set%10;
				Seg_Point[6] = 0;
			break;		
		}
	}
	
}

/* 其他显示函数*/
void Led_Proc()
{

	ucLed[0] = (Sys_Mode == 0);
	ucLed[1] = (Sys_Mode == 1);
	ucLed[2] = (Sys_Mode == 2);
	ucLed[3] = (Led_flag)?Led_Warning:0;
	ucLed[4] = Led_error_flag?1:0;
	ucLed[5] = Led_L6_flag?1:0;	
}

/* 中断服务函数 */
void Timer1_Server() interrupt 3
{
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;
	if(++Seg_Slow_Down == 20) Seg_Slow_Down = 0;
	if(++Seg_Pos == 8) Seg_Pos = 0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
	Led_Disp(Seg_Pos,ucLed[Seg_Pos]);
	
	if(flag)//用于亮暗切换
	{
		if(++Timer_3000ms >=3000)
			Timer_3000ms =3001;
	}
	else
		Timer_3000ms = 0;
	
	if(Press_flag == 1)//用于长按
	{
		if(++Timer_2000ms >2000)
			Timer_2000ms = 2001;
	}
	else
		Timer_2000ms = 0;
	
	if(Led_flag)
	{		
		if(++Timer_100ms >= 100)//用于灯闪烁
		{
			Timer_100ms = 0;
			Led_Warning ^= 1; 
		}
	}
	
	if(++Timer_1000ms == 1000)// 获取频率
	{
		Timer_1000ms = 0;
		freq = TH0 << 8 | TL0;
		TH0 = TL0 = 0;
	}
}


/* 定时器1初始化函数 */
void Timer1_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0xBF;			//定时器时钟12T模式
	TMOD &= 0x0F;			//设置定时器模式
	TL1 = 0x18;				//设置定时初始值
	TH1 = 0xFC;				//设置定时初始值
	TF1 = 0;				//清除TF0标志
	TR1 = 1;				//定时器0开始计时
	ET1 = 1;				//中断定时器0打开
	EA = 1;					//总中断打开
}

/*计数器初始化函数*/
void Timer0_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TMOD |= 0x05; 			//设置定时器0，技术模式
	TL0 = 0x00;				//设置定时初始值
	TH0 = 0x00;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
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


/* Main */
void main()
{
	System_Init();//系统初始化（关闭灯，蜂鸣器）
	Timer1_Init();//定时器初始化
	Timer0_Init();
	Set_Rtc(Rtc);//设置时间
	Delay750ms();
	Read_T();
	
	
	while(1)
	{
		Seg_Proc();
		Key_Proc();
		Led_Proc();		
	}
}



