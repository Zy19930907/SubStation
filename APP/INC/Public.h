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
#define MaxDeviceNum  			20          //����Ǳ����������أ����������16����ʵ��ֻ��9�����ϼ�����Դ������Ϣ��
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

// 11���ֽڱ�ʾһ����¼��
#define FramBegin   2045
#define FramEnd     32768
#define MaxOneFrame 330//660       //���һ֡����60����¼,ÿ����¼11���ֽ�

#define ACK             0x10
#define PREPARE         0xEC
#define WRONGACK        0xED
#define UnRegister      0x50
#define TimeShift       5        // ���5��ƫ��

#define ComRecover     1   //ͨѶ�ָ�
#define ComInterrupt   0   //ͨѶ�ж�

/*
 ϵͳ��־λFlag����:
 *  D0 ����ʱ��              0������  1���쳣
 *  D1 ����λ��ͨѶ�жϱ��  0������  1���ж�
 *  D2 ��У�������Ƿ�����    0��������   1������
 *  D3 socket0 ״̬  0������   1���쳣    TCPģʽ
 *  D4 socket1 ״̬  0������   1���쳣    TCPģʽ
 *  D5 socket3 ״̬  0������   1���쳣    TCPģʽ
 *  D6 socket4 ״̬  0������   1���쳣    UDPģʽ
 *  D7 ����
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
    u8 InitLeft;        // ��ʼ����Ϣ�Զ�ͬ��������������Ϊ0ʱ��֤����ʼ����Ϣ�Ѿ�ƥ�䣬����ͬ���ײ��豸
    u8 SyncLeft;        //�Զ���ʱ,��λ��ʱ�����30s�Զ���ʱ,����ÿ��Сʱ�Զ���ʱ
    u8 BiaoJiaoAddr;
    u8 BaiaoJiaoTime;
    u8 BiaoJiaoVal;
    u8 BiaoJiaoDuanDianAddr;
	u8 UdpDipr[4];
	u16 UdpPort;
    u16 InitLen;       //��ʼ������
    u16 Crc;           //��ʼ��У��
    u16 R;              //�����ָ��
    u16 W;             //����дָ��
    u16 Shift;         //�������ƶ�����
    u16 PauseSec;
    u16 FilterTime;
    u16 BiSuoTime;
	char position[100];
	char RtcStr[50];
}_Sys;
extern _Sys Sys;

/*
 Flag: 
 * D0: �豸��Ч����λ���Ѿ����ã�  0 δ����   1 �Ѿ�����
 * D1��δע���豸��ͨѶ����λ�����û�����ø��豸�� 0 ����Ϣ   1 ����Ϣ
 * D2: �豸������߿������0 ���������  1�������   ,���߿����豸��ѯʱ�䲻һ��
 * D3: ��Ҫͨ��UDP��Ϣת��
 * D4: �������豸�й�����ϵ��0 �й���      1�޹���
 * D5: ���ڳ�ʼ�������У�    0 �豸����    1���������У��������������е��豸����ʱ����������
 */
/*Status   
 * D7      0 ͨѶ����       1 ͨѶ�ж�  
 * D6 D5  ���߹���  00  01 10 11 �ֱ��ʾ��1--4������
 * D4     ��ʼ������ 0 ����   1��ʼ������
 * D3......D0 ����ڴ���������ʾ����״̬��С���㣬�����豸����
 * ʵʱ������3�����ݣ�������������״̬�ֽ�+2�������ֽڣ�
 */
/*
 CRC �ļ��㣬�����ģ����������������13���ֽ� ��ַ �ϱ� �ϸ� �±� �¿� �¸�
 *           ����ڿ�������������D0��D1��Ч��D0������뱨��  D1���������߿���
 *           ����ڶ��������㲥���ϵ��������� ��ַ+������Ϣ�� ��CRC��
 ShowFlag:	D0: 0---δ���� 	1---�ѳ���		
 *			D1��0---����	1---����
 *			D2��0---����	1---���Ͳ�һ��
 *			D3-D5�� ����
 *			D6-D7�� �ּ������ȼ� 00--���� 01--1������ 02--2������ 03--3������
 */
typedef struct
{
	u8  Name;
	u8 	DefineName;
	u8  Status;
	u8  Buf[3];
    u8  Flag;
    //��Ƭ����ʱ�����4�ֽ�����  ��Ƭ״̬ ID 2���ֽ�  RSSI
    u8  Crc;
    u8  CrcCnt;   // Crc ��ʼ������
    u8  Warn;     // ����ڶ������Ͷϵ�����ִ���豸�����ֽڱ�ʾ��ʱ�����¼�
    u8  RecTimes;
    u32 Tick;
	
	u8 ShowFlag;//���޻����ʱ��Ǹô�������ʾ��ɫ
	u8 MusicFlag;//���ֲ��ű�־
	u8 BoardCastGroup;//�㲥��
}_Sensor;

typedef struct
{
    u8 Addr;
    u8 BatInfo[46];
}_Power;

// ������ַ��ִ�е�ַ
// ͨ��ִ�е�ַ�ʹ�����ַҲ�����жϴ�����ַ��״̬�Ƿ���Ҫͨ��UDPת��
typedef struct
{
    u8 TriggerAddr;    // ������ַ , �紫����
    u8 Trigger;        // ����״̬
    u8 ExcuteAddr;     // ִ�е�ַ�� ��ϵ���
    u8 Warn;           // ������־
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
