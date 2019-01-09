#ifndef __NETDATAPRO_H
#define __NETDATAPRO_H

#include "Public.h"
#define	ACK					0x10

typedef enum
{
     Start,
     Process,
     End,
}_LianDongStatus;

void HandleNetData(u8 Socketn,u8 *buf,u16 len,u8 flag);
void CanInfoToNet(u8 sn, u8 cmd, u8 *buf, u8 len);
#endif
