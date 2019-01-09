#ifndef __CRCMAKER_H
#define __CRCMAKER_H

#include "Public.h"

#define CRCGENP   0xA001

u16 CalCrc16(u8 *buf,u16 len,u8 flag);
void SetCrc16(u8 *buf,u16 len,u8 flag);
u8 CheckCrc16(u8 *buf,u16 len,u8 flag);
u8 SumAll(u8 *buf,u8 len);
u8 SumCheck(u8 *buf,u8 len);

#endif
