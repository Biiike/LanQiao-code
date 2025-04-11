/* ͷ�ļ������� */
#include <Init.h>
#include <Led.h>
#include <Seg.h>
#include <Key.h>
#include <intrins.h>
#include <ds1302.h>
#include <string.h> //����memcpy(Set_Dat,ucDate,9) ��Set_Dat��9Ϊ���ݸ��Ƶ�ucDate��
#include <STC15F2K60S2.H>


/* ����������*/
unsigned char Key_Up,Key_Down,Key_Old,Key_Val;
unsigned int Key_Slow_Down,Seg_Slow_Down;
unsigned char Seg_Pos;
unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0};
unsigned char ucRtc[3] = {0x23,0x59,0x55};// ʱ�����ݴ������
unsigned char Seg_Disp_Mode = 0; // 0-ʱ����ʾ 1-������ʾ 2-������ʾ 3-ʱ������ 4-�������� 5-��������
unsigned char ucAlarm[9] = {0x00,0x00,0x00,0x00,0x01,0x00,0xbb,0xbb,0xbb};//0xbb��ʾΪ-
unsigned char Alarm_Index;
unsigned char ucDate[3] = {0x22,0x12,0x12};//�������ݴ������
unsigned char Set_Dat[9];//�������ݴ������

/* ���̴����� */
void Key_Proc()
{
	if(Key_Slow_Down) return;
	Key_Slow_Down = 1;
	
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Val ^ Key_Old);
	Key_Up =~Key_Val & (Key_Val ^ Key_Old);
	Key_Old = Key_Val;
	
	switch(Key_Down)
	{
		case 14:
			if(Seg_Disp_Mode < 3)
			{
				if(++Seg_Disp_Mode == 3) Seg_Disp_Mode = 0;
			}
		break;
		case 15:
			if(Seg_Disp_Mode < 3)
			{
				Seg_Disp_Mode += 3;
				Alarm_Index = 0;
				switch(Seg_Disp_Mode)
				{
					case 3://ʱ�����ý���
						memcpy(Set_Dat,ucRtc,9);//��ʱ�����ݸ�����������
					break;
					case 4://�������ý���
						memcpy(Set_Dat,ucAlarm,9);//���������ݸ�����������
					break;
					case 5://�������ý���
						memcpy(Set_Dat,ucDate,9);
					break;
				}
			}
		}
		
}

/* ��Ϣ������ */
void Seg_Proc()
{
	unsigned char i;
	if(Seg_Slow_Down) return;
	Seg_Slow_Down = 1;
	
	/* ��Ϣ��ȡ���� */
	Read_Rtc(ucRtc);//��ȡʱ��ʵʱ����
	Read_Date(ucDate);//��ȡ����ʵʱ����
	
	/* ���ݴ����� */
	switch(Seg_Disp_Mode)
	{
		case 0 ://ʱ����ʾ
			for(i=0;i <3;i++)
				{
					Seg_Buf[3*i] = ucRtc[i] /16;
					Seg_Buf[3*i+1] = ucRtc[i] %16;
				}
				Seg_Buf[2] = Seg_Buf[5] = 11;
		break;		
		case 1://������ʾ
			for(i=0;i<3;i++)
			{
				Seg_Buf[3*i] = ucAlarm[3*Alarm_Index+i] /16;
				Seg_Buf[3*i+1] = ucAlarm[3*Alarm_Index+i] %16;
			}
			Seg_Buf[2] = Seg_Buf[5] = 11;		
		break;
		case 2://������ʾ
			for(i=0;i<3;i++)
		
				{
					Seg_Buf[3*i] = ucDate[i] /16;
					Seg_Buf[3*i+1] = ucDate[i] %16;
				}
			Seg_Buf[2] = Seg_Buf[5] = 11;
		break;
		default:
			for(i=0;i <3;i++)
				{
					Seg_Buf[3*i] = Set_Dat[i+Alarm_Index*3] /16;
					Seg_Buf[3*i+1] = Set_Dat[i+Alarm_Index*3] %16;
				}
				Seg_Buf[2] = Seg_Buf[5] = 11;	
	}
			
}
/* ������ʾ����*/
void Led_Proc()
{
	
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
	System_Init();
	Timer0_Init();
	Set_Rtc(ucRtc);
	Set_Date(ucDate);
	
	while(1)
	{
		Seg_Proc();
		Key_Proc();
		Led_Proc();		
	}
}
