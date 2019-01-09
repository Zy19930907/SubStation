#include "NetDataPro.h"
#include "Config.h"
#include "CrcMaker.h"

u8 AckBuf[12];
//_LianDong LianDong[MaxLianDongNum];
//_LianDongStatus LianDongStatus;

void MakeFrameHeader(void)
{
    Net.Buf[0] = 0xEF;
    Net.Buf[1] = 0xEF;
    Net.Buf[2] = 0xEF;
    Net.Buf[3] = 0xEF;
    Net.Buf[4] = 0x00;
    Net.Buf[5] = Sys.Addr;
    Net.Buf[6] = 0x00;
    Net.Buf[7] = 0x00;
}

void SetCrcAndAck(u8 sn, u16 len)
{
    SetCrc16(Net.Buf, len, 0);
    NetManger.SendData(sn, Net.Buf, len + 2);
}

void AnswerAck(u8 sn, u8 flag)
{
    u8 t;
    t = 8;
    MakeFrameHeader();
    Net.Buf[t++] = 0x01;           //协议个数
    if(flag == 0)
    {
        Net.Buf[t++] = ACK;            //协议编码
        Net.Buf[t++] = 0x03;           //编码长度低八位
        Net.Buf[t++] = 0x00;           //编码长度高八位
    }
    else                                //倒计时...S
    {
        Net.Buf[t++] = PREPARE;
        Net.Buf[t++] = 0x04;           //编码长度低八位
        Net.Buf[t++] = 0x00;           //编码长度高八位
        Net.Buf[t++] = Sys.Delay;
    }
    Net.Buf[6] = t + 2;            //低八位
    Net.Buf[7] = 0;                //高八位
    SetCrcAndAck(sn, t);
}

void AnswerWrongAck(u8 sn)
{
    u8 t;
    t = 8;
    MakeFrameHeader();
    Net.Buf[t++] = 0x01;           //协议个数
    Net.Buf[t++] = WRONGACK;       //协议编码
    Net.Buf[t++] = 0x03;           //编码长度低八位
    Net.Buf[t++] = 0x00;           //编码长度高八位
    Net.Buf[6] = t + 2;            //低八位
    Net.Buf[7] = 0;                //高八位
    SetCrcAndAck(sn, t);
}

u16 CalPtrLen(void)
{
    if(Sys.W >= Sys.R)
    {
        if(Sys.W - Sys.R >= MaxOneFrame)
            Sys.Shift = MaxOneFrame;
        else
            Sys.Shift = Sys.W - Sys.R;
    }
    else
    {
        if(FramEnd - Sys.R + Sys.W - FramBegin >= MaxOneFrame)
            Sys.Shift = MaxOneFrame;
        else
            Sys.Shift = FramEnd - Sys.R + Sys.W - FramBegin;
    }
    return Sys.Shift;
}

