/*ͷ�ļ���������*/
#include <REGX52.H>
#include "Key.h"
#include "Seg.h"


/*����������*/
unsigned char Key_SlowDown;
unsigned char Key_Val,Key_Up,Key_Down,Key_Old;
unsigned char Seg_SlowDown;
unsigned char Seg_Pos;//�����ɨ�����
unsigned char Seg_Buf[] = {10,10,10,10,10,10} ;//�������ʾ���ݴ�ű���
unsigned int Timer_1000ms;
unsigned char Time_Count = 30;
unsigned char Seg_Mode = 0; //��ʾ����ģʽ 0Ϊ��ʾ 1Ϊ����
unsigned char System_Flag ; //1Ϊ��ʼ 2Ϊ��λ 3Ϊ�л� 4Ϊ����
unsigned char Set_Date[] = {15,30,60};//���ò���
unsigned char Set_Date_Index = 1;
unsigned int Seg_Flicker500ms; 
bit Seg_FlickerMode;


/*����������*/
void Key_Proc()
{
	if(Key_SlowDown) return;
	Key_SlowDown = 1; //��������
	
	Key_Val = Key_Read();
	Key_Down = Key_Val & (Key_Val^Key_Old);//����½�
	Key_Up = ~Key_Val & (Key_Val^Key_Old);//�������
	Key_Old = Key_Val;
	
	switch(Key_Down)
	{
		case 1://1���� ��ʼ
				if(Seg_Mode == 0) System_Flag = 1; 
		break;
		case 2://2���� ��λ
				if(Seg_Mode == 0) Time_Count = Set_Date[Set_Date_Index];
		break;
		case 3://3���� �л�
				if(Seg_Mode == 1) Time_Count = Set_Date[Set_Date_Index]; //�������õĲ���
				Seg_Mode ^= 1 ;//��Seg_Mode = Seg_Mode^1 
		break;
		case 4://4���� ����
				if(Seg_Mode == 1)
				{
					if(++Set_Date_Index == 3) Set_Date_Index =0;//���ò���		
				}
	}
}

/*��Ϣ������*/



void Seg_Proc()
{
	if(Seg_SlowDown) return;
	Seg_SlowDown = 1;//����ܼ���
	
	Seg_Buf[0] = Seg_Mode + 1;
	if(Seg_Mode == 0)//ϵͳ����ʾ����
	{
		Seg_Buf[4] = Time_Count / 10 %10;
		Seg_Buf[5] = Time_Count % 10;
	}
	else//ϵͳ�����ý���
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

/*������ʾ����*/
void Led_Proc()
{
	if(Time_Count == 0) 
	{
		P1 = 0x00;//��ȫ��
		P2_3 = 0;//������ʹ��
	}
	else
	{
		P1 = 0xff;
		P2_3 = 1;
	}
}

/*��ʱ��0��ʼ��*/
void Timer0_Init(void)		//1����@12.000MHz
{	
	TMOD &= 0xF0;			//���ö�ʱ��ģʽ
	TMOD |= 0x01;			//���ö�ʱ��ģʽ
	TL0 = 0x18;				//���ö�ʱ��ʼֵ
	TH0 = 0xFC;				//���ö�ʱ��ʼֵ
	TF0 = 0;					//���TF0��־
	TR0 = 1;					//��ʱ��0��ʼ��ʱ
	ET0 = 1;	
	EA = 1;	
}

/*��ʱ��0�жϷ�����*/
void Timer0Server() interrupt 1
{
	TL0 = 0x18;				
	TH0 = 0xFC;

	if(++Key_SlowDown == 10) Key_SlowDown = 0;
	if(++Seg_SlowDown == 500) Seg_SlowDown = 0;
	if(++Seg_Pos==6) Seg_Pos=0;
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos]);//�������ʾģ��
	
	if(System_Flag == 1) //����ʱģ��
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











