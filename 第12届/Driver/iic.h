#include <STC15F2K60S2.H>
#include <intrins.h>
sbit sda = P2^1;
sbit scl = P2^0;
void Da_Write(unsigned int dat);//��daд0-255��
unsigned char Ad_Read(unsigned char addr);//����0-255��������51.0��Ϊ��ѹ
void EEPROM_Write(unsigned char* EEPROM_String,unsigned char addr,unsigned char num);//д���ݣ�����������ַ������Ϊ8�ı����������ݸ���
void EEPROM_Read(unsigned char* EEPROM_String,unsigned char addr,unsigned char num);//����������ŵ�����������ַ�����ݸ���
