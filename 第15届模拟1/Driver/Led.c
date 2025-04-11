#include "Led.h"

idata unsigned char temp_1 = 0x00;
idata unsigned char temp_old_1 = 0xff;

void Led_Disp(unsigned char *ucLed)
{
 unsigned char temp;
   temp_1=0x00;
   temp_1 = (ucLed[0] << 0) | (ucLed[1] << 1) | (ucLed[2] << 2) | (ucLed[3] << 3) |
         (ucLed[4] << 4) | (ucLed[5] << 5) | (ucLed[6] << 6) | (ucLed[7] << 7);
  if (temp_1 != temp_old_1)
  {
    P0 = ~temp_1;
    

    temp = P2 & 0x1f;    // ????P2???5¦Ë
    temp = temp | 0x80;  // ??0x80???§Ý????
    P2 = temp;           // §Õ??P2
    temp = P2 & 0x1f;    // ????P2???5¦Ë
    P2 = temp;           // §Õ??P2???????????
    
    temp_old_1 = temp_1;
  }
}