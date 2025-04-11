#include <ultrasound.h>

void Delay12us(void)	//@12.000MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}


void Ut_Wave_Send() //����8��ռ�ձ�Ϊ50%�ķ����ź�
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		Tx = 1;
		Delay12us();
		Tx = 0;
		Delay12us();
	}
}

unsigned int Ut_Wave_Receive()
{
	unsigned int time;
	TMOD &= 0x0f;
	TH1=TL1=0;
	TR1=1;
	
	while((Rx==1) && (TF1==0));//�ȴ��������յ��źŻ��ʱ�����ʱ����
	TR1=0;
	
	if(TF1 == 0)
	{
		time = TH1 << 8 | TL1;
		return (time*0.017); //���ؾ���ֵ
	}
	else
	{
		TF1 = 0;//�ָ������־λ
		return 0;
	}
}