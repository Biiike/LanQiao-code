#include "led.h"
/// @brief Ledɨ��
/// @param addr ��Ҫ���Ƶ�Led�ĵ�ַ��0-7��
/// @param enable ���Ƹõ�ַ��Led�Ƿ����
#include <STC15F2K60S2.H>

 idata  unsigned char temp_1 = 0x00;
 idata  unsigned char temp_old_1 = 0xff;

void Led_Disp(unsigned char *ucLed)
{
   unsigned char temp;
   temp_1=0x00;
   temp_1 = (ucLed[0] << 0) | (ucLed[1] << 1) | (ucLed[2] << 2) | (ucLed[3] << 3) |
         (ucLed[4] << 4) | (ucLed[5] << 5) | (ucLed[6] << 6) | (ucLed[7] << 7);
  if (temp_1 != temp_old_1)
  {
    P0 = ~temp_1;
    
    // ����P2������
    temp = P2 & 0x1f;    // ����P2�ĵ�5λ
    temp = temp | 0x80;  // ��0x80���л����
    P2 = temp;           // д��P2
    temp = P2 & 0x1f;    // ����P2�ĵ�5λ
    P2 = temp;           // д��P2���ر�������
    
    temp_old_1 = temp_1;
  }
}

void Led_Off()
{
    unsigned char temp;
	
    P0 = 0xff;
    
    // ����P2������
    temp = P2 & 0x1f;    // ����P2�ĵ�5λ
    temp = temp | 0x80;  // ��0x80���л����
    P2 = temp;           // д��P2
    temp = P2 & 0x1f;    // ����P2�ĵ�5λ
    P2 = temp;           // д��P2���ر�������
    
    temp_old_1=0x00;
}

idata unsigned char temp_0 = 0x00;
idata unsigned char temp_old_0 = 0xff;
/// @brief ����������
/// @param enable
void Beep(bit enable)
{
  unsigned char temp;
  
  if (enable)
    temp_0 |= 0x40;
  else
    temp_0 &= ~(0x40);
  if (temp_0 != temp_old_0)
  {
    P0 = temp_0;
    
    // ����P2������
    temp = P2 & 0x1f;    // ����P2�ĵ�5λ
    temp = temp | 0xa0;  // ��0xa0���л����
    P2 = temp;           // д��P2
    temp = P2 & 0x1f;    // ����P2�ĵ�5λ
    P2 = temp;           // д��P2���ر�������
    
    temp_old_0 = temp_0;
  }
}
/// @brief �̵�������
/// @param enable
void Relay(bit enable)
{
  unsigned char temp;
  
  if (enable)
    temp_0 |= 0x10;
  else
    temp_0 &= ~(0x10);
  if (temp_0 != temp_old_0)
  {
    P0 = temp_0;
    
    // ����P2������
    temp = P2 & 0x1f;    // ����P2�ĵ�5λ
    temp = temp | 0xa0;  // ��0xa0���л����
    P2 = temp;           // д��P2
    temp = P2 & 0x1f;    // ����P2�ĵ�5λ
    P2 = temp;           // д��P2���ر�������
    
    temp_old_0 = temp_0;
  }
}
/// @brief MOTOR����
/// @param enable
void MOTOR(bit enable)
{
  unsigned char temp;
  
  if (enable)
    temp_0 |= 0x20;
  else
    temp_0 &= ~(0x20);
  if (temp_0 != temp_old_0)
  {
    P0 = temp_0;
    
    // ����P2������
    temp = P2 & 0x1f;    // ����P2�ĵ�5λ
    temp = temp | 0xa0;  // ��0xa0���л����
    P2 = temp;           // д��P2
    temp = P2 & 0x1f;    // ����P2�ĵ�5λ
    P2 = temp;           // д��P2���ر�������
    
    temp_old_0 = temp_0;
  }
}