// flag = 0 TCP   FLAG = 1 UDP
void AnswerCurInfo(u8 sn, u8 flag)
{
    u16 i, j, k, l, t;
    MakeFrameHeader();
    t = 8;
    if(Sys.R == Sys.W)       // 如果有变化量数据，则协议块个数为2，否则为1
        Net.Buf[t++] = 0x01;
    else
        Net.Buf[t++] = 0x02;

    Net.Buf[t++] = 0x60;        //协议编码
    Net.Buf[t++] = 0;          //编码长度低八位  编码 + 长度2字节 + 网关信息8字节 + N*5
    Net.Buf[t++] = 0;           //编码长度高八位, 最后更新

    Net.Buf[t++] = Sys.R;        //读指针
    Net.Buf[t++] = Sys.R >> 8;
    Net.Buf[t++] = Sys.W;        //写指针
    Net.Buf[t++] = Sys.W >> 8;

    TimeChange(&Net.Buf[t]);
    t += 4;
    j = 0;                      //判断设备有效个数，从而得到协议块实时数据长度
    for(i = 0; i < MaxDeviceNum; i ++)
    {
        if(Device[i].Flag & 0x01)    //设备有效
        {
            Net.Buf[t++] = i + 1;                //地址编号
            Net.Buf[t++] = Device[i].Name;       //名称
            Net.Buf[t++] = Device[i].Status;    //状态
            Net.Buf[t++] = Device[i].Buf[0];    //数据低八位
            Net.Buf[t++] = Device[i].Buf[1];    //数据高八位
            j ++;
        }
    }
    j = j * 5 + 11;
    Net.Buf[10] = j;    // 协议块数据长度
    Net.Buf[11] = j >> 8;

    if(Sys.R != Sys.W)
    {
        CalPtrLen();
        Net.Buf[t++] = 0x61;                    //CMD
        k = t;                         //临时记录当前长度
        Net.Buf[t++] = 0;           //长度稍后填充
        Net.Buf[t++] = 0;
        j = Sys.R;                      // 临时读指针
        l = 0;
        for(i = 0; i < Sys.Shift;)
        {
            if(ReadRecord(j, &Net.Buf[t]))
            {
                t += 10;
                l += 10;            //长度记录
            }
            i += 11;
            j += 11;
            if(j >= FramEnd)
                j = FramBegin;
        }
        Net.Buf[k] = l + 3;
        Net.Buf[k + 1] = (l + 3) >> 8;
    }

    Net.Buf[t++] = Sys.Crc;
    Net.Buf[t++] = Sys.Crc >> 8;

    Net.Buf[6] = t + 2;    //整个数据帧长度字节
    Net.Buf[7] = (t + 2) >> 8;
    SetCrcAndAck(sn, t);
}

void AnswerErrorBitsInfo(u8 sn, u8 *buf, u16 len)
{
    u16 i, t;
    MakeFrameHeader();
    t = 8;
    Net.Buf[t++] = 0x01;
    Net.Buf[t++] = 0x63;        //协议编码
    Net.Buf[t++] = 0;          //编码长度低八位  编码 + 长度2字节 + N*5
    Net.Buf[t++] = 0;           //编码长度高八位, 最后更新

    for(i = 0; i < len; i ++)
        Net.Buf[t++] = buf[i];

    Net.Buf[10] = len + 3;  // 协议块数据长度
    Net.Buf[11] = (len + 3) >> 8;

    Net.Buf[6] = t + 2;    //整个数据帧长度字节
    Net.Buf[7] = (t + 2) >> 8;
    SetCrcAndAck(sn, t);
}

void AnswerUnRegisterInfo(u8 sn)
{
    u16 i, j, t;
    MakeFrameHeader();
    t = 8;
    Net.Buf[t++] = 0x01;
    Net.Buf[t++] = 0x62;        //协议编码
    Net.Buf[t++] = 0;          //编码长度低八位  编码 + 长度2字节 + N*5
    Net.Buf[t++] = 0;           //编码长度高八位, 最后更新
    j = 0;                      //判断设备有效个数，从而得到协议块实时数据长度
    for(i = 0; i < MaxDeviceNum; i ++)
    {
        if(Device[i].Flag & 0x02)    //设备未注册
        {
            Net.Buf[t++] = i + 1;                //地址编号
            Net.Buf[t++] = Device[i].Name;       //名称
            j ++;
        }
    }
    j = j * 2 + 3;
    Net.Buf[10] = j;    // 协议块数据长度
    Net.Buf[11] = j >> 8;

    Net.Buf[6] = t + 2;    //整个数据帧长度字节
    Net.Buf[7] = (t + 2) >> 8;

    SetCrcAndAck(sn, t);
}

void AnswerResetCrcFail(u8 sn)
{
    Device[Net.Buf[12] - 1].CrcCnt = 0;
    AnswerAck(sn, 0);
}

void AnswerCleanNetCrc(u8 sn)
{
    u8 i;
    for(i = 0; i < MaxDeviceNum; i ++)       // 初始化默认，将所有的设备设置为未知类型
        Device[i].Name = 0xFF;
    Sys.Crc = 0;
    AnswerAck(sn, 0);
}

