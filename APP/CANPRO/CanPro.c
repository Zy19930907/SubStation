#include "CanPro.h"
#include "Config.h"
#include "CrcMaker.h"

u32 TASKID_CANPRO = 0xFFFFFFFF;

_AckAddr Ack;
_SendInit Init;
_LianDong LianDong;

//初始化6路CAN
void InitCan(void)
{
    u8 i;
    for(i = 0; i < 6; i++)
    {
        CanxInit(i);
        Can.Tick[i] = SYS_TICK;
    }
}

//检查CAN芯片是否死机
void CheckCanDev(void)
{
    u8 i;
    for(i = 0; i < 6; i++)
    {
        if(MsTickDiff(Can.Tick[i]) >= 30000)
        {
            CanxInit(i);
            Can.Tick[i] = SYS_TICK;
        }
    }
}

void CanPro(void)
{
	u8 i;
    switch(Can.Status)
    {
    case CANINIT:
		SetTaskStatus(TASKID_CANPRO,"CAN初始化");
        InitCan();
        Can.Status = CANRECV;
        break;
    case CANRECV:
		SetTaskStatus(TASKID_CANPRO,"CAN数据接收");
        for(i = 0; i < 6; i++)
			CanRecvData(i);
        Can.Status = CANCHECKDEV;
        break;
    case CANCHECKDEV://长时间未收到数据重新初始化CAN外设
		SetTaskStatus(TASKID_CANPRO,"CAN硬件检测");
        CheckCanDev();
        Can.Status = CANRECV;
        break;
    }
}

// CAN 总线归属，返回1,2,3,4,5   其中，5代表没有该设备信息，则广播发送
u8 CanBusBelong(u8 addr)
{
    u8 n;
    if(!(Device[addr - 1].Flag & 0x03)) // 网关没有收到该设备的信息
        return 0;
    n = (Device[addr - 1].Status & 0x60);
    n >>= 5;
    return (n);
}

// 查看是否需要有联动
void CheckTriggerStatus(void)
{
    u8 i, warn;
    warn = 0;
    for(i = 0; i < MaxTriggerNum; i ++)
    {
        warn |=  Excute[i].Warn;
        if(Excute[i].ExcuteAddr != Excute[i + 1].ExcuteAddr)
        {
            if((Device[Excute[i].ExcuteAddr - 1].Warn & 0x7F) != warn)      // 有变化
                Device[Excute[i].ExcuteAddr - 1].Warn = (warn + 0x80);
            warn = 0;
        }
    }
}

void CanSendLianDongInfo(void)
{
	u8 i,j;
	u8 buf[8];
	for(i = 0;i < MaxDeviceNum;i ++)
	{
		if((Device[i].Warn & 0x80) && (!(Device[i].Status & 0x80)))        // 需要发送
		{
			Can.ID = (u32) MakeFramID(0,0x39,i+1);
			buf[0] = Device[i].Warn & 0x7F;
			buf[1] = Excute[j].TriggerAddr;
			buf[2] = Device[i].Name;
			CanSendData(CanBusBelong(i+1),Can.ID,&buf[0],3);
			for(j = 0; j < MaxTriggerNum; j ++)
			{
				if(Excute[j].TriggerAddr == 0)
					break;
				if(Excute[j].ExcuteAddr == i+1 && (Excute[j].Warn !=0))
					SaveBreakPowerLog(i+1,Excute[j].TriggerAddr,buf[0]);
			}
			LianDong.Retry ++;
			if(LianDong.Retry > 5)
			{
				Device[i].Warn &= 0x7F;
				LianDong.Retry = 0;
			}
			break;
		}
	}
}

void SendLianDongTask(void)
{
	CheckTriggerStatus();
	CanSendLianDongInfo();
}

