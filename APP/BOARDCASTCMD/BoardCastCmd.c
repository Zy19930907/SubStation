#include "BoardCastCmd.h"
//帧头55AA55AA 帧长度XX XX 指令码XX 指令长度XX XX 指令数据域
void HandleBoardCastCmd(u8 *buf,u16 len,u8 sn)
{
	u16 framLen,musicIndex;
	_Can cmdCan;
	
	if(!CheckCrc16(buf,len-2,0))//crc校验错误
		return;
	
	framLen = *(u16*)&buf[4];
	if(framLen != len)//帧长度错误
		return;
	
	switch(buf[6])
	{
		case 0x01://单分站播放音乐(指令码0x01,指令长度2,指令数据域：音乐序号，2字节)
			musicIndex = *(u16*)&buf[9];
			StartPlayMusic(musicIndex);
			break;
		
		case 0x02://单分站对讲
			
			break;

		case 0x03://单广播播放音乐
			cmdCan.ID = MakeFeimoCanId(0,0x65,0,0,0x28,buf[9]);
			cmdCan.Buf[0] = buf[10];//歌曲序号
			cmdCan.Buf[1] = buf[11];
			cmdCan.Len = 2;
			CanSendData(CanBusBelong(buf[9]),cmdCan.ID,cmdCan.Buf,cmdCan.Len);
			break;
		
		case 0x04://单广播对讲
			cmdCan.ID = MakeFeimoCanId(0,0x68,0,0,0x28,buf[9]);
			cmdCan.Buf[0] = 0x04;//单播指令
			BufCmp(&cmdCan.Buf[1],&buf[10],6);
			cmdCan.Len = 7;
			CanSendData(CanBusBelong(buf[9]),cmdCan.ID,cmdCan.Buf,cmdCan.Len);
			break;
		
		case 0x05://播放音乐组播
			
			break;
		
		case 0x06://暂停-继续播放
			cmdCan.ID = MakeFeimoCanId(0,0x67,0,0,0x28,buf[9]);
			cmdCan.Buf[0] = buf[10];//单播指令
			cmdCan.Len = 1;
			CanSendData(CanBusBelong(buf[9]),cmdCan.ID,cmdCan.Buf,cmdCan.Len);
			break;
		
		case 0x07://对讲组播
			
			break;
		
		case 0x08://配置广播组
			UpdateBoardCastGroupConfig(&buf[9]);
			ReadBoardCastGroupConfg();
			break;
	}
}


