/* ͷ�ļ������� */
#include <STC15F2K60S2.H>
#include <Led.h>
#include <Seg.h>
#include <Key.h>
#include <intrins.h>
#include <onewire.h>




/* ����������*/
unsigned char Key_Up,Key_Down,Key_Old,Key_Val;
unsigned int Key_Slow_Down,Seg_Slow_Down;
unsigned char Seg_Pos;
unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0};

unsigned char Sys_Mode; //0�¶� 1У׼ 2����

float T_Data;
char Adjust_Data = 0;
char canshu = 26;

bit Trigger = 0; //0Ϊ�� 1Ϊ��
 
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


void Seg_Reset()
{
	unsigned char i = 0;
	for(i = 0; i < 8; i++)
	{
		Seg_Buf[i] = 10;
		Seg_Point[i] = 0;
	}
}

/* ���̴����� */
void Key_Proc()
{
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;
	
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Val ^ Key_Old);
	Key_Up =~Key_Val & (Key_Val ^ Key_Old);
	Key_Old = Key_Val;//��������
	
	switch(Key_Down)
	{
		case 4:
			Seg_Reset();
			if(++Sys_Mode >2)
				Sys_Mode = 0;
		break;
		case 8:
			if(Sys_Mode == 1)
			{
				Adjust_Data -= 1;
				if(Adjust_Data < -99) Adjust_Data = -99;
			}
			if(Sys_Mode == 2)
			{
				canshu -= 1;
				if(canshu < -99) canshu = -99;
			}
		break;
		case 9:
			if(Sys_Mode == 1)
			{
				Adjust_Data += 1;
				if(Adjust_Data > 99) Adjust_Data = 99;
			}
			if(Sys_Mode == 2)
			{
				canshu += 1;
				if(canshu > 99) canshu = 99;
			}
		break;
		case 5:
			Trigger ^= 1;
		break;
	}
	
}

/* ��Ϣ������ */
void Seg_Proc()
{
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;
	
	switch(Sys_Mode)
	{
		case 0:
			Seg_Buf[0] = 12;
			if(T_Data <10)
				Seg_Buf[5] = 10;
			else
				Seg_Buf[5] = (unsigned char) T_Data / 10 %10;
			Seg_Buf[6] = (unsigned char) T_Data % 10;
			Seg_Buf[7] = (unsigned int) (T_Data * 10) % 10;
			Seg_Point[6] = 1;
			break;
		case 1:
			Seg_Buf[0] = 13;
			if(Adjust_Data >= 0 && Adjust_Data<10)
			{
				Seg_Buf[7] = Adjust_Data;
				Seg_Buf[6] = 10;
			}
			if(Adjust_Data>=10)
			{
				Seg_Buf[6] = Adjust_Data / 10 % 10;
				Seg_Buf[7] = Adjust_Data % 10;
			}
			if(Adjust_Data < 0 && Adjust_Data > -10)
			{
				Seg_Buf[7] = (-Adjust_Data);
				Seg_Buf[6] = 11;
				Seg_Buf[5] = 10;

			}
			if(Adjust_Data <= -10)
			{
				Seg_Buf[6] = (-Adjust_Data) / 10 % 10;
				Seg_Buf[7] = (-Adjust_Data) % 10;
				Seg_Buf[5] = 11;
			}
			break;
		case 2:
			Seg_Buf[0] = 14;
			if(canshu >= 0 && canshu<10)
			{
				Seg_Buf[7] = canshu;
				Seg_Buf[6] = 10;
			}
			if(canshu>=10)
			{
				Seg_Buf[6] = canshu / 10 % 10;
				Seg_Buf[7] = canshu % 10;
			}
			if(canshu < 0 && canshu > -10)
			{
				Seg_Buf[7] = (-canshu);
				Seg_Buf[6] = 11;
				Seg_Buf[5] = 10;

			}
			if(canshu <= -10)
			{
				Seg_Buf[6] = (-canshu) / 10 % 10;
				Seg_Buf[7] = (-canshu) % 10;
				Seg_Buf[5] = 11;
			}
		break;
	}
	/* ��Ϣ��ȡ���� */


	
	/* ���ݴ����� */
	T_Data = Read_T() + Adjust_Data;

}
/* ������ʾ����*/
void Led_Proc()
{
	ucLed[0] = (Sys_Mode == 0) ? 1 : 0;
	ucLed[1] = (Sys_Mode == 1) ? 1 : 0;
	ucLed[2] = (Sys_Mode == 2) ? 1 : 0;
	ucLed[3] = Trigger == 0 ? 1 : 0;
	ucLed[4] = Trigger == 1 ? 1 : 0;

	ucLed[7] = (Trigger == 1 && T_Data < canshu) || (Trigger == 0 && T_Data > canshu) ? 1 : 0;
}

/* �жϷ����� */
void Timer0_Server() interrupt 1
{
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;
	if(++Seg_Slow_Down == 100) Seg_Slow_Down = 0;
	if(++Seg_Pos == 8) Seg_Pos = 0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
	Led_Disp(Seg_Pos,ucLed[Seg_Pos]);
}


/* ��ʱ����ʼ������ */
void Timer0_Init(void)		//1����@12.000MHz
{
	AUXR &= 0x7F;			//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;			//���ö�ʱ��ģʽ
	TL0 = 0x18;				//���ö�ʱ��ʼֵ
	TH0 = 0xFC;				//���ö�ʱ��ʼֵ
	TF0 = 0;				//���TF0��־
	TR0 = 1;				//��ʱ��0��ʼ��ʱ
	ET0 = 1;				//�ж϶�ʱ��0��
	EA = 1;					//���жϴ�
}


/* Main */
void main()
{
	Timer0_Init();//��ʱ����ʼ��
	Read_T();
	Delay750ms();
	Read_T();
	
	while(1)
	{
		Seg_Proc();
		Key_Proc();
		Led_Proc();		
	}
}