// 有可能一个传感器关联多个联动设备，如[3,5][4,5]
void CheckLianDongInfo(u8 addr, u8 trigger, u8 val)
{
    u8 i, flag, j;
    flag = 0;
    if(!(Device[addr - 1].Flag & 0x10))        // 没有关联控制设备，直接退出
        return;
    for(i = 0; i < MaxTriggerNum; i ++)
    {
        if(Excute[i].TriggerAddr == 0)
        {
            if(flag == 0)
                return;
            else
                break;
        }
        if((Excute[i].TriggerAddr == addr) && (Excute[i].Trigger & trigger))
        {
            j = Excute[i].ExcuteAddr - 1;
            if((Device[j].Name == DuanDianQi) && (Device[j].Status & 0x0C))      // 断电器有手控或者有交控
                continue;
            if(!(Device[j].Status & 0x80))     // 只有通讯正常的设备才处理
            {
                if(val)         // 需要联动，由于之前协议已经定了，断电器D0，读卡器和广播D1
                {
                    if(Device[j].Name == DuanDianQi)
                        Device[j].Status |= 0x01;       // 断电
                    else
                        Device[j].Status |= 0x02;
                }
            }
            if(val != Excute[i].Warn)
            {
                LianDong.Retry = 0;
                flag = 1;
                if(val == 0)       // 取消标记
                    Excute[i].Warn &= ~trigger;
                else              // 增加标记
                    Excute[i].Warn |= trigger;
            }
        }
    }
    if(flag == 1)
        CheckTriggerStatus();
}

// 查询电源所在的存储编号
u8 CheckPowerIndex(u8 addr)
{
    u8 i;
    for(i = 0; i < MaxPowerNum; i ++)
    {
        if(addr == Power[i].Addr)
            return i;
    }
    return 0xFF;
}
// 首先判断该地址是否存在在设备列表中，如果存在，则在判断是否填充
u8 FilterPowerAddr(u8 addr)
{
    u8 j;
    if(!(Device[addr - 1].Flag & 0x01))
        return 0xFF;
    for(j = 0; j < MaxPowerNum; j ++)   //再查找该地址是否已经在电源列表中
    {
        if(addr == Power[j].Addr)
            return j;
    }
    // 该地址不存在，则添加到缓冲区中(地址列表为0，则填充)
    for(j = 0; j < MaxPowerNum; j ++)
    {
        if(Power[j].Addr == 0)
            return j;
    }
    return 0xFF;
}

u8 MakeCanBusStatus(u8 CanIndex)
{
    u8 status;
    switch(CanIndex+1)                          //CAN总线序数
    {
    case 1:
        status = 0x00;
        break;               //第 1...4条CAN总线
    case 2:
        status = 0x20;
        break;
    case 3:
        status = 0x40;
        break;
    case 4:
        status = 0x60;
        break;
    default :
        status = 0x00;
        break;
    }
    return status;
}

u32 MakeFramID(u8 Fram, u8 Cmd, u8 DestAddr)
{
    u32 ID;
    ID = 0;
    ID = Fram;         // 帧序数
    ID <<= 7;
    ID += Cmd;           // 命令  7bit
    ID <<= 17;
    ID += DestAddr;    // 目标地址  8bit
    return ID;
}

void NetInfoToCan(u8 cmd, u8 *buf, u8 len)
{
    u8 i, index, t[8];
    index = CanBusBelong(buf[0]);
    Ack.NeedToNet = 1;
    Ack.Addr = buf[0];
    Ack.Tick = SYS_TICK;
    Can.ID = (u32)MakeFramID(0, cmd, buf[0]);
    for(i = 1; i < len; i ++)
        t[i - 1] = buf[i];
    CanSendData(index, Can.ID, t, len - 1);
}

/*
 总共8帧数据：帧序数0----7  前面7帧都是7个字节，最后一帧5个字节
 * 每一帧的第一个字节是帧序数+交直流状态
 */
void WritePowerRecord(u8 addr, u8 *buf)
{
    u8 i, j, t, fram;
    if(addr == 0)
        return;
    i = FilterPowerAddr(addr);
    if(i == 0xFF)
        return;
    Power[i].Addr = addr;
    fram = buf[0] >> 1;
    if(fram == 7)
        t = 4;
    else
        t = 6;
    for(j = 0; j < t; j ++)
    {
        Power[i].BatInfo[fram * 6 + j] = buf[1 + j];
    }
}
// 同步时间
void SyncTime(u8 CanIndex, u8 addr)
{
    u8 buf[6];
    Can.ID = (u32)MakeFramID(0, 0x23, addr);
    buf[0] = SD2068.Hex[0];       // 年
    buf[1] = (SD2068.Hex[3] << 5) + SD2068.Hex[1];      // 月和date合并
    buf[2] = SD2068.Hex[2];
    buf[3] = SD2068.Hex[4];
    buf[4] = SD2068.Hex[5];
    buf[5] = SD2068.Hex[6];      // 秒
    CanSendData(CanIndex, Can.ID, buf, 6);
}

