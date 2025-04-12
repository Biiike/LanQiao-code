#include <ultrasound.h>
#include <intrins.h>

sbit Tx = P1^0;
sbit Rx = P1^1;

void Delay12us(void)	//@12.000MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	i = 38;
	while (--i);
}

void Ut_Wava_Init()
{
	unsigned char i;
	EA = 0;
	for(i = 0; i<8; i++)
	{
		Tx = 1;
		Delay12us();
		Tx = 0;
		Delay12us();
	}
	EA = 1;
}

unsigned char Ut_Wava_Read()
{
	unsigned time = 0 ;
	CMOD = 0x00;
	CH = CL = 0;
	Ut_Wava_Init();
	CR = 1;
	while ((Rx == 1) && (CF == 0));
	CR = 0;
	
	if(CF == 0)
	{
		time = (CH<<8|CL) * 0.017;
		return time;
	}
	else
	{
		CF = 0;
		return 0;
	}
	
}