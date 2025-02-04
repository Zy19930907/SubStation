#include "Config.h"
#include "CrcMaker.h"
#include "FmInit.h"
#include "malloc.h"
#include "rng.h"

u8 ConfigBuf[1500];

void WriteConfig(u16 addr, u8 *buf, u16 len)
{
	SetCrc16(buf, len, 0);
	FramWriteBurstByte(addr, buf, len + 2); //配置信息存入主存储区
	FramWriteBurstByte(addr + BACKUPBASEADDR, buf, len + 2); //配置信息存入备份存储区
}

void WriteCurConfig(u16 addr, u16 len)
{
	SetCrc16(ConfigBuf, len-2, 0);
    FramWriteBurstByte(addr, ConfigBuf, len); //配置信息存入主存储区
    FramWriteBurstByte(addr + BACKUPBASEADDR, ConfigBuf, len); //配置信息存入备份存储区
}

u8 ConfigCheck(u16 addr, u8 len)
{
    FramReadBurstByte(addr, ConfigBuf, len);
    if(CheckCrc16(ConfigBuf, len - 2, 0))
        return 1;
    else
        return 0;
}

void ReadNetAddr(u8 *Addr)
{
    *Addr = 61;
    if(ConfigCheck(NetAddrAddr, 3))
        *Addr = *ConfigBuf;
    else if(ConfigCheck(BK_NetAddrAddr, 3))
        *Addr = *ConfigBuf;
	*ConfigBuf = *Addr;
    WriteCurConfig(NetAddrAddr, 3);
}

void ReadFactoryTime(u8 *FtyTime)
{
    u8 i;
    mymemset(FtyTime, 0x00, 4);
    //主存储区数据
    if(ConfigCheck(FactoryTimeAddr, 6))
    {
        for(i = 0; i < 4; i++)
            *(FtyTime + i) = *(ConfigBuf + i);
    }//备份存储区数据
    else if(ConfigCheck(BK_FactoryTimeAddr, 6))
    {
        for(i = 0; i < 4; i++)
            *(FtyTime + i) = *(ConfigBuf + i);
    }
    WriteCurConfig(FactoryTimeAddr, 6);
}

void ReadNetIpAddr(u8 *ip)
{
    u8 i;
    *ip = 192;
    *(ip + 1) = 168;
    *(ip + 2) = 1;
    *(ip + 3) = 251;
    *(ip + 4) = 0x88;
    *(ip + 5) = 0x13;
    if(ConfigCheck(NetIpAddr, 8))
    {
        for(i = 0; i < 4; i++)
            *(ip + i) = *(ConfigBuf + i);
    }
    else if(ConfigCheck(BK_NetIpAddr, 8))
    {
        for(i = 0; i < 4; i++)
            *(ip + i) = *(ConfigBuf + i);
    }
    WriteCurConfig(NetIpAddr, 8);
}

void ReadGateWayIp(u8 *DefaultGateWayIp)
{
    u8 i;
    *DefaultGateWayIp = 192;
    *(DefaultGateWayIp + 1) = 168;
    *(DefaultGateWayIp + 2) = 1;
    *(DefaultGateWayIp + 3) = 1;
    if(ConfigCheck(GateWayIpAddr, 6))
    {
        for(i = 0; i < 4; i++)
            *(DefaultGateWayIp + i) = *(ConfigBuf + i);
    }
    else if(ConfigCheck(BK_GateWayIpAddr, 6))
    {
        for(i = 0; i < 4; i++)
            *(DefaultGateWayIp + i) = *(ConfigBuf + i);
    }
    WriteCurConfig(GateWayIpAddr, 6);
}

void ReadResetTimes(u8 *ResetTimes)
{
    *ResetTimes = 0;
    if(ConfigCheck(ResetTimesAddr, 3))
        *ResetTimes = *ConfigBuf;
    else if(ConfigCheck(BK_ResetTimesAddr, 3))
        *ResetTimes = *ConfigBuf;
    WriteCurConfig(ResetTimesAddr, 3);
}

