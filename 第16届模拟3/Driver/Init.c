#include <Init.h>

void Sys_Init()
{
	unsigned char temp = 0;
	
	P0 = 0xff;//��
	temp = (P2 & 0x1f) | 0x80;
	P2 = temp;
	temp = P2 & 0x1f;
	P2 = temp;
	
	P0 = 0x00;//�������ͼ̵���
	temp = (P2 & 0x1f) | 0xa0;
	P2 = temp;
	temp = P2 & 0x1f;
	P2 = temp;
}