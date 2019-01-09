#include "W5500.h"
#include "malloc.h"

_W5500 W5500;
void W5500_Init(void(*SetRst)(u8 val),
				void(*SetCs)(u8 val),
				void(*delay)(u32 nms),
				unsigned char (*spi_sentbyte)(unsigned char byte))
{
	//需实现下列函数以使用W5500
	W5500.SetCs = SetCs;
	W5500.SetRst = SetRst;
	W5500.Delay = delay;//延时
	W5500.SendByte = spi_sentbyte;//通过SPI发送1字节数据
}

void W5500Reset(void)
{
	W5500.SetRst(0);
	W5500.Delay(50);
	W5500.SetRst(1);
}

void WriteReg1Byte(u16 addr,unsigned char dat)
{
	W5500.SetCs(0);
	W5500.SendByte(addr >> 8);
	W5500.SendByte(addr);
	W5500.SendByte(VDM|RWB_WRITE|COMMON_R);
	W5500.SendByte(dat);
	W5500.SetCs(1);
}

void WriteReg2Byte(u16 addr,u16 dat)
{
	W5500.SetCs(0);
	W5500.SendByte(addr >> 8);
	W5500.SendByte(addr);
	W5500.SendByte(VDM|RWB_WRITE|COMMON_R);
	W5500.SendByte(dat>>8);
	W5500.SendByte(dat);
	W5500.SetCs(1);
}

void WriteBurstReg(u16 addr,unsigned char *buf,unsigned char len)
{
	unsigned char i;
	W5500.SetCs(0);
	W5500.SendByte(addr >> 8);
	W5500.SendByte(addr);
	W5500.SendByte(VDM|RWB_WRITE|COMMON_R);
	for(i = 0;i < len;i ++)
		W5500.SendByte(buf[i]);
	W5500.SetCs(1);
}

unsigned char   ReadReg1Byte(u16 addr)
{
	unsigned char dat;
	W5500.SetCs(0);
	W5500.SendByte(addr >> 8);
	W5500.SendByte(addr);
	W5500.SendByte(VDM|RWB_READ|COMMON_R);//通过SPI2写控制字节,1个字节数据长度,读数据,选择通用寄存器
	dat = W5500.SendByte(0x00);
	W5500.SetCs(1);
	return dat;
}

u16 ReadReg2Byte(u16 addr)
{
	u16 dat;
	W5500.SetCs(0);
	W5500.SendByte(addr >> 8);
	W5500.SendByte(addr);
	W5500.SendByte(VDM|RWB_READ|COMMON_R);//通过SPI2写控制字节,1个字节数据长度,读数据,选择通用寄存器
	dat = (W5500.SendByte(0x00)<<8);
	dat |= W5500.SendByte(0x00);
	W5500.SetCs(1);
	return dat;
}

void ReadBurstReg(u16 addr,unsigned char *buf,unsigned char len)
{
	unsigned char i;
	W5500.SetCs(0);
	W5500.SendByte(addr >> 8);
	W5500.SendByte(addr);
	W5500.SendByte(VDM|RWB_READ|COMMON_R);//通过SPI2写控制字节,1个字节数据长度,读数据,选择通用寄存器
	for(i = 0;i < len;i ++)
		buf[i] = W5500.SendByte(0x00);
	W5500.SetCs(1);
}

void WriteSocket1Byte(unsigned char index,u16 addr,unsigned char dat)
{
	W5500.SetCs(0);
	W5500.SendByte(addr >> 8);
	W5500.SendByte(addr);
	W5500.SendByte(VDM|RWB_WRITE|((index<<5)+0x08));
	W5500.SendByte(dat);
	W5500.SetCs(1);
}

void WriteSocket2Byte(unsigned char index,u16 addr,u16 dat)
{
	W5500.SetCs(0);
	W5500.SendByte(addr >> 8);
	W5500.SendByte(addr);
	W5500.SendByte(VDM|RWB_WRITE|((index<<5)+0x08));
	W5500.SendByte(dat >> 8);
	W5500.SendByte(dat);
	W5500.SetCs(1);
}

void WriteSocket4Byte(unsigned char index,u16 addr,unsigned char *buf)
{
	unsigned char i;
	W5500.SetCs(0);
	W5500.SendByte(addr >> 8);
	W5500.SendByte(addr);
	W5500.SendByte(VDM|RWB_WRITE|((index<<5)+0x08));
	for(i = 0;i < 4;i ++)
		W5500.SendByte(buf[i]);
	W5500.SetCs(1);
}

unsigned char ReadSocket1Byte(unsigned char index,u16 addr)
{
	unsigned char dat;
	W5500.SetCs(0);
	W5500.SendByte(addr >> 8);
	W5500.SendByte(addr);
	W5500.SendByte(VDM|RWB_READ|((index<<5)+0x08));
	dat = W5500.SendByte(0x00);
	W5500.SetCs(1);
	return dat;
}

u16 ReadSocket2Byte(unsigned char index,u16 addr)
{
	u16 dat;
	W5500.SetCs(0);
	W5500.SendByte(addr >> 8);
	W5500.SendByte(addr);
	W5500.SendByte(VDM|RWB_READ|((index<<5)+0x08));
	dat = (W5500.SendByte(0x00)<<8);
	dat |= W5500.SendByte(0x00);
	W5500.SetCs(1);
	return dat;
}

void ReadSocket4Byte(unsigned char index,u16 addr,unsigned char *buf)
{
	unsigned char i;
	W5500.SetCs(0);
	W5500.SendByte(addr >> 8);
	W5500.SendByte(addr);
	W5500.SendByte(VDM|RWB_READ|((index<<5)+0x08));
	for(i=0;i<4;i++)
		*(buf+i) = W5500.SendByte(0x00);
	W5500.SetCs(1);
}