void ReadUdpIp(u8 *DefaultUdp)
{
    u8 i;
    mymemset(DefaultUdp, 0, 6);
    if(ConfigCheck(NetUdpIpAddr, 8))
    {
        for(i = 0; i < 6; i++)
            *(DefaultUdp + i) = *(ConfigBuf + i);
    }
    else if(ConfigCheck(BK_NetUdpIpAddr, 8))
    {
        for(i = 0; i < 6; i++)
            *(DefaultUdp + i) = *(ConfigBuf + i);
    }
    WriteCurConfig(NetUdpIpAddr, 8);
}

void ReadMacAddr(u8 *DefaultMac)
{
    u8 i;
    u32 *p = (u32*)(DefaultMac + 1);
    *DefaultMac = 0x00;
    RNG_Init();
    *p = RNG_Get_RandomNum();//默认Mac地址，采用STM32F4硬件随机数生成
    if(ConfigCheck(MacBufAddr, 8))
    {
        for(i = 0; i < 6; i++)
            *(DefaultMac + i) = *(ConfigBuf + i);
    }
    else if(ConfigCheck(BK_MacBufAddr, 8))
    {
        for(i = 0; i < 6; i++)
            *(DefaultMac + i) = *(ConfigBuf + i);
    }
    WriteCurConfig(MacBufAddr, 8);
}

void ReadReadPtr(u16 *ReadPtr)
{
    *ReadPtr = 0;
    if(ConfigCheck(ReadPointerAddr, 4))
        *ReadPtr = *(u16*)ConfigBuf;
    else if(ConfigCheck(BK_ReadPointerAddr, 4))
        *ReadPtr = *(u16*)ConfigBuf;
    WriteCurConfig(ReadPointerAddr, 4);
}

void ReadWritePtr(u16 *WritePtr)
{
    *WritePtr = 0;
    if(ConfigCheck(WritePointerAddr, 4))
        *WritePtr = *(u16*)ConfigBuf;
    else if(ConfigCheck(BK_WritePointerAddr, 4))
        *WritePtr = *(u16*)ConfigBuf;
    WriteCurConfig(WritePointerAddr, 4);
}

void ReadFilterTime(u16 *FilerTime)
{
    *FilerTime = 200;
    if(ConfigCheck(FilterTimeAddr, 4))
    {
        *FilerTime = SWAP16(*(u16*)ConfigBuf);
    }
    else if(ConfigCheck(BK_FilterTimeAddr, 4))
    {
        *FilerTime = SWAP16(*(u16*)ConfigBuf);
    }
    WriteCurConfig(FilterTimeAddr, 4);
}

void ReadCrcCnt(u8 *CrcCnt)
{
    *CrcCnt = 0;
    if(ConfigCheck(NetCrcCntAddr, 3))
        *CrcCnt = *ConfigBuf;
    else if(ConfigCheck(BK_NetCrcCntAddr, 3))
        *CrcCnt = *ConfigBuf;
    WriteCurConfig(NetCrcCntAddr, 3);
}

void ReadInitLen(u16 *InitLen)
{
    *InitLen = 0;
    if(ConfigCheck(InitLenAddr, 4))
    {
        *InitLen = *(u16*)ConfigBuf;
    }
    else if(ConfigCheck(BK_InitLenAddr, 4))
    {
        *InitLen = *(u16*)ConfigBuf;
    }
    WriteCurConfig(InitLenAddr, 4);
}

void ReadInitCrc(u16 *InitCrc)
{
    *InitCrc = 0;
    if(ConfigCheck(InitCrcAddr, 4))
        *InitCrc = *(u16*)ConfigBuf;
    else if(ConfigCheck(BK_InitCrcAddr, 4))
        *InitCrc = *(u16*)ConfigBuf;
    WriteCurConfig(InitCrcAddr, 4);
}

void ReadBiSuoFilterTime(u16 *BiSuo, u16 *FilterTime)
{
    *BiSuo = 20000;
    *FilterTime = 25000;
    if(ConfigCheck(BiSuoFilterTimeAddr, 6))
    {
        *FilterTime = *(u16*)(ConfigBuf+2);
		*BiSuo = *(u16*)ConfigBuf;
    }
    else if(ConfigCheck(BK_BiSuoFilterTimeAddr, 6))
    {
		*FilterTime = *(u16*)(ConfigBuf+2);
		*BiSuo = *(u16*)ConfigBuf;
    }
    WriteCurConfig(BiSuoFilterTimeAddr, 6);
}