u16 ShiftTimeDiff(u16 t)
{
    if(SD2068.AllSecond >= t)
        return SD2068.AllSecond - t;
    else
        return 3600 - t + SYS_TICK;
}


void CanSendInitInfo(void)
{
     u8 i,buf[8];
     switch(Init.Status)
     {
     case INITIDLE:
          if(Init.W == 0)
               break;
          if(Init.W <= 8)
               Init.FramStatus = F_Single;
          else
               Init.FramStatus = F_First;
          Init.FramCnt = 0;
          Init.Status = SENDPREPARE;
          break;
     case SENDPREPARE:
          // D27 D26 帧类型  01 首帧 10 中间帧  11 末尾帧   D25 D24  00...11实际帧序数
          // 对应 frame D3 D2帧类型  D1 D0帧序数
          if(Init.FramStatus == F_Single)
          {
               Can.ID = MakeFramID(0,0x51,Init.DestAddr);
               Init.Shift = Init.W;
          }
          if(Init.FramStatus == F_First)
          {
               Can.ID = MakeFramID(0x04+Init.FramCnt,0x51,Init.DestAddr);
               Init.Shift = 8;
          }
          if(Init.FramStatus == F_Middle)
          {
               Can.ID = MakeFramID(0x08+Init.FramCnt,0x51,Init.DestAddr);
               Init.Shift = 8;
          }
          if(Init.FramStatus == F_End)
          {
               Can.ID = MakeFramID(0x0C+Init.FramCnt,0x51,Init.DestAddr);
               Init.Shift = Init.W - Init.R;
          }
          for(i = 0;i < Init.Shift;i ++)
               buf[i] = Init.Buf[Init.R + i];
          Init.Tick = SYS_TICK;
          while(MsTickDiff(Init.Tick) < 20);
          Init.Status = SENDING;
          Init.Tick = SYS_TICK;
          CanSendData(CanBusBelong(Init.DestAddr),Can.ID,buf,Init.Shift);
          Init.Retry ++;
          break;
     case SENDING:
          if(Init.Retry > 5)
          {
               Init.Retry = 0;
               Init.Status = INITIDLE;
               Init.W = 0;
               Init.R = 0;
          }
          if(MsTickDiff(Init.Tick) > 1500)       //1.5s超时,退出
               Init.Status = INITIDLE;  
          break;
     default:
          Init.Status = INITIDLE;
          break;
     }
}


//  简单比较时间：比较分钟和秒数据即可,在５秒误差之内
//  D11.....D6 分　　　　　　D5.....D0 秒
//  需要同步时间，返回1，不需要同步时间，返回0
u8 CheckTime(u8 *buf)
{
    u16 t;
    u8 min, sec;
    if(Sys.SyncLeft)
        return 0;
    t = buf[1];
    t <<= 8;
    t += buf[0];
    t &= 0x0FFF;
    min = t >> 6;
    sec = t & 0x3F;
    t = (min * 60) + sec;   // 判断时钟相差5s
    if(t <= SD2068.AllSecond)
    {
        if(((SD2068.AllSecond - t) < TimeShift) || ((t + 3600 - SD2068.AllSecond) < TimeShift))
            return 0;
    }
    else
    {
        if(((t - SD2068.AllSecond) < TimeShift) || ((3600 + SD2068.AllSecond - t) < TimeShift))
            return 0;
    }
    return 1;
}

