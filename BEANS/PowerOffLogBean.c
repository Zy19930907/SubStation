#include "PowerOffLogBean.h"

cJSON *PowerOffLogBean;
//将数组转换为Json对象
void JsonBytesToLogBean(char *logBuf)
{
	PowerOffLogBean = cJSON_Parse(logBuf);
}
//释放Json对象占用内存
void DelatePowerOffLogBean(void)
{
	cJSON_Delete(PowerOffLogBean);
}
//获取Json对象中的断电时间
void GetPowerOffTime(char *time)
{
	cJSON *pItem;
	pItem = cJSON_GetObjectItem(PowerOffLogBean,POWEROFFTIMKEY);
	strcpy(time,pItem->valuestring);
}
//获取Json对象中的锻炼类型
void GetPowerOffTriggerInfo(char *triggerInfo)
{
	cJSON *pItem;
	pItem = cJSON_GetObjectItem(PowerOffLogBean,POWEROFFTRIGGERINFO);
	strcpy(triggerInfo,pItem->valuestring);
}

void GetPowerOffType(char *type)
{
	cJSON *pItem;
	pItem = cJSON_GetObjectItem(PowerOffLogBean,POWEROFFTYPE);
	strcpy(type,pItem->valuestring);
}
//从JSON对象中获取断电信息
void GetPowerOffInfo(char *time,char *triggerInfo,char *type)
{
	cJSON *pItem;
	pItem = cJSON_GetObjectItem(PowerOffLogBean,POWEROFFTIMKEY);
	strcpy(time,pItem->valuestring);
	
	pItem = cJSON_GetObjectItem(PowerOffLogBean,POWEROFFTRIGGERINFO);
	strcpy(triggerInfo,pItem->valuestring);
	
	pItem = cJSON_GetObjectItem(PowerOffLogBean,POWEROFFTYPE);
	strcpy(type,pItem->valuestring);
}
//创建断电日志JSON对象
void CreatePowerOffBean(char *time,char *triggerInfo,char *type)
{
	PowerOffLogBean = cJSON_CreateObject();
	cJSON_AddItemToObject(PowerOffLogBean,POWEROFFTIMKEY,cJSON_CreateString(time));//断电时间
	cJSON_AddItemToObject(PowerOffLogBean,POWEROFFTRIGGERINFO,cJSON_CreateString(triggerInfo));//断电时间
	cJSON_AddItemToObject(PowerOffLogBean,POWEROFFTYPE,cJSON_CreateString(type));//断电时间
}
//断电日志JSON对象转换为JSON字符串
void PowerOffLogBeanToJsonBytes(char *jsonBytes)
{
	char *p;
	p = cJSON_PrintUnformatted(PowerOffLogBean);
	strcpy(jsonBytes,p);
	myfree(SRAMIN,p);
}