void ReadNetWarn(u8 *warn)
{
    *warn |= 0x04;
    if(ConfigCheck(NetWarnAddr, 3))
    {
        if(!ConfigBuf[0])
            *warn &= ~0x04;
    }
    else if(ConfigCheck(BK_NetWarnAddr, 3))
    {
        if(!ConfigBuf[0])
			*warn &= ~0x04;
    }
}

//读取广播分组配置
void ReadBoardCastGroupConfg(void)
{
	u64 configL=0,configH=0;
	u8 i,j;
	if(ConfigCheck(BoardCastGroupConfigAddr, 130))//先读主存储区域内的配置
    {
		for(i=0;i<8;i++)//8个广播组
		{
			configL = 0;
			configH = 0;
			for(j=0;j<8;j++)
			{
				configL |= ((ConfigBuf[(2*i)*8+j]) << j*8);
				configH |= ((ConfigBuf[(2*i+1)*8+j]) << j*8);
			}
			for(j=0;j<64;j++)
			{
				if((configL >> j) & 0x01)
					Device[j].BoardCastGroup |= (1<<i);
				else
					Device[j].BoardCastGroup &= ~(1<<i);
				if((configH >> j) & 0x01)
					Device[64+j].BoardCastGroup |= (1<<i);
				else
					Device[64+j].BoardCastGroup &= ~(1<<i);
			}
		}
    }
    else if(ConfigCheck(BK_BoardCastGroupConfigAddr, 130))//备份存储区域
    {
		for(i=0;i<8;i++)
		{
			configL = 0;
			configH = 0;
			for(j=0;j<8;j++)
			{
				configL |= ((ConfigBuf[(2*i)*8+j]) << j*8);
				configH |= ((ConfigBuf[(2*i+1)*8+j]) << j*8);
			}
			for(j=0;j<64;j++)
			{
				if((configL >> j) & 0x01)
					Device[j].BoardCastGroup |= (1<<i);
				else
					Device[j].BoardCastGroup &= ~(1<<i);
				if((configH >> j) & 0x01)
					Device[64+j].BoardCastGroup |= (1<<i);
				else
					Device[64+j].BoardCastGroup &= ~(1<<i);
			}
		}
    }	
}

void ReadSubStationPosition(char *positon)
{
    strcpy((char*)positon,(char*)"安装地址未配置");
    if(ConfigCheck(SUBSTATIONPOSITIONADDR, 102))
    {
        BufCopy((u8*)Sys.position,ConfigBuf,100);
    }
    else if(ConfigCheck(BK_SUBSTATIONPOSITIONADDR, 102))
    {
        BufCopy((u8*)Sys.position,ConfigBuf,100);
    }
}

void UpdateNetAddr(u8 Addr)
{
    *ConfigBuf = Addr;
    WriteConfig(NetAddrAddr, ConfigBuf, 1);
}

void UpdateNetIpAddr(u8* IpAddr)
{
    u8 i;
    for(i = 0; i < 6; i++)
        *(ConfigBuf + i) = *(IpAddr + i);
    WriteConfig(NetIpAddr, ConfigBuf, 6);
}

void UpdateFactoryTime(u32 FactoryTime)
{
    u8 i;
    for(i = 0; i < 4; i++)
        *(ConfigBuf + i) = ((FactoryTime >> (i * 8)) & 0xFF);
    WriteConfig(FactoryTimeAddr, ConfigBuf, 4);
}

void UpdateResetTimes(u8 ResetTimes)
{
    *ConfigBuf = ResetTimes;
    WriteConfig(ResetTimesAddr, ConfigBuf, 1);
}

void UpdateGatewayIpAddr(u8 *DefaultIpAddr)
{
    u8 i;
    for(i = 0; i < 4; i++)
        *(ConfigBuf + i) = *(DefaultIpAddr + i);
    WriteConfig(GateWayIpAddr, ConfigBuf, 4);
}

