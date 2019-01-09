#include "DisplayUpdate.h"
#include "malloc.h"

u32 TASKID_QueenUpdate = 0xFFFFFFFF;

_QueenUpdate QueenUpdate;

void QueenUpdateDatRecv(u8 *buf,u16 len)
{
	u8 temp = QueenUpdate.Fram+1,temp1;
	temp1 = ~temp;
	if(buf[0] == 0xAA)
	{
		QueenUpdate.Tick = SYS_TICK;
		QueenUpdate.Ststus = QUEENREADUPDATEDATE;
	}
	if((buf[0] == temp) && (buf[1] == temp1))
	{
		QueenUpdate.Fram++;
		QueenUpdate.Ststus = QUEENREADUPDATEDATE;
	}
}

void QueenUpdateCmdInit(void)
{
	QueenUpdate.Fram = 0;
	QueenUpdate.SendCnt = 0;
	QueenUpdate.Retry = 0;
	QueenUpdate.UpDateCmd[0] = 0xEE;
	QueenUpdate.UpDateCmd[1] = 0xF1;
	QueenUpdate.UpDateCmd[2] = 0x00;
	QueenUpdate.UpDateCmd[3] = 0x00;
	QueenUpdate.UpDateCmd[4] = 0x00;
	QueenUpdate.UpDateCmd[5] = 0x00;
	QueenUpdate.UpDateCmd[6] = 0x00;
	QueenUpdate.UpDateCmd[7] = 0x00;
	QueenUpdate.UpDateCmd[8] = 0x00;
	QueenUpdate.UpDateCmd[9] = 0x00;
	QueenUpdate.UpDateCmd[10] = 0x00;
	QueenUpdate.UpDateCmd[11] = 0xFF;
	QueenUpdate.UpDateCmd[12] = 0xFC;
	QueenUpdate.UpDateCmd[13] = 0xFF;
	QueenUpdate.UpDateCmd[14] = 0xFF;
}

void MakeQueenUpdateCmd(void)
{
	u8 i,sum = 0;
	QueenUpdate.UpDateCmd[2] = ((QueenUpdate.FileSize >> 24) & 0xFF);
	QueenUpdate.UpDateCmd[3] = ((QueenUpdate.FileSize >> 16) & 0xFF);
	QueenUpdate.UpDateCmd[4] = ((QueenUpdate.FileSize >> 8) & 0xFF);
	QueenUpdate.UpDateCmd[5] = (QueenUpdate.FileSize & 0xFF);
	QueenUpdate.UpDateCmd[6] = ((460800 >> 24) & 0xFF);
	QueenUpdate.UpDateCmd[7] = ((460800 >> 16) & 0xFF);
	QueenUpdate.UpDateCmd[8] = ((460800 >> 8) & 0xFF);
	QueenUpdate.UpDateCmd[9] = (460800 & 0xFF);
	for(i=1;i<10;i++)
		sum+=QueenUpdate.UpDateCmd[i];
	QueenUpdate.UpDateCmd[10] = sum;
}

void MakeQueenUpdatePack(u32 DataLen)
{
	u16 i,sum = 0;
	u8 temp = QueenUpdate.Fram;
	QueenUpdate.DataPack[0] = temp;
	QueenUpdate.DataPack[1] = ~temp;
	for(i=DataLen;i<2048;i++)
		QueenUpdate.DataPack[2+i] = 0;
	for(i=0;i<2050;i++)
		sum += QueenUpdate.DataPack[i];
	sum = ~sum;
	QueenUpdate.DataPack[2050] = ((sum >> 8) & 0xFF);
	QueenUpdate.DataPack[2051] = (sum & 0xFF);
}

void QueenUpdateClose(void)
{
	QueenUpdate.SendCnt = 0;
	QueenUpdate.Fram = 0;
	QueenUpdate.Retry = 0;
	QueenUpdate.Ststus = QUEENUPDATEINIT;
	myfree(SRAMEX,QueenUpdate.DataPack);
	DelTask(&TASKID_QueenUpdate);
}

