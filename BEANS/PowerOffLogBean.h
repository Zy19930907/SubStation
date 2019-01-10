#ifndef __POWEROFFLOGBEAN_H
#define __POWEROFFLOGBEAN_H

#include "Public.h"
//json�����壬ȡֵʱʹ��
#define POWEROFFTIMKEY			"bkTime"//�ϵ�ʱ��
#define POWEROFFTRIGGERINFO		"tgDev"//�����ϵ�Ĵ�������Ϣ
#define POWEROFFTYPE			"actType"//�����ϵ���¼�����

void DelatePowerOffLogBean(void);
void JsonBytesToLogBean(char *logBuf);
void GetPowerOffTime(char *time);
void GetPowerOffTriggerInfo(char *triggerInfo);
void GetPowerOffType(char *type);
void GetPowerOffInfo(char *time,char *triggerInfo,char *type);
void CreatePowerOffBean(char *time,char *triggerInfo,char *type);
void PowerOffLogBeanToJsonBytes(char *jsonBytes);

#endif

