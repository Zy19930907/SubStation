#ifndef __SENSORFACTORY_H
#define __SENSORFACTORY_H

#include "Public.h"

#define SENSOR_CH4LOW					0x00 //�����Ũ��
#define SENSOR_CH4HL					0x01 //����ߵ�Ũ
#define SENSOR_CH4INFR					0x02 //�������
#define SENSOR_CH4LASER					0x03 //�������
#define SENSOR_CO						0x04 //һ����̼
#define SENSOR_O2						0x05 //����
#define SENSOR_NATIVEPRE				0x06 //��ѹ������
#define SENSOR_TEMP						0x07 //��ʪ�ȴ�����
#define SENSOR_WINDSPEED				0x08 //���ٴ�����
#define SENSOR_CO2						0x09 //������̼
#define SENSOR_H2S						0x0A //���⴫����
#define SENSOR_PRESS					0x0B //ѹ��������
#define SENSOR_DUST						0x0C //�۳�������
#define SENSOR_LQLEVEL					0x0D //Һλ������
#define SENSOR_SMOKE					0x0E //��������
#define SENSOR_WINDDOOR					0x0F //��������
#define SENSOR_BREAKER					0x1F //�ϵ�������
#define SENSOR_SWVALUE					0x22 //��ͣ������
#define SENSOR_POWER					0x23 //��Դ
#define SENSOR_READER					0x24 //������
#define SENSOR_CARDWRITER				0x25 //������
#define SENSOR_CARDCHECKER				0x26 //�쿨��
#define SENSOR_IDCARD					0x27 //��ʶ��
#define SENSOR_BOARDCAST				0x28 //�㲥�ն�
#define SENSOR_AIRDUCT					0x2A //��Ͳ����
#define SENSOR_LOCKGATE					0x2B //��������
#define SENSOR_WARNER					0x2C //���ⱨ����

u16 MakeSensorInfo(u8 *sensorbuf,u8 *infobuf);
//void MakeSensorInfo_Name_Val(u8 *sensorbuf,char *nameBuf,char *valBuf,u8 *color,u8 flag);
void MakeSensorInfo_Name_Val(u8 index,char *nameBuf,char *valueBuf,u8 *color);
u8 GetLinkInfo(u8 index,char *triggerAddrName,char *excuteAddrName,char *excuteType);
void *SensorMalloc(u32 size);
void SensorFree(void *ptr);
u16 GetDivisor(u8 flag);
void OpenSensorFactory(void);
void CloseSensorFactory(void);
u16 GetSensorValue(u8 addr);
void GetSensorName(u8 index,char *name);
void GetActType(u8 trigger,char *buf);

#endif