void UpdateReadPtr(u16 ReadPointer)
{
    *ConfigBuf = (ReadPointer & 0xFF);
    *(ConfigBuf + 1) = ((ReadPointer >> 8) & 0xFF);
    WriteConfig(ReadPointerAddr, ConfigBuf, 2);
}

void UpdateWritePtr(u16 WritePointer)
{
    *ConfigBuf = (WritePointer & 0xFF);
    *(ConfigBuf + 1) = ((WritePointer >> 8) & 0xFF);
    WriteConfig(WritePointerAddr, ConfigBuf, 2);
}

void UpdateInitLen(u16 InitLen)
{
    *ConfigBuf = (InitLen & 0xFF);
    *(ConfigBuf + 1) = ((InitLen >> 8) & 0xFF);
    WriteConfig(InitLenAddr, ConfigBuf, 2);
}

void UpdateInitCrc(u16 InitCrc)
{
    *ConfigBuf = (InitCrc & 0xFF);
    *(ConfigBuf + 1) = ((InitCrc >> 8) & 0xFF);
    WriteConfig(InitCrcAddr, ConfigBuf, 2);
}

void UpdateMacAddr(u8 *Mac)
{
    u8 i;
    for(i = 0; i < 6; i++)
        *(ConfigBuf + i) = *(Mac + i);
    WriteConfig(MacBufAddr, ConfigBuf, 6);
}

void UpdateFilterTime(u16 FilterTime)
{
    *ConfigBuf = (FilterTime & 0xFF);
    *(ConfigBuf + 1) = ((FilterTime >> 8) & 0xFF);
    WriteConfig(FilterTimeAddr, ConfigBuf, 2);
}

void UpdateBootLoader(u8 *BootLoader)
{
    u8 i;
    for(i = 0; i < 6; i++)
        *(ConfigBuf + i) = *(BootLoader + i);
    WriteConfig(BootLoaderAddr, ConfigBuf, 6);
}

void UpdateNetCrcCnt(u8 NetCrcCnt)
{
    *ConfigBuf = NetCrcCnt;
    WriteConfig(NetCrcCntAddr, ConfigBuf, 1);
}

void UpdateNetUdpIp(u8 *NetUdpIp)
{
    u8 i;
    for(i = 0; i < 6; i++)
        *(ConfigBuf + i) = *(NetUdpIp + i);
    WriteConfig(NetUdpIpAddr, ConfigBuf, 6);
}

void UpdateBiSuoFilterTime(u8 *BiSuoFilter)
{
    u8 i;
    for(i = 0; i < 4; i++)
        *(ConfigBuf + i) = *(BiSuoFilter + i);
    WriteConfig(BiSuoFilterTimeAddr, ConfigBuf, 4);
}

void UpdateNetWarn(u8 warn)
{
    if(warn & 0x04)
        ConfigBuf[0] = 0x01;
    else
        ConfigBuf[0] = 0x00;
    WriteConfig(NetWarnAddr, ConfigBuf, 1);
}

void UpdateBoardCastGroupConfig(u8 *config)
{
	u8 i;
    for(i = 0; i < 128; i++)
        *(ConfigBuf + i) = *(config + i);
	WriteConfig(BoardCastGroupConfigAddr, ConfigBuf, 128);
}

void UpdatePowerInfo(u8 addr)
{
    u8 i;
    i = CheckPowerIndex(addr);
    if(i == 0xFF)
        return;
    Power[i].Addr = 0;
}

void UpdateSubStionPosition(char* position)
{
	BufCopy(ConfigBuf,(u8*)position,100);
	WriteConfig(SUBSTATIONPOSITIONADDR, ConfigBuf, 100);
}

