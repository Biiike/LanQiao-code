#include <Led.h>

unsigned char temp_1 = 0x00;
unsigned char temp_old_1 = 0xff;

void Led_Disp(unsigned char *ucLed)
{
	unsigned char temp;
	if(temp_1 != temp_old_1 )
	{
		temp_1 = 0x00;
		temp_1 = (ucLed[0]<<0)|(ucLed[1]<<1)|(ucLed[2]<<2)|(ucLed[3]<<3)|(ucLed[4]<<4)|(ucLed[5]<<5)|(ucLed[6]<<6)|(ucLed[7]<<7);
		
		P0 = ~temp_1;
		temp = P2 & 0x1f | 0x80;
		P2 = temp;
		temp = P2 & 0x1f;
		P2 = temp;
	
		temp_old_1 = temp_1;
	}
}