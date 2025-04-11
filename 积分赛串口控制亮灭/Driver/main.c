/***************************************************************************************
* Copyright (C) 2025 �״׵��ӹ�����
*
* �汾         ��V4T
* ����         ���״׵��ӹ�����
* ����         ���ð汾ΪV2�汾�ĸĽ��棬���4Tƽ̨����ר���Ż����޸��˴�ǰ�汾������Bug
*
* ��Ȩ���״׵��ӹ��������У�����������Ȩ����
* δ����ȷ������ɣ��κθ��˻������������������ҵ��;��
***********************************************************************************************/

/* ͷ�ļ������� */
#include <STC15F2K60S2.H>//��Ƭ���Ĵ���ר��ͷ�ļ�
#include <Init.h>//��ʼ���ײ�����ר��ͷ�ļ�
#include <Led.h>//Led�ײ�����ר��ͷ�ļ�
#include <Key.h>//�����ײ�����ר��ͷ�ļ�
#include <Seg.h>//����ܵײ�����ר��ͷ�ļ�
#include <Uart.h>//���ڵײ�����ר��ͷ�ļ�
#include <string.h>
/* ���������� */
unsigned char Key_Val,Key_Down,Key_Old,Key_Up;//����ר�ñ���
unsigned char Key_Slow_Down;//��������ר�ñ���
unsigned char Seg_Buf[8] = {10,10,10,10,10,10,10,10};//�������ʾ���ݴ������
unsigned char Seg_Pos;//�����ɨ��ר�ñ���
idata unsigned int Seg_Slow_Down;//����ܼ���ר�ñ���
idata unsigned char ucLed[8] = {0,0,0,0,0,0,0,0};//Led��ʾ���ݴ������
idata unsigned char Uart_Slow_Down;//���ڼ���ר�ñ���
idata unsigned char Uart_Recv[10];//���ڽ������ݴ������� Ĭ��10���ֽ� ���������ݽϳ� �ɸ�������ֽ���
idata unsigned char Uart_Recv_Index;//���ڽ�������ָ��
idata unsigned char Uart_Send[10];//���ڽ������ݴ������� Ĭ��10���ֽ� ���������ݽϳ� �ɸ�������ֽ���
/* �������� */
pdata unsigned char Uart_Buf[10] = {0}; // ���ڽ��ջ�����
idata unsigned char Uart_Rx_Index;      // ���ڽ�������
idata unsigned char Uart_Recv_Tick;     // ���ڽ���ʱ���־
idata unsigned char Uart_Rx_Flag;
/* ���̴����� */
void Key_Proc()
{
	if(Key_Slow_Down<10) return;
	Key_Slow_Down = 0;//���̼��ٳ���

	Key_Val = Key_Read();//ʵʱ��ȡ����ֵ
	Key_Down = Key_Val & (Key_Old ^ Key_Val);//��׽�����½���
	Key_Up = ~Key_Val & (Key_Old ^ Key_Val);//��׽�����Ͻ���
	Key_Old = Key_Val;//����ɨ�����

}

/* ��Ϣ������ */
void Seg_Proc()
{
	if(Seg_Slow_Down<500) return;
	Seg_Slow_Down = 0;//����ܼ��ٳ���

}

/* ������ʾ���� */
void Led_Proc()
{
	
}

/* ���ڴ����� */
void Uart_Proc()
{
	if(Uart_Slow_Down<10)return;
	Uart_Slow_Down=0;
	
  if (Uart_Rx_Index == 0)
    return; // �����ݣ�ֱ�ӷ���
  if (Uart_Recv_Tick >= 10)
  { // �����ճ�ʱ�������ݶ�ȡ����ջ�����
    Uart_Recv_Tick = 0;
    Uart_Rx_Flag = 0;
    memset(Uart_Buf, 0, Uart_Rx_Index); // ��ս�������
    Uart_Rx_Index = 0;
  }
}

/* ��ʱ��0�жϳ�ʼ������ */
void Timer0Init(void)		//1����@12.000MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x18;		//���ö�ʱ��ʼֵ
	TH0 = 0xFC;		//���ö�ʱ��ʼֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;    //��ʱ���ж�0��
	EA = 1;     //���жϴ�
}

/* ��ʱ��0�жϷ����� */
void Timer0Server() interrupt 1
{  
	Key_Slow_Down ++;
	Seg_Slow_Down ++;
	Uart_Slow_Down++;
	
	if(++Seg_Pos == 8) Seg_Pos = 0;//�������ʾר��
	 // �������ʾ����
  if (Seg_Buf[Seg_Pos] > 20)
    Seg_Disp(Seg_Pos, Seg_Buf[Seg_Pos] - ',', 1); // ��С����
  else
    Seg_Disp(Seg_Pos, Seg_Buf[Seg_Pos], 0); // ��С����
   Led_Disp(ucLed); 
}

/* ����1�жϷ����� */
void Uart1Server() interrupt 4
{
	if(RI == 1) //���ڽ�������
	{
		Uart_Recv[Uart_Recv_Index] = SBUF;
		Uart_Recv_Index++;
		RI = 0;
	}
}

/* Main */
void main()
{
	System_Init();
	Timer0Init();
	UartInit();
	while (1)
	{
		Key_Proc();
		Seg_Proc();
		Led_Proc();
		Uart_Proc();
	}
}