/* 头文件声明区 */
#include <Init.h>
#include <Led.h>
#include <STC15F2K60S2.H>


/* Main */
void main()
{
	System_Init();
	
	while(1)
	{
		Led_Disp(3,1);
	}
}