// 存储11个字节代表一条记录
// 编号、名称、  状态、数据1、数据2、保留、时间域4字节   SUM
// 参数t代表地址编号，flag代表存储类型
void WriteSensorRecord(u8 addr, u8 flag)
{
    u8 buf[11], D5Bit;
    if(Sys.Flag & 0x02)    // 如果通讯中断，则D5置1
        D5Bit = 0x20;
    else
        D5Bit = 0;
    switch(flag)
    {
    case DEVICERESET:
        buf[0] = addr;   //地址
        buf[1] = Device[addr - 1].Name;  //名称
        buf[2] = 0x40 + D5Bit;    //代表是复位记录
        buf[3] = Can.Buf[2];
        buf[4] = 0;
        buf[5] = 0;

        buf[6] = Can.Buf[3];      //时间参数
        buf[7] = Can.Buf[4];
        buf[8] = Can.Buf[5];
        buf[9] = Can.Buf[6];
        break;
    case SENSORVARY:
    case SENSORAUTOSEND:
        buf[0] = addr;    //地址
        buf[1] = Device[addr - 1].Name; //名称
        buf[2] = (Device[addr - 1].Status & 0x0F) + D5Bit; //状态属性--通讯是否中断
        buf[3] = Can.Buf[0];//Device[addr-1].Buf[0];  //数据
        buf[4] = Can.Buf[1] & 0x07;//Device[addr-1].Buf[1];
        buf[5] = 0x00;//Device[addr-1].Buf[2];
        if(flag == SENSORAUTOSEND)
        {
            buf[6] = Can.Buf[2];
            buf[7] = Can.Buf[3];
            buf[8] = Can.Buf[4];
            buf[9] = Can.Buf[5];
        }
        else
        {
            buf[6] = Can.Buf[3];
            buf[7] = Can.Buf[4];
            buf[8] = Can.Buf[5];
            buf[9] = Can.Buf[6];
        }
        break;

    case CARDVARY:
        buf[0] = addr;    //地址
        buf[1] = Device[addr - 1].Name; //名称
        buf[2] = (Can.Buf[1] >> 4) + D5Bit;  //状态属性--小数点和加热预热等
        buf[3] = Can.Buf[0];
        buf[4] = (Can.Buf[1] & 0x0F);
        buf[5] = Can.Buf[2];    //RSSI

        buf[6] = Can.Buf[3];
        buf[7] = Can.Buf[4];
        buf[8] = Can.Buf[5];
        buf[9] = Can.Buf[6];
        break;
    case NEWCARDVARY:
        buf[0] = addr;    //地址
        buf[1] = Device[addr - 1].Name; //名称
        buf[2] = Can.Buf[3] + D5Bit;  //状态属性--小数点和加热预热等
        buf[3] = Can.Buf[0];
        buf[4] = (Can.Buf[1] & 0x3F);
        buf[5] = Can.Buf[2];    //RSSI

        buf[6] = Can.Buf[4];
        buf[7] = Can.Buf[5];
        buf[8] = Can.Buf[6];
        buf[9] = Can.Buf[7];
        break;
    case NETRESET:           //网关自身复位
        buf[0] = Sys.Addr;       //地址
        buf[1] = NetAttri;      //名称
        buf[2] = 0x40 + D5Bit;          //状态
        buf[3] = Sys.ResetTimes;  //数据
        buf[4] = 0;
        buf[5] = 0;
        TimeChange(&buf[6]);   //buf[6,7,8,9]装载时间
        break;  /*
     case LianDongRecord:
          buf[0] = addr;    //地址
          buf[1] = Device[addr-1].Name;   //名称
          buf[2] = 0x10 + D5Bit;         //代表是联动记录
          for(i = 0;i < 7;i ++)
               buf[3+i] = Can.Buf[i];
          break;       */
    }
	WriteRecord(Sys.W,buf,10);
}

// 传递参数：CAN编号  命令  目标地址
// 如果是风电闭锁网关，则回应广播信息，并且带上断电控制状态
void AckFunc(u8 index, u8 cmd, u8 DestAddr)
{
    u8 buf[8];
#ifdef FengDianNet
    Can.ID = (u32) MakeFramID(0, cmd, 0);
    buf[0] = FengDian.DuanDianStatus;
    CanSendData(index, Can.ID, &buf[0], 1);
#else
    Can.ID = (u32) MakeFramID(0, cmd, DestAddr);
//  if(Device[DestAddr - 1].Flag & 0x04)            // 参与断线控制
	if(Device[DestAddr - 1].Warn)
        Can.ID |= 0x8000;
    buf[0] = Device[DestAddr - 1].Warn & 0x7F;
    if(Sys.Delay)                                  // 倒计时未结束，不发控制指令
        CanSendData(index, Can.ID, &buf[0], 0);
    else
	{
		if(Device[DestAddr - 1].Name == 0x28)
		{
			buf[1] = Device[DestAddr-1].BoardCastGroup;
			CanSendData(index, Can.ID, &buf[0], 2);
		}else
			CanSendData(index, Can.ID, &buf[0], 1);
	}
#endif
}

