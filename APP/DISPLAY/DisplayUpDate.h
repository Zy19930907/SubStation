#ifndef __DISPLAYUPDATE_H
#define __DISPLAYUPDATE_H

#include "Public.h"

#define QUEENUPDATEPACKLEN				2052

typedef enum
{
	QUEENUPDATEINIT,
	QUEENLOADUPDATEFILE,
	QUEENSENDUPDATECMD,
	QUEENREADUPDATEDATE,
	QUEENSENDUPDATEPACK,
	QUEENWAITACK,
	QUEENWAITACK1,
	QUEENUPDATEFAIL,
	QUEENUPDATESUCCESS,
}_QueenUpdateStatus;

typedef struct
{
	_QueenUpdateStatus Ststus;
	u8 UpDateCmd[15];
	u8* DataPack;
	u32 Tick;
	u8 Fram;
	u32 FileSize;
	u32 FileCnt;
	u32	Remain;
	u32 SendCnt;
	u32 PackLen;
	u8 Retry;
	void(*SendData)(u8 *buf,u16 len);
}_QueenUpdate;

extern u32 TASKID_QueenUpdate;

void QueenUpdatePro(void);

#endif
