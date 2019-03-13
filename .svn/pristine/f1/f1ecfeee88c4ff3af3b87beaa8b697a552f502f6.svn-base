#ifndef __KEYPRO_H
#define __KEYPRO_H

#include "Public.h"

typedef enum
{
	KEYINIT,
	KEYIDLE,
	KEYPRESS,
	KEYLONGPRESS,
	KEYUP,
	KEYSHORT,
	KEYLONG,
}_KeyStatus;

typedef struct
{
	_KeyStatus Status;
	u8 Value[2];
	u8 ScanCnt;
	u32 Tick;
	u32 LTick;
}_Key;

void KeyPro(void);
	
#endif
