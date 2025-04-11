/*头文件声明区域*/
#include <STC15F2K60S2.H>
#include <Init.h>
#include <Key.h>
#include <Seg.h>
#include <Led.h>
#include <onewire.h>

/*变量声明区域*/
unsigned char Key_slow_down,Key_val,Key_down,Key_up,Key_old;
unsigned char Seg_slow_down;
unsigned char Seg_pos,Seg_buf[8] = {10,10,10,10,10,10,10,10},Seg_point[8] = {0,0,0,0,0,0,0,0};
unsigned char ucLed[8] = {0,1,0,1,0,1,0,1};
float T;
unsigned char Seg_Disp_Mode; //0温度 1温度设置 2精度设置
unsigned char P_disp[2] = {30,20},P_real[2] = {30,20};
unsigned P_disp_index;
bit Seg_flag;
unsigned int Time_500ms;
unsigned int Key_time = 0;
bit Key_flag;

/*按键处理函数*/
void Key_Proc()
{
	static unsigned int count = 0;
	if(Key_slow_down) return;
	Key_slow_down = 1;
	
	Key_val = Key_Read();
	Key_down = Key_val & (Key_val ^Key_old);
	Key_up = ~Key_val & (Key_val ^Key_old);
	Key_old = Key_val;
	
	if(Seg_Disp_Mode ==1)
	{
		if(Key_down == 14 || 15)
			Key_flag = 1;	
	}
	if(Key_time < 500)
	{
		if(Key_up == 14)
		{
			Key_flag = Key_time = 0;
			if(++P_disp[P_disp_index] == 71) 
				P_disp[P_disp_index] = 10;
		}
		if(Key_up == 15)
		{
			Key_flag = Key_time = 0;
			if(--P_disp[P_disp_index] == 9) 
				P_disp[P_disp_index] = 70;
		}		
	}
	else
		{
			count++;
			if(count == 25)
			{
				count = 0;
				if(Key_old == 14)
				{
					if(++P_disp[P_disp_index] == 71) 
						P_disp[P_disp_index] = 10;
				}
				if(Key_up == 14)
				{
					Key_flag = Key_time = 0;
				}
				if(Key_old == 15)
				{
					if(--P_disp[P_disp_index] == 9) 
						P_disp[P_disp_index] = 70;
				}
				if(Key_up == 15)
				{
					Key_flag = Key_time = 0;
				}
			}
		}
		
	switch (Key_down)
	{
		case 12:
			if(++Seg_Disp_Mode == 2) Seg_Disp_Mode = 0;
		break;
		
		case 13:
			if(Seg_Disp_Mode == 1)
				P_disp_index ^= 1;
		break;
			
	}
	
}

/*信息处理函数*/
void Seg_Proc()
{
	if(Seg_slow_down) return;
	Seg_slow_down = 1;
	
	/*信息获取函数*/
	T = Read_t();
	
	/*数据显示函数*/
	switch(Seg_Disp_Mode)
	{
		case 0:
			Seg_buf[0] = 11;
			Seg_buf[3] = 10;
			Seg_buf[4] = (unsigned char) T / 10 % 10;
			Seg_buf[5] = (unsigned char) T % 10;
			Seg_buf[6] = (unsigned int) (T * 10) %10;
			Seg_buf[7] = 11;
			Seg_point[5] = 1;
		break;
		case 1:
			Seg_buf[0] = 12;
			Seg_buf[3] = P_disp[0] /10%10;
			Seg_buf[4] = P_disp[0] %10;
			Seg_buf[5] = 13;
			Seg_buf[6] = P_disp[1] /10%10;
			Seg_buf[7] = P_disp[1] %10;
			Seg_point[5] = 0;
			if(P_disp_index == 0)
			{
				Seg_buf[3] = Seg_flag?P_disp[0] /10%10:10;
				Seg_buf[4] = Seg_flag?P_disp[0] %10:10;
			}
			else
			{
				Seg_buf[6] = Seg_flag?P_disp[1] /10%10:10;
				Seg_buf[7] = Seg_flag?P_disp[1] %10:10;
			}
		break;
	}

	
}

/*Led处理函数*/
void Led_Proc()
{
	
}


/*定时器0初始化*/
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

/*定时器0中断函数*/
void Timer0_Isr(void) interrupt 1
{
	if(++Key_slow_down == 10) Key_slow_down = 0;
	if(++Seg_slow_down == 500) Seg_slow_down = 0;
	if(++Seg_pos == 8) Seg_pos = 0;
	
	Seg_Disp(Seg_pos,Seg_buf[Seg_pos],Seg_point[Seg_pos]);
	
	Led_Disp(Seg_pos,ucLed[Seg_pos]);
	
	if(++Time_500ms == 500) 
	{
		Time_500ms = 0;
		Seg_flag ^= 1;
	}
	if(Key_flag == 1)
	{
		Key_time++;
		if(Key_time == 600)
			Key_time = 599;
	}
}

void main()
{
	Sys_Init();
	Timer0_Init();
	while(1)
	{
		Key_Proc();
		Seg_Proc();
		Led_Proc();
		
	}
}