// 首先查看设备的配置crc是否一致，如果不一致，发送配置命令，如果一致，则发送正常回应命令
// flag: 0 表示开关量传感器(无信息域)，1代表其它设备
void CheckCrc(u8 addr, u8 crc, u8 flag)
{
    u16 tick;
    if(Sys.InitLeft)                //该变量为0，才能允许同步设备
        return;
    if(Init.Status != INITIDLE)     //如果正在下发初始化
        return;
    if(Device[addr - 1].CrcCnt > 5)        //配置次数大于5次，证明初始化出错异常，不配置
    {
//      Device[addr-1].Status |= 0x10;
        return;
    }
    if(crc == Device[addr - 1].Crc) //设备配置过，则检验上下配置是否一致
    {
        Device[addr - 1].CrcCnt = 0;
        Device[addr - 1].Status &= ~0x10;
        Device[addr - 1].Flag &= ~0x20;
        return;
    }
    Device[addr - 1].Flag |= 0x20;   // 处于配置中
    tick = SYS_TICK;
    while(MsTickDiff(tick) < 20);
    Device[addr - 1].CrcCnt ++;
    if(Device[addr - 1].Crc == 0)    //上位机没有配置过,则下发清除配置指令
    {
        Can.ID = (u32) MakeFramID(0, 0x52, addr);
        CanSendData(CanBusBelong(addr), Can.ID, &Can.Buf[0], 0);
        return;
    }
    else      //上位机有初始化信息，则先组合数据
    {
        Init.R = 0;
        Init.W = 0;
        if(!flag)     // 开关量传感器只关心是否参与控制和断线触发
            Init.Buf[Init.W++] = Device[addr - 1].Crc;
        else          // 只有断电器、广播、读卡器、传感器才需要去读取相关信息
            ReadInitInfo(addr);
    }
    Init.DestAddr = addr;
}

//数据上传：D15正负号  D14 D13小数点  D12 D11 状态
void MakeSensorCanData(u32 ID, u8 CanIndex, u8 addr)
{
    u8 status;
    status = 0;
    status += ((Can.Buf[1] >> 3) & 0x03);     //状态
    status <<= 2;
    status += ((Can.Buf[1] >> 5) & 0x03);     //小数点
    status &= 0x0F;
    status += MakeCanBusStatus(CanIndex);

    Device[addr - 1].Status = status;           //状态位

    if(ID & 0x00010000)                         // D16: 断电测试，自检数据
        Device[addr - 1].Status |= 0x10;

    if((Sys.BaiaoJiaoTime) && (Sys.BiaoJiaoAddr == addr))   //插入人为标定数据
    {
        Device[addr - 1].Status &= ~0x0C;
        Device[addr - 1].Status += 0x08;
        Sys.BiaoJiaoVal += (SYS_TICK % 30);
        if(Sys.BiaoJiaoVal > 199)
            Sys.BiaoJiaoVal = 200;
        if(Sys.BiaoJiaoVal > 100)
        {
            if(Sys.BiaoJiaoDuanDianAddr)
                Device[Sys.BiaoJiaoDuanDianAddr - 1].Status |= 0x03;
        }
        Device[addr - 1].Buf[0] = Sys.BiaoJiaoVal;
        Device[addr - 1].Buf[1] = 0;

    }
    else     // 数据发生变化，则赋值
    {
        if((Device[addr - 1].Buf[0] !=  Can.Buf[0]) || (Device[addr - 1].Buf[1] != (Can.Buf[1] & 0x07)))
        {
            Device[addr - 1].Buf[0] =  Can.Buf[0];
            if((Can.Buf[0] == 0xFF) && ((Can.Buf[1] & 0x07) == 0x07))
                Device[addr - 1].Buf[1] = 0xFF;
            else
                Device[addr - 1].Buf[1] = (Can.Buf[1] & 0x07);
            if(Sys.Flag & 0x02)                  // 与上位机通讯中断，则存储数据
                WriteSensorRecord(addr, SENSORAUTOSEND);
        }
    }
}

