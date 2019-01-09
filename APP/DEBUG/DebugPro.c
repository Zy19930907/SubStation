#include "DebugPro.h"

#define DEBUGFRAM				0x55AAFEEF

void HandleDebugCmd(u8 *buf,u16 len)
{
	u32 *p = (u32*)buf;
	u32 ID;
	u8 cmd[8];
	
	if(SWAP32(*p) == DEBUGFRAM)
	{
		ID = MakeFramID(0,buf[4],30);
		cmd[0] = buf[5];
		cmd[1] = buf[6];
		CanSendData(CAN_1,ID,cmd,2);
	}
}
