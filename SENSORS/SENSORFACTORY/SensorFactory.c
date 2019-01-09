#include "SensorFactory.h"

u16 InfoLen;
char valueStr[30];

const char *sensorTypes[]={"�����Ũ","����ߵ�Ũ","�������","�������","�ܵ�һ����̼","����","��ѹ","��ʪ��","����",
								"������̼","����","ѹ��","�۳�","Һλ","����","��������",
								"","","","","","","","","","","","","","",
								"����","�ϵ�������","���Ʒ�վ","�ź�ת����","��ͣ","��Դ","������","������","�쿨��","��ʶ��",
								"�㲥�ն�","������","��Ͳ����","��������","������"};
 
const char *units[] = {"%%","%%","%%","%%","ppm","%%","Kpa","��","m/s","%%","%%","Kpa","mg/m3","m","","",
								"","","","","","","","","","","","","","",
								"","","","","","","","","","","","","","","",
							}; 
const char *TriggerType[] = {"","�豸1̬","�豸0̬","�豸����","���ޱ���","���޶ϵ�","���ޱ���","���޶ϵ�"};
//С����λ��
const char *pointfomat[] = {"%4.0f","%4.1f","%4.2f","%4.3f"};
//CAN���߱��
const char *canCodes[] = {"1;","2;","3;","4;","5;","6;"};
//����״̬
const char *linkStatus[] ={"����;","�ж�;"};
//�ϵ���״̬
const char *breakStatus[] = {"����|�е�;","�ϵ�|�е�;","����|�޵�;","�ϵ�|�޵�;"};

const char *valueStatus[] = {"��������;","����Ԥ��;","�궨;","��ʷ����;"};

//����С����λ�����س���
u16 GetDivisor(u8 flag)
{
	u16 Divisor[4] = {1, 10, 100, 1000};
	return *(Divisor+flag);
}

//ģ�������������ֵתΪ�ַ���
void MakeAnalogValue(u8 type,u8 status,u16 value)
{
	float val;
	u8 point;
	u8 valType;
	char fmtcmd[6] = {0};
		
	//���������ֵת�ַ���
	val = (float)value;
	point = status & 0x03;
	val /= GetDivisor(point);//���ֵת��Ϊ������
	strcpy(fmtcmd,pointfomat[point]);//����С����λ��ѡ���ʽ���Ʒ�
	strcat(fmtcmd,units[type]);//���ɸ�ʽ���Ʒ������ݴ���������������ݵ�λ
	
	sprintf(valueStr,fmtcmd,val);//���ֵ�ɸ�����ת��Ϊ�ַ���
	valType = ((status >> 2) & 0x03);
	if(valType)
		strcpy(valueStr,valueStatus[valType]);
	InfoLen += strlen(valueStr);
}

//��ͣ���������ֵת��Ϊ�ַ���
void MakeOpenStopValue(u8 val)
{
	if(val & 0x01)
		strcpy(valueStr,"ͣ");
	else
		strcpy(valueStr,"��");
	InfoLen += 2;
}

//�ϵ���״̬ת��Ϊ�ַ���
void MakeBreakerInfo(u8 status)
{
	strcpy(valueStr,breakStatus[status & 0x03]);
	InfoLen += strlen(valueStr);
}

void MakeBoardCastInfo(u8 status,u8 val)
{
	float vol;
	(status & 0x01)?	strcpy(valueStr,"æµ") : strcpy(valueStr,"����");
	vol = (((float)val) / 10);
	sprintf(&valueStr[strlen(valueStr)],"(%02.1fV)",vol);
	InfoLen += strlen(valueStr);
}

u16 GetSensorValue(u8 addr)
{
	u16 val;
	_Sensor Sensor = Device[addr-1];
	val = Sensor.Buf[1];
	val <<= 8;
	val += Sensor.Buf[0];
	val /= GetDivisor(Sensor.Status & 0x03);
	return val;
}

void GetSensorName(u8 index,char *name)
{
	_Sensor *Sensor;
	Sensor = &Device[index];
	sprintf(name,"%d-",index+1);
	if(Sensor->Flag & 0x01)//��λ���Ѷ���
	{
		if(Sensor->Name == 0xFF)//δ�ҽӴ�����
		{
			strcat(name,"δ����");
			return;
		}
	}
	else
		strcpy(name,"δ֪");
}

