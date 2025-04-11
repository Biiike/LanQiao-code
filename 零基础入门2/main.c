#include <REGX52.H>
#include "Key.h"
#include <intrins.h> 
#include "Delay.h"

unsigned char Key_val,Key_down,Key_up,Key_old;
unsigned char SystemFlag=1;
unsigned char LedMod=1;
unsigned char Led = 0xfe;
unsigned char Led2[4] = {0x7e,0xbd,0xdb,0xe7};
unsigned char Led2num=0;

void main ()
{
	
	
	while(1)
	{
		Key_val = Key_read();
		Key_down = Key_val & (Key_val ^ Key_old);
		Key_up = ~Key_val & (Key_val ^ Key_old);
		Key_old = Key_val;
		
		switch(Key_up)
		{
			case 1:
				SystemFlag = 1;//Æô¶¯
			break;
			case 2:
				SystemFlag = 0;//ÔÝÍ£
			break;
			case 3:
				LedMod++;
				if(LedMod>4) LedMod = 1;
			break;
			case 4:
				LedMod--;
				if(LedMod<1) LedMod =4;
			break;
			
		}
		

			if(SystemFlag == 1)
			{
				switch(LedMod)
				{
					case 1:
						P1 = Led;
						Delay(500);
						Led = _crol_(Led,1); 
					break;
					case 2:
						P1 = Led;
						Delay(500);
						Led = _cror_(Led,1);
					break;
					case 3:
						P1 = Led2[Led2num];
						Delay(500);
						Led2num++;
						if(Led2num>3) Led2num = 0;
					break;
					case 4:
						P1= Led2[Led2num];
						Delay(500);
						Led2num--;
						if(Led2num == 255) Led2num = 3;
					break;
				}
			}
		}	
	}


