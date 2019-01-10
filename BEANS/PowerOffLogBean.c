#include "PowerOffLogBean.h"

cJSON *PowerOffLogBean;
//������ת��ΪJson����
void JsonBytesToLogBean(char *logBuf)
{
	PowerOffLogBean = cJSON_Parse(logBuf);
}
//�ͷ�Json����ռ���ڴ�
void DelatePowerOffLogBean(void)
{
	cJSON_Delete(PowerOffLogBean);
}
//��ȡJson�����еĶϵ�ʱ��
void GetPowerOffTime(char *time)
{
	cJSON *pItem;
	pItem = cJSON_GetObjectItem(PowerOffLogBean,POWEROFFTIMKEY);
	strcpy(time,pItem->valuestring);
}
//��ȡJson�����еĶ�������
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
//��JSON�����л�ȡ�ϵ���Ϣ
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
//�����ϵ���־JSON����
void CreatePowerOffBean(char *time,char *triggerInfo,char *type)
{
	PowerOffLogBean = cJSON_CreateObject();
	cJSON_AddItemToObject(PowerOffLogBean,POWEROFFTIMKEY,cJSON_CreateString(time));//�ϵ�ʱ��
	cJSON_AddItemToObject(PowerOffLogBean,POWEROFFTRIGGERINFO,cJSON_CreateString(triggerInfo));//�ϵ�ʱ��
	cJSON_AddItemToObject(PowerOffLogBean,POWEROFFTYPE,cJSON_CreateString(type));//�ϵ�ʱ��
}
//�ϵ���־JSON����ת��ΪJSON�ַ���
void PowerOffLogBeanToJsonBytes(char *jsonBytes)
{
	char *p;
	p = cJSON_PrintUnformatted(PowerOffLogBean);
	strcpy(jsonBytes,p);
	myfree(SRAMIN,p);
}
