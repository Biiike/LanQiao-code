#include "ds1302.h"						

sbit SCK = P1^7;
sbit SDA = P2^3;
sbit RST = P1^3;

void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK = 0;
		SDA = temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

//
void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
 	Write_Ds1302(dat);		
 	RST=0; 
}

//
unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1;	_nop_();
 	Write_Ds1302(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
	SCK=1;	_nop_();
	SDA=0;	_nop_();
	SDA=1;	_nop_();
	return (temp);			
}

void Set_Rtc(unsigned char* ucRtc)
{
	Write_Ds1302_Byte(0x8e,0x00);
	Write_Ds1302_Byte(0x84,ucRtc[0]);//小时
	Write_Ds1302_Byte(0x82,ucRtc[1]);//分钟
	Write_Ds1302_Byte(0x80,ucRtc[2]);//秒
	Write_Ds1302_Byte(0x8e,0x80);
}

void Read_Rtc(unsigned char* ucRtc)
{
	ucRtc[0] = Read_Ds1302_Byte(0x85);//小时
	ucRtc[1] = Read_Ds1302_Byte(0x83);//分钟
	ucRtc[2] = Read_Ds1302_Byte(0x81);//秒
}

void Set_Date(unsigned char* ucDate)
{
	Write_Ds1302_Byte(0x8e,0x00);
	Write_Ds1302_Byte(0x8c,ucDate[0]);//年
	Write_Ds1302_Byte(0x88,ucDate[1]);//月
	Write_Ds1302_Byte(0x86,ucDate[2]);//日
	Write_Ds1302_Byte(0x8e,0x80);
}

void Read_Date(unsigned char* ucDate)
{
	ucDate[0] = Read_Ds1302_Byte(0x8d);//年
	ucDate[1] = Read_Ds1302_Byte(0x89);//月
	ucDate[2] = Read_Ds1302_Byte(0x87);//日
}