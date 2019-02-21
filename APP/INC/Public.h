#ifndef __PUBLIC_H
#define __PUBLIC_H

#include <stdio.h>
#include <string.h>

#include "sys.h"
#include "Socket.h"
#include "malloc.h"
#include "SD2068.h"
#include "Mcu.h"
#include "sdram.h"
#include "sdio_sdcard.h"
#include "cJSON.h"
#include "TaskManger.h"
#include "string.h"
#include "fattester.h"
#include "exfuns.h"
#include "FmInit.h"
#include "malloc.h"
#include "W5500.h"
#include "W5500Spi.h"
#include "NetPro.h"
#include "Can.h"
#include "CanPro.h"
#include "Config.h"
#include "NetDataPro.h"
#include "KeyPro.h"
#include "SensorFactory.h"
#include "DateTool.h"
#include "LogRecorder.h"
#include "WT2031.h"
#include "VS1003.h"
#include "Key.h"
#include "TolkPro.h"
#include "MP3Pro.h"
#include "CrcMaker.h"
#include "BoardCastCmd.h"
//UI
#include "GUI.h"
#include "WM.h"
#include "gui_private.h"
#include "PageManger.h"

#define BREAKERLOGPATH	"0:/Logs/BreakerLogs"

#define SWAP16(s) ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff)) 
#define SWAP32(s) (((s) >> 24) |(((s) & 0x00ff0000) >> 8)  |(((s) & 0x0000ff00) << 8)  | ((s) << 24)) 
#define LED0CHANGE				GPIOB->ODR ^= 1<<0;
#ifdef FengDianNet
#define MaxDeviceNum  			20          //如果是闭锁管理网关，则最大配置16个（实际只有9个加上几个电源箱体信息）
#define MaxPowerNum   			10
#else
#define MaxDeviceNum  			128
#define MaxPowerNum   			15
#endif

#define Version					"T1.3"
#define SoftVerb       			12

#define DelayAckTime         	30
#define SystemSyncTime       	5
#define SystemInitTime       	5

#define NetwayName     			30
#define PowerName      			0x23
#define DuanDianQi     			0x1F
#define MaxTriggerNum  			200

// 11个字节表示一条记录。
#define FramBegin   2045
#define FramEnd     32768
#define MaxOneFrame 330//660       //最大一帧传输60条记录,每条记录11个字节

#define ACK             0x10
#define PREPARE         0xEC
#define WRONGACK        0xED
#define UnRegister      0x50
#define TimeShift       5        // 最大5秒偏差

#define ComRecover     1   //通讯恢复
#define ComInterrupt   0   //通讯中断

/*
 系统标志位Flag定义:
 *  D0 网关时间              0：正常  1：异常
 *  D1 与上位机通讯中断标记  0：正常  1：中断
 *  D2 标校传感器是否联动    0：不联动   1：联动
 *  D3 socket0 状态  0：正常   1：异常    TCP模式
 *  D4 socket1 状态  0：正常   1：异常    TCP模式
 *  D5 socket3 状态  0：正常   1：异常    TCP模式
 *  D6 socket4 状态  0：正常   1：异常    UDP模式
 *  D7 保留
 */ 
typedef struct
{
    u8 LastAck;
    u8 Addr;
    u8 Vol;
    u8 ResetTimes;
    u8 Flag;
    u8 Delay;
    u8 CrcCnt;
    u8 InitLeft;        // 初始化信息自动同步变量，当变量为0时，证明初始化信息已经匹配，可以同步底层设备
    u8 SyncLeft;        //自动对时,上位机时间相差30s自动对时,或者每个小时自动对时
    u8 BiaoJiaoAddr;
    u8 BaiaoJiaoTime;
    u8 BiaoJiaoVal;
    u8 BiaoJiaoDuanDianAddr;
	u8 UdpDipr[4];
	u16 UdpPort;
    u16 InitLen;       //初始化长度
    u16 Crc;           //初始化校验
    u16 R;              //铁电读指针
    u16 W;             //铁电写指针
    u16 Shift;         //缓冲区移动数据
    u16 PauseSec;
    u16 FilterTime;
    u16 BiSuoTime;
	char position[100];
	char RtcStr[50];
}_Sys;
extern _Sys Sys;

/*
 Flag: 
 * D0: 设备有效（上位机已经配置）  0 未配置   1 已经配置
 * D1：未注册设备有通讯（上位机软件没有配置该设备） 0 无信息   1 有信息
 * D2: 设备参与断线控制与否：0 不参与控制  1参与控制   ,断线控制设备轮询时间不一样
 * D3: 需要通过UDP信息转发
 * D4: 和其它设备有关联关系：0 有关联      1无关联
 * D5: 处于初始化配置中：    0 设备正常    1处于配置中，，，处于配置中的设备，暂时不触发动作
 */
/*Status   
 * D7      0 通讯正常       1 通讯中断  
 * D6 D5  总线归属  00  01 10 11 分别表示第1--4条总线
 * D4     初始化错误： 0 正常   1初始化错误
 * D3......D0 针对于传感器，表示数据状态与小数点，其余设备保留
 * 实时数据域3个数据，依次是数据域（状态字节+2个数据字节）
 */
/*
 CRC 的计算，针对于模拟量传感器：计算13个字节 地址 上报 上复 下报 下控 下复
 *           针对于开关量传感器：D0和D1有效，D0代表参与报警  D1代表参与断线控制
 *           针对于读卡器、广播、断电器：计算 地址+配置信息域 的CRC。
 ShowFlag:	D0: 0---未超限 	1---已超限		
 *			D1：0---在线	1---断线
 *			D2：0---正常	1---类型不一致
 *			D3-D5： 保留
 *			D6-D7： 分级报警等级 00--正常 01--1级报警 02--2级报警 03--3级报警
 */
typedef struct
{
	u8  Name;
	u8 	DefineName;
	u8  Status;
	u8  Buf[3];
    u8  Flag;
    //卡片数据时，存放4字节数据  卡片状态 ID 2个字节  RSSI
    u8  Crc;
    u8  CrcCnt;   // Crc 初始化次数
    u8  Warn;     // 针对于读卡器和断电器等执行设备，该字节表示此时报警事件
    u8  RecTimes;
    u32 Tick;
	
	u8 ShowFlag;//超限或断线时标记该传感器显示颜色
	u8 MusicFlag;//音乐播放标志
	u8 BoardCastGroup;//广播组
}_Sensor;

typedef struct
{
    u8 Addr;
    u8 BatInfo[46];
}_Power;

// 触发地址与执行地址
// 通过执行地址和触发地址也可以判断触发地址的状态是否需要通过UDP转发
typedef struct
{
    u8 TriggerAddr;    // 触发地址 , 如传感器
    u8 Trigger;        // 触发状态
    u8 ExcuteAddr;     // 执行地址， 如断电器
    u8 Warn;           // 报警标志
}_Excute;

#define NetAttri    0x1E

#define DEVICERESET 1
#define SENSORVARY  2
#define CARDVARY    3
#define NETRESET    4
#define NEWCARDVARY 5
//#define LianDongRecord   5
#define SENSORAUTOSEND   6

extern _Sensor Device[MaxDeviceNum];
extern _Excute Excute[MaxTriggerNum];
extern _Power  Power[MaxPowerNum];

void BufCopy(u8 *d, u8 *s,u16 len);
u8 BufCmp(u8 *s1,u8 *s2,u16 len);
u32 MakeFeimoCanId(u8 FramCnt, u8 Cmd, u8 CtrFlag, u8 Dir, u8 Type, u8 Addr);

#endif
