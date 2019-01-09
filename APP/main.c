#include "Public.h"

//RUNLED---PE4
#define RUNLEDCHANGE	GPIOE->ODR ^= 1<<4
u32 TASKID_RUNLED 		= 0xFFFFFFFF;
u32 TASKID_SDINIT		= 0xFFFFFFFF;
extern u32 TASKID_MP3TASK;

void UserIoInit(void)
{
	RCC->AHB1ENR |= (1<<4); //PEʱ��ʹ��
	GPIOE->MODER |= (1 << 8);//�������
	GPIOE->OSPEEDR |= (3 << 8);
}

void RunLedTask(void)
{
	RUNLEDCHANGE;
}

void FreshGui(void)
{
	GUI_Delay(1);
}

void FreshSysRtc(void)
{
	GetDateY_M_D_H_M_S(Sys.RtcStr);
}

void SdInit(void)
{
	if(SD_Init())
		return;
	exfuns_init();													//ΪFATFS�����ڴ�
	mf_mount((u8*)"0:/");											//����SD��
	CheckLogDirs();													//���SD�����Ƿ������־�ļ��У�û�����½�
	SetTaskDelay(TASKID_RUNLED,0,0,0,0,500);
//	TASKID_MP3TASK = CreateTask("Mp3Pro",0,0,0,0,0,MP3Pro);			//����MP3��������
	DelTask(&TASKID_SDINIT);
}

int main(void)
{
	McuInit();														//��ʼ��ʱ�ӡ�ϵͳʱ����ʱ��
	UserIoInit();													//��ʼ��LED����
	SD2068Init();													//��ʼ��RTCʱ��оƬ
	SDRAM_Init();													//��ʼ��SDRAM
	my_mem_init(SRAMIN);											//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);											//��ʼ���ⲿ�ڴ�� 
	my_mem_init(SRAMCCM);											//��ʼ��CCM�ڴ�� 
	
	FmMemoryInit();													//��ʼ��EEPROM��д�ӿ�
	delay_ms(100);													//��ʱ�ȴ�EEPROM��������
	ReadDefaultConfig();											//��ȡĬ������
//	LcdInit();														//��ʼ��LCD spi�ӿڣ���ʼ��EMWIN
	
	TASKID_SDINIT = CreateTask("SD����ʼ��",0,0,0,0,500,SdInit);	//SD����ʼ������
	TASKID_RUNLED = CreateTask("����ָʾ��",0,0,0,0,30,RunLedTask); //����ָʾ������
	CreateTask("����ʱ��",0,0,0,1,0,FreshSysRtc);					//��ȡʵʱʱ��
	TASKID_NETPRO = CreateTask("���������շ�",0,0,0,0,0,NetPro);	//���������շ�����������
	TASKID_CANPRO = CreateTask("CAN�����շ�",0,0,0,0,0,CanPro);		//CAN�����շ�����������
	CreateTask("�������",0,0,0,0,20,KeyPro);						//������������ÿ20����ɨ��һ�ΰ���ֵ
	CreateTask("����������ˢ��",0,0,0,1,0,UpdateDeviceInfo);		//������״̬��������
	CreateTask("SendLianDong",0,0,0,0,200,SendLianDongTask);		//����������ָ�������
	CreateTask("SendInit",0,0,0,0,200,CanSendInitInfo);				//��������ʼ����������
	CreateTask("TOLKTASK",0,0,0,0,0,TolkPro);//�Խ�����
//	CreateTask("����ˢ��",0,0,0,0,5,FreshGui);						//EMWINҳ��ˢ������
	
	ExecTask();														//ѭ��ִ�������б��е�����
}

