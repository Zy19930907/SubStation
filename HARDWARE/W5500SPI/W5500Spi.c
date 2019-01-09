#include "W5500Spi.h"

//SPI2 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 W5500Spi_SendByte(u8 dat)
{		 			 
	unsigned char i, returnData;
    returnData = 0;
    W5500CLRCLK;
    for(i = 0; i < 8; i ++)
    {
        if(dat & 0x80)
			W5500SETMOSI;
        else
			W5500CLRMOSI;
        dat <<= 1;
        W5500SETCLK;
        returnData <<= 1;
        if(W5500MISOVALUE)
           returnData++;     
        W5500CLRCLK;
    }
	return returnData;
}

//W5500数据接口
void W5500Spi_Init(void)
{
	W5500IOCLKEN;
	W5500MOSIPORT->MODER |= ((u32)1 << (W5500MOSIPIN << 1));
	W5500MOSIPORT->OSPEEDR |= ((u32)3 << (W5500MOSIPIN << 1));
	W5500MOSIPORT->PUPDR |= ((u32)1 << (W5500MOSIPIN << 1));
	
	W5500CLKPORT->MODER |= ((u32)1 << (W5500CLKPIN << 1));
	W5500CLKPORT->OSPEEDR |= ((u32)3 << (W5500CLKPIN << 1));
	W5500CLKPORT->PUPDR |= ((u32)1 << (W5500CLKPIN << 1));
	
	W5500CSPORT->MODER |= ((u32)1 << (W5500CSPIN << 1));
	W5500CSPORT->OSPEEDR |= ((u32)3 << (W5500CSPIN << 1));
	W5500CSPORT->PUPDR |= ((u32)1 << (W5500CSPIN << 1));
	
	W5500RSTPORT->MODER |= ((u32)1 << (W5500RSTPIN << 1));
	W5500RSTPORT->OSPEEDR |= ((u32)3 << (W5500RSTPIN << 1));
	W5500RSTPORT->PUPDR |= ((u32)1 << (W5500RSTPIN << 1));
	
	W5500INTPORT->PUPDR |= ((u32)2 << (W5500INTPIN << 1));
	
	W5500MOSIPORT->ODR |= ((u32)1 << W5500MOSIPIN);
	W5500CSPORT->ODR |= ((u32)1 << W5500MOSIPIN);
	W5500CLKPORT->ODR |= ((u32)1 << W5500MOSIPIN); 
}

void W5500SetRstValue(u8 val)
{
	if(val)
		W5500SETRST;
	else
		W5500CLRRST;
}

void W5500SetCsValue(u8 val)
{
	if(val)
		W5500SETCS;
	else
		W5500CLRCS;
}

u8 W5500GetIntIoState(void)
{
	return ISW5500NETINT;
}

