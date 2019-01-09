#include "SensorFactory.h"

u16 InfoLen;
char valueStr[30];

const char *sensorTypes[]={"甲烷低浓","甲烷高低浓","红外甲烷","激光甲烷","管道一氧化碳","氧气","负压","温湿度","风速",
								"二氧化碳","硫化氢","压力","粉尘","液位","烟雾","语音风门",
								"","","","","","","","","","","","","","",
								"网关","断电馈电器","控制分站","信号转换器","开停","电源","读卡器","发卡器","检卡器","标识卡",
								"广播终端","控制量","风筒风量","闭锁网关","报警器"};
 
const char *units[] = {"%%","%%","%%","%%","ppm","%%","Kpa","℃","m/s","%%","%%","Kpa","mg/m3","m","","",
								"","","","","","","","","","","","","","",
								"","","","","","","","","","","","","","","",
							}; 
const char *TriggerType[] = {"","设备1态","设备0态","设备断线","下限报警","下限断电","上限报警","上限断电"};
//小数点位数
const char *pointfomat[] = {"%4.0f","%4.1f","%4.2f","%4.3f"};
//CAN总线编号
const char *canCodes[] = {"1;","2;","3;","4;","5;","6;"};
//连接状态
const char *linkStatus[] ={"正常;","中断;"};
//断电器状态
const char *breakStatus[] = {"复电|有电;","断电|有电;","复电|无电;","断电|无电;"};

const char *valueStatus[] = {"正常数据;","开机预热;","标定;","历史数据;"};

//根据小数点位数返回除数
u16 GetDivisor(u8 flag)
{
	u16 Divisor[4] = {1, 10, 100, 1000};
	return *(Divisor+flag);
}

//模拟量传感器监测值转为字符串
void MakeAnalogValue(u8 type,u8 status,u16 value)
{
	float val;
	u8 point;
	u8 valType;
	char fmtcmd[6] = {0};
		
	//传感器监测值转字符串
	val = (float)value;
	point = status & 0x03;
	val /= GetDivisor(point);//监测值转换为浮点数
	strcpy(fmtcmd,pointfomat[point]);//根据小数点位数选择格式控制符
	strcat(fmtcmd,units[type]);//生成格式控制符，根据传感器类型添加数据单位
	
	sprintf(valueStr,fmtcmd,val);//监测值由浮点数转换为字符串
	valType = ((status >> 2) & 0x03);
	if(valType)
		strcpy(valueStr,valueStatus[valType]);
	InfoLen += strlen(valueStr);
}

//开停传感器监测值转换为字符串
void MakeOpenStopValue(u8 val)
{
	if(val & 0x01)
		strcpy(valueStr,"停");
	else
		strcpy(valueStr,"开");
	InfoLen += 2;
}

//断电器状态转换为字符串
void MakeBreakerInfo(u8 status)
{
	strcpy(valueStr,breakStatus[status & 0x03]);
	InfoLen += strlen(valueStr);
}

void MakeBoardCastInfo(u8 status,u8 val)
{
	float vol;
	(status & 0x01)?	strcpy(valueStr,"忙碌") : strcpy(valueStr,"空闲");
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
	if(Sensor->Flag & 0x01)//上位机已定义
	{
		if(Sensor->Name == 0xFF)//未挂接传感器
		{
			strcat(name,"未连接");
			return;
		}
	}
	else
		strcpy(name,"未知");
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
	if(Sensor->Flag & 0x01)//上位机已定义
	{
		sprintf(nameBuf,"%02d ",index+1);
		strcpy(valueBuf,"-----");
		if(Sensor->Name == 0xFF)//上位机已定义但CAN总线上未挂接
		{
			strcat(nameBuf,"未连接");
			*color = 0x00;
			return;
		}
		
		strcat(nameBuf,sensorTypes[Sensor->Name]);//传感器类型
		if(Sensor->Status & 0x80)//传感器断线（收到过传感器数据之后断线）
		{
			*color = 0x01;
			return;
		}
			
		if(!Sensor->ShowFlag & 0x01)//当前监测值未超限
			*color = 0x02;
		else
			*color = 0x03;//监测值超限
		
		//传感器监测值
		value = Sensor->Buf[1];
		value <<= 8;
		value += Sensor->Buf[0];
		
		switch(Sensor->Name)
		{
			//模拟量传感器
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
			case SENSOR_SWVALUE: MakeOpenStopValue(value); break;//开停传感器
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
	else//上位机未定义
	{
		sprintf(nameBuf,"%02d 未定义",index+1);
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
