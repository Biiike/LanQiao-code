#include <Seg.h>

unsigned char Seg_Wela[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned char Seg_Dula[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};

void Seg_Disp(Wela,Dula,Point)
{
	unsigned char temp;
	
	P0 = 0xff;
	temp = P2 & 0x1f | 0xe0;
	P2 = temp;
	temp = P2 & 0x1f;
	P2 = temp;
	
	P0 = Seg_Wela[Wela];
	temp = P2 & 0x1f | 0xc0;
	P2 = temp;
	temp = P2 & 0x1f;
	P2 = temp;
	
	P0 = Seg_Dula[Dula];
	if(Point)
		P0 &= 0x7f;
	temp = P2 & 0x1f | 0xe0;
	P2 = temp;
	temp = P2 & 0x1f;
	P2 = temp;
}