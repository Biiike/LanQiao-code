#include "Init.h"
#include <STC15F2K60S2.H>

void Sys_Init()
{
	P0 = 0xff;//Ledȫ��
	P2 = P2 & 0x1f | 0x80;
	P2 &= 0x1f;
	
	P0 = 0x00;//�رշ��������̵���
	P2 = P2 & 0x1f | 0xa0;
	P2 &= 0x1f;
}