void AnswerSetNetLianDong(u8 sn, u8 *buf)
{
    if(buf[12] == 0x01)
        Sys.Flag |= 0x04;
    else
        Sys.Flag &= ~0x04;
    UpdateNetWarn(Sys.Flag);
    AnswerAck(sn, 0);
}

void AnswerCrcInfo(u8 sn)
{
    u16 i, j, t;
    MakeFrameHeader();
    t = 8;
    Net.Buf[t++] = 0x01;
    Net.Buf[t++] = 0x68;        //协议编码
    Net.Buf[t++] = 0;          //编码长度低八位  编码 + 长度2字节 + N*5
    Net.Buf[t++] = 0;           //编码长度高八位, 最后更新
    j = 0;                      //判断设备有效个数，从而得到协议块实时数据长度
    for(i = 0; i < MaxDeviceNum; i ++)
    {
        if(Device[i].Flag & 0x01)    //设备有效
        {
            Net.Buf[t++] = i + 1;                //地址编号
            Net.Buf[t++] = Device[i].Name;       //crc
            Net.Buf[t++] = Device[i].Crc;       //crc
            Net.Buf[t++] = Device[i].RecTimes;  //接收次数
            Net.Buf[t++] = Device[i].Warn;      // 触发报警条件
            j ++;
        }
    }
    j = j * 5 + 3;
    Net.Buf[10] = j;    // 协议块数据长度
    Net.Buf[11] = j >> 8;

    Net.Buf[6] = t + 2;    //整个数据帧长度字节
    Net.Buf[7] = (t + 2) >> 8;

    SetCrcAndAck(sn, t);
}

// 回复输入电压，网关地址，重启次数，当前时间,产品出厂时间
void AnswerBasePara(u8 sn, u8 *buf)
{
    u8 t;
    t = 8;
    MakeFrameHeader();

    Net.Buf[t++] = 0x01;           //协议个数
    Net.Buf[t++] = 0x71;           //协议编码
    Net.Buf[t++] = 0x0E;           //编码长度低八位
    Net.Buf[t++] = 0x00;           //编码长度高八位

    Net.Buf[t++] = Sys.Vol;
    Net.Buf[t++] = Sys.Addr;
    Net.Buf[t++] = Sys.ResetTimes;

    TimeChange(&Net.Buf[t]);
    t += 4;

    ReadFactoryTime(&Net.Buf[t]);
    t += 4;

    Net.Buf[t++] = Sys.Flag;           // 主要是D2:标定是否断电

    // 先修改数据个数
    Net.Buf[6] = t + 2;      // 低八位
    Net.Buf[7] = 0;          // 高八位
    SetCrcAndAck(sn, t);
}

// 回复软件版本号
void AnswerSoftVerb(u8 sn)
{
    u8 t;
    t = 8;
    MakeFrameHeader();

    Net.Buf[t++] = 0x01;           //协议个数
    Net.Buf[t++] = 0x72;           //协议编码
    Net.Buf[t++] = 0x04;           //编码长度低八位
    Net.Buf[t++] = 0x00;           //编码长度高八位

    Net.Buf[t++] = SoftVerb;
    // 先修改数据个数
    Net.Buf[6] = t + 2;      // 低八位
    Net.Buf[7] = 0;          // 高八位
    SetCrcAndAck(sn, t);
}

// 回复IP地址和端口号
void AnswerIpAddr(u8 sn)
{
    u8 t;
    t = 8;
    MakeFrameHeader();
    Net.Buf[t++] = 0x01;           //协议个数
    Net.Buf[t++] = 0x70;           //协议编码
    Net.Buf[t++] = 0x10;           //编码长度低八位
    Net.Buf[t++] = 0x00;           //编码长度高八位

    ReadNetIpAddr(&Net.Buf[t]);
    t += 6;
    ReadGateWayIp(&Net.Buf[t]);
    t += 4;
    ReadUdpIp(&Net.Buf[t]);
    t += 6;

    // 先修改数据个数
    Net.Buf[6] = t + 2;      // 低八位
    Net.Buf[7] = 0;          // 高八位
    SetCrcAndAck(sn, t);
}

