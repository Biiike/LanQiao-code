#include <REGX52.H>

unsigned char MatrixKey()
{
	unsigned char temp = 0;
	
	P3_4=0, P3_5=1, P3_6=1, P3_7=1;
	if(P3_0 == 0) temp=1;
	if(P3_1 == 0) temp=5;
	if(P3_2 == 0) temp=9;
	if(P3_3 == 0) temp=13;
	
	P3_4=1, P3_5=0, P3_6=1, P3_7=1;
	if(P3_0 == 0) temp=2;
	if(P3_1 == 0) temp=6;
	if(P3_2 == 0) temp=10;
	if(P3_3 == 0) temp=14;
	
	P3_4=1, P3_5=1, P3_6=0, P3_7=1;
	if(P3_0 == 0) temp=3;
	if(P3_1 == 0) temp=7;
	if(P3_2 == 0) temp=11;
	if(P3_3 == 0) temp=15;
	
	P3_4=1, P3_5=1, P3_6=1, P3_7=0;
	if(P3_0 == 0) temp=4;
	if(P3_1 == 0) temp=8;
	if(P3_2 == 0) temp=12;
	if(P3_3 == 0) temp=16;
	
	return temp;
		
}

//		MatrixKey_val =MatrixKey(); //Ω®¬Î÷µ
//		MatrixKey_down = MatrixKey_val & (MatrixKey_val ^ MatrixKey_old); //ºÏ≤‚œ¬Ωµ—ÿ 
//		MatrixKey_up = ~MatrixKey_val & (MatrixKey_val ^ MatrixKey_old); //ºÏ≤‚…œ…˝—ÿ
//		MatrixKey_old = MatrixKey_val;//∏®÷˙ºÏ≤‚