void UpdateDeviceInfo(void)
{
    u8 i, buf[12];
    u16 FilterTime;
	 // 模拟测试100个设备数据,正常运行时，需要去掉该部分
//	for(i = 0;i < 20;i ++)
//	{
//		Device[i].Flag = 0x01;
//		Device[i].Tick = SYS_TICK;
//		Device[i].Status = 0x22;
//		if(i <= 5)
//		{
//			Device[i].Status = 0x20;
//			Device[i].Buf[0] = RNG_Get_RandomRange(0,200);
//			if(Device[i].Buf[0]>=180)
//				Device[i].ShowFlag |= 0x01;
//			else
//				Device[i].ShowFlag &= ~0x01;
//			Device[i].Name = 0x04;
//		}
//		else if(i > 5 && i <= 10)
//		{
//			Device[i].Buf[0] = RNG_Get_RandomRange(0,200);
//			if(Device[i].Buf[0]>=180)
//				Device[i].ShowFlag |= 0x01;
//			else
//				Device[i].ShowFlag &= ~0x01;
//			Device[i].Name = 0x00;
//		}
//		else if(i > 10 && i <= 15)
//		{
//			Device[i].Status = 0x21;
//			Device[i].Buf[0] = RNG_Get_RandomRange(150,190);
//			if(Device[i].Buf[0]>=180)
//				Device[i].ShowFlag |= 0x01;
//			else
//				Device[i].ShowFlag &= ~0x01;
//			Device[i].Name = 0x07;
//		}
//		else if(i > 15 && i <= 20)
//		{
//			Device[i].Status = 0x21;
//			Device[i].Buf[0] = RNG_Get_RandomRange(200,250);
//			if(Device[i].Buf[0]>=240)
//				Device[i].ShowFlag |= 0x01;
//			else
//				Device[i].ShowFlag &= ~0x01;
//			Device[i].Name = 0x05;
//		}
//		Device[i].Buf[1] = 0;
//	}
	
    if(Sys.Delay > 5)              // 倒计时未结束
        return;
    for(i = 0; i < MaxDeviceNum; i ++)
    {
        if((Device[i].Flag & 0x04) && (Device[i].Status & 0x80))      // 参与断线控，并且设备已经断线
        {
            CheckLianDongInfo(i + 1, 0x10, 0x10);
            continue;
        }
        if(!(Device[i].Flag & 0x03))      // 与网关有过通讯的设备: 注册信息 未注册信息
            continue;
        if(Device[i].Status & 0x80)      // 通讯不正常的设备
            continue;
        if(Device[i].Flag & 0x04)        // 参与断线控制,并且设备(没有处于初始化)之前正常，现在错误
        {
            if(!(Device[i].Flag & 0x20))   // 刚设置了初始化，暂不参与执行
                FilterTime = Sys.BiSuoTime;
            else
                FilterTime = Sys.FilterTime;
        }
        else
            FilterTime = Sys.FilterTime;
        if(MsTickDiff(Device[i].Tick) < FilterTime)
            continue;
        CheckLianDongInfo(i + 1, 0x10, 0x10);
#ifdef FengDianNet
        if(i < 6)   //只判断 A1 A2 A3 D4 D5状态  0 正常 1中断
            FengDian.ComNormal |= (1 << i);
#endif
        Device[i].Status |= 0x80;
        Device[i].Flag &= 0x15;    // 只保留D4 D2  D0
        buf[0] = i + 1;
        buf[1] = Device[i].Name;   //名称
        buf[2] = 0x80;             //通讯中断
        buf[3] = 0;
        buf[4] = 0;
        buf[5] = 0;
        TimeChange(&buf[6]);
        buf[10] = SumAll(buf, 10);
        FramWriteBurstByte(Sys.W, buf, 11);
        Sys.W += 11;
        if(Sys.W == FramEnd)
            Sys.W = FramBegin;
        UpdateWritePtr(Sys.W);
        // 再查看该设备是否为电源，如果是，在清除相应的电源列表信息
        if(buf[1] == PowerName)
        {
            UpdatePowerInfo(buf[0]);
        }
    }
}

void UpdateInitInfo(u8 *buf,u16 len)
{
	u16 InitCrc;
	UpdateInitLen(len);
	FramWriteBurstByte(BaseInfoAddr,buf, len);      //写初始化信息
	FramWriteBurstByte(BK_BaseInfoAddr,buf,len);
	InitCrc = CalCrc16(buf, len, 0);
	UpdateInitCrc(InitCrc);
}