void AnswerForBootLoader(u8 sn, u8 *buf)
{
    UpdateBootLoader(&buf[12]);
    AnswerAck(sn, 0);
    Net.CloseAllSocket();
    Sys_Soft_Reset();
}

void AnswerCrcFail(u8 sn)
{
    u16 i, j, t;
    MakeFrameHeader();
    t = 8;
    Net.Buf[t++] = 0x01;
    Net.Buf[t++] = 0x66;        //协议编码
    Net.Buf[t++] = 0;          //编码长度低八位  编码 + 长度2字节 + N*5
    Net.Buf[t++] = 0;           //编码长度高八位, 最后更新
    j = 0;                      //判断设备有效个数，从而得到协议块实时数据长度
    for(i = 0; i < MaxDeviceNum; i ++)
    {
        if(Device[i].CrcCnt > 5)    //设备有效
        {
            Net.Buf[t++] = i + 1;                //地址编号
            Net.Buf[t++] = Device[i].Name;       //名称
            j ++;
        }
    }
    j = j * 2 + 3;
    Net.Buf[10] = j;    // 协议块数据长度
    Net.Buf[11] = j >> 8;

    Net.Buf[6] = t + 2;    //整个数据帧长度字节
    Net.Buf[7] = (t + 2) >> 8;

    SetCrcAndAck(sn, t);
}

void AnswerCheckCrcCntInfo(u8 sn)
{
    u8 t;
    t = 8;
    MakeFrameHeader();
    Net.Buf[t++] = 0x01;           //协议个数
    Net.Buf[t++] = 0x64;            //协议编码
    Net.Buf[t++] = 0x04;           //编码长度低八位
    Net.Buf[t++] = 0x00;           //编码长度高八位
    Net.Buf[t++] = Sys.CrcCnt;
    Net.Buf[6] = t + 2;            //低八位
    Net.Buf[7] = 0;                //高八位
    SetCrcAndAck(sn, t);
}

void AnswerCleanCrcCntInfo(u8 sn)
{
    Sys.CrcCnt = 0;
    UpdateNetCrcCnt(Sys.CrcCnt);
    AnswerAck(sn, 0);
}

void AnswerSetTime(u8 sn, u8 *buf)
{
    SD2068.Buf[0] = buf[12];           // 年
    SD2068.Buf[1] = buf[13] & 0x1f;      //月和周几合并，;
    SD2068.Buf[2] = buf[14];          //日
    SD2068.Buf[3] = buf[13] >> 5;     //date
    SD2068.Buf[4] = buf[15] & 0x3f;   //小时 6
    SD2068.Buf[5] = buf[16];
    SD2068.Buf[6] = buf[17];
    WriteTime();
    AnswerAck(sn, 0);
}

void AnswerFactoryTime(u8 sn, u8 *buf)
{
    u8 t;
    u32 time;
    buf[15] &= 0x1F;
    for(t = 12; t < 18; t ++)
        Net.Buf[t] = (buf[t] / 16) * 10 + buf[t] % 16;
    t = 0;
    time = 0;
    time = Net.Buf[12];    //年
    time <<= 4;
    time += Net.Buf[13];   //月
    time <<= 5;
    time += Net.Buf[14];   //日
    time <<= 5;
    time += Net.Buf[15];   //小时
    time <<= 6;
    time += Net.Buf[16];   //分
    time <<= 6;
    time += Net.Buf[17];   //秒
    UpdateFactoryTime(time);
    AnswerAck(sn, 0);
}

void AnswerSetMac(u8 sn, u8 *buf)
{
    UpdateMacAddr(&buf[12]);
    AnswerAck(sn, 0);
    Sys_Soft_Reset();
}

void AnswerCheckMac(u8 sn)
{
    u8 t, i;
    t = 8;
    MakeFrameHeader();

    Net.Buf[t++] = 0x01;           //协议个数
    Net.Buf[t++] = 0x88;           //协议编码
    Net.Buf[t++] = 0x09;           //编码长度低八位
    Net.Buf[t++] = 0x00;           //编码长度高八位

    for(i = 0; i < 6; i ++)
        Net.Buf[t++] = Net.MacAddr[i];

    // 先修改数据个数
    Net.Buf[6] = t + 2;      // 低八位
    Net.Buf[7] = 0;          // 高八位
    SetCrcAndAck(sn, t);
}

