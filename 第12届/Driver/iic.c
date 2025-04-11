#define DELAY_TIME	5
#include <iic.h>
//
static void I2C_Delay(unsigned char n)
{
    do
    {
        _nop_();_nop_();_nop_();_nop_();_nop_();
        _nop_();_nop_();_nop_();_nop_();_nop_();
        _nop_();_nop_();_nop_();_nop_();_nop_();		
    }
    while(n--);      	
}

//
void I2CStart(void)
{
    sda = 1;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 0;
	I2C_Delay(DELAY_TIME);
    scl = 0;    
}

//
void I2CStop(void)
{
    sda = 0;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 1;
	I2C_Delay(DELAY_TIME);
}

//
void I2CSendByte(unsigned char byt)
{
    unsigned char i;
	
    for(i=0; i<8; i++){
        scl = 0;
		I2C_Delay(DELAY_TIME);
        if(byt & 0x80){
            sda = 1;
        }
        else{
            sda = 0;
        }
		I2C_Delay(DELAY_TIME);
        scl = 1;
        byt <<= 1;
		I2C_Delay(DELAY_TIME);
    }
	
    scl = 0;  
}

//
unsigned char I2CReceiveByte(void)
{
	unsigned char da;
	unsigned char i;
	for(i=0;i<8;i++){   
		scl = 1;
		I2C_Delay(DELAY_TIME);
		da <<= 1;
		if(sda) 
			da |= 0x01;
		scl = 0;
		I2C_Delay(DELAY_TIME);
	}
	return da;    
}

//
unsigned char I2CWaitAck(void)
{
	unsigned char ackbit;
	
    scl = 1;
	I2C_Delay(DELAY_TIME);
    ackbit = sda; 
    scl = 0;
	I2C_Delay(DELAY_TIME);
	
	return ackbit;
}

//
void I2CSendAck(unsigned char ackbit)
{
    scl = 0;
    sda = ackbit; 
	I2C_Delay(DELAY_TIME);
    scl = 1;
	I2C_Delay(DELAY_TIME);
    scl = 0; 
	sda = 1;
	I2C_Delay(DELAY_TIME);
}


unsigned char Ad_Read(unsigned char addr) //ad转换器读数据，转换为电压需要除51.0
{
	unsigned char temp;
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(addr);
	I2CWaitAck();
	I2CStart();
	I2CSendByte(0x91);
	I2CWaitAck();
	temp = I2CReceiveByte();
	I2CSendAck(1);
	I2CStop();
	return temp;
}

void Da_Write(unsigned int dat)//ad转换器写数据，0-255转换为电压，数据需要乘51.0
{
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x41);
	I2CWaitAck();
	I2CSendByte(dat);
	I2CWaitAck();
	I2CStop();
}


//读取EEPROM中某个地址的数据，存放在变量中
void EEPROM_Read(unsigned char* EEPROM_String,unsigned char addr,unsigned char num)
{
	I2CStart();
	I2CSendByte(0xA0);//确定写的模式
	I2CWaitAck();
	I2CSendByte(addr);//写入要读取数据的地址
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(0xA1);//确定读的模式
	I2CWaitAck();
	
	while(num--)
	{
		*EEPROM_String++ = I2CReceiveByte();
		if(num) I2CSendAck(0);
			else I2CSendAck(1);
	}
	I2CStop();
}


//向EEPROM的某个地址写入字符串中特定数量的字符
void EEPROM_Write(unsigned char* EEPROM_String,unsigned char addr,unsigned char num)
{
	I2CStart();
	I2CSendByte(0xA0);//确定写的模式
	I2CWaitAck();
	I2CSendByte(addr);//写入要读取数据的地址
	I2CWaitAck();
	
	while(num--)
	{
	I2CSendByte(*EEPROM_String++);
	I2CWaitAck();
	I2C_Delay(200);	
	}
	I2CStop();
}
