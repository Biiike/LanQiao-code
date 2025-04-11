/* ͷ�ļ������� */
#include <STC15F2K60S2.H>
#include <Init.h>
#include <Led.h>
#include <Seg.h>
#include <Key.h>
#include <intrins.h>
#include <ds1302.h>
#include <iic.h>
#include <onewire.h>



/* ����������*/
unsigned char Key_Up,Key_Down,Key_Old,Key_Val;
unsigned int Key_Slow_Down,Seg_Slow_Down;
unsigned char Seg_Pos;
unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};
unsigned char UcLed[8] = {0,0,0,0,0,0,0,0};
unsigned char Rtc[3] = {0x23,0x59,0x55};//ʱ��

unsigned char Sys_Mode = 0;//0-�¶� 1-���� 2-DAC
float T;
unsigned char T_Data_Disp = 25;
unsigned char T_Data = 25;//����Ĭ��ֵΪ25��
float V_Data;
float V_Data_Disp;

unsigned char Mode;

/* ���̴����� */
void Key_Proc()
{
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;
	
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Old ^ Key_Val);
	Key_Up =~Key_Val & (Key_Old ^ Key_Val);
	Key_Old = Key_Val;//��������
	
	

	switch(Key_Down)
	{
		case 4:
			if(++Sys_Mode >2) Sys_Mode = 0;
			if(Sys_Mode != 1)	T_Data = T_Data_Disp;
		break;
		case 8:
			if(Sys_Mode == 1)
			{
				if(--T_Data_Disp>100) T_Data_Disp = 0;
			}
		break;
		case 9:
			if(Sys_Mode == 1)
			{
				if(++T_Data_Disp==255) T_Data_Disp = 100;
			}
		break;
		case 5:
		Mode ^= 1;
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
		case 0://�¶Ƚ���
			Seg_Buf[0] = 12;
			Seg_Buf[1] = 16;
			Seg_Buf[2] = 16;
			Seg_Buf[3] = 16;
			Seg_Buf[4] = (unsigned char) T /10 % 10;
			Seg_Buf[5] = (unsigned char) T % 10;
			Seg_Buf[6] = (int)(T* 10)% 10;
			Seg_Buf[7] = (int)(T* 100)% 10;
			Seg_Point[5] = 1;
		break;
		case 1://��������
			Seg_Buf[0] = 17;
			Seg_Buf[1] = 16;
			Seg_Buf[2] = 16;
			Seg_Buf[3] = 16;
			Seg_Buf[4] = 16;
			Seg_Buf[5] = 16;
			Seg_Buf[6] = (T_Data_Disp /10) % 10;
			Seg_Buf[7] = T_Data_Disp % 10;
			Seg_Point[5] = 0;
		break;
		case 2://�������
			if(Mode == 0)//ģʽһ
			{
				if(T<T_Data)
					V_Data = 0;
				else if (T>T_Data)
					V_Data = 5;
			}
			else//ģʽ��
			{
				if(T<20)
					V_Data = 1;
				else if (T>20 && T<40)
					V_Data = 1+((T-20)*0.15);
				else if (T>40)
					V_Data = 4;
			}
					
			Seg_Buf[0] = 10;
			Seg_Buf[1] = 16;
			Seg_Buf[2] = 16;
			Seg_Buf[3] = 16;
			Seg_Buf[4] = 16;
			Seg_Buf[5] = (unsigned char)V_Data%10;
			Seg_Buf[6] = (unsigned int)(V_Data*100)/10 % 10;
			Seg_Buf[7] = (unsigned int)(V_Data*100) % 10;
			Seg_Point[5] = 1;

		break;
	}
	
	/* ��Ϣ��ȡ���� */
	Da_Write(V_Data * 50.1);
	T = Read_T();
	
	/* ���ݴ����� */

}
/* ������ʾ����*/
void Led_Proc()
{

	UcLed[0] = !Mode;
	
		if(Sys_Mode == 0)
			UcLed[1] = 1;
		else
			UcLed[1] = 0;

		if(Sys_Mode == 1)
			UcLed[2] = 1;
		else
			UcLed[2] = 0;

		if(Sys_Mode == 2)
			UcLed[3] = 1;
		else
			UcLed[3] = 0;
	
}

/* �жϷ����� */
void Timer0_Server() interrupt 1
{
	if(++Key_Slow_Down == 10) Key_Slow_Down = 0;
	if(++Seg_Slow_Down == 100) Seg_Slow_Down = 0;
	if(++Seg_Pos == 8) Seg_Pos = 0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);
	Led_Disp(Seg_Pos,UcLed[Seg_Pos]);

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
	System_Init();//ϵͳ��ʼ�����رյƣ���������
	Timer0_Init();//��ʱ����ʼ��
//	Set_Rtc(Rtc);����ʱ��
	
	while(1)
	{
		Seg_Proc();
		Key_Proc();
		Led_Proc();		
	}
}



