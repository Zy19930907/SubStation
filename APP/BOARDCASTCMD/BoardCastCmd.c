#include "BoardCastCmd.h"
//֡ͷ55AA55AA ֡����XX XX ָ����XX ָ���XX XX ָ��������
void HandleBoardCastCmd(u8 *buf,u8 sn,u16 len)
{
	u8 i,destAddr;
	u16 framLen,cmdLen,musicIndex;
	
	if(!CheckCrc16(buf,len-2,0))//crcУ�����
		return;
	
	framLen = *(u16*)&buf[4];
	if(framLen != len)//֡���ȴ���
		return;
	
	cmdLen = *(u16*)&buf[6];
	
	switch(buf[4])
	{
		case 0x01://����վ��������
			break;
		
		case 0x02://���㲥��������
			break;

		case 0x03://���㲥�Խ�
			break;
		
		case 0x04://���������鲥
			break;
		
		case 0x05://�Խ��鲥
			break;
		
		case 0x06://���ù㲥��
			break;
	}
}


