#include "FmInit.h"

void FmMemoryInit(void)
{
	EEIOPORTCLK;
	EESDAPORT->MODER &= ~(3 << (EESDAPIN << 1));
    EESDAPORT->MODER |=  (1 << (EESDAPIN << 1));
    EESDAPORT->OSPEEDR |= (3 << (EESDAPIN << 1));
    EESDAPORT->PUPDR |= (1 << (EESDAPIN << 1));
	
	EESCLPORT->MODER &= ~(3 << (EESCLPIN << 1));
    EESCLPORT->MODER |=  (1 << (EESCLPIN << 1));
    EESCLPORT->OSPEEDR |= (3 << (EESCLPIN << 1));
    EESCLPORT->PUPDR |= (1 << (EESCLPIN << 1));
	
	EEWPPORT->MODER &= ~(3 << (EEWPPIN << 1));
    EEWPPORT->MODER |=  (1 << (EEWPPIN << 1));
    EEWPPORT->OSPEEDR |= (3 << (EEWPPIN << 1));
    EEWPPORT->PUPDR |= (1 << (EEWPPIN << 1));
	
}

void FmIICStart(void)
{
	EESDAOUT;//SDA输出模式
	delay_us(2);
	SETEESDA(EEIOVALUEHIGH);//SDA拉高
	delay_us(2);
	SETEESCL(EEIOVALUEHIGH);//SCL拉高
	delay_us(4);
	SETEESDA(EEIOVALUELOW);//SDA拉低
	delay_us(4);
	SETEESCL(EEIOVALUELOW);//SCL拉低
}

void FmIICStop(void)
{
	EESDAOUT;//SDA输出模式
	delay_us(2);
	SETEESCL(EEIOVALUELOW);//SCL拉低
	delay_us(2);
	SETEESDA(EEIOVALUELOW);//SDA拉低
	delay_us(4);
	SETEESCL(EEIOVALUEHIGH);//SCL拉高
	delay_us(2);
	SETEESDA(EEIOVALUEHIGH);//SDA拉高
	delay_us(4);
}

void FmIICAck(void)
{
	SETEESCL(EEIOVALUELOW);//SCL拉低
	delay_us(2);
	EESDAOUT;//SDA输出模式
	delay_us(2);
	SETEESDA(EEIOVALUELOW);//SDA拉低
	delay_us(2);
	SETEESCL(EEIOVALUEHIGH);//SCL拉高
	delay_us(2);
	SETEESCL(EEIOVALUELOW);//SCL拉低
	delay_us(4);
}

void FmIICNack(void)
{
	SETEESCL(EEIOVALUELOW);//SCL拉低
	delay_us(20);
	EESDAOUT;//SDA输出模式
	delay_us(20);
	SETEESDA(EEIOVALUEHIGH);//SDA拉高
	delay_us(20);
	SETEESCL(EEIOVALUEHIGH);//SCL拉高
	delay_us(20);
	SETEESCL(EEIOVALUELOW);//SCL拉低
}

void FmIICSendByte(u8 val)
{
	u8 i;   
	EESDAOUT;//SDA输出模式
	delay_us(2);   
	SETEESCL(EEIOVALUELOW);//SCL拉低
	for(i=0x80;i>0;i>>=1)
	{              
		SETEESDA((val&i));	  
		delay_us(2);
		SETEESCL(EEIOVALUEHIGH);
		delay_us(2); 
		SETEESCL(EEIOVALUELOW);
		delay_us(2);
	}	
}

u8 FmIICReadByte(void)
{
	u8 i,receive=0;
	EESDAIN;
	delay_us(2);
	for(i=0;i<8;i++)
	{
		SETEESCL(EEIOVALUELOW);
		delay_us(2);
		SETEESCL(EEIOVALUEHIGH);
		receive <<= 1;
		receive += GETEESDA; 	
		delay_us(1); 
	}
	SETEESCL(EEIOVALUELOW);
	return receive;
}

void FramWriteBurstByte(u16 addr, u8 *buf, u16 len)
{
	u16 i;
	EEDISWP;
	FmIICStart();  
	FmIICSendByte(0xA0); 
	FmIICAck();
	FmIICSendByte(addr >> 8); 
	FmIICAck(); 	 
	FmIICSendByte(addr&0xFF); 
	FmIICAck();
	for(i = 0;i < len;i ++)
	{
		FmIICSendByte(buf[i]); 						   
		FmIICAck();
	}
	FmIICStop();
	EEENWP;
}

void FramReadBurstByte(u16 addr, u8 *buf, u16 len)
{
	u16 i;
	FmIICStart();  
	FmIICSendByte(0xA0);	   
	FmIICAck(); 
	FmIICSendByte(addr >> 8);
	FmIICAck();
	FmIICSendByte(addr&0xFF);
	FmIICAck();	    
	FmIICStart();  	 	   
	FmIICSendByte(0xA1);		   
	FmIICAck();
	for(i = 0;i < len; i ++)
	{
		*(buf+i) = FmIICReadByte();
		if(i == (len-1))
			FmIICNack();
		else
			FmIICAck();
	}
	FmIICStop();
}