void AnswerSetNetBiSuoFilterTime(u8 sn, u8 *buf)
{
    UpdateBiSuoFilterTime(&buf[12]);
    AnswerAck(sn, 0);
}

void AnswerCheckNetBiSuoFilterTime(u8 sn)
{
    u8 t;
    t = 8;
    MakeFrameHeader();

    Net.Buf[t++] = 0x01;           //协议个数
    Net.Buf[t++] = 0x8A;           //协议编码
    Net.Buf[t++] = 0x07;           //编码长度低八位
    Net.Buf[t++] = 0x00;           //编码长度高八位

    ReadBiSuoFilterTime(&Sys.BiSuoTime, &Sys.FilterTime);
    Net.Buf[t++] = Sys.BiSuoTime;
    Net.Buf[t++] = Sys.BiSuoTime >> 8;

    Net.Buf[t++] = Sys.FilterTime;
    Net.Buf[t++] = Sys.FilterTime>> 8;

    // 先修改数据个数
    Net.Buf[6] = t + 2;      // 低八位
    Net.Buf[7] = 0;          // 高八位
    SetCrcAndAck(sn, t);
}

void AnswerSetBiaoJiao(u8 sn, u8 *buf)
{
    Sys.BiaoJiaoAddr = buf[12];
    Sys.BaiaoJiaoTime = buf[13];
    Sys.BiaoJiaoDuanDianAddr = buf[14];
    Sys.BiaoJiaoVal = 0;
    AnswerAck(sn, 0);
}

void AnswerReseetNet(u8 sn)
{
    AnswerAck(sn, 0);
    Sys_Soft_Reset();
}

// 设置IP地址  端口号   网关默认IP
void AnswerSetIp(u8 sn, u8 *buf)
{
    UpdateNetIpAddr(&buf[12]);
    UpdateGatewayIpAddr (&buf[18]);
    UpdateNetAddr(buf[22]);
    AnswerAck(sn, 0);
    Sys_Soft_Reset();
}

void AnswerPowerInfo(u8 sn, u8 addr)
{
    u8 i, j;
    u16 k, t;
    t = 8;
    k = 3;
    MakeFrameHeader();
    Net.Buf[t++] = 0x01;           //协议个数
    Net.Buf[t++] = 0x94;           //协议编码
    Net.Buf[t++] = 0x00;           //编码长度低八位
    Net.Buf[t++] = 0x00;           //编码长度高八位

    if(addr == 0)       // 所有的电源信息一起上传
    {
        for(i = 0; i < MaxPowerNum; i ++)
        {
            if(Power[i].Addr == 0)
                break;
            Net.Buf[t++] = Power[i].Addr;
            for(j = 0; j < 46; j ++)
                Net.Buf[t++] = Power[i].BatInfo[j];
            k += 47;
        }
    }
    else
    {
        i = CheckPowerIndex(addr);
        if(i != 0xFF)                 // 存在该电源地址
        {
            Net.Buf[t++] = Power[i].Addr;
            for(j = 0; j < 46; j ++)
                Net.Buf[t++] = Power[i].BatInfo[j];
            k += 47;
        }
    }

    Net.Buf[10] = k;           //编码长度低八位
    Net.Buf[11] = k >> 8;

    // 先修改数据个数
    Net.Buf[6] = t + 2;               // 低八位
    Net.Buf[7] = (t + 2) >> 8;        // 高八位
    SetCrcAndAck(sn, t);
}

void AnawerSetUdp(u8 sn, u8 *buf)
{
    u8 i;
    UpdateNetUdpIp(&buf[12]);
    for(i = 0; i < 4; i++)
        Sys.UdpDipr[i] = buf[12 + i];
    Sys.UdpPort = Net.Buf[17];
    Sys.UdpPort <<= 8;
    Sys.UdpPort += Net.Buf[16];
    AnswerAck(sn, 0);
}