// 依次是 CH4 O2 CO WD
void MakeMASWirelessSensor(u8 CanIndex, u8 addr)
{   // CH4
    Device[addr - 1].Tick =  SYS_TICK;
    Device[addr - 1].Name =  26;
    Device[addr - 1].Status = MakeCanBusStatus(CanIndex) + 0x02;
    Device[addr - 1].Buf[0] =  Can.Buf[0];
    Device[addr - 1].Buf[1] =  Can.Buf[1] & 0x07;
    // O2
    Device[addr].Tick =  SYS_TICK;
    Device[addr].Name =  27;
    Device[addr].Status = MakeCanBusStatus(CanIndex) + 0x01;
    Device[addr].Buf[0] =  Can.Buf[2];
    Device[addr].Buf[1] =  Can.Buf[3] & 0x07;
    // CO
    Device[addr + 1].Tick =  SYS_TICK;
    Device[addr + 1].Name =  28;
    Device[addr + 1].Status = MakeCanBusStatus(CanIndex) + 0x00;
    Device[addr + 1].Buf[0] =  Can.Buf[4];
    Device[addr + 1].Buf[1] =  Can.Buf[5] & 0x07;
    // WD
    Device[addr + 2].Tick =  SYS_TICK;
    Device[addr + 2].Name =  29;
    Device[addr + 2].Status = MakeCanBusStatus(CanIndex) + 0x01;
    Device[addr + 2].Buf[0] =  Can.Buf[6];
    Device[addr + 2].Buf[1] =  Can.Buf[7] & 0x07;
}

