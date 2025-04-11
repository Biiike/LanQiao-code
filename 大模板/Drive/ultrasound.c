#include <ultrasound.h>

void Delay12us(void)	//@12.000MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}


void Ut_Wave_Send() //产生8个占空比为50%的方波信号
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
	
	while((Rx==1) && (TF1==0));//等待，当接收到信号或计时器溢出时跳出
	TR1=0;
	
	if(TF1 == 0)
	{
		time = TH1 << 8 | TL1;
		return (time*0.017); //返回距离值
	}
	else
	{
		TF1 = 0;//恢复溢出标志位
		return 0;
	}
}