void AnswerSetResetTimes(u8 sn)
{
    Sys.ResetTimes = 0;
    UpdateResetTimes(Sys.ResetTimes);
    AnswerAck(sn, 0);
}

void AnswerCleanRWptr(u8 sn)
{
    Sys.R = FramBegin;
    Sys.W = FramBegin;
    Sys.Shift = 0;        //偏移量归零，否则会造成下次取数时，读大于写
    UpdateReadPtr(Sys.R);
    UpdateWritePtr(Sys.W);
    AnswerAck(sn, 0);
}

// CAN数据通过网络上传
void CanInfoToNet(u8 sn, u8 cmd, u8 *buf, u8 len)
{
    u8 t, i;
    t = 8;
    MakeFrameHeader();
    Net.Buf[t++] = 0x01;           //协议个数
    Net.Buf[t++] = cmd;           //协议编码
    Net.Buf[t++] = len + 3;         //编码长度低八位
    Net.Buf[t++] = 0x00;           //编码长度高八位
    for(i = 0; i < len; i ++)
        Net.Buf[t++] = buf[i];
    // 先修改数据个数
    Net.Buf[6] = t + 2;      // 低八位
    Net.Buf[7] = 0;          // 高八位
    SetCrcAndAck(sn, t);
}

void AnswerCleanRecTimes(u8 sn)
{
    u8 i;
    for(i = 0; i < MaxDeviceNum; i ++)
        Device[i].RecTimes = 0;
    AnswerAck(sn, 0);
}

void AnswerExcuteList(u8 sn)
{
    u8 i;
    u16 k, t;
    t = 8;
    k = 3;
    MakeFrameHeader();
    Net.Buf[t++] = 0x01;
    Net.Buf[t++] = 0xEA; //协议编码
    Net.Buf[t++] = 0; //编码长度低八位  编码 + 长度2字节 + 网关信息8字节 + N*5
    Net.Buf[t++] = 0; //编码长度高八位, 最后更新
    for(i = 0; i < MaxTriggerNum; i++)
    {
        Net.Buf[t++] = Excute[i].TriggerAddr;
        k++;
        Net.Buf[t++] = Excute[i].ExcuteAddr;
        k++;
        Net.Buf[t++] = Excute[i].Trigger;
        k++;
    }

    Net.Buf[10] = k;           //???????
    Net.Buf[11] = k >> 8;

    Net.Buf[6] = t + 2; //整个数据帧长度字节
    Net.Buf[7] = (t + 2) >> 8;
    Net.Buf[t++] = Sys.Crc;
    Net.Buf[t++] = Sys.Crc >> 8;
    SetCrcAndAck(sn, t);
}

