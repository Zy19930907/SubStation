#include "BoardCastCmd.h"
//帧头55AA55AA 帧长度XX XX 指令码XX 指令长度XX XX 指令数据域
void HandleBoardCastCmd(u8 *buf,u8 sn,u16 len)
{
	u8 i,destAddr;
	u16 framLen,cmdLen,musicIndex;
	
	if(!CheckCrc16(buf,len-2,0))//crc校验错误
		return;
	
	framLen = *(u16*)&buf[4];
	if(framLen != len)//帧长度错误
		return;
	
	cmdLen = *(u16*)&buf[6];
	
	switch(buf[4])
	{
		case 0x01://单分站播放音乐
			break;
		
		case 0x02://单广播播放音乐
			break;

		case 0x03://单广播对讲
			break;
		
		case 0x04://播放音乐组播
			break;
		
		case 0x05://对讲组播
			break;
		
		case 0x06://配置广播组
			break;
	}
}


