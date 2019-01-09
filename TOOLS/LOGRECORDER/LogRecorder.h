#ifndef __LOGCORDER_H
#define __LOGCORDER_H

#include "Public.h"

void SaveLog(char *path,char *log,u16 logLen);
void OpenLogFile(char *path);
u16 ReadBreakPowerLog(char *buf);
void SaveBreakPowerLog(u8 BreakerAddr,u8 TriggerAddr,u8 warn);
void CheckLogDirs(void);

#endif
