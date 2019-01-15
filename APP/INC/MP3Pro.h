#ifndef __MP3PRO_H
#define __MP3PRO_H

#include "Public.h"

#define MP3BUFLEN				512
#define MP3PACKLEN				32

typedef enum
{
	MP3INIT,
	MP3IDLE,
	MP3LOADDIR,
	MP3LOADFILENAME,
	MP3LOADSONG,
	MP3READSONG,
	MP3PLAY,
	MP3PAUSE,
	MP3END,
}_MP3Status;

typedef struct
{
	_MP3Status Status;
	DIR Mp3Dir;
	FILINFO fileinfo;
	char MusicPath[100];
	FIL MusicFil;
	u8 Buf[MP3BUFLEN];
	u16 R;
	u8 Volum;
	uint64_t SongSize;
	u32 SongCnt;
	u32 PlayCnt;
	u16 SongRemain;
	u16 SendLen;
	u8 lastFlag;
	u8 (*SendByte)(u8 data);
	u8 (*PlayerIdle)(void);
	char *fn;
}_MP3Player;

extern u32 TASKID_MP3TASK;
extern _MP3Player MP3Player;

void MP3Pro(void);
void MP3PlayerSet(u8 mode, u8 val);
void MP3_WifiCmdDeal(u8 *buf,u16 len);
void CanMp3Handler(u32 ID,u8 *buf,u8 len);
void ResetMp3Player(void);
void SetVolum(void);
void StartPlayMusic(u16 musicIndex);
void StopMusic(void);
void PauseMusic(void);

#endif
