#include <REGX52.H>
#include "Key.h"
#include "Seg.h"

/*����������*/
unsigned char Key_val,Key_down,Key_up,Key_old;
unsigned int Key_slowdown, Seg_slowdown;
unsigned char Seg_Pos; //�����ɨ�����
unsigned char Seg_Buf[] = {10,10,10,10,10,10}; //�������ʾ��ֵ���� 10Ϊȫ��
unsigned char Seg_Disp_Mode = 0;//0Ϊ��ʾ 1Ϊʱ������ 2Ϊ�������� 
unsigned char Clock_Disp[3] = {23,59,55};
unsigned int Timer_1000ms = 0;
unsigned char Seg_Point[6] = {0,1,0,1,0,1};
unsigned char Clock_Set[3];
unsigned char Clock_Save[3];
unsigned int Timer_250ms = 0;
unsigned int Seg_Set_Index = 0;
bit Seg_Flag;

/*����������*/
void Key_Proc()
{
	if(Key_slowdown) return;	//���庯��
	Key_slowdown = 1;
	
	Key_val = Key_Read();
	Key_down = Key_val & (Key_val ^ Key_old);
	Key_up = ~Key_val & (Key_val ^ Key_old);
	Key_old = Key_val;
	
	switch(Key_down)
	{
		case 1:
			Clock_Set[0]=Clock_Disp[0];
			Clock_Set[1]=Clock_Disp[1];
			Clock_Set[2]=Clock_Disp[2];		
			Seg_Disp_Mode = 1;
		break;
		case 3:
			if(Seg_Disp_Mode == 1) if(++Seg_Set_Index == 3) Seg_Set_Index = 0;
		break;
		case 5:
			if(Seg_Disp_Mode == 1)
			{
					switch(Seg_Set_Index)
					{
						case 0:
							if(++Clock_Set[0] == 24) Clock_Set[0] = 0;
						break;
						case 1:
							if(++Clock_Set[1] == 60) Clock_Set[1] = 0;
						break;
						case 2:
							if(++Clock_Set[2] == 60) Clock_Set[2] = 0;
						break;
					}
			}
		break;
		case 6:
			if(Seg_Disp_Mode == 1)
			{
					switch(Seg_Set_Index)
					{
						case 0:
							if(--Clock_Set[0] == 0) Clock_Set[0] = 23;
						break;
						case 1:
							if(--Clock_Set[1] == 0) Clock_Set[1] = 59;
						break;
						case 2:
							if(--Clock_Set[2] == 0) Clock_Set[2] = 59;
						break;
					}
			}				
		break;	
		case 7:
			Clock_Disp[0] = Clock_Set[0];
			Clock_Disp[1] = Clock_Set[1];
			Clock_Disp[2] = Clock_Set[2];
			Seg_Disp_Mode = 0;
		break;
		case 8:
			Seg_Disp_Mode = 0;
		break;
	}
		
}

/*����ܴ�����*/
void Seg_Proc()
{
	if(Seg_slowdown) return ;
	Seg_slowdown = 1;
	
	switch(Seg_Disp_Mode)
	{
		case 0://ʱ����ʾ����
			Seg_Buf[0] = Clock_Disp[0]/10%10; 
			Seg_Buf[1] = Clock_Disp[0]%10; 
		
			Seg_Buf[2] = Clock_Disp[1]/10%10; 
			Seg_Buf[3] = Clock_Disp[1]%10;
		
			Seg_Buf[4] = Clock_Disp[2]/10%10; 
			Seg_Buf[5] = Clock_Disp[2]%10;		
		break;
		case 1://ʱ�����ý���
			Seg_Buf[0] = Clock_Set[0]/10%10; 
			Seg_Buf[1] = Clock_Set[0]%10; 
		
			Seg_Buf[2] = Clock_Set[1]/10%10; 
			Seg_Buf[3] = Clock_Set[1]%10;
		
			Seg_Buf[4] = Clock_Set[2]/10%10; 
			Seg_Buf[5] = Clock_Set[2]%10;	
		
			switch(Seg_Set_Index)
			{
				case 0:
					Seg_Buf[0] = Seg_Flag ? Clock_Set[0]/10%10 : 10;
					Seg_Buf[1] = Seg_Flag ? Clock_Set[0]%10 : 10; 	
				break;
				case 1:
					Seg_Buf[2] = Seg_Flag ? Clock_Set[1]/10%10 : 10;
					Seg_Buf[3] = Seg_Flag ? Clock_Set[1]%10 : 10; 
				break;
				case 2:
					Seg_Buf[4] = Seg_Flag ? Clock_Set[2]/10%10 : 10;
					Seg_Buf[5] = Seg_Flag ? Clock_Set[2]%10 : 10;
				break;
			}
	}
}

/*Led������*/
void Led_Proc()
{
	
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

void Timer0Server() interrupt 1
{
	TL0 = 0x18;				
	TH0 = 0xFC;

	if(++Key_slowdown == 10) Key_slowdown = 0;
	if(++Seg_slowdown == 100) Seg_slowdown = 0;
	if(++Seg_Pos == 6) Seg_Pos = 0;
	
	Seg_Disp(Seg_Pos,Seg_Buf[Seg_Pos],Seg_Point[Seg_Pos]);//�������ʾ�������ò���
	
	if(++Timer_1000ms == 1000)//ʱ�Ӽ�ʱ����
	{
		Timer_1000ms = 0;
		Clock_Disp[2]++;
		if(Clock_Disp[2] == 60)
		{
			Clock_Disp[2] = 0;
			Clock_Disp[1]++;
			if(Clock_Disp[1] == 60)
			{
				Clock_Disp[1] = 0;
				Clock_Disp[0]++;
				if(Clock_Disp[0] == 24)
				{
					Clock_Disp[0] = 0;
				}
			}
		}
	}
	
	if(++Timer_250ms == 500) // 500ms������˸
	{
		Timer_250ms = 0;
		Seg_Flag ^= 1;
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