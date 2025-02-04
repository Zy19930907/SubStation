#include "SD2068.h"

_SD2068 SD2068;

void TIM_Start(void)
{
    SD2068SDAOUT;
    SETSD2068SDA(SD2068IOVALUEHIGH);
    SETSD2068SCL(SD2068IOVALUEHIGH);
    delay_us(4);
    SETSD2068SDA(SD2068IOVALUELOW);
    delay_us(4);
    SETSD2068SCL(SD2068IOVALUELOW);
}

void TIM_Stop(void)
{
    SD2068SDAOUT;//SD2068.delay_us(4);
    SETSD2068SCL(SD2068IOVALUELOW);//SD2068.delay_us(4);
    SETSD2068SDA(SD2068IOVALUELOW);//SD2068.delay_us(4);
    delay_us(4);
    SETSD2068SCL(SD2068IOVALUEHIGH);//SD2068.delay_us(4);
    SETSD2068SDA(SD2068IOVALUEHIGH);
    delay_us(4);
}

u8 TIM_Wait_ACK(void)
{
    u8 ucErr = 0;
    SD2068SDAIN;
    //delay_us(4);
    SETSD2068SCL(SD2068IOVALUEHIGH);
    //delay_us(1);
    while(GETSD2068SDA)
    {
        ucErr++;
        if(ucErr > 250)
        {
            TIM_Stop();
            return 1;
        }
    }
    SETSD2068SCL(SD2068IOVALUELOW);
    return 0;
}

void TIM_Ack(void)
{
    SETSD2068SCL(SD2068IOVALUELOW);//SD2068.delay_us(4);
    SD2068SDAOUT;//SD2068.delay_us(4);
    SETSD2068SDA(SD2068IOVALUELOW);
    delay_us(2);
    SETSD2068SCL(SD2068IOVALUEHIGH);
    delay_us(2);
    SETSD2068SCL(SD2068IOVALUELOW);
}

void TIM_NAck(void)
{
    SETSD2068SCL(SD2068IOVALUELOW);//SD2068.delay_us(4);
    SD2068SDAOUT;//SD2068.delay_us(4);
    SETSD2068SDA(SD2068IOVALUEHIGH);
    delay_us(2);
    SETSD2068SCL(SD2068IOVALUEHIGH);
    delay_us(2);
    SETSD2068SCL(SD2068IOVALUELOW);
}

void TIM_Send_Byte(u8 txd)
{
    u8 t;
    SD2068SDAOUT;//SD2068.delay_us(4);
    SETSD2068SCL(SD2068IOVALUELOW);//SD2068.delay_us(4);
    for(t = 0; t < 8; t++)
    {
        SETSD2068SDA((txd & 0x80));
        txd <<= 1;
        delay_us(2);
        SETSD2068SCL(SD2068IOVALUEHIGH);
        delay_us(2);
        SETSD2068SCL(SD2068IOVALUELOW);
        delay_us(2);
    }
}

u8 TIM_Read_Byte(void)
{
    unsigned char i, receive = 0;
    SD2068SDAIN;//SD2068.delay_us(4);
    for(i = 0; i < 8; i++ )
    {
        SETSD2068SCL(SD2068IOVALUELOW);
        delay_us(2);
        SETSD2068SCL(SD2068IOVALUEHIGH);
        receive <<= 1;
        receive += GETSD2068SDA;
        delay_us(1);
    }
    SETSD2068SCL(SD2068IOVALUELOW);
    return receive;
}