void HandleNetData(u8 sn, u8 *buf, u16 len, u8 flag)
{
    u16 t;
    if(CheckCrc16(buf, len - 2, 0) == 0)
        return;

    //............以下按照协议格式解析数据
    switch(buf[9])   //命令码
    {
    case 0x60:       //查询网关实时数据
        if(Sys.SyncLeft)
            Sys.SyncLeft --;
        if(Sys.InitLeft)
            Sys.InitLeft --;
        if(Sys.Delay)                     // 系统倒计时未结束，发送确认命令，表示未准备好数据
        {
			Sys.Delay--;
            AnswerAck(sn, 1);
            break;
        }
        if(buf[len - 3] == 0x00) //上帧接收正确,修改读指针
        {
            if(Sys.R != Sys.W)
            {
                if(Sys.R + Sys.Shift < FramEnd)
                    Sys.R += Sys.Shift;
                else
                    Sys.R = Sys.Shift + Sys.R - FramEnd + FramBegin;
                UpdateReadPtr(Sys.R);
                Sys.Flag &= ~0x01;
                Sys.Shift = 0;
            }
        }
        AnswerCurInfo(sn, flag);
        break;
    case 0x62:
        AnswerUnRegisterInfo(sn);
        break;//查询未注册信息
    case 0x64:
        AnswerCheckCrcCntInfo(sn);
        break;//查询网关初始化次数
    case 0x65:
        AnswerCleanCrcCntInfo(sn);
        break;//清零初始化次数
    case 0x66:
        AnswerCrcFail(sn);
        break;//查询初始化失败的设备
    case 0x67:
        AnswerResetCrcFail(sn);
        break;//复位某个初始化失败的设备
    case 0x68:
        AnswerCrcInfo(sn);
        break;//查询网关所有设备的crc
    case 0x6F:
        AnswerSetNetLianDong(sn, buf);
        break;//设置网关：标定传感器时是否联动
    case 0x70:
        AnswerIpAddr(sn);
        break;//查询IP地址，端口号
    case 0x71:
        AnswerBasePara(sn, buf);
        break;//查询网关基本参数，回复输入电压、网关地址、重启次数、时间
    case 0x72:
        AnswerSoftVerb(sn);
        break;//查询网关的嵌入式软件版本号
    case 0x80:
        AnswerSetIp(sn, buf);
        break;//设置网关IP地址，端口号，默认网关
    case 0x81:
        AnswerSetResetTimes(sn);
        break;//清零重启次数
    case 0x82:
        AnswerSetTime(sn, buf);
        break;//配置时间
    case 0x83:
        AnswerFactoryTime(sn, buf);
        break;//出厂时间配置
    case 0x84:
        AnswerCleanRWptr(sn);
        break;//清零网关的读写指针
    case 0x87:
        AnswerSetMac(sn, buf);
        break;//设置MAC地址
    case 0x88:
        AnswerCheckMac(sn);
        break;//查询MAC地址
    case 0x89:
        AnswerSetNetBiSuoFilterTime(sn, buf);
        break;//设置网关的失效时间
    case 0x8A:
        AnswerCheckNetBiSuoFilterTime(sn);
        break;//查询网关的失效时间
    case 0x8B:
        AnswerSetBiaoJiao(sn, buf);
        break;//设置自动标定数据,buf[12] 地址 buf[13] 时间
    case 0x8C:
        AnswerReseetNet(sn);
        break;//复位网关
    case 0x8D:
        AnawerSetUdp(sn, buf);
        break;//设置UDP
    case 0x8E:
        AnswerCleanRecTimes(sn);
        break;//清零所有设备的通讯次数
    case 0x8F:
        AnswerCleanNetCrc(sn);
        break;//清零网关Crc
    case 0x94:
        AnswerPowerInfo(sn, buf[12]);
        break;				//查询电源信息
    case 0xEA:
        AnswerExcuteList(sn);
        break;
    case 0xEE:
        AnswerForBootLoader(sn, buf);
        break;//进入bootloader,设置与上位机通讯的ＩＰ相关地址
    case 0x63:   //误码率测试
        t = buf[11];   //整个误码率长度
        t <<= 8;
        t += buf[10];
        t -= 3;
        AnswerErrorBitsInfo(sn, &buf[12], t);
        break;
    case 0x85:  //下发初始化信息，先应答，再存储，最后读取
        Sys.InitLeft = SystemInitTime;
        Sys.Delay = DelayAckTime;
        Sys.CrcCnt ++;
        UpdateNetCrcCnt(Sys.CrcCnt);
        t = buf[11];   //整个初始化信息的数据个数
        t <<= 8;
        t += buf[10];
        t -= 3;
		UpdateInitInfo(&buf[12],t);
//        ClrWdt();
//        FeedWDG();
        AnswerAck(sn, 0);
#ifndef FengDianNet
        ReadDefaultInit();
#endif
        break;
    case 0x86:   //设置网关新地址
        Sys.Addr = buf[12];
        UpdateNetAddr(Sys.Addr);
        AnswerAck(sn, 0);
        break;
    default:
        if((buf[10] >= 3) && (buf[10] <= 12))        // 数据长度判断，下发命令有误，不处理(暂时不考虑UDP的转发)
            NetInfoToCan(buf[9], &buf[12], buf[10] - 3);
        else
            AnswerWrongAck(sn);
        break;
    }
}
