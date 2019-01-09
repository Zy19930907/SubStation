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
    Net.Buf[t++] = 0x01;           //Э�����
    if(flag == 0)
    {
        Net.Buf[t++] = ACK;            //Э�����
        Net.Buf[t++] = 0x03;           //���볤�ȵͰ�λ
        Net.Buf[t++] = 0x00;           //���볤�ȸ߰�λ
    }
    else                                //����ʱ...S
    {
        Net.Buf[t++] = PREPARE;
        Net.Buf[t++] = 0x04;           //���볤�ȵͰ�λ
        Net.Buf[t++] = 0x00;           //���볤�ȸ߰�λ
        Net.Buf[t++] = Sys.Delay;
    }
    Net.Buf[6] = t + 2;            //�Ͱ�λ
    Net.Buf[7] = 0;                //�߰�λ
    SetCrcAndAck(sn, t);
}

void AnswerWrongAck(u8 sn)
{
    u8 t;
    t = 8;
    MakeFrameHeader();
    Net.Buf[t++] = 0x01;           //Э�����
    Net.Buf[t++] = WRONGACK;       //Э�����
    Net.Buf[t++] = 0x03;           //���볤�ȵͰ�λ
    Net.Buf[t++] = 0x00;           //���볤�ȸ߰�λ
    Net.Buf[6] = t + 2;            //�Ͱ�λ
    Net.Buf[7] = 0;                //�߰�λ
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
    if(Sys.R == Sys.W)       // ����б仯�����ݣ���Э������Ϊ2������Ϊ1
        Net.Buf[t++] = 0x01;
    else
        Net.Buf[t++] = 0x02;

    Net.Buf[t++] = 0x60;        //Э�����
    Net.Buf[t++] = 0;          //���볤�ȵͰ�λ  ���� + ����2�ֽ� + ������Ϣ8�ֽ� + N*5
    Net.Buf[t++] = 0;           //���볤�ȸ߰�λ, ������

    Net.Buf[t++] = Sys.R;        //��ָ��
    Net.Buf[t++] = Sys.R >> 8;
    Net.Buf[t++] = Sys.W;        //дָ��
    Net.Buf[t++] = Sys.W >> 8;

    TimeChange(&Net.Buf[t]);
    t += 4;
    j = 0;                      //�ж��豸��Ч�������Ӷ��õ�Э���ʵʱ���ݳ���
    for(i = 0; i < MaxDeviceNum; i ++)
    {
        if(Device[i].Flag & 0x01)    //�豸��Ч
        {
            Net.Buf[t++] = i + 1;                //��ַ���
            Net.Buf[t++] = Device[i].Name;       //����
            Net.Buf[t++] = Device[i].Status;    //״̬
            Net.Buf[t++] = Device[i].Buf[0];    //���ݵͰ�λ
            Net.Buf[t++] = Device[i].Buf[1];    //���ݸ߰�λ
            j ++;
        }
    }
    j = j * 5 + 11;
    Net.Buf[10] = j;    // Э������ݳ���
    Net.Buf[11] = j >> 8;

    if(Sys.R != Sys.W)
    {
        CalPtrLen();
        Net.Buf[t++] = 0x61;                    //CMD
        k = t;                         //��ʱ��¼��ǰ����
        Net.Buf[t++] = 0;           //�����Ժ����
        Net.Buf[t++] = 0;
        j = Sys.R;                      // ��ʱ��ָ��
        l = 0;
        for(i = 0; i < Sys.Shift;)
        {
            if(ReadRecord(j, &Net.Buf[t]))
            {
                t += 10;
                l += 10;            //���ȼ�¼
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

    Net.Buf[6] = t + 2;    //��������֡�����ֽ�
    Net.Buf[7] = (t + 2) >> 8;
    SetCrcAndAck(sn, t);
}

void AnswerErrorBitsInfo(u8 sn, u8 *buf, u16 len)
{
    u16 i, t;
    MakeFrameHeader();
    t = 8;
    Net.Buf[t++] = 0x01;
    Net.Buf[t++] = 0x63;        //Э�����
    Net.Buf[t++] = 0;          //���볤�ȵͰ�λ  ���� + ����2�ֽ� + N*5
    Net.Buf[t++] = 0;           //���볤�ȸ߰�λ, ������

    for(i = 0; i < len; i ++)
        Net.Buf[t++] = buf[i];

    Net.Buf[10] = len + 3;  // Э������ݳ���
    Net.Buf[11] = (len + 3) >> 8;

    Net.Buf[6] = t + 2;    //��������֡�����ֽ�
    Net.Buf[7] = (t + 2) >> 8;
    SetCrcAndAck(sn, t);
}

void AnswerUnRegisterInfo(u8 sn)
{
    u16 i, j, t;
    MakeFrameHeader();
    t = 8;
    Net.Buf[t++] = 0x01;
    Net.Buf[t++] = 0x62;        //Э�����
    Net.Buf[t++] = 0;          //���볤�ȵͰ�λ  ���� + ����2�ֽ� + N*5
    Net.Buf[t++] = 0;           //���볤�ȸ߰�λ, ������
    j = 0;                      //�ж��豸��Ч�������Ӷ��õ�Э���ʵʱ���ݳ���
    for(i = 0; i < MaxDeviceNum; i ++)
    {
        if(Device[i].Flag & 0x02)    //�豸δע��
        {
            Net.Buf[t++] = i + 1;                //��ַ���
            Net.Buf[t++] = Device[i].Name;       //����
            j ++;
        }
    }
    j = j * 2 + 3;
    Net.Buf[10] = j;    // Э������ݳ���
    Net.Buf[11] = j >> 8;

    Net.Buf[6] = t + 2;    //��������֡�����ֽ�
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
    for(i = 0; i < MaxDeviceNum; i ++)       // ��ʼ��Ĭ�ϣ������е��豸����Ϊδ֪����
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
    Net.Buf[t++] = 0x68;        //Э�����
    Net.Buf[t++] = 0;          //���볤�ȵͰ�λ  ���� + ����2�ֽ� + N*5
    Net.Buf[t++] = 0;           //���볤�ȸ߰�λ, ������
    j = 0;                      //�ж��豸��Ч�������Ӷ��õ�Э���ʵʱ���ݳ���
    for(i = 0; i < MaxDeviceNum; i ++)
    {
        if(Device[i].Flag & 0x01)    //�豸��Ч
        {
            Net.Buf[t++] = i + 1;                //��ַ���
            Net.Buf[t++] = Device[i].Name;       //crc
            Net.Buf[t++] = Device[i].Crc;       //crc
            Net.Buf[t++] = Device[i].RecTimes;  //���մ���
            Net.Buf[t++] = Device[i].Warn;      // ������������
            j ++;
        }
    }
    j = j * 5 + 3;
    Net.Buf[10] = j;    // Э������ݳ���
    Net.Buf[11] = j >> 8;

    Net.Buf[6] = t + 2;    //��������֡�����ֽ�
    Net.Buf[7] = (t + 2) >> 8;

    SetCrcAndAck(sn, t);
}

// �ظ������ѹ�����ص�ַ��������������ǰʱ��,��Ʒ����ʱ��
void AnswerBasePara(u8 sn, u8 *buf)
{
    u8 t;
    t = 8;
    MakeFrameHeader();

    Net.Buf[t++] = 0x01;           //Э�����
    Net.Buf[t++] = 0x71;           //Э�����
    Net.Buf[t++] = 0x0E;           //���볤�ȵͰ�λ
    Net.Buf[t++] = 0x00;           //���볤�ȸ߰�λ

    Net.Buf[t++] = Sys.Vol;
    Net.Buf[t++] = Sys.Addr;
    Net.Buf[t++] = Sys.ResetTimes;

    TimeChange(&Net.Buf[t]);
    t += 4;

    ReadFactoryTime(&Net.Buf[t]);
    t += 4;

    Net.Buf[t++] = Sys.Flag;           // ��Ҫ��D2:�궨�Ƿ�ϵ�

    // ���޸����ݸ���
    Net.Buf[6] = t + 2;      // �Ͱ�λ
    Net.Buf[7] = 0;          // �߰�λ
    SetCrcAndAck(sn, t);
}

// �ظ�����汾��
void AnswerSoftVerb(u8 sn)
{
    u8 t;
    t = 8;
    MakeFrameHeader();

    Net.Buf[t++] = 0x01;           //Э�����
    Net.Buf[t++] = 0x72;           //Э�����
    Net.Buf[t++] = 0x04;           //���볤�ȵͰ�λ
    Net.Buf[t++] = 0x00;           //���볤�ȸ߰�λ

    Net.Buf[t++] = SoftVerb;
    // ���޸����ݸ���
    Net.Buf[6] = t + 2;      // �Ͱ�λ
    Net.Buf[7] = 0;          // �߰�λ
    SetCrcAndAck(sn, t);
}

// �ظ�IP��ַ�Ͷ˿ں�
void AnswerIpAddr(u8 sn)
{
    u8 t;
    t = 8;
    MakeFrameHeader();
    Net.Buf[t++] = 0x01;           //Э�����
    Net.Buf[t++] = 0x70;           //Э�����
    Net.Buf[t++] = 0x10;           //���볤�ȵͰ�λ
    Net.Buf[t++] = 0x00;           //���볤�ȸ߰�λ

    ReadNetIpAddr(&Net.Buf[t]);
    t += 6;
    ReadGateWayIp(&Net.Buf[t]);
    t += 4;
    ReadUdpIp(&Net.Buf[t]);
    t += 6;

    // ���޸����ݸ���
    Net.Buf[6] = t + 2;      // �Ͱ�λ
    Net.Buf[7] = 0;          // �߰�λ
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
    Net.Buf[t++] = 0x66;        //Э�����
    Net.Buf[t++] = 0;          //���볤�ȵͰ�λ  ���� + ����2�ֽ� + N*5
    Net.Buf[t++] = 0;           //���볤�ȸ߰�λ, ������
    j = 0;                      //�ж��豸��Ч�������Ӷ��õ�Э���ʵʱ���ݳ���
    for(i = 0; i < MaxDeviceNum; i ++)
    {
        if(Device[i].CrcCnt > 5)    //�豸��Ч
        {
            Net.Buf[t++] = i + 1;                //��ַ���
            Net.Buf[t++] = Device[i].Name;       //����
            j ++;
        }
    }
    j = j * 2 + 3;
    Net.Buf[10] = j;    // Э������ݳ���
    Net.Buf[11] = j >> 8;

    Net.Buf[6] = t + 2;    //��������֡�����ֽ�
    Net.Buf[7] = (t + 2) >> 8;

    SetCrcAndAck(sn, t);
}

void AnswerCheckCrcCntInfo(u8 sn)
{
    u8 t;
    t = 8;
    MakeFrameHeader();
    Net.Buf[t++] = 0x01;           //Э�����
    Net.Buf[t++] = 0x64;            //Э�����
    Net.Buf[t++] = 0x04;           //���볤�ȵͰ�λ
    Net.Buf[t++] = 0x00;           //���볤�ȸ߰�λ
    Net.Buf[t++] = Sys.CrcCnt;
    Net.Buf[6] = t + 2;            //�Ͱ�λ
    Net.Buf[7] = 0;                //�߰�λ
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
    SD2068.Buf[0] = buf[12];           // ��
    SD2068.Buf[1] = buf[13] & 0x1f;      //�º��ܼ��ϲ���;
    SD2068.Buf[2] = buf[14];          //��
    SD2068.Buf[3] = buf[13] >> 5;     //date
    SD2068.Buf[4] = buf[15] & 0x3f;   //Сʱ 6
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
    time = Net.Buf[12];    //��
    time <<= 4;
    time += Net.Buf[13];   //��
    time <<= 5;
    time += Net.Buf[14];   //��
    time <<= 5;
    time += Net.Buf[15];   //Сʱ
    time <<= 6;
    time += Net.Buf[16];   //��
    time <<= 6;
    time += Net.Buf[17];   //��
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

    Net.Buf[t++] = 0x01;           //Э�����
    Net.Buf[t++] = 0x88;           //Э�����
    Net.Buf[t++] = 0x09;           //���볤�ȵͰ�λ
    Net.Buf[t++] = 0x00;           //���볤�ȸ߰�λ

    for(i = 0; i < 6; i ++)
        Net.Buf[t++] = Net.MacAddr[i];

    // ���޸����ݸ���
    Net.Buf[6] = t + 2;      // �Ͱ�λ
    Net.Buf[7] = 0;          // �߰�λ
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

    Net.Buf[t++] = 0x01;           //Э�����
    Net.Buf[t++] = 0x8A;           //Э�����
    Net.Buf[t++] = 0x07;           //���볤�ȵͰ�λ
    Net.Buf[t++] = 0x00;           //���볤�ȸ߰�λ

    ReadBiSuoFilterTime(&Sys.BiSuoTime, &Sys.FilterTime);
    Net.Buf[t++] = Sys.BiSuoTime;
    Net.Buf[t++] = Sys.BiSuoTime >> 8;

    Net.Buf[t++] = Sys.FilterTime;
    Net.Buf[t++] = Sys.FilterTime>> 8;

    // ���޸����ݸ���
    Net.Buf[6] = t + 2;      // �Ͱ�λ
    Net.Buf[7] = 0;          // �߰�λ
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

// ����IP��ַ  �˿ں�   ����Ĭ��IP
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
    Net.Buf[t++] = 0x01;           //Э�����
    Net.Buf[t++] = 0x94;           //Э�����
    Net.Buf[t++] = 0x00;           //���볤�ȵͰ�λ
    Net.Buf[t++] = 0x00;           //���볤�ȸ߰�λ

    if(addr == 0)       // ���еĵ�Դ��Ϣһ���ϴ�
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
        if(i != 0xFF)                 // ���ڸõ�Դ��ַ
        {
            Net.Buf[t++] = Power[i].Addr;
            for(j = 0; j < 46; j ++)
                Net.Buf[t++] = Power[i].BatInfo[j];
            k += 47;
        }
    }

    Net.Buf[10] = k;           //���볤�ȵͰ�λ
    Net.Buf[11] = k >> 8;

    // ���޸����ݸ���
    Net.Buf[6] = t + 2;               // �Ͱ�λ
    Net.Buf[7] = (t + 2) >> 8;        // �߰�λ
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
    Sys.Shift = 0;        //ƫ�������㣬���������´�ȡ��ʱ��������д
    UpdateReadPtr(Sys.R);
    UpdateWritePtr(Sys.W);
    AnswerAck(sn, 0);
}

// CAN����ͨ�������ϴ�
void CanInfoToNet(u8 sn, u8 cmd, u8 *buf, u8 len)
{
    u8 t, i;
    t = 8;
    MakeFrameHeader();
    Net.Buf[t++] = 0x01;           //Э�����
    Net.Buf[t++] = cmd;           //Э�����
    Net.Buf[t++] = len + 3;         //���볤�ȵͰ�λ
    Net.Buf[t++] = 0x00;           //���볤�ȸ߰�λ
    for(i = 0; i < len; i ++)
        Net.Buf[t++] = buf[i];
    // ���޸����ݸ���
    Net.Buf[6] = t + 2;      // �Ͱ�λ
    Net.Buf[7] = 0;          // �߰�λ
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
    Net.Buf[t++] = 0xEA; //Э�����
    Net.Buf[t++] = 0; //���볤�ȵͰ�λ  ���� + ����2�ֽ� + ������Ϣ8�ֽ� + N*5
    Net.Buf[t++] = 0; //���볤�ȸ߰�λ, ������
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

    Net.Buf[6] = t + 2; //��������֡�����ֽ�
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

    //............���°���Э���ʽ��������
    switch(buf[9])   //������
    {
    case 0x60:       //��ѯ����ʵʱ����
        if(Sys.SyncLeft)
            Sys.SyncLeft --;
        if(Sys.InitLeft)
            Sys.InitLeft --;
        if(Sys.Delay)                     // ϵͳ����ʱδ����������ȷ�������ʾδ׼��������
        {
			Sys.Delay--;
            AnswerAck(sn, 1);
            break;
        }
        if(buf[len - 3] == 0x00) //��֡������ȷ,�޸Ķ�ָ��
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
        break;//��ѯδע����Ϣ
    case 0x64:
        AnswerCheckCrcCntInfo(sn);
        break;//��ѯ���س�ʼ������
    case 0x65:
        AnswerCleanCrcCntInfo(sn);
        break;//�����ʼ������
    case 0x66:
        AnswerCrcFail(sn);
        break;//��ѯ��ʼ��ʧ�ܵ��豸
    case 0x67:
        AnswerResetCrcFail(sn);
        break;//��λĳ����ʼ��ʧ�ܵ��豸
    case 0x68:
        AnswerCrcInfo(sn);
        break;//��ѯ���������豸��crc
    case 0x6F:
        AnswerSetNetLianDong(sn, buf);
        break;//�������أ��궨������ʱ�Ƿ�����
    case 0x70:
        AnswerIpAddr(sn);
        break;//��ѯIP��ַ���˿ں�
    case 0x71:
        AnswerBasePara(sn, buf);
        break;//��ѯ���ػ����������ظ������ѹ�����ص�ַ������������ʱ��
    case 0x72:
        AnswerSoftVerb(sn);
        break;//��ѯ���ص�Ƕ��ʽ����汾��
    case 0x80:
        AnswerSetIp(sn, buf);
        break;//��������IP��ַ���˿ںţ�Ĭ������
    case 0x81:
        AnswerSetResetTimes(sn);
        break;//������������
    case 0x82:
        AnswerSetTime(sn, buf);
        break;//����ʱ��
    case 0x83:
        AnswerFactoryTime(sn, buf);
        break;//����ʱ������
    case 0x84:
        AnswerCleanRWptr(sn);
        break;//�������صĶ�дָ��
    case 0x87:
        AnswerSetMac(sn, buf);
        break;//����MAC��ַ
    case 0x88:
        AnswerCheckMac(sn);
        break;//��ѯMAC��ַ
    case 0x89:
        AnswerSetNetBiSuoFilterTime(sn, buf);
        break;//�������ص�ʧЧʱ��
    case 0x8A:
        AnswerCheckNetBiSuoFilterTime(sn);
        break;//��ѯ���ص�ʧЧʱ��
    case 0x8B:
        AnswerSetBiaoJiao(sn, buf);
        break;//�����Զ��궨����,buf[12] ��ַ buf[13] ʱ��
    case 0x8C:
        AnswerReseetNet(sn);
        break;//��λ����
    case 0x8D:
        AnawerSetUdp(sn, buf);
        break;//����UDP
    case 0x8E:
        AnswerCleanRecTimes(sn);
        break;//���������豸��ͨѶ����
    case 0x8F:
        AnswerCleanNetCrc(sn);
        break;//��������Crc
    case 0x94:
        AnswerPowerInfo(sn, buf[12]);
        break;				//��ѯ��Դ��Ϣ
    case 0xEA:
        AnswerExcuteList(sn);
        break;
    case 0xEE:
        AnswerForBootLoader(sn, buf);
        break;//����bootloader,��������λ��ͨѶ�ģɣ���ص�ַ
    case 0x63:   //�����ʲ���
        t = buf[11];   //���������ʳ���
        t <<= 8;
        t += buf[10];
        t -= 3;
        AnswerErrorBitsInfo(sn, &buf[12], t);
        break;
    case 0x85:  //�·���ʼ����Ϣ����Ӧ���ٴ洢������ȡ
        Sys.InitLeft = SystemInitTime;
        Sys.Delay = DelayAckTime;
        Sys.CrcCnt ++;
        UpdateNetCrcCnt(Sys.CrcCnt);
        t = buf[11];   //������ʼ����Ϣ�����ݸ���
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
    case 0x86:   //���������µ�ַ
        Sys.Addr = buf[12];
        UpdateNetAddr(Sys.Addr);
        AnswerAck(sn, 0);
        break;
    default:
        if((buf[10] >= 3) && (buf[10] <= 12))        // ���ݳ����жϣ��·��������󣬲�����(��ʱ������UDP��ת��)
            NetInfoToCan(buf[9], &buf[12], buf[10] - 3);
        else
            AnswerWrongAck(sn);
        break;
    }
}