void ReadTime(void)
{
	u8 i;
	TIM_Start();
	TIM_Send_Byte(0x65);
	TIM_Wait_ACK();
	for(i = 7; i > 0; i --)
	{
		SD2068.Hex[i - 1] = TIM_Read_Byte();
		if(i != 1)
			TIM_Ack();
	}
	TIM_NAck();
	TIM_Stop();
	SD2068.Hex[4] &= 0x7f;  //小时，把最高位过滤
	//转换成BCD码
	for(i = 0; i < 7; i ++)
		SD2068.Buf[i] = (SD2068.Hex[i] / 16) * 10 + SD2068.Hex[i] % 16;
	SD2068.AllSecond = SD2068.Buf[5] * 60 + SD2068.Buf[6];
}
/**********************************************************************
 * 允许写入
***********************************************************************/
void WriteOn(void)
{
    TIM_Start();
    TIM_Send_Byte(0x64);
    TIM_Wait_ACK();
    TIM_Send_Byte(0x10);
    TIM_Wait_ACK();
    TIM_Send_Byte(0x80);
    TIM_Wait_ACK();
    TIM_Stop();

    TIM_Start();
    TIM_Send_Byte(0x64);
    TIM_Wait_ACK();
    TIM_Send_Byte(0x0F);
    TIM_Wait_ACK();
    TIM_Send_Byte(0xFF);
    TIM_Wait_ACK();
    TIM_Stop();
}
/**********************************************************************
 * 禁止写入
***********************************************************************/
void WriteOff(void)
{
    TIM_Start();
    TIM_Send_Byte(0x64);
    TIM_Wait_ACK();
    TIM_Send_Byte(0x0F);
    TIM_Wait_ACK();
    TIM_Send_Byte(0x7B);
    TIM_Wait_ACK();
    TIM_Send_Byte(0x0);
    TIM_Wait_ACK();
    TIM_Stop();
}

void WriteTime(void)
{
    WriteOn();				//允许写入
    TIM_Start();
    TIM_Send_Byte(0x64);
    TIM_Wait_ACK();
    TIM_Send_Byte(0x00);			//设置起始地址
    TIM_Wait_ACK();
    TIM_Send_Byte(SD2068.Buf[6]);		//秒
    TIM_Wait_ACK();
    TIM_Send_Byte(SD2068.Buf[5]);		//分
    TIM_Wait_ACK();
    TIM_Send_Byte(SD2068.Buf[4] | 0x80);  //时  24h制
    TIM_Wait_ACK();
    TIM_Send_Byte(SD2068.Buf[3]);		//date
    TIM_Wait_ACK();
    TIM_Send_Byte(SD2068.Buf[2]);		//月
    TIM_Wait_ACK();
    TIM_Send_Byte(SD2068.Buf[1]);		//日
    TIM_Wait_ACK();
    TIM_Send_Byte(SD2068.Buf[0]);		//年
    TIM_Wait_ACK();
    TIM_Stop();
    WriteOff();             // 关闭写
}

void TimeChange(u8 *buf)
{
	u32 timeHex;
	ReadTime();
	timeHex = SD2068.Buf[0];    //年
	timeHex <<= 4;
	timeHex += SD2068.Buf[1];   //月
	timeHex <<= 5;
	timeHex += SD2068.Buf[2];   //日
	timeHex <<= 5;
	timeHex += SD2068.Buf[4];   //小时
	timeHex <<= 6;
	timeHex += SD2068.Buf[5];   //分
	timeHex <<= 6;
	timeHex += SD2068.Buf[6];   //秒
	buf[0] = timeHex;
	buf[1] = timeHex >> 8;
	buf[2] = timeHex >> 16;
	buf[3] = timeHex >> 24;
}

void SD2068Init(void)
{
	SD2068PORTCLKEN;
    SD2068SDAPORT->MODER &= ~((u32)3 << (SD2068SDAPIN << 1));
    SD2068SDAPORT->MODER |=  ((u32)1 << (SD2068SDAPIN << 1));
    SD2068SDAPORT->OSPEEDR |= ((u32)3 << (SD2068SDAPIN << 1));
    SD2068SDAPORT->PUPDR |= ((u32)1 << (SD2068SDAPIN << 1));
    SD2068SCLPORT->MODER &= ~((u32)3 << (SD2068SCLPIN << 1));
    SD2068SCLPORT->MODER |=  ((u32)1 << (SD2068SCLPIN << 1));
    SD2068SCLPORT->OSPEEDR |= ((u32)3 << (SD2068SCLPIN << 1));
    SD2068SCLPORT->PUPDR |= ((u32)1 << (SD2068SCLPIN << 1));
    SD2068INTPORT->MODER &= ~((u32)3 << SD2068INTPIN);
}
