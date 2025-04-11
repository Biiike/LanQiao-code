#include<Key.h>

unsigned char Key_Read()
{
	unsigned char temp = 0;
	
	P44 = 0; P42 = 1; P35 = 1; P34 = 1;
	//if(P30 == 0) temp = 7;
	if(P31 == 0) temp = 99;
	if(P32 == 0) temp = 12;//key 5
	if(P33 == 0) temp = 11;//key 4
	
	P44 = 1; P42 = 0; P35 = 1; P34 = 1;
//	if(P30 == 0) temp = 11;
	if(P31 == 0) temp = 1;
	if(P32 == 0) temp = 4;
	if(P33 == 0) temp = 7;
	
	P44 = 1; P42 = 1; P35 = 0; P34 = 1;
	if(P30 == 0) temp = 15;
	if(P31 == 0) temp = 2;
	if(P32 == 0) temp = 5;
	if(P33 == 0) temp = 8;

	P44 = 1; P42 = 1; P35 = 1; P34 = 0;
	if(P30 == 0) temp = 19;
	if(P31 == 0) temp = 3;
	if(P32 == 0) temp = 6;
	if(P33 == 0) temp = 9;
	
	return temp;

}