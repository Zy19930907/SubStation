#ifndef __CONFIG_H
#define __CONFIG_H

#include "Public.h"

#define BACKUPBASEADDR				(1024 * 5) //备份存储区基地址

#define RECORDBASEADDR				1024 * 8
//配置信息表
#define NetAddrAddr             	1
#define NetIpAddr               	4              
#define FactoryTimeAddr         	12
#define ResetTimesAddr          	18
#define GateWayIpAddr           	21
#define ReadPointerAddr         	27
#define WritePointerAddr        	31
#define InitInfoAddr            	35
#define InitLenAddr             	43
#define InitCrcAddr             	47
#define MacBufAddr              	51
#define FilterTimeAddr          	59
#define BootLoaderAddr          	63
#define NetCrcCntAddr           	71
#define NetUdpIpAddr            	74
#define BiSuoFilterTimeAddr     	82
#define NetWarnAddr             	88
#define BoardCastGroupConfigAddr	91
#define SUBSTATIONPOSITIONADDR		121
#define BaseInfoAddr            	400
//配置信息备份表
#define BK_NetAddrAddr             	BACKUPBASEADDR+NetAddrAddr
#define BK_NetIpAddr               	BACKUPBASEADDR+NetIpAddr           
#define BK_FactoryTimeAddr        	BACKUPBASEADDR+FactoryTimeAddr
#define BK_ResetTimesAddr         	BACKUPBASEADDR+ResetTimesAddr
#define BK_GateWayIpAddr           	BACKUPBASEADDR+GateWayIpAddr
#define BK_ReadPointerAddr         	BACKUPBASEADDR+ReadPointerAddr
#define BK_WritePointerAddr        	BACKUPBASEADDR+WritePointerAddr
#define BK_InitInfoAddr            	BACKUPBASEADDR+InitInfoAddr
#define BK_InitLenAddr             	BACKUPBASEADDR+InitLenAddr
#define BK_InitCrcAddr             	BACKUPBASEADDR+InitCrcAddr
#define BK_MacBufAddr              	BACKUPBASEADDR+MacBufAddr
#define BK_FilterTimeAddr          	BACKUPBASEADDR+FilterTimeAddr
#define BK_BootLoaderAddr          	BACKUPBASEADDR+BootLoaderAddr
#define BK_NetCrcCntAddr           	BACKUPBASEADDR+NetCrcCntAddr
#define BK_NetUdpIpAddr            	BACKUPBASEADDR+NetUdpIpAddr
#define BK_BiSuoFilterTimeAddr     	BACKUPBASEADDR+BiSuoFilterTimeAddr
#define BK_NetWarnAddr             	BACKUPBASEADDR+NetWarnAddr
#define BK_BoardCastGroupConfigAddr	BACKUPBASEADDR+BoardCastGroupConfigAddr
#define BK_BaseInfoAddr            	BACKUPBASEADDR+BaseInfoAddr
#define BK_SUBSTATIONPOSITIONADDR   BACKUPBASEADDR+SUBSTATIONPOSITIONADDR

void ReadNetAddr(u8 *Addr);
void ReadFactoryTime(u8 *FtyTime);
void ReadNetIpAddr(u8 *ip);
void ReadGateWayIp(u8 *DefaultGateWayIp);
void ReadResetTimes(u8 *ResetTimes);
void ReadUdpIp(u8 *DefaultUdp);
void ReadMacAddr(u8 *DefaultMac);
void ReadReadPtr(u16 *ReadPtr);
void ReadWritePtr(u16 *WritePtr);
void ReadFilterTime(u16 *FileTime);
void ReadCrcCnt(u8 *CrcCnt);
void ReadInitLen(u16 *InitLen);
void ReadInitCrc(u16 *InitCrc);
void ReadBiSuoFilterTime(u16 *BiSuo,u16 *FilterTime);
void ReadNetWarn(u8 *warn);
void UpdateNetAddr(u8 Addr);
void UpdateNetIpAddr(u8* IpAddr);
void UpdateFactoryTime(u32 FactoryTime);
void UpdateResetTimes(u8 ResetTimes);
void UpdateGatewayIpAddr(u8 *DefaultIpAddr);
void UpdateReadPtr(u16 ReadPointer);
void UpdateWritePtr(u16 WritePointer);
void UpdateInitLen(u16 InitLen);
void UpdateInitCrc(u16 InitCrc);
void UpdateMacAddr(u8 *Mac);
void UpdateFilterTime(u16 FilterTime);
void UpdateBootLoader(u8 *BootLoader);
void UpdateNetCrcCnt(u8 NetCrcCnt);
void UpdateNetUdpIp(u8 *NetUdpIp);
void UpdateBiSuoFilterTime(u8 *BiSuoFilter);
void UpdateNetWarn(u8 warn);
void UpdateInitInfo(u8 *buf,u16 len);

void WriteRecord(u16 addr,u8 *buf,u16 len);
u8 ReadRecord(u16 addr,u8 *buf);
void ReadDefaultConfig(void);
void ReadDefaultInit(void);
void ReadInitInfo(u8 addr);
void CalDeviceCrc(void);
void ReadDeviceDefine(void);
void UpdateDeviceInfo(void);
void ReadBoardCastGroupConfg(void);
void UpdateBoardCastGroupConfig(u8 *config);
void UpdateSubStionPosition(char* position);
void ReadSubStationPosition(char *positon);

#endif
