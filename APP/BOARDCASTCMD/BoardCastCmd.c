#include "BoardCastCmd.h"
//֡ͷ55AA55AA ֡����XX XX ָ����XX ָ���XX XX ָ��������
void HandleBoardCastCmd(u8 *buf,u16 len,u8 sn)
{
	u8 i,j,destAddr;
	u16 framLen,cmdLen,musicIndex,addr;
	
	_Can cmdCan;
	
	if(!CheckCrc16(buf,len-2,0))//crcУ�����
		return;
	
	framLen = *(u16*)&buf[4];
	if(framLen != len)//֡���ȴ���
		return;
	
	cmdLen = *(u16*)&buf[7];//ָ�������򳤶�
	
	switch(buf[6])
	{
		case 0x01://����վ��������(ָ����0x01,ָ���2,ָ��������������ţ�2�ֽ�)
			musicIndex = *(u16*)&buf[9];
			StartPlayMusic(musicIndex);
			break;
		
		case 0x02://����վ�Խ�
			
			break;

		case 0x03://���㲥��������
			cmdCan.ID = MakeFeimoCanId(0,0x65,0,0,0x28,buf[9]);
			cmdCan.Buf[0] = buf[10];//�������
			cmdCan.Buf[1] = buf[11];
			cmdCan.Len = 2;
			CanSendData(CanBusBelong(buf[9]),cmdCan.ID,cmdCan.Buf,cmdCan.Len);
			break;
		
		case 0x04://���㲥�Խ�
			cmdCan.ID = MakeFeimoCanId(0,0x09,0,0,0x28,buf[9]);
			cmdCan.Buf[0] = 0x04;//����ָ��
			BufCmp(&cmdCan.Buf[1],&buf[10],6);
			cmdCan.Len = 7;
			CanSendData(CanBusBelong(buf[9]),cmdCan.ID,cmdCan.Buf,cmdCan.Len);
			break;
		
		case 0x05://���������鲥
			
			break;
		
		case 0x06://�Խ��鲥
			
			break;
		
		case 0x07://���ù㲥��
			UpdateBoardCastGroupConfig(&buf[9]);
			ReadBoardCastGroupConfg();
			break;
	}
}