void HandleCanData(u32 ID, u8 CanIndex)
{
    u8 addr, cmd, status;
    status = 0;
    addr = Can.ID;
    Can.Tick[CanIndex] = SYS_TICK;
    if((addr == 0) || (addr > MaxDeviceNum))
        return;
    cmd = ((ID >> 17) & 0x7F);
    if((cmd != 0x0B) && (cmd != 0x0C))                  // 0B 0C 为多参数设备上传信息,关联梅安森的
    {
        Device[addr - 1].Name = ((ID >> 8) & 0x3F);    //设备名称
        Device[addr - 1].Tick = SYS_TICK;
        Device[addr - 1].Status &= ~0x80;
        CheckLianDongInfo(addr, 0x10, 0);
        Device[addr - 1].RecTimes ++;
    }
    if(!(Device[addr - 1].Flag & 0x01))  // 设备无效并且是主动上传数据
    {
        Device[addr - 1].Flag |= 0x02;  // 设备没有注册标记位
        CanSendData(CanIndex, MakeFramID(0, UnRegister, addr), &Can.Buf[0], 0);
        Device[addr - 1].Status = MakeCanBusStatus(CanIndex);
        if(cmd < 0x10)                 // 主动上传数据，不存储，如果是查询命令等，需要应答
            return;
    }
    if(Ack.NeedToNet == 1)       //应答超时判断
    {
        if(MsTickDiff(Ack.Tick) > 1000)    // 1S 超时
            Ack.NeedToNet = 0;
    }
#ifdef FengDianNet
    if(addr < 6)   //只判断 A1 A2 A3 D4 D5状态  0 正常 置为正常
        FengDian.ComNormal &= ~(1 << (addr - 1));
#endif
    switch(cmd)        //根据命令来跳转不同的协议
    {
    case 0x01:        //传感器主动上传当前状态：数据域（2字节） 时间域（4字节） 校验（1字节）
        if(ID & 0x01000000)                       // 产生断电,暂时只考虑断电的情况
            CheckLianDongInfo(addr, 0x01, 0x01);
        else
            CheckLianDongInfo(addr, 0x01, 0x00);
        if(CheckTime(&Can.Buf[2]) == 1)
        {
            SyncTime(CanIndex, addr);
            //return;                   // 时钟信息不对，则丢弃该数据帧
        }
        else
			AckFunc(CanIndex, ACK, addr);
        MakeSensorCanData(ID, CanIndex, addr);  //总线编号
        CheckCrc(addr, Can.Buf[6], 1);
        break;
    case 0x02:
        AckFunc(CanIndex, ACK, addr);
        if((Can.Buf[0] == 0xFF) && (Can.Buf[1] == 0xFF))  //复位信息
        {
            WriteSensorRecord(addr, DEVICERESET);
            break;
        }
        WriteSensorRecord(addr, SENSORVARY);
        break;
    case 0x03:    //开关量/频率量经过转换板上传，3字节模式：2字节数据+1字节crc
        status += MakeCanBusStatus(CanIndex);
        Device[addr - 1].Status &= 0x0E; // 清除 D6 D5 D0
        Device[addr - 1].Status += status;
        if(Device[addr - 1].Name == 0x21) //频率转换板,上传频率
        {
            Device[addr - 1].Buf[0] = Can.Buf[0];
            Device[addr - 1].Buf[1] = Can.Buf[1];
            AckFunc(CanIndex, ACK, addr);
        }
        else // 开关量传感器，上传0或者1  2个字节传输，0000表示0  0001表示1
        {
            if(ID & 0x01000000)      // 发生了变化并且有关联设备触发
            {
                if(Device[addr - 1].Crc & 0x08)           // 1态触发
                {
                    if(Can.Buf[1] & 0x01)
                        CheckLianDongInfo(addr, 0x40, 0x40);
                }
                if(Device[addr - 1].Crc & 0x04)           // 0态触发
                {
                    if(!(Can.Buf[1] & 0x01))
                        CheckLianDongInfo(addr, 0x20, 0x20);
                }
            }
            else
            {
                if(Device[addr - 1].Crc & 0x08)          // 1态触发
                    CheckLianDongInfo(addr, 0x40, 0);
                if(Device[addr - 1].Crc & 0x04)           // 0态触发
                    CheckLianDongInfo(addr, 0x20, 0);
            }
            Device[addr - 1].Status += (Can.Buf[1] & 0x01); //状态位+开停
            Device[addr - 1].Buf[0] = 0;
            Device[addr - 1].Buf[1] = 0;
            AckFunc(CanIndex, ACK, addr);
            CheckCrc(addr, Can.Buf[2], 0);
        }
        break;
    case 0x04:                 //读卡器定时自动上传信息
        if(CheckTime(&Can.Buf[0]) == 1)
        {
            SyncTime(CanIndex, addr);
            return;
        }
        else
            AckFunc(CanIndex, ACK, addr);
        Device[addr - 1].Status = MakeCanBusStatus(CanIndex);      //状态位
        if(Device[addr - 1].Warn)
            Device[addr - 1].Status |= 0x02;
        Device[addr - 1].Buf[0] = 0;
        Device[addr - 1].Buf[1] = 0;
#ifndef FengDianNet
        CheckCrc(addr, Can.Buf[4], 1);
#endif
        break;
    case 0x05:                //读卡器上传卡片数据， 卡片编号低  高  RSSI  时间域
        AckFunc(CanIndex, ACK, addr);
        if((Can.Buf[0] == 0xFF) && (Can.Buf[1] == 0xFF))  //复位信息
        {
            WriteSensorRecord(addr, DEVICERESET);
            Device[addr - 1].Buf[0] = 0x00;      //状态位
            break;
        }
        Device[addr - 1].Status &= 0x0F;
        Device[addr - 1].Status += MakeCanBusStatus(CanIndex);      //状态位
        if(Can.Len == 7)
            WriteSensorRecord(addr, CARDVARY);
        if(Can.Len == 8)
            WriteSensorRecord(addr, NEWCARDVARY);
        break;
    case 0x06:              //电源设备信息上传,根据1---8帧来填充数据
        AckFunc(CanIndex, ACK, addr);
        Device[addr - 1].Status &= 0x0E;
        Device[addr - 1].Status += (MakeCanBusStatus(CanIndex) + (Can.Buf[0] & 0x01)); //状态位   交直流
        Device[addr - 1].Buf[0] = 0;
        Device[addr - 1].Buf[1] = 0;
        WritePowerRecord(addr, &Can.Buf[0]);
        break;
    case 0x07:          // 应急广播上传状态信息
        status += MakeCanBusStatus(CanIndex);
        Device[addr - 1].Status &= 0x0E;
        Device[addr - 1].Status += (status + (Can.Buf[0] >> 7)); //状态位+按键状态位
        if(Device[addr - 1].Warn & 0x7F)
			Device[addr - 1].Status |= 0x02;
        Device[addr - 1].Buf[0] = Can.Buf[0] & 0x7F;
        Device[addr - 1].Buf[1] = Can.Buf[1];
        AckFunc(CanIndex, ACK, addr);
        CheckCrc(addr, Can.Buf[5], 1);
        break;
    case 0x08:        //广播联动记录
        break;
    case 0x09:             //断电器信息,当前状态
        if(CheckTime(&Can.Buf[2]) == 1)
            SyncTime(CanIndex, addr);
        else
            AckFunc(CanIndex, ACK, addr);
        if((Can.Buf[0] == 0xFF) && (Can.Buf[1] == 0xFF))  //复位信息
        {
            WriteSensorRecord(addr, DEVICERESET);
            break;
        }
        if(Sys.BiaoJiaoDuanDianAddr)
            break;
        // buf[0]当前状态  buf[1]触发断电的传感器地址
        status += MakeCanBusStatus(CanIndex);
        Device[addr - 1].Status &= 0x80;      //D4位对断电器无效
        if((Device[addr - 1].Warn & 0x7F) && (!(Can.Buf[0] & 0x0C)))      //需要断电，则强制断电位
            Device[addr - 1].Status += (status + (Can.Buf[0] & 0x1E) + 0x01);
        else                                  //需要复电，则接收实时值
            Device[addr - 1].Status += (status + (Can.Buf[0] & 0x1F)); //D7 D6 D5 CAN总线当前状态
        Device[addr - 1].Buf[0] =  Can.Buf[0]; //断电器当前状态
        Device[addr - 1].Buf[1] =  Can.Buf[1]; //触发断电地址
#ifndef FengDianNet
        CheckCrc(addr, Can.Buf[6], 1);
#endif
        break;
    
    case 0x0A:                // 断电器上传联动记录  触发地址 触发状态  触发事件  时间4字节
    AckFunc(CanIndex,ACK,addr);
    if((Can.Buf[2] & 0x90) == 0x90)          // 断线联动
         Device[Can.Buf[0]-1].Status |= 0x80;
//    WriteSensorRecord(addr,LianDongRecord);
		
    break;
    case 0x0B:               // MAS的无线传感器传输数据
        AckFunc(CanIndex, ACK, addr);
        MakeMASWirelessSensor(CanIndex, addr);
        break;
    case 0x0F:                  // 分时复用上传数据
        MakeSensorCanData(ID, CanIndex, addr);
        break;
    case 0x10:   //只有2种情况会收到传感器发送的确认：1.上位机配置   2.网关自动配置传感器
        if((Ack.NeedToNet == 1) && (Ack.Addr == addr))
        {
            CanInfoToNet(Net.SocketSelect->socketn, ACK, &Can.Buf[0], 0);
            Ack.NeedToNet = 0;
            break;
        }
        break;
    case 0x39:      //对联动信息回应
        Device[addr - 1].Warn &= 0x7F;
        LianDong.Retry = 0;
        if(Device[addr-1].Name == DuanDianQi)           // 断电器
        {
             Device[addr-1].Status &= ~0x03;
             Device[addr-1].Status += Can.Buf[0];
        }
        break;
    case 0x51:      //初始化信息的回复
        Init.Retry = 0;
        if((Init.FramStatus == F_Single) || (Init.FramStatus == F_End))
        {
            Init.Status = INITIDLE;     //退出初始化
            Init.W = 0;
            break;
        }
        Init.R += Init.Shift;
        if(Init.R + 8 < Init.W)
            Init.FramStatus = F_Middle;
        else
            Init.FramStatus = F_End;
        Init.FramCnt ++;
        if(Init.FramCnt > 3)
            Init.FramCnt = 0;
        Init.Status = SENDPREPARE;
        break;
    default:                   // 其它指令，直接转发到上位机
        CanInfoToNet(Net.SocketSelect->socketn, cmd, &Can.Buf[0], Can.Len);
        break;
    }
}
