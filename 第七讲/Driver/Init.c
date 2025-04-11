#include "Init.h"
#include <STC15F2K60S2.H>

void Sys_Init()
{
	P0 = 0xff;//LedÈ«Ãğ
	P2 = P2 & 0x1f | 0x80;
	P2 &= 0x1f;
	
	P0 = 0x00;//¹Ø±Õ·äÃùÆ÷¡¢¼ÌµçÆ÷
	P2 = P2 & 0x1f | 0xa0;
	P2 &= 0x1f;
}
