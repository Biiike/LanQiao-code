#include <STC15F2K60S2.H>
#include <intrins.h>
sbit sda = P2^1;
sbit scl = P2^0;
void Da_Write(unsigned char dat);
unsigned char Ad_Read(unsigned char addr);
