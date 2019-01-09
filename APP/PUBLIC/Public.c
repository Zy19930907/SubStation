#include "Public.h"

_Sys Sys;
_Sensor Device[MaxDeviceNum];
_Excute Excute[MaxTriggerNum];
_Power  Power[MaxPowerNum];

void BufCopy(u8 *d, u8 *s,u16 len)
{
	if(len <= 0)
		return;
	do{
		*d++ = *s++;
	}while(--len);
}

u8 BufCmp(u8 *s1,u8 *s2,u16 len)
{
	u16 i;
	for(i=0;i<len;i++)
	{
		if(*(s1+i) != *(s2+i))
			return 0;
	}
	return 1;
}

u32 MakeFeimoCanId(u8 FramCnt, u8 Cmd, u8 CtrFlag, u8 Dir, u8 Type, u8 Addr)
{
	u32 FeimoId = 0;
	FeimoId = FramCnt;
	FeimoId <<= 7;
	FeimoId += Cmd;
	FeimoId <<= 2;
	FeimoId += CtrFlag;
	FeimoId <<= 1;
	FeimoId += Dir;
	FeimoId <<= 6;
	FeimoId += Type;
	FeimoId <<= 8;
	FeimoId += Addr;
	return FeimoId;
}
