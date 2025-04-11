#include <STC15F2K60S2.H>
#include <onewire.h>
#include <Seg.h>
#include <ds1302.h>
#include <Init.h>
#include <Key.h>
#include <Led.h>

unsigned char Seg_Buf[8] = {0,0,0,0,0,0,0,0}, Seg_Pos, Seg_Point[8] = {0,0,0,0,0,0,0,0};
unsigned char Seg_Slow_Down,Key_Slow_Down,Led_Slow_Down;
unsigned char Key_Val,Key_Up,Key_Down,Key_Old;
unsigned char Led_Buf[8] = {0,0,0,0,0,0,0,0};

unsigned char Sys_Mode = 0; //0�¶� 1ʱ�� 2����
bit Work_Mode = 0;

float T_Data;//�¶�
unsigned char Rtc[3] = {0x23,0x59,0x50};//ʱ��
unsigned char T_Set = 23;//�¶Ȳ���

bit T_Flag = 0;
bit Relay_Flag;
bit Led_Flag;
bit Led_Swing_Flag;

unsigned int Timer_5000ms = 0;
unsigned char Timer_100ms = 0;

void Relay(bit a)
{
	if(a)
	{
		P0 = 0x10;
		P2 = P2 & 0x1f |0xa0;
		P2 &= 0x1f;
	}
	else
	{
		P0 = 0x00;//�رռ̵���
		P2 = P2 & 0x1f |0xa0;
		P2 &= 0x1f;
	}
}

void Seg_Reset()
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		Seg_Buf[i] = 16;
	}
}

void Seg_Proc()
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;
	
	T_Data = Read_T();
	Read_Rtc(Rtc);
	
	switch(Sys_Mode)
	{
		case 0:
			Seg_Reset();
			Seg_Buf[0] = 17;
			Seg_Buf[1] = 1;
			Seg_Buf[5] = (unsigned char) T_Data /10 %10;
			Seg_Buf[6] = (unsigned char) T_Data %10;
			Seg_Buf[7] = (unsigned int) (T_Data *10) %10;
			Seg_Point[6] = 1;
		break;
		
		case 1:
			if(T_Flag == 0)
			{
				Seg_Reset();
				Seg_Buf[0] = 17;
				Seg_Buf[1] = 2;
				Seg_Buf[3] = Rtc[0] /16 %16;
				Seg_Buf[4] = Rtc[0]  %16;
				Seg_Buf[5] = 18;
				Seg_Buf[6] = Rtc[1] /16 %16;
				Seg_Buf[7] = Rtc[1]  %16;
				Seg_Point[6] = 0;
			}
			else
			{
				Seg_Reset();
				Seg_Buf[0] = 17;
				Seg_Buf[1] = 2;
				Seg_Buf[3] = Rtc[1] /16 %16;
				Seg_Buf[4] = Rtc[1]  %16;
				Seg_Buf[5] = 18;
				Seg_Buf[6] = Rtc[2] /16 %16;
				Seg_Buf[7] = Rtc[2]  %16;
				Seg_Point[6] = 0;
			}
		break;
		
		case 2:
			Seg_Reset();
			Seg_Buf[0] = 17;
			Seg_Buf[1] = 3;
			Seg_Buf[6] = T_Set / 10 %10;
			Seg_Buf[7] = T_Set %10;
		break;
	}
	
}
	
void Key_Proc()
{
	if(Key_Slow_Down) 
		return;
	Key_Slow_Down = 1;
	
	Key_Val = Key_Read();
	Key_Down = Key_Val &(Key_Old ^ Key_Val);
	Key_Up = ~Key_Val &(Key_Old ^ Key_Val);
	Key_Old = Key_Val;
	
	switch(Key_Down)
	{
		case 12:
			if(++Sys_Mode == 3) Sys_Mode = 0;
		break;
		
		case 16://������
			if(Sys_Mode == 2)
				if(++T_Set >99) 
					T_Set = 99;
		break;
		
		case 17://������ ʱ���������
			if(Sys_Mode == 2)
				if(--T_Set <10)
					T_Set = 10;
		break;
				
		case 13:
			Work_Mode ^= 1;	
			Relay(0);		
		break;
	}
	
	
	if(Key_Old == 17 && Sys_Mode == 1)
		T_Flag = 1;
	else 
		T_Flag = 0;
}

void Led_Proc()
{
	if(Led_Slow_Down) 
		return;
	Led_Slow_Down = 1;
	
	if(Work_Mode == 0)//�̵������ƹ���
	{
		Led_Flag = 0;
		Led_Buf[1] = 1;
		if(T_Data>T_Set)//�¶ȿ���ģʽ
		{
			Relay(1);
		}
		else
		{
			Relay(0);	
		}
		
	}
	else
	{
		Led_Buf[1] = 0;
		if(Rtc[1] == 0 && Rtc[2] == 0 )//ʱ�����ģʽ
		{
			Relay_Flag = 1;		
			Relay(1);
			Led_Buf[0] = 1;//��L1
		}
	}
}
	
	


	void Timer0_Isr(void) interrupt 1	
{
	if(++Seg_Slow_Down == 100) Seg_Slow_Down = 0;
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;
	if(++Led_Slow_Down == 10) Led_Slow_Down = 0;	
	if(++Seg_Pos == 8) Seg_Pos = 0;
	Led_Disp(Seg_Pos,Led_Buf[Seg_Pos]);
	Seg_Disp(Seg_Buf[Seg_Pos],Seg_Pos,Seg_Point[Seg_Pos]);
	
	
	if(Relay_Flag == 1 )//�̵�������ر�
		if(++Timer_5000ms == 5000) 
		{
			Relay_Flag = 0;
			Timer_5000ms = 0;		
			Relay(0);	
			Led_Flag = 0;
			Led_Buf[0] = 0;//�ر�L1
		}
}

	void Timer0_Init(void)		//1����@12.000MHz
{
	AUXR &= 0x7F;			//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;			//���ö�ʱ��ģʽ
	TL0 = 0x18;				//���ö�ʱ��ʼֵ
	TH0 = 0xFC;				//���ö�ʱ��ʼֵ
	TF0 = 0;				//���TF0��־
	TR0 = 1;				//��ʱ��0��ʼ��ʱ
	ET0 = 1;
	EA = 1;
}


void main()
{
	Timer0_Init();
	Sys_Init();
	Write_Rtc(Rtc);
	
	while(1)
	{
		Seg_Proc();
		Key_Proc();
		Led_Proc();
	}
}