void ReadDefaultConfig(void)
{
    u8 i;
    for(i = 0; i < MaxDeviceNum; i ++)       // 初始化默认，将所有的设备设置为未知类型
        Device[i].Name = 0xFF;
    ReadTime();
    ReadNetAddr(&Sys.Addr);
    ReadNetIpAddr(Net.localIp);
    ReadGateWayIp(Net.GatewayIP);
    ReadUdpIp(Sys.UdpDipr);
    ReadMacAddr(Net.MacAddr);
    ReadReadPtr(&Sys.R);
    ReadWritePtr(&Sys.W);
    ReadCrcCnt(&Sys.CrcCnt);
    ReadBiSuoFilterTime(&Sys.BiSuoTime, &Sys.FilterTime);
    ReadNetWarn(&Sys.Flag);
    ReadResetTimes(&Sys.ResetTimes);
    Sys.ResetTimes ++;
    UpdateResetTimes(Sys.ResetTimes);
    WriteSensorRecord(0, NETRESET);
    ReadDefaultInit();           //读取初始化信息
	ReadBoardCastGroupConfg();
	ReadSubStationPosition(Sys.position);
}

// 查看设备的有效性
void ReadDeviceDefine(void)
{
    u8 i, j, n, buf[32];
    FramReadBurstByte(BaseInfoAddr, buf, 32);
    for(i = 0; i < MaxDeviceNum; i ++)          //先清除
        Device[i].Flag = 0;
    n = 0;
#ifndef FengDianNet
    for(i = 0; i < 16; i ++)
    {
        for(j = 0; j < 8; j ++)
        {
            if(buf[i] & 0x01)                    // 设备有效
            {
                Device[n].Flag |= 0x01;
                Device[n].Crc  = 0;
                Device[n].CrcCnt = 0;
            }
            buf[i] >>= 1;
            n ++;
        }
    }
#else
    for(i = 0; i < MaxDeviceNum; i ++)
    {
        Device[i].Status = 0;
        Device[i].Flag |= 0x01;
        Device[i].Buf[0] = 0;
        Device[i].Buf[1] = 0;
        Device[i].Buf[2] = 0;
        Device[i].Tick = SYS_TICK;
    }
#endif
}

/*
 * 真正初始化信息域的长度是Sys.InitLen-3(除去命令 自身长度占用的3个字节)
 * 前面32个字节是设备定义域
 * 然后第33个字节是01，代表传感器配置信息域，34,35个字节是长度字节。从第36个字节开始
 * 最后是配置信息（一般指断电器）
*/
// 先计算 配置信息域，如果有断线报警，则修改传感器的报警信息，最后再来计算传感器的crc