void QueenUpdatePro(void)
{
	switch(QueenUpdate.Ststus)
	{
		case QUEENUPDATEINIT:
			ComManger.ComInit(COM3,460800,QueenUpdateDatRecv);//初始化串口3，用于与屏幕通信
			myfree(SRAMEX,QueenUpdate.DataPack);
			QueenUpdate.DataPack = mymalloc(SRAMEX,QUEENUPDATEPACKLEN);//申请数据缓存
			QueenUpdateCmdInit();//初始化屏幕指令
			QueenUpdate.SendData = ComManger.Com3Send;//屏幕固件升级数据通道选择串口3
			QueenUpdate.Ststus = QUEENLOADUPDATEFILE;
		break;
		
		case QUEENLOADUPDATEFILE:
			//打开屏幕固件升级文件
			QueenUpdate.FileSize = mf_open("0:/DCIOT.PKG",FA_OPEN_EXISTING | FA_READ);//升级固件字节数
			QueenUpdate.FileCnt = QueenUpdate.FileSize / (QUEENUPDATEPACKLEN-4);//计算需要发送整包数据次数
			QueenUpdate.Remain = QueenUpdate.FileSize % (QUEENUPDATEPACKLEN-4);//计算最后不足一包数据字节数
			QueenUpdate.Ststus = QUEENSENDUPDATECMD;
		break;
		
		case QUEENSENDUPDATECMD:
			MakeQueenUpdateCmd();//生成固件升级指令
			QueenUpdate.SendData(QueenUpdate.UpDateCmd,15);//向屏幕发送固件升级指令
			QueenUpdate.Tick = SYS_TICK;
			QueenUpdate.Ststus = QUEENWAITACK;
			break;
		
		case QUEENWAITACK://等待屏幕应答
			//应答超时，固件升级失败
			if(MsTickDiff(QueenUpdate.Tick) >= 5000)
			{
				QueenUpdate.Tick = SYS_TICK;
				QueenUpdate.Ststus = QUEENUPDATEFAIL;
			}
			break;
		
		case QUEENWAITACK1:
			if(MsTickDiff(QueenUpdate.Tick) >= 5)
				QueenUpdate.Ststus = QUEENREADUPDATEDATE;
			break;
			
		case QUEENREADUPDATEDATE:
			if(QueenUpdate.SendCnt < QueenUpdate.FileCnt)
			{
				mf_read(&QueenUpdate.DataPack[2],(QUEENUPDATEPACKLEN-4));
				QueenUpdate.PackLen = (QUEENUPDATEPACKLEN-4);
				QueenUpdate.Ststus = QUEENSENDUPDATEPACK;
			}
			else if((QueenUpdate.SendCnt == QueenUpdate.FileCnt) && (QueenUpdate.Remain !=0))
			{
				mf_read(&QueenUpdate.DataPack[2],QueenUpdate.Remain);
				QueenUpdate.PackLen = QueenUpdate.Remain;
				QueenUpdate.Ststus = QUEENSENDUPDATEPACK;
			}else
			{
				QueenUpdate.Tick = SYS_TICK;
				QueenUpdate.Ststus = QUEENUPDATESUCCESS;
			}
			if(QueenUpdate.Ststus != QUEENUPDATESUCCESS)
				MakeQueenUpdatePack(QueenUpdate.PackLen);
			break;
		
		case QUEENSENDUPDATEPACK:
			QueenUpdate.SendData(QueenUpdate.DataPack,QUEENUPDATEPACKLEN);
			QueenUpdate.SendCnt++;
			
			QueenUpdate.Tick = SYS_TICK;
			QueenUpdate.Ststus = QUEENWAITACK;
			break;
		
		case QUEENUPDATEFAIL:
//			if(MsTickDiff(QueenUpdate.Tick) >= 20000)
//				QueenUpdate.Ststus = QUEENUPDATEINIT;
			QueenUpdateClose();
			break;
		
		case QUEENUPDATESUCCESS:
//			if(MsTickDiff(QueenUpdate.Tick) >= 35000)
//				QueenUpdate.Ststus = QUEENUPDATEINIT;
			QueenUpdateClose();
			break;
	}
}
