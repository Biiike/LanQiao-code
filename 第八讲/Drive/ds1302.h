#include <STC15F2K60S2.H>
#include <intrins.h>
void Write_Ds1302(unsigned  char temp);
void Write_Ds1302_Byte( unsigned char address,unsigned char dat ) ;
unsigned char Read_Ds1302_Byte ( unsigned char address );
void Set_Rtc(unsigned char ucRtc[3]);
void Read_Rtc(unsigned char ucRtc[3]);
