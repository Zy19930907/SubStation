#include "Public.h"

//RUNLED---PE4
#define RUNLEDCHANGE	GPIOE->ODR ^= 1<<4
u32 TASKID_RUNLED 		= 0xFFFFFFFF;
u32 TASKID_SDINIT		= 0xFFFFFFFF;
extern u32 TASKID_MP3TASK;

void UserIoInit(void)
{
	RCC->AHB1ENR |= (1<<4); //PE时钟使能
	GPIOE->MODER |= (1 << 8);//推挽输出
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
	exfuns_init();													//为FATFS分配内存
	mf_mount((u8*)"0:/");											//挂载SD卡
	CheckLogDirs();													//检查SD卡中是否存在日志文件夹，没有则新建
	SetTaskDelay(TASKID_RUNLED,0,0,0,0,500);
//	TASKID_MP3TASK = CreateTask("Mp3Pro",0,0,0,0,0,MP3Pro);			//创建MP3播放任务
	DelTask(&TASKID_SDINIT);
}

int main(void)
{
	McuInit();														//初始化时钟、系统时基定时器
	UserIoInit();													//初始化LED引脚
	SD2068Init();													//初始化RTC时钟芯片
	SDRAM_Init();													//初始化SDRAM
	my_mem_init(SRAMIN);											//初始化内部内存池
	my_mem_init(SRAMEX);											//初始化外部内存池 
	my_mem_init(SRAMCCM);											//初始化CCM内存池 
	
	FmMemoryInit();													//初始化EEPROM读写接口
	delay_ms(100);													//延时等待EEPROM正常工作
	ReadDefaultConfig();											//读取默认配置
//	LcdInit();														//初始化LCD spi接口，初始化EMWIN
	
	TASKID_SDINIT = CreateTask("SD卡初始化",0,0,0,0,500,SdInit);	//SD卡初始化任务
	TASKID_RUNLED = CreateTask("运行指示灯",0,0,0,0,30,RunLedTask); //运行指示灯任务
	CreateTask("更新时钟",0,0,0,1,0,FreshSysRtc);					//获取实时时钟
	TASKID_NETPRO = CreateTask("网络数据收发",0,0,0,0,0,NetPro);	//网络数据收发及处理任务
	TASKID_CANPRO = CreateTask("CAN数据收发",0,0,0,0,0,CanPro);		//CAN数据收发及处理任务
	CreateTask("按键检测",0,0,0,0,20,KeyPro);						//按键处理任务，每20毫秒扫描一次按键值
	CreateTask("传感器数据刷新",0,0,0,1,0,UpdateDeviceInfo);		//传感器状态更新任务
	CreateTask("SendLianDong",0,0,0,0,200,SendLianDongTask);		//传感器联动指令发送任务
	CreateTask("SendInit",0,0,0,0,200,CanSendInitInfo);				//传感器初始化发送任务
	CreateTask("TOLKTASK",0,0,0,0,0,TolkPro);//对讲任务
//	CreateTask("界面刷新",0,0,0,0,5,FreshGui);						//EMWIN页面刷新任务
	
	ExecTask();														//循环执行任务列表中的任务
}