void CalDeviceCrc(void)
{
    u8 crc, k, addr, sensoraddr, trigger, buf[255], n;
    u16 i, j, allcnt01, allcnt02, t;
    for(k = 0; k < MaxDeviceNum; k ++) // 先全部清零和清除设备的断线报警状态
    {
        Device[k].Crc = 0;
        Device[k].CrcCnt = 0;
        Device[k].Flag &= ~0x04;
        Device[k].Warn = 0;
    }
    n = 0;
    for(k = 0; k < MaxTriggerNum; k ++)
    {
        Excute[k].ExcuteAddr = 0;
        Excute[k].TriggerAddr = 0;
        Excute[k].Trigger = 0;
        Excute[k].Warn = 0;
    }
    if(Sys.InitLen == 0x20)    // 只有设备定义域，没有报警信息域
        return;
    allcnt01 = Net.Buf[34];
    allcnt01 <<= 8;
    allcnt01 += Net.Buf[33];
    allcnt01 -= 3;                 //除去命令标记和长度自身字节
    if((allcnt01 % 13) != 0)       //1条报警记录13个字节
        return;
    // 先计算配置信息中的设备 主要是指断电器,关联读卡器，广播
    if(Sys.InitLen - 3 == 32 + allcnt01) //没有配置控制信息了，返回
        goto CalSensorWarnCrc;
    allcnt02 = Net.Buf[32 + allcnt01 + 5];
    allcnt02 <<= 8;
    allcnt02 += Net.Buf[32 + allcnt01 + 4];
    allcnt02 -= 3;
    if((allcnt02 % 4) != 0)     //控制信息域不对，则不计算控制信息域
        goto CalSensorWarnCrc;
    t = 0;
    // 断电器的校验按照  关联传感器编号 触发状态  上报......下复
    for(i = 0; i < allcnt02;)
    {
        addr = Net.Buf[38 + allcnt01 + i];     // 断电器(执行)地址
        trigger = Net.Buf[38 + allcnt01 + i + 1]; // 触发状态
        sensoraddr = Net.Buf[38 + allcnt01 + i + 2]; // 关联传感器/开关量编号
        Device[sensoraddr - 1].Flag |= 0x10;
        if(trigger & 0x10)               // 断线控制
            Device[sensoraddr - 1].Flag |= 0x04;
        Excute[n].TriggerAddr = sensoraddr;
        Excute[n].ExcuteAddr = addr;
        Excute[n].Trigger = trigger;
        n ++;
        if(Net.Buf[38 + allcnt01 + i + 3] == 0x01) // 开关量
        {
            Device[sensoraddr - 1].Crc |= ((trigger >> 3) + 1); // D3 1态 D2 0态 D1 断线控 D0 参与控制（其实D0无太大意义，只是为了兼容以前老版本）
            trigger |= 0x80;
        }
        buf[t++] = sensoraddr;
        buf[t++] = trigger;
        for(j = 0; j < allcnt01;)              // 如果是开关量，则找不到报警数据
        {
            if(sensoraddr == Net.Buf[35 + j]) //地址相同,填入12个字节的初始化参数
            {
                for(k = 0; k < 12; k ++)
                    buf[t++] = Net.Buf[35 + j + k + 1];
                break;
            }
            j += 13;                         //没有找到，继续查找下一个
        }
        i += 4;    // 判断该断电器地址是否还有其它关联传感器,针对于开关量传感器，只计算开关量地址、触发状态两个字节
        if((addr != Net.Buf[38 + allcnt01 + i]) || (i == allcnt02)) // 查看下一个，地址不相同，则计算crc
        {
            crc = CalCrc16(buf, t, 1);
            Device[addr - 1].Crc = crc;
            t = 0;     //为下一个断电器做准备
        }
    }
    // 最后计算配置信息域（传感器）的crc,13个字节代表一个设备的报警信息域
CalSensorWarnCrc:
    for(i = 0; i < allcnt01;)
    {
        addr = Net.Buf[35 + i];
        if(Device[addr - 1].Flag & 0x04)
            Net.Buf[35 + i + 1 + 1] |= 0x80;
        crc = CalCrc16(&Net.Buf[35 + i], 13, 1);
        Device[addr - 1].Crc = crc;
        i += 13;
    }
}


