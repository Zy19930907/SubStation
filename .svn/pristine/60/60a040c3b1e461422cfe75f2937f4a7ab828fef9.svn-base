#ifndef __CANPRO_H
#define __CANPRO_H

#include "Public.h"

typedef struct
{
    u8 NeedToNet;
    u8 Addr;
    u16 Tick;
}_AckAddr;

typedef enum
{
    INITIDLE,
    SENDPREPARE,
    SENDING,
}_SendInitStatus;

typedef enum
{
    F_Single,
    F_First,
    F_Middle,
    F_End,
}_Fram;

typedef struct
{
    _SendInitStatus Status;
    _Fram FramStatus;
    u8  DestAddr;     //命令发送的目标地址  
    u8  W;        // 写
    u8  R;        // 读
    u8  FramCnt;
    u8  Shift;
    u8  Retry;
    u8  Buf[250];
    u16 Tick;
}_SendInit;
extern _SendInit Init;

typedef struct
{
    u8 Shift;
    u8 Retry;
}_LianDong;

extern _LianDong LianDong;
extern u32 TASKID_CANPRO;
	
void CanPro(void);
void SendDataCAN1(u32 ID,u8 *buf,u8 len);
void SendDataCAN234(u8 index,u32 ID,u8 *buf,u8 len);
void CanReceiveFunc(void);
void CanSendInitInfo(void);
void HandleCanData(u32 ID,u8 CanIndex);
void WriteSensorRecord(u8 t,u8 flag);
void TimeChange(u8 *buf);
void AckFunc(u8 index,u8 cmd,u8 DestAddr);
u32 MakeFramID(u8 Fram,u8 Cmd,u8 DestAddr);
void NetInfoToCan(u8 cmd,u8 *buf,u8 len);
void WritePowerRecord(u8 addr,u8 *buf);
void ReadCanData(u8 t);
u8 CheckCanRecBuf(void);
u8 MakeCanBusStatus(u8 CanIndex);
u8 CheckPowerIndex(u8 addr);
void SyncTime(u8 CanIndex,u8 addr);
u8 CheckTime(u8 *buf);
void CheckCrc(u8 addr,u8 crc,u8 flag);
void MakeMASWirelessSensor(u8 CanIndex,u8 addr);
void CheckTriggerStatus(void);
void MakeSensorCanData(u32 ID,u8 CanIndex,u8 addr);
void CheckLianDongInfo(u8 addr,u8 trigger,u8 val);
void HandleCanData(u32 ID, u8 CanIndex);
void SendLianDongTask(void);

#endif
