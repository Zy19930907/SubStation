#ifndef __POWEROFFLOGBEAN_H
#define __POWEROFFLOGBEAN_H

#include "Public.h"
//json键定义，取值时使用
#define POWEROFFTIMKEY			"bkTime"//断电时间
#define POWEROFFTRIGGERINFO		"tgDev"//触发断电的传感器信息
#define POWEROFFTYPE			"actType"//触发断电的事件类型

void DelatePowerOffLogBean(void);
void JsonBytesToLogBean(char *logBuf);
void GetPowerOffTime(char *time);
void GetPowerOffTriggerInfo(char *triggerInfo);
void GetPowerOffType(char *type);
void GetPowerOffInfo(char *time,char *triggerInfo,char *type);
void CreatePowerOffBean(char *time,char *triggerInfo,char *type);
void PowerOffLogBeanToJsonBytes(char *jsonBytes);

#endif