void ReadInitInfo(u8 addr)
{
    u16 i, allcnt01, allcnt02, j;
    u8  SensorID, ControlDeviceID, k, trigger;
    if(addr == 24)
        allcnt01 = 5;
    FramReadBurstByte(BaseInfoAddr, &Net.Buf[0], Sys.InitLen);
    allcnt01 = Net.Buf[34];
    allcnt01 <<= 8;
    allcnt01 += Net.Buf[33];
    allcnt01 -= 3;
// 传感器关联信息：上报 上控 上复 下报 下控 下复
    for(i = 0; i < allcnt01;)
    {
        if(addr == Net.Buf[35 + i]) //找到设备地址了，数据填充之后字节返回
        {
            if(Device[addr - 1].Flag & 0x04)    //是否触发断线报警
                Net.Buf[35 + i + 1 + 1] |= 0x80;
            for(j = 0; j < 12; j ++)
                Init.Buf[Init.W ++] = Net.Buf[35 + i + 1 + j];
            return;
        }
        i += 13;
    }
    //.....如果上面没有找到，则继续搜索断电器的配置类型
    allcnt02 = Net.Buf[32 + allcnt01 + 5];
    allcnt02 <<= 8;
    allcnt02 += Net.Buf[32 + allcnt01 + 4];
    allcnt02 -= 3;

    /* 断电器关联每个传感器（有可能一个断电器关联多个传感器）
     * 一条传感器关联记录为14个字节：触发状态 传感器编号  上报 上控 上复 下报 下控 下复
     * 一条开关量关联记录为2个字节： 触发状态 传感器编号
     * 有可能是多条传感器记录（同时关联多个传感器）
     */
    for(i = 0; i < allcnt02;)
    {
        ControlDeviceID = Net.Buf[38 + allcnt01 + i]; // 断电器地址
        if(addr != ControlDeviceID)                // 先判断地址是否相等
        {
            i += 4;
            continue;
        }
        SensorID = Net.Buf[38 + allcnt01 + i + 2]; //关联传感器编号
        Init.Buf[Init.W ++] = SensorID;
        trigger = Net.Buf[38 + allcnt01 + i + 1]; //传感器/开关量 触发状态
        if(Net.Buf[38 + allcnt01 + i + 3] == 0x01) // 开关量传感器
        {
            trigger |= 0x80;
            Init.Buf[Init.W ++] = trigger;     //触发状态
        }
        else                                     //模拟量传感器
        {
            Init.Buf[Init.W ++] = trigger;     //触发状态
            for(j = 0; j < allcnt01;)
            {
                if(SensorID == Net.Buf[35 + j]) //找到传感器编号，填入上报 上控等
                {
                    for(k = 0; k < 12; k ++)
                        Init.Buf[Init.W ++] = Net.Buf[35 + j + 1 + k];
                    break;
                }
                j += 13;
            }
        }
        i += 4;
        // 查看下一个，地址不相同，则结束,此时将之前的数据帧结构修改
        // 或者已经到表的末尾了，也结束
        if((ControlDeviceID != Net.Buf[38 + allcnt01 + i]) || (i == allcnt02))
        {
            return;
        }
    }
}

u8 CheckInitInfo(void)
{
	u16 crc;
	ReadInitLen(&Sys.InitLen);
	ReadInitCrc(&Sys.Crc);
	if(Sys.InitLen)//有配置信息
	{
		FramReadBurstByte(BaseInfoAddr,Net.Buf, Sys.InitLen);
        crc = CalCrc16(Net.Buf, Sys.InitLen, 0);
		if(crc == Sys.Crc)//主信息存储区中配置信息正确
			return 1;
		
		//主信息存储区中配置信息错误，读取备用存储区中配置信息
		FramReadBurstByte(BK_BaseInfoAddr,Net.Buf, Sys.InitLen);
        crc = CalCrc16(Net.Buf, Sys.InitLen, 0);
		if(crc == Sys.Crc)
			return 1;
	}
	return 0;
}

// 读取初始化信息
void ReadDefaultInit(void)
{
	u8 flag = 0;

	if(!CheckInitInfo())//配置信息CRC校验错误，请求上位机重新配置
	{
		Sys.Crc = 0;
		flag = 0;
	}
	else            //有正确的配置信息
	{
		flag = 1;
		ReadDeviceDefine();      //设备定义
		CalDeviceCrc();          //计算相应设备的crc
	}

	if(flag == 0)          //网关没有数据信息
	{
		UpdateInitCrc(0);
		UpdateInitLen(0);
	}
}

void WriteRecord(u16 addr,u8 *buf,u16 len)
{
	buf[len] = SumAll(buf, len);
    FramWriteBurstByte(Sys.W + RECORDBASEADDR, buf, len+1);
    Sys.W += (len+1);
    if(Sys.W >= FramEnd)
        Sys.W = FramBegin;
    UpdateWritePtr(Sys.W);
}

u8 ReadRecord(u16 addr, u8 *buf)
{
    u8 t[11], i;
    FramReadBurstByte(addr + RECORDBASEADDR, t, 11);
    if(SumCheck(t, 11))
    {
        for(i = 0; i < 10; i ++)
            buf[i] = t[i];
        return 1;
    }
    else
        return 0;
}


