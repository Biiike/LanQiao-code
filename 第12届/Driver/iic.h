#include <STC15F2K60S2.H>
#include <intrins.h>
sbit sda = P2^1;
sbit scl = P2^0;
void Da_Write(unsigned int dat);//向da写0-255，
unsigned char Ad_Read(unsigned char addr);//返回0-255的数，乘51.0即为电压
void EEPROM_Write(unsigned char* EEPROM_String,unsigned char addr,unsigned char num);//写内容，数组名，地址（必须为8的倍数），内容个数
void EEPROM_Read(unsigned char* EEPROM_String,unsigned char addr,unsigned char num);//读，读出存放的数组名，地址，内容个数
