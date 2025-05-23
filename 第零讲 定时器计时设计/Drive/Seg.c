#include "Seg.h"

unsigned char Seg_Wela[] = {0xfe,0xfd,0xfb,0xf7,0xef,0xdf};
unsigned char Seg_Dula[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00};

void Seg_Disp(unsigned Wela,Dula,point) //位，段，1为有小数点 0为吴无 
{
	P0 = 0x00;
	P2_6 = 1;
	P2_6 = 0;
	
	P0 = Seg_Wela[Wela];
	P2_7 = 1;
	P2_7 = 0;
	
	if(point == 1)
		P0 = Seg_Dula[Dula] | 0x80;
	else
		P0 = Seg_Dula[Dula];
	P2_6 = 1;
	P2_6 = 0;	
	
}