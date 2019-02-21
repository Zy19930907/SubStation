#include "CanPro.h"
#include "Config.h"
#include "CrcMaker.h"

u32 TASKID_CANPRO = 0xFFFFFFFF;

_AckAddr Ack;
_SendInit Init;
_LianDong LianDong;

//��ʼ��6·CAN
void InitCan(void)
{
    u8 i;
    for(i = 0; i < 6; i++)
    {
        CanxInit(i);
        Can.Tick[i] = SYS_TICK;
    }
}

//���CANоƬ�Ƿ�����
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
		SetTaskStatus(TASKID_CANPRO,"CAN��ʼ��");
        InitCan();
        Can.Status = CANRECV;
        break;
    case CANRECV:
		SetTaskStatus(TASKID_CANPRO,"CAN���ݽ���");
        for(i = 0; i < 6; i++)
			CanRecvData(i);
        Can.Status = CANCHECKDEV;
        break;
    case CANCHECKDEV://��ʱ��δ�յ��������³�ʼ��CAN����
		SetTaskStatus(TASKID_CANPRO,"CANӲ�����");
        CheckCanDev();
        Can.Status = CANRECV;
        break;
    }
}

// CAN ���߹���������1,2,3,4,5   ���У�5����û�и��豸��Ϣ����㲥����
u8 CanBusBelong(u8 addr)
{
    u8 n;
    if(!(Device[addr - 1].Flag & 0x03)) // ����û���յ����豸����Ϣ
        return 0;
    n = (Device[addr - 1].Status & 0x60);
    n >>= 5;
    return (n);
}

// �鿴�Ƿ���Ҫ������
void CheckTriggerStatus(void)
{
    u8 i, warn;
    warn = 0;
    for(i = 0; i < MaxTriggerNum; i ++)
    {
        warn |=  Excute[i].Warn;
        if(Excute[i].ExcuteAddr != Excute[i + 1].ExcuteAddr)
        {
            if((Device[Excute[i].ExcuteAddr - 1].Warn & 0x7F) != warn)      // �б仯
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
		if((Device[i].Warn & 0x80) && (!(Device[i].Status & 0x80)))        // ��Ҫ����
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

// �п���һ��������������������豸����[3,5][4,5]
void CheckLianDongInfo(u8 addr, u8 trigger, u8 val)
{
    u8 i, flag, j;
    flag = 0;
    if(!(Device[addr - 1].Flag & 0x10))        // û�й��������豸��ֱ���˳�
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
            if((Device[j].Name == DuanDianQi) && (Device[j].Status & 0x0C))      // �ϵ������ֿػ����н���
                continue;
            if(!(Device[j].Status & 0x80))     // ֻ��ͨѶ�������豸�Ŵ���
            {
                if(val)         // ��Ҫ����������֮ǰЭ���Ѿ����ˣ��ϵ���D0���������͹㲥D1
                {
                    if(Device[j].Name == DuanDianQi)
                        Device[j].Status |= 0x01;       // �ϵ�
                    else
                        Device[j].Status |= 0x02;
                }
            }
            if(val != Excute[i].Warn)
            {
                LianDong.Retry = 0;
                flag = 1;
                if(val == 0)       // ȡ�����
                    Excute[i].Warn &= ~trigger;
                else              // ���ӱ��
                    Excute[i].Warn |= trigger;
            }
        }
    }
    if(flag == 1)
        CheckTriggerStatus();
}

// ��ѯ��Դ���ڵĴ洢���
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
// �����жϸõ�ַ�Ƿ�������豸�б��У�������ڣ������ж��Ƿ����
u8 FilterPowerAddr(u8 addr)
{
    u8 j;
    if(!(Device[addr - 1].Flag & 0x01))
        return 0xFF;
    for(j = 0; j < MaxPowerNum; j ++)   //�ٲ��Ҹõ�ַ�Ƿ��Ѿ��ڵ�Դ�б���
    {
        if(addr == Power[j].Addr)
            return j;
    }
    // �õ�ַ�����ڣ�����ӵ���������(��ַ�б�Ϊ0�������)
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
    switch(CanIndex+1)                          //CAN��������
    {
    case 1:
        status = 0x00;
        break;               //�� 1...4��CAN����
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
    ID = Fram;         // ֡����
    ID <<= 7;
    ID += Cmd;           // ����  7bit
    ID <<= 17;
    ID += DestAddr;    // Ŀ���ַ  8bit
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
 �ܹ�8֡���ݣ�֡����0----7  ǰ��7֡����7���ֽڣ����һ֡5���ֽ�
 * ÿһ֡�ĵ�һ���ֽ���֡����+��ֱ��״̬
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
// ͬ��ʱ��
void SyncTime(u8 CanIndex, u8 addr)
{
    u8 buf[6];
    Can.ID = (u32)MakeFramID(0, 0x23, addr);
    buf[0] = SD2068.Hex[0];       // ��
    buf[1] = (SD2068.Hex[3] << 5) + SD2068.Hex[1];      // �º�date�ϲ�
    buf[2] = SD2068.Hex[2];
    buf[3] = SD2068.Hex[4];
    buf[4] = SD2068.Hex[5];
    buf[5] = SD2068.Hex[6];      // ��
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
          // D27 D26 ֡����  01 ��֡ 10 �м�֡  11 ĩβ֡   D25 D24  00...11ʵ��֡����
          // ��Ӧ frame D3 D2֡����  D1 D0֡����
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
          if(MsTickDiff(Init.Tick) > 1500)       //1.5s��ʱ,�˳�
               Init.Status = INITIDLE;  
          break;
     default:
          Init.Status = INITIDLE;
          break;
     }
}


//  �򵥱Ƚ�ʱ�䣺�ȽϷ��Ӻ������ݼ���,�ڣ������֮��
//  D11.....D6 �֡�����������D5.....D0 ��
//  ��Ҫͬ��ʱ�䣬����1������Ҫͬ��ʱ�䣬����0
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
    t = (min * 60) + sec;   // �ж�ʱ�����5s
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

// �洢11���ֽڴ���һ����¼
// ��š����ơ�  ״̬������1������2��������ʱ����4�ֽ�   SUM
// ����t�����ַ��ţ�flag����洢����
void WriteSensorRecord(u8 addr, u8 flag)
{
    u8 buf[11], D5Bit;
    if(Sys.Flag & 0x02)    // ���ͨѶ�жϣ���D5��1
        D5Bit = 0x20;
    else
        D5Bit = 0;
    switch(flag)
    {
    case DEVICERESET:
        buf[0] = addr;   //��ַ
        buf[1] = Device[addr - 1].Name;  //����
        buf[2] = 0x40 + D5Bit;    //�����Ǹ�λ��¼
        buf[3] = Can.Buf[2];
        buf[4] = 0;
        buf[5] = 0;

        buf[6] = Can.Buf[3];      //ʱ�����
        buf[7] = Can.Buf[4];
        buf[8] = Can.Buf[5];
        buf[9] = Can.Buf[6];
        break;
    case SENSORVARY:
    case SENSORAUTOSEND:
        buf[0] = addr;    //��ַ
        buf[1] = Device[addr - 1].Name; //����
        buf[2] = (Device[addr - 1].Status & 0x0F) + D5Bit; //״̬����--ͨѶ�Ƿ��ж�
        buf[3] = Can.Buf[0];//Device[addr-1].Buf[0];  //����
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
        buf[0] = addr;    //��ַ
        buf[1] = Device[addr - 1].Name; //����
        buf[2] = (Can.Buf[1] >> 4) + D5Bit;  //״̬����--С����ͼ���Ԥ�ȵ�
        buf[3] = Can.Buf[0];
        buf[4] = (Can.Buf[1] & 0x0F);
        buf[5] = Can.Buf[2];    //RSSI

        buf[6] = Can.Buf[3];
        buf[7] = Can.Buf[4];
        buf[8] = Can.Buf[5];
        buf[9] = Can.Buf[6];
        break;
    case NEWCARDVARY:
        buf[0] = addr;    //��ַ
        buf[1] = Device[addr - 1].Name; //����
        buf[2] = Can.Buf[3] + D5Bit;  //״̬����--С����ͼ���Ԥ�ȵ�
        buf[3] = Can.Buf[0];
        buf[4] = (Can.Buf[1] & 0x3F);
        buf[5] = Can.Buf[2];    //RSSI

        buf[6] = Can.Buf[4];
        buf[7] = Can.Buf[5];
        buf[8] = Can.Buf[6];
        buf[9] = Can.Buf[7];
        break;
    case NETRESET:           //��������λ
        buf[0] = Sys.Addr;       //��ַ
        buf[1] = NetAttri;      //����
        buf[2] = 0x40 + D5Bit;          //״̬
        buf[3] = Sys.ResetTimes;  //����
        buf[4] = 0;
        buf[5] = 0;
        TimeChange(&buf[6]);   //buf[6,7,8,9]װ��ʱ��
        break;  /*
     case LianDongRecord:
          buf[0] = addr;    //��ַ
          buf[1] = Device[addr-1].Name;   //����
          buf[2] = 0x10 + D5Bit;         //������������¼
          for(i = 0;i < 7;i ++)
               buf[3+i] = Can.Buf[i];
          break;       */
    }
	WriteRecord(Sys.W,buf,10);
}

// ���ݲ�����CAN���  ����  Ŀ���ַ
// ����Ƿ��������أ����Ӧ�㲥��Ϣ�����Ҵ��϶ϵ����״̬
void AckFunc(u8 index, u8 cmd, u8 DestAddr)
{
    u8 buf[8];
#ifdef FengDianNet
    Can.ID = (u32) MakeFramID(0, cmd, 0);
    buf[0] = FengDian.DuanDianStatus;
    CanSendData(index, Can.ID, &buf[0], 1);
#else
    Can.ID = (u32) MakeFramID(0, cmd, DestAddr);
//  if(Device[DestAddr - 1].Flag & 0x04)            // ������߿���
	if(Device[DestAddr - 1].Warn)
        Can.ID |= 0x8000;
    buf[0] = Device[DestAddr - 1].Warn & 0x7F;
    if(Sys.Delay)                                  // ����ʱδ��������������ָ��
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

// ���Ȳ鿴�豸������crc�Ƿ�һ�£������һ�£���������������һ�£�����������Ӧ����
// flag: 0 ��ʾ������������(����Ϣ��)��1���������豸
void CheckCrc(u8 addr, u8 crc, u8 flag)
{
    u16 tick;
    if(Sys.InitLeft)                //�ñ���Ϊ0����������ͬ���豸
        return;
    if(Init.Status != INITIDLE)     //��������·���ʼ��
        return;
    if(Device[addr - 1].CrcCnt > 5)        //���ô�������5�Σ�֤����ʼ�������쳣��������
    {
//      Device[addr-1].Status |= 0x10;
        return;
    }
    if(crc == Device[addr - 1].Crc) //�豸���ù�����������������Ƿ�һ��
    {
        Device[addr - 1].CrcCnt = 0;
        Device[addr - 1].Status &= ~0x10;
        Device[addr - 1].Flag &= ~0x20;
        return;
    }
    Device[addr - 1].Flag |= 0x20;   // ����������
    tick = SYS_TICK;
    while(MsTickDiff(tick) < 20);
    Device[addr - 1].CrcCnt ++;
    if(Device[addr - 1].Crc == 0)    //��λ��û�����ù�,���·��������ָ��
    {
        Can.ID = (u32) MakeFramID(0, 0x52, addr);
        CanSendData(CanBusBelong(addr), Can.ID, &Can.Buf[0], 0);
        return;
    }
    else      //��λ���г�ʼ����Ϣ�������������
    {
        Init.R = 0;
        Init.W = 0;
        if(!flag)     // ������������ֻ�����Ƿ������ƺͶ��ߴ���
            Init.Buf[Init.W++] = Device[addr - 1].Crc;
        else          // ֻ�жϵ������㲥��������������������Ҫȥ��ȡ�����Ϣ
            ReadInitInfo(addr);
    }
    Init.DestAddr = addr;
}

//�����ϴ���D15������  D14 D13С����  D12 D11 ״̬
void MakeSensorCanData(u32 ID, u8 CanIndex, u8 addr)
{
    u8 status;
    status = 0;
    status += ((Can.Buf[1] >> 3) & 0x03);     //״̬
    status <<= 2;
    status += ((Can.Buf[1] >> 5) & 0x03);     //С����
    status &= 0x0F;
    status += MakeCanBusStatus(CanIndex);

    Device[addr - 1].Status = status;           //״̬λ

    if(ID & 0x00010000)                         // D16: �ϵ���ԣ��Լ�����
        Device[addr - 1].Status |= 0x10;

    if((Sys.BaiaoJiaoTime) && (Sys.BiaoJiaoAddr == addr))   //������Ϊ�궨����
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
    else     // ���ݷ����仯����ֵ
    {
        if((Device[addr - 1].Buf[0] !=  Can.Buf[0]) || (Device[addr - 1].Buf[1] != (Can.Buf[1] & 0x07)))
        {
            Device[addr - 1].Buf[0] =  Can.Buf[0];
            if((Can.Buf[0] == 0xFF) && ((Can.Buf[1] & 0x07) == 0x07))
                Device[addr - 1].Buf[1] = 0xFF;
            else
                Device[addr - 1].Buf[1] = (Can.Buf[1] & 0x07);
            if(Sys.Flag & 0x02)                  // ����λ��ͨѶ�жϣ���洢����
                WriteSensorRecord(addr, SENSORAUTOSEND);
        }
    }
}

// ������ CH4 O2 CO WD
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
    if((cmd != 0x0B) && (cmd != 0x0C))                  // 0B 0C Ϊ������豸�ϴ���Ϣ,����÷��ɭ��
    {
        Device[addr - 1].Name = ((ID >> 8) & 0x3F);    //�豸����
        Device[addr - 1].Tick = SYS_TICK;
        Device[addr - 1].Status &= ~0x80;
        CheckLianDongInfo(addr, 0x10, 0);
        Device[addr - 1].RecTimes ++;
    }
    if(!(Device[addr - 1].Flag & 0x01))  // �豸��Ч�����������ϴ�����
    {
        Device[addr - 1].Flag |= 0x02;  // �豸û��ע����λ
        CanSendData(CanIndex, MakeFramID(0, UnRegister, addr), &Can.Buf[0], 0);
        Device[addr - 1].Status = MakeCanBusStatus(CanIndex);
        if(cmd < 0x10)                 // �����ϴ����ݣ����洢������ǲ�ѯ����ȣ���ҪӦ��
            return;
    }
    if(Ack.NeedToNet == 1)       //Ӧ��ʱ�ж�
    {
        if(MsTickDiff(Ack.Tick) > 1000)    // 1S ��ʱ
            Ack.NeedToNet = 0;
    }
#ifdef FengDianNet
    if(addr < 6)   //ֻ�ж� A1 A2 A3 D4 D5״̬  0 ���� ��Ϊ����
        FengDian.ComNormal &= ~(1 << (addr - 1));
#endif
    switch(cmd)        //������������ת��ͬ��Э��
    {
    case 0x01:        //�����������ϴ���ǰ״̬��������2�ֽڣ� ʱ����4�ֽڣ� У�飨1�ֽڣ�
        if(ID & 0x01000000)                       // �����ϵ�,��ʱֻ���Ƕϵ�����
            CheckLianDongInfo(addr, 0x01, 0x01);
        else
            CheckLianDongInfo(addr, 0x01, 0x00);
        if(CheckTime(&Can.Buf[2]) == 1)
        {
            SyncTime(CanIndex, addr);
            //return;                   // ʱ����Ϣ���ԣ�����������֡
        }
        else
			AckFunc(CanIndex, ACK, addr);
        MakeSensorCanData(ID, CanIndex, addr);  //���߱��
        CheckCrc(addr, Can.Buf[6], 1);
        break;
    case 0x02:
        AckFunc(CanIndex, ACK, addr);
        if((Can.Buf[0] == 0xFF) && (Can.Buf[1] == 0xFF))  //��λ��Ϣ
        {
            WriteSensorRecord(addr, DEVICERESET);
            break;
        }
        WriteSensorRecord(addr, SENSORVARY);
        break;
    case 0x03:    //������/Ƶ��������ת�����ϴ���3�ֽ�ģʽ��2�ֽ�����+1�ֽ�crc
        status += MakeCanBusStatus(CanIndex);
        Device[addr - 1].Status &= 0x0E; // ��� D6 D5 D0
        Device[addr - 1].Status += status;
        if(Device[addr - 1].Name == 0x21) //Ƶ��ת����,�ϴ�Ƶ��
        {
            Device[addr - 1].Buf[0] = Can.Buf[0];
            Device[addr - 1].Buf[1] = Can.Buf[1];
            AckFunc(CanIndex, ACK, addr);
        }
        else // ���������������ϴ�0����1  2���ֽڴ��䣬0000��ʾ0  0001��ʾ1
        {
            if(ID & 0x01000000)      // �����˱仯�����й����豸����
            {
                if(Device[addr - 1].Crc & 0x08)           // 1̬����
                {
                    if(Can.Buf[1] & 0x01)
                        CheckLianDongInfo(addr, 0x40, 0x40);
                }
                if(Device[addr - 1].Crc & 0x04)           // 0̬����
                {
                    if(!(Can.Buf[1] & 0x01))
                        CheckLianDongInfo(addr, 0x20, 0x20);
                }
            }
            else
            {
                if(Device[addr - 1].Crc & 0x08)          // 1̬����
                    CheckLianDongInfo(addr, 0x40, 0);
                if(Device[addr - 1].Crc & 0x04)           // 0̬����
                    CheckLianDongInfo(addr, 0x20, 0);
            }
            Device[addr - 1].Status += (Can.Buf[1] & 0x01); //״̬λ+��ͣ
            Device[addr - 1].Buf[0] = 0;
            Device[addr - 1].Buf[1] = 0;
            AckFunc(CanIndex, ACK, addr);
            CheckCrc(addr, Can.Buf[2], 0);
        }
        break;
    case 0x04:                 //��������ʱ�Զ��ϴ���Ϣ
        if(CheckTime(&Can.Buf[0]) == 1)
        {
            SyncTime(CanIndex, addr);
            return;
        }
        else
            AckFunc(CanIndex, ACK, addr);
        Device[addr - 1].Status = MakeCanBusStatus(CanIndex);      //״̬λ
        if(Device[addr - 1].Warn)
            Device[addr - 1].Status |= 0x02;
        Device[addr - 1].Buf[0] = 0;
        Device[addr - 1].Buf[1] = 0;
#ifndef FengDianNet
        CheckCrc(addr, Can.Buf[4], 1);
#endif
        break;
    case 0x05:                //�������ϴ���Ƭ���ݣ� ��Ƭ��ŵ�  ��  RSSI  ʱ����
        AckFunc(CanIndex, ACK, addr);
        if((Can.Buf[0] == 0xFF) && (Can.Buf[1] == 0xFF))  //��λ��Ϣ
        {
            WriteSensorRecord(addr, DEVICERESET);
            Device[addr - 1].Buf[0] = 0x00;      //״̬λ
            break;
        }
        Device[addr - 1].Status &= 0x0F;
        Device[addr - 1].Status += MakeCanBusStatus(CanIndex);      //״̬λ
        if(Can.Len == 7)
            WriteSensorRecord(addr, CARDVARY);
        if(Can.Len == 8)
            WriteSensorRecord(addr, NEWCARDVARY);
        break;
    case 0x06:              //��Դ�豸��Ϣ�ϴ�,����1---8֡���������
        AckFunc(CanIndex, ACK, addr);
        Device[addr - 1].Status &= 0x0E;
        Device[addr - 1].Status += (MakeCanBusStatus(CanIndex) + (Can.Buf[0] & 0x01)); //״̬λ   ��ֱ��
        Device[addr - 1].Buf[0] = 0;
        Device[addr - 1].Buf[1] = 0;
        WritePowerRecord(addr, &Can.Buf[0]);
        break;
    case 0x07:          // Ӧ���㲥�ϴ�״̬��Ϣ
        status += MakeCanBusStatus(CanIndex);
        Device[addr - 1].Status &= 0x0E;
        Device[addr - 1].Status += (status + (Can.Buf[0] >> 7)); //״̬λ+����״̬λ
        if(Device[addr - 1].Warn & 0x7F)
			Device[addr - 1].Status |= 0x02;
        Device[addr - 1].Buf[0] = Can.Buf[0] & 0x7F;
        Device[addr - 1].Buf[1] = Can.Buf[1];
        AckFunc(CanIndex, ACK, addr);
        CheckCrc(addr, Can.Buf[5], 1);
        break;
    case 0x08:        //�㲥������¼
        break;
    case 0x09:             //�ϵ�����Ϣ,��ǰ״̬
        if(CheckTime(&Can.Buf[2]) == 1)
            SyncTime(CanIndex, addr);
        else
            AckFunc(CanIndex, ACK, addr);
        if((Can.Buf[0] == 0xFF) && (Can.Buf[1] == 0xFF))  //��λ��Ϣ
        {
            WriteSensorRecord(addr, DEVICERESET);
            break;
        }
        if(Sys.BiaoJiaoDuanDianAddr)
            break;
        // buf[0]��ǰ״̬  buf[1]�����ϵ�Ĵ�������ַ
        status += MakeCanBusStatus(CanIndex);
        Device[addr - 1].Status &= 0x80;      //D4λ�Զϵ�����Ч
        if((Device[addr - 1].Warn & 0x7F) && (!(Can.Buf[0] & 0x0C)))      //��Ҫ�ϵ磬��ǿ�ƶϵ�λ
            Device[addr - 1].Status += (status + (Can.Buf[0] & 0x1E) + 0x01);
        else                                  //��Ҫ���磬�����ʵʱֵ
            Device[addr - 1].Status += (status + (Can.Buf[0] & 0x1F)); //D7 D6 D5 CAN���ߵ�ǰ״̬
        Device[addr - 1].Buf[0] =  Can.Buf[0]; //�ϵ�����ǰ״̬
        Device[addr - 1].Buf[1] =  Can.Buf[1]; //�����ϵ��ַ
#ifndef FengDianNet
        CheckCrc(addr, Can.Buf[6], 1);
#endif
        break;
    
    case 0x0A:                // �ϵ����ϴ�������¼  ������ַ ����״̬  �����¼�  ʱ��4�ֽ�
    AckFunc(CanIndex,ACK,addr);
    if((Can.Buf[2] & 0x90) == 0x90)          // ��������
         Device[Can.Buf[0]-1].Status |= 0x80;
//    WriteSensorRecord(addr,LianDongRecord);
		
    break;
    case 0x0B:               // MAS�����ߴ�������������
        AckFunc(CanIndex, ACK, addr);
        MakeMASWirelessSensor(CanIndex, addr);
        break;
    case 0x0F:                  // ��ʱ�����ϴ�����
        MakeSensorCanData(ID, CanIndex, addr);
        break;
    case 0x10:   //ֻ��2��������յ����������͵�ȷ�ϣ�1.��λ������   2.�����Զ����ô�����
        if((Ack.NeedToNet == 1) && (Ack.Addr == addr))
        {
            CanInfoToNet(Net.SocketSelect->socketn, ACK, &Can.Buf[0], 0);
            Ack.NeedToNet = 0;
            break;
        }
        break;
    case 0x39:      //��������Ϣ��Ӧ
        Device[addr - 1].Warn &= 0x7F;
        LianDong.Retry = 0;
        if(Device[addr-1].Name == DuanDianQi)           // �ϵ���
        {
             Device[addr-1].Status &= ~0x03;
             Device[addr-1].Status += Can.Buf[0];
        }
        break;
    case 0x51:      //��ʼ����Ϣ�Ļظ�
        Init.Retry = 0;
        if((Init.FramStatus == F_Single) || (Init.FramStatus == F_End))
        {
            Init.Status = INITIDLE;     //�˳���ʼ��
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
    default:                   // ����ָ�ֱ��ת������λ��
        CanInfoToNet(Net.SocketSelect->socketn, cmd, &Can.Buf[0], Can.Len);
        break;
    }
}
