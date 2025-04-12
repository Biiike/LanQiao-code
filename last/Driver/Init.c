#include <Init.h>

void Sys_Init()
{
	unsigned char temp;
	P0 = 0xff;//µÆ
	temp = P2 & 0x1f | 0x80;
	P2 = temp;
	temp = P2 & 0x1f;
	P2 = temp;
	
	P0 = 0x00;
	temp = P2 & 0x1f | 0xa0;
	P2 = temp;
	temp = P2 & 0x1f;
	P2 = temp;
}