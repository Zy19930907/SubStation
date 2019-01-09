#ifndef __TOLKPRO_H
#define __TOLKPRO_H

#include "Public.h"
typedef enum
{
	TOLKUP,
	TOLKDOWN,
}_TolkDir;

typedef enum
{
	TOLKINIT,
	TOLKIDLE,
	TOLKING,
}_TolkStatus;

typedef struct
{
	_TolkStatus Status;
	_TolkDir Dir;
	u8 DevFlag;
	u32 Tick;
	u32 TolkCnt;
	u8 VoiceRecv[48];
	u8 VoiceLen;
	u8 VoiceSend[48];
	u8 VoiceSendLen;
	u8(*SendTolk)(u32 ID,u8 *buf,u8 len);
	void(*PlayTolk)(u8 *buf,u8 len);
	void(*SetVolum)(u8 volum);
}_TolkManger;
extern _TolkManger TolkManger;
void TolkKeyDeal(u8 mode,u8 val);
void TolkPro(void);
void CanTolkHandler(u32 ID,u8 *buf,u8 len);
void UpTolkHandler(u8 *buf,u8 len);
void Tolk_WifiCmdDeal(u8 *buf,u16 len);

#endif
