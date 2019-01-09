#include "MP3Pro.h"

_MP3Player MP3Player;
u32 TASKID_MP3TASK = 0xFFFFFFFF;
u8 tb[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};

void SetVolum(void)
{
	u16 temp;
	temp = MP3Player.Volum;
	temp <<= 8;
	temp |= MP3Player.Volum;
	VS1003_Voice(temp);
}

void ResetMp3Player(void)
{
	MP3Player.Status = MP3INIT;
	MP3Player.PlayCnt = 0;
	MP3Player.SongSize = 0;
	MP3Player.SongCnt = 0;
	VS1003_Voice(0xFEFE);
}

void SendMP3Pack(void)
{
	u8 i;
	SetSpiSpeed(9000000);  
	SET_VS_XDCS(0);
	for(i=0;i<MP3PACKLEN;i++)
	{
		MP3Player.SendByte(MP3Player.Buf[MP3Player.R++]);
	}
	SET_VS_XDCS(1);
}

void MP3Pro(void)
{
	UINT t;
	switch(MP3Player.Status)
	{
		case MP3INIT:
			VS1003Init();//VS1003芯片初始化
			SetVolum();
			MP3Player.PlayerIdle = VS1003Idle;
			MP3Player.SendByte = VS1003SendByte;
			MP3Player.Status = MP3LOADDIR;
			break;
		
		case MP3IDLE:
			
			break;
		
		case MP3LOADDIR:
			f_closedir(&MP3Player.Mp3Dir);
			f_opendir(&MP3Player.Mp3Dir,"0:/Musics");
			MP3Player.Status = MP3LOADFILENAME;
			break;
		
		case MP3LOADFILENAME:
			if(!f_readdir(&MP3Player.Mp3Dir,&MP3Player.fileinfo))
			{
				strcpy(MP3Player.MusicPath,"0:/Musics/");
				strcat(MP3Player.MusicPath,MP3Player.fileinfo.fname);
				MP3Player.Status = MP3LOADSONG;
			}else
				MP3Player.Status = MP3LOADDIR;
			break;
		
		case MP3LOADSONG:
			f_open(&MP3Player.MusicFil,MP3Player.MusicPath,FA_READ);
			MP3Player.SongSize = MP3Player.MusicFil.obj.objsize;
			MP3Player.SongCnt = MP3Player.SongSize / MP3BUFLEN;
			MP3Player.SongRemain = MP3Player.SongSize % MP3BUFLEN;
			MP3Player.PlayCnt = 0;
			MP3Player.lastFlag = 0;
			MP3Player.Status = MP3READSONG;
			break;
		
		case MP3READSONG:
			if(MP3Player.PlayCnt < MP3Player.SongCnt)
			{
				f_read(&MP3Player.MusicFil,&MP3Player.Buf[0],MP3BUFLEN,&t);
				MP3Player.Status = MP3PLAY;
				MP3Player.PlayCnt++;
			}else if((MP3Player.PlayCnt == MP3Player.SongCnt) && (MP3Player.SongRemain != 0))
			{
				f_read(&MP3Player.MusicFil,&MP3Player.Buf[0],MP3Player.SongRemain,&t);
				memset(&MP3Player.Buf[MP3Player.SongRemain],0,(MP3BUFLEN - MP3Player.SongRemain));//不足一包部分填0
				MP3Player.Status = MP3PLAY;
				MP3Player.lastFlag = 1;
				MP3Player.PlayCnt++;
			}
			else
			{
				f_close(&MP3Player.MusicFil);
				MP3Player.Status = MP3LOADFILENAME;
			}
			break;
		
		case MP3PLAY:
			SET_VS_EN(1);
			if(MP3Player.PlayerIdle())
				SendMP3Pack();
			if((MP3Player.R >= MP3BUFLEN) || (MP3Player.lastFlag == 1 && MP3Player.R >= MP3Player.SongRemain))
			{
				MP3Player.R = 0;
				MP3Player.Status = MP3READSONG;
			}
			break;
			
		case MP3PAUSE:
			
			break;
		
		case MP3END:
			ResetMp3Player();
			DelTask(&TASKID_MP3TASK);
			break;
	}
}

//CAN指令处理
void CanMp3Handler(u32 ID,u8 *buf,u8 len)
{
	if(ID == 0x00AA55AA)
	{
		switch(buf[0])
		{
			case 0x01:

				break;
			
			case 0x02:
				MP3Player.Volum = buf[1];
				SetVolum();
				break;
			
			case 0x03:
				MP3Player.Status = MP3PAUSE;
				SET_VS_EN(0);
				break;
			
			case 0x04:
				MP3Player.Status = MP3PLAY;
				SET_VS_EN(1);
				break;
			
			case 0x05:
				Sys_Soft_Reset();
				break;
			
			case 0x06:
				MP3Player.PlayCnt = buf[1];
				MP3Player.PlayCnt <<= 8;
				MP3Player.PlayCnt |= buf[2];
				MP3Player.PlayCnt <<= 8;
				MP3Player.PlayCnt |= buf[3];
				MP3Player.PlayCnt <<= 8;
				MP3Player.PlayCnt |= buf[4];
				break;
		}
	}
}
