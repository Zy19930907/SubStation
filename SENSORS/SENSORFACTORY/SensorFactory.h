#ifndef __SENSORFACTORY_H
#define __SENSORFACTORY_H

#include "Public.h"

#define SENSOR_CH4LOW					0x00 //甲烷低浓度
#define SENSOR_CH4HL					0x01 //甲烷高低浓
#define SENSOR_CH4INFR					0x02 //红外甲烷
#define SENSOR_CH4LASER					0x03 //激光甲烷
#define SENSOR_CO						0x04 //一氧化碳
#define SENSOR_O2						0x05 //氧气
#define SENSOR_NATIVEPRE				0x06 //负压传感器
#define SENSOR_TEMP						0x07 //温湿度传感器
#define SENSOR_WINDSPEED				0x08 //风速传感器
#define SENSOR_CO2						0x09 //二氧化碳
#define SENSOR_H2S						0x0A //硫化氢传感器
#define SENSOR_PRESS					0x0B //压力传感器
#define SENSOR_DUST						0x0C //粉尘传感器
#define SENSOR_LQLEVEL					0x0D //液位传感器
#define SENSOR_SMOKE					0x0E //烟雾传感器
#define SENSOR_WINDDOOR					0x0F //语音风门
#define SENSOR_BREAKER					0x1F //断电馈电器
#define SENSOR_SWVALUE					0x22 //开停传感器
#define SENSOR_POWER					0x23 //电源
#define SENSOR_READER					0x24 //读卡器
#define SENSOR_CARDWRITER				0x25 //发卡器
#define SENSOR_CARDCHECKER				0x26 //检卡器
#define SENSOR_IDCARD					0x27 //标识卡
#define SENSOR_BOARDCAST				0x28 //广播终端
#define SENSOR_AIRDUCT					0x2A //风筒风量
#define SENSOR_LOCKGATE					0x2B //闭锁网关
#define SENSOR_WARNER					0x2C //声光报警器

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
