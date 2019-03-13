#include "CrcMaker.h"

u16 CalCrc16(u8 *buf,u16 len,u8 flag)
{
	u16 i,Crc16 = 0xFFFF;
	u8 j,k ;
	for(i=0;i<len;i++)
	{
		k = 0;
		Crc16 ^= *(buf + i);
		for(j=0;j<8;j++)
		{
			k = (Crc16 & 0x0001);
			Crc16 >>= 1;
			Crc16 &= 0x7FFF;
			if(k)
				Crc16 ^= CRCGENP;
		}
	}
	if(flag)
		Crc16 += (Crc16 >> 8);
	return Crc16;
}

void SetCrc16(u8 *buf,u16 len,u8 flag)
{
	u16 Crc16 = CalCrc16(buf,len,flag);
	*(buf+len) = (u8)(Crc16 & 0xFF);
	*(buf+len+1) = (u8)((Crc16 >> 8) & 0xFF);
}

u8 CheckCrc16(u8 *buf,u16 len,u8 flag)
{
	u16 Crc16 = CalCrc16(buf,len,flag);
	if(flag)
		Crc16 += (Crc16 >> 8);
	if((((Crc16 >> 8) & 0xFF) != *(buf+len+1)) ||(Crc16 & 0xFF) != *(buf+len))
		return 0;
	else
		return 1;
}

u8 SumAll(u8 *buf,u8 len)
{
     u8 i,sum;
     sum = 0;
     for(i = 0;i < len;i ++)
          sum += buf[i];
     return sum;
}

u8 SumCheck(u8 *buf,u8 len)
{
     u8 sum;
     sum = SumAll(buf,len-1);
     if(sum == buf[len-1])
          return 1;
     else
          return 0;
}