void GetActType(u8 trigger,char *buf)
{
	u8 i,k=0;
	for(i=0;i<8;i++)
	{
		if((trigger << i) & 0x80)
		{
			if(k)
			{
				strcat(buf,"-");
				strcat(buf,TriggerType[i]);
				k++;
			}
			else
			{
				strcpy(buf,TriggerType[i]);
				k++;
			}
		}
	}
}

void MakeSensorInfo_Name_Val(u8 index,char *nameBuf,char *valueBuf,u8 *color)
{
	u16 value;
	_Sensor *Sensor;
	Sensor = &Device[index];
	if(Sensor->Flag & 0x01)//��λ���Ѷ���
	{
		sprintf(nameBuf,"%02d ",index+1);
		strcpy(valueBuf,"-----");
		if(Sensor->Name == 0xFF)//��λ���Ѷ��嵫CAN������δ�ҽ�
		{
			strcat(nameBuf,"δ����");
			*color = 0x00;
			return;
		}
		
		strcat(nameBuf,sensorTypes[Sensor->Name]);//����������
		if(Sensor->Status & 0x80)//���������ߣ��յ�������������֮����ߣ�
		{
			*color = 0x01;
			return;
		}
			
		if(!Sensor->ShowFlag & 0x01)//��ǰ���ֵδ����
			*color = 0x02;
		else
			*color = 0x03;//���ֵ����
		
		//���������ֵ
		value = Sensor->Buf[1];
		value <<= 8;
		value += Sensor->Buf[0];
		
		switch(Sensor->Name)
		{
			//ģ����������
			case SENSOR_CH4LOW:
			case SENSOR_CH4HL:
			case SENSOR_CH4INFR:
			case SENSOR_CH4LASER:
			case SENSOR_O2:	
			case SENSOR_CO: 
			case SENSOR_NATIVEPRE:
			case SENSOR_TEMP:
			case SENSOR_WINDSPEED:
			case SENSOR_CO2:
			case SENSOR_H2S:
			case SENSOR_PRESS:
			case SENSOR_DUST:
			case SENSOR_LQLEVEL: MakeAnalogValue(Sensor->Name,Sensor->Status,value); break;
			case SENSOR_SMOKE:break;
			case SENSOR_WINDDOOR:break;
			case SENSOR_BREAKER: MakeBreakerInfo(Sensor->Status); break;
			case SENSOR_SWVALUE: MakeOpenStopValue(value); break;//��ͣ������
			case SENSOR_POWER:break;
			case SENSOR_READER:break;
			case SENSOR_CARDWRITER:break;
			case SENSOR_CARDCHECKER:break;
			case SENSOR_IDCARD:break;
			case SENSOR_BOARDCAST:MakeBoardCastInfo(Sensor->Status,value);break;
			case SENSOR_AIRDUCT:break;
			case SENSOR_LOCKGATE:break;
			case SENSOR_WARNER:break;
		}
		strcpy(valueBuf,valueStr);
	}
	else//��λ��δ����
	{
		sprintf(nameBuf,"%02d δ����",index+1);
		strcpy(valueBuf,"----");
		*color = 0x04;
	}
}

u8 GetLinkInfo(u8 index,char *triggerAddrName,char *excuteAddrName,char *excuteType)
{
	u8 i,k=0;
	
	_Excute excute;
	_Sensor TriggerSensor;
	_Sensor ExcuteSensor;
	excute = Excute[index];
	
	if(excute.ExcuteAddr == 0)
		return 0;
	
	TriggerSensor = Device[excute.TriggerAddr-1];
	ExcuteSensor = Device[excute.ExcuteAddr-1];
	
	TriggerSensor.Name == 0xFF ? sprintf(triggerAddrName,"%d",excute.TriggerAddr) : sprintf(triggerAddrName,"%d-%s",excute.TriggerAddr,sensorTypes[TriggerSensor.Name]);
	ExcuteSensor.Name == 0xFF ? sprintf(excuteAddrName,"%d",excute.ExcuteAddr) : sprintf(excuteAddrName,"%d-%s",excute.TriggerAddr,sensorTypes[TriggerSensor.Name]);
	
	for(i=0;i<8;i++)
	{
		if((excute.Trigger << i) & 0x80)
		{
			if(k)
			{
				strcat(excuteType,"-");
				strcat(excuteType,TriggerType[i]);
				k++;
			}
			else
			{
				strcpy(excuteType,TriggerType[i]);
				k++;
			}
		}
	}
	return 1;
}

void *SensorMalloc(u32 size)
{
	return mymalloc(SRAMEX,size);
}

void SensorFree(void *ptr)
{
	myfree(SRAMEX,ptr);
}
