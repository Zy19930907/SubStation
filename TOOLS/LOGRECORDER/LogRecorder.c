#include "LogRecorder.h"
#include "fattester.h"	
#include "DateTool.h"
#include "PowerOffLogBean.h"

char BreakerType[][10] = {"","1态断电","0态断电","断线断电","下限报警","下限断电","上限报警","上限断电"};

//检查SD卡中是否存在日志文件夹
void CheckLogDirs(void)
{
	if(mf_opendir("0:/Logs"))//日志文件夹
	{
		mf_mkdir("0:/Logs");
		mf_opendir("0:/Logs");
	}
	if(mf_opendir("0:/Logs/BreakerLogs"))//断电日志文件夹
	{
		mf_mkdir("0:/Logs/BreakerLogs");
		mf_opendir("0:/Logs/BreakerLogs");
	}
}

void SaveLog(char *path,char *log,u16 logLen)
{
	u32 size;
	size = mf_open(path,FA_OPEN_ALWAYS | FA_WRITE);
	mf_lseek(size);//文件读写指针指向文件末尾，向文件追加内容
	mf_write((char*)&logLen,2);
	mf_write(log,strlen(log));
	mf_close();
}

////断电器断电记录格式：时间戳+断电触发传感器地址+断电触发事件
////BreakerAddr---执行断电的断电器地址 
////TriggerAddr---触发断电的传感器地址
////warn---触发断电的事件类型
//void SaveBreakPowerLog(u8 BreakerAddr,u8 TriggerAddr,u8 warn)
//{
//	u8 i,k = 0;
//	char BreakerLogPath[100] = "0:/Logs/BreakerLogs/";//断电日志文件夹
//	char BreakInfo[100] = {0};
//	
//	u16 logLen;
//	GetDateH_M_S(&BreakInfo[0]);//获取当前时间（时:分:秒）
//	
//	sprintf(&BreakInfo[strlen(BreakInfo)],"    触发地址：%d---断电类型：",TriggerAddr);
//	
//	//添加触发断电事件类型
//	for(i=0;i<8;i++)
//	{
//		if(warn & 0x80)
//		{
//			if(k)
//				strcat(BreakInfo," | ");
//			strcat(BreakInfo,BreakerType[i]);
//			k++;
//		}
//		warn <<= 1;
//	}
//	strcat(BreakInfo,"\n");
//	
//	//获取当前日期作为文件夹名,每天一个日志文件夹
//	GetDateY_M_D(&BreakerLogPath[strlen(BreakerLogPath)]);
//	if(mf_opendir(BreakerLogPath))
//		mf_mkdir(BreakerLogPath);
//	sprintf(&BreakerLogPath[strlen(BreakerLogPath)],"/%d#断电日志.txt",BreakerAddr);
//	logLen = strlen(BreakInfo);
//	SaveLog(BreakerLogPath,BreakInfo,logLen);
//}

//断电器断电记录格式：时间戳+断电触发传感器地址+断电触发事件
//BreakerAddr---执行断电的断电器地址 
//TriggerAddr---触发断电的传感器地址
//warn---触发断电的事件类型
void SaveBreakPowerLog(u8 BreakerAddr,u8 TriggerAddr,u8 warn)
{
	char BreakerLogPath[100] = "0:/Logs/BreakerLogs/";//断电日志文件夹
	char time[30] = {0};
	char triggerInfo[50];
	char type[100];
	char log[500];
	u16 logLen;
	
	GetDateH_M_S(&time[0]);//获取当前时间（时:分:秒）
	GetSensorName(TriggerAddr-1,triggerInfo);
	GetActType(warn,type);
	CreatePowerOffBean(time,triggerInfo,type);
	PowerOffLogBeanToJsonBytes(log);
	strcat(log,"\n");
	
	//获取当前日期作为文件夹名,每天一个日志文件夹
	GetDateY_M_D(&BreakerLogPath[strlen(BreakerLogPath)]);
	if(mf_opendir(BreakerLogPath))//文件夹不存在
		mf_mkdir(BreakerLogPath);//创建文件夹
	sprintf(&BreakerLogPath[strlen(BreakerLogPath)],"/%d#断电器.txt",BreakerAddr);
	logLen = strlen(log);
	SaveLog(BreakerLogPath,log,logLen);
	DelatePowerOffLogBean();
}

void OpenLogFile(char *path)
{
	mf_open(path,FA_OPEN_EXISTING | FA_READ);
}

u16 ReadBreakPowerLog(char *buf)
{
	u16 len = 0;
	char log[2];
	if(!mf_read((char*)&log,2))
	{
		len = log[1];
		len <<= 8;
		len += log[0];
		mf_read(buf,len);
	}else
		return 0;
	return len;
}
