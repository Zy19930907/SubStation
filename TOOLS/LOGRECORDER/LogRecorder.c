#include "LogRecorder.h"
#include "fattester.h"	
#include "DateTool.h"
#include "PowerOffLogBean.h"

char BreakerType[][10] = {"","1̬�ϵ�","0̬�ϵ�","���߶ϵ�","���ޱ���","���޶ϵ�","���ޱ���","���޶ϵ�"};

//���SD�����Ƿ������־�ļ���
void CheckLogDirs(void)
{
	if(mf_opendir("0:/Logs"))//��־�ļ���
	{
		mf_mkdir("0:/Logs");
		mf_opendir("0:/Logs");
	}
	if(mf_opendir("0:/Logs/BreakerLogs"))//�ϵ���־�ļ���
	{
		mf_mkdir("0:/Logs/BreakerLogs");
		mf_opendir("0:/Logs/BreakerLogs");
	}
}

void SaveLog(char *path,char *log,u16 logLen)
{
	u32 size;
	size = mf_open(path,FA_OPEN_ALWAYS | FA_WRITE);
	mf_lseek(size);//�ļ���дָ��ָ���ļ�ĩβ�����ļ�׷������
	mf_write((char*)&logLen,2);
	mf_write(log,strlen(log));
	mf_close();
}

////�ϵ����ϵ��¼��ʽ��ʱ���+�ϵ紥����������ַ+�ϵ紥���¼�
////BreakerAddr---ִ�жϵ�Ķϵ�����ַ 
////TriggerAddr---�����ϵ�Ĵ�������ַ
////warn---�����ϵ���¼�����
//void SaveBreakPowerLog(u8 BreakerAddr,u8 TriggerAddr,u8 warn)
//{
//	u8 i,k = 0;
//	char BreakerLogPath[100] = "0:/Logs/BreakerLogs/";//�ϵ���־�ļ���
//	char BreakInfo[100] = {0};
//	
//	u16 logLen;
//	GetDateH_M_S(&BreakInfo[0]);//��ȡ��ǰʱ�䣨ʱ:��:�룩
//	
//	sprintf(&BreakInfo[strlen(BreakInfo)],"    ������ַ��%d---�ϵ����ͣ�",TriggerAddr);
//	
//	//��Ӵ����ϵ��¼�����
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
//	//��ȡ��ǰ������Ϊ�ļ�����,ÿ��һ����־�ļ���
//	GetDateY_M_D(&BreakerLogPath[strlen(BreakerLogPath)]);
//	if(mf_opendir(BreakerLogPath))
//		mf_mkdir(BreakerLogPath);
//	sprintf(&BreakerLogPath[strlen(BreakerLogPath)],"/%d#�ϵ���־.txt",BreakerAddr);
//	logLen = strlen(BreakInfo);
//	SaveLog(BreakerLogPath,BreakInfo,logLen);
//}

//�ϵ����ϵ��¼��ʽ��ʱ���+�ϵ紥����������ַ+�ϵ紥���¼�
//BreakerAddr---ִ�жϵ�Ķϵ�����ַ 
//TriggerAddr---�����ϵ�Ĵ�������ַ
//warn---�����ϵ���¼�����
void SaveBreakPowerLog(u8 BreakerAddr,u8 TriggerAddr,u8 warn)
{
	char BreakerLogPath[100] = "0:/Logs/BreakerLogs/";//�ϵ���־�ļ���
	char time[30] = {0};
	char triggerInfo[50];
	char type[100];
	char log[500];
	u16 logLen;
	
	GetDateH_M_S(&time[0]);//��ȡ��ǰʱ�䣨ʱ:��:�룩
	GetSensorName(TriggerAddr-1,triggerInfo);
	GetActType(warn,type);
	CreatePowerOffBean(time,triggerInfo,type);
	PowerOffLogBeanToJsonBytes(log);
	strcat(log,"\n");
	
	//��ȡ��ǰ������Ϊ�ļ�����,ÿ��һ����־�ļ���
	GetDateY_M_D(&BreakerLogPath[strlen(BreakerLogPath)]);
	if(mf_opendir(BreakerLogPath))//�ļ��в�����
		mf_mkdir(BreakerLogPath);//�����ļ���
	sprintf(&BreakerLogPath[strlen(BreakerLogPath)],"/%d#�ϵ���.txt",BreakerAddr);
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
