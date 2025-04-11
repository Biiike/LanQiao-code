#include <REGX52.H>


unsigned char Key_read()
{
	unsigned char temp = 0;
	if(P3_4==0) temp = 1;
	if(P3_5==0) temp = 2;
	if(P3_6==0) temp = 3;
	if(P3_7==0) temp = 4;

	return temp;
}


//Key_val =Key_read; //½¨ÂëÖµ
//Key_down = Key_val & (Key_val ^ Key_old); //¼ì²âÏÂ½µÑØ 
//Key_up = ~Key_val & (Key_val ^ Key_old); //¼ì²âÉÏÉıÑØ
//Key_old = Key_val;//¸¨Öú¼ì²â