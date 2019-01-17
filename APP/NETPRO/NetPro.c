#include "NetPro.h"
#include "NetDataPro.h"
#include "DebugPro.h"

void UdpPro(void);
void NetSendData(u8 socketn,u8 *buf,u16 len);
void SerchCmdDeal(void);
void BootCmdDeal(void);

W5500SOCKET* Sockets[MAXSOCKETCNT];

u32 TASKID_NETPRO = 0xFFFFFFFF;
u32 TASKID_UDPPRO = 0xFFFFFFFF;
_NetManger NetManger;
_Udp Udp;

//TCP�˿�0���յ�����
void TcpSocket0Recv(u8 *buf,u16 len)
{	
//	SetPcLinkStatus(TCPSOCKET0,1);
	NetManger.SocketTick[TCPSOCKET0] = SYS_TICK;
	HandleNetData(TCPSOCKET0,buf,len,0);
}
//TCP�˿�1���յ�����
void TcpSocket1Recv(u8 *buf,u16 len)
{
	u32 fram;
//	SetPcLinkStatus(TCPSOCKET1,1);
	NetManger.SocketTick[TCPSOCKET1] = SYS_TICK;
	fram = *(u32*)buf;
	if(fram == 0xAA55AA55)
		HandleBoardCastCmd(buf,len,TCPSOCKET1);
	else
		HandleNetData(TCPSOCKET1,buf,len,0);
}
//TCP�˿�2���յ�����
void TcpSocket2Recv(u8 *buf,u16 len)
{
//	SetPcLinkStatus(TCPSOCKET2,1);
	NetManger.SocketTick[TCPSOCKET2] = SYS_TICK;
	HandleNetData(TCPSOCKET2,buf,len,0);
}
//UDP�˿�0���յ�����
void UdpSocket0Recv(u8 *buf,u16 len)
{
	NetManger.SocketTick[UDPSOCKET0] = SYS_TICK;
	HandleNetData(UDPSOCKET0,&buf[8],len-8,1);
}

void DebugSocketRecv(u8 *buf,u16 len)
{
	NetManger.SocketTick[DEBUGSOCKET] = SYS_TICK;
	HandleDebugCmd(buf,len);
}
//TCP�˿�1���յ�����
void UdpSocket1Recv(u8 *buf,u16 len)
{
	u8 i;
	u8 iapServerIp[8];
	cJSON* BootCmd;
	cJSON* pItem;
	cJSON* pArray;
	
	NetManger.SocketTick[UDPSOCKET1] = SYS_TICK;
	Udp.remotePort = Net.SocketSelect->remotePort;
	BootCmd = cJSON_Parse((char*)buf);
	if(BootCmd != 0)
	{
		myfree(SRAMIN,Udp.ackData);
		pItem = cJSON_GetObjectItem(BootCmd,"cmdType");
		if(BufCmp((u8*)pItem->valuestring,(u8*)"SERCHDEV",7))//�����豸ָ��
			SerchCmdDeal();
		else if(BufCmp((u8*)pItem->valuestring,(u8*)"ENTERBOOT",8))//������������ָ��
		{
			pItem = cJSON_GetObjectItem(BootCmd,"data");//��ȡ������
			pArray = pItem->child;
			for(i=0;i<6;i++)//��ȡ�̼����·�������ַ
			{
				iapServerIp[i] = (pArray->valueint & 0xFF);
				pArray = pArray->next;
			}
			UpdateBootLoader(iapServerIp);//�̼����·�������ַ����EEPROM�����������ȡ������
			BootCmdDeal();
		}
		Udp.Tick = SYS_TICK;
		Udp.Status = UDPRECV;
	}
	cJSON_Delete(BootCmd);
}

void CreatUserPort(void)
{
	Sockets[TCPSOCKET0] = Net.newTcpServer(5000,TcpSocket0Recv);//����TCPSERVER�˿�1
	Sockets[TCPSOCKET1] = Net.newTcpServer(5001,TcpSocket1Recv);//����TCPSERVER�˿�2
	Sockets[TCPSOCKET2] = Net.newTcpServer(5002,TcpSocket2Recv);//����TCPSERVER�˿�3
	Sockets[UDPSOCKET0] = Net.newUdpSocket(5003,UdpSocket0Recv);//����UDP�˿�0
	Sockets[UDPSOCKET1] = Net.newUdpSocket(5005,UdpSocket1Recv);//����UDP�˿�1
	Sockets[DEBUGSOCKET] = Net.newTcpServer(5004,DebugSocketRecv);//����TCPSERVER�˿�4
	TASKID_UDPPRO = CreateTask("UdpPro",0,0,0,0,0,UdpPro);
	NetManger.SocketTick[TCPSOCKET0] = SYS_TICK;
	NetManger.SocketTick[TCPSOCKET1] = SYS_TICK;
	NetManger.SocketTick[TCPSOCKET2] = SYS_TICK;
	NetManger.SocketTick[UDPSOCKET0] = SYS_TICK;
}

void NetOfflineDeal(void)
{
	u8 i;
	for(i=0;i<2;i++)
	{
		if(MsTickDiff(NetManger.SocketTick[i]) >= NETOFFLINETIME)
		{
			Net.ReCreatSocket(Sockets[i]);
			NetManger.SocketTick[i] = SYS_TICK;
//			SetPcLinkStatus(i,0);
			return;
		}
	}
}

void NetSendData(u8 socketn,u8 *buf,u16 len)
{
	Net.SendTcpData(Sockets[socketn],buf,len);
}

void NetPro(void)
{
	switch(NetManger.Status)
	{
		case NETINIT:
			SetTaskStatus(TASKID_NETPRO,"W5500��ʼ��");
			NetManger.SendData = NetSendData;
			W5500Spi_Init();//��ʼ��W5500���ݽӿ�
			//ע��W5500��������
			W5500Init(W5500SetRstValue,W5500SetCsValue,delay_ms,W5500Spi_SendByte,W5500GetIntIoState);
			NetManger.Tick   = SYS_TICK;
			NetManger.Status = NETWAITHARDLINK;
			break;
		case NETWAITHARDLINK:
			SetTaskStatus(TASKID_NETPRO,"W5500Ӳ������");
			if(Net.WireIsLink())//�ȴ���̫���������
			{
				Net.SubMask[0] = 255;
				Net.SubMask[1] = 255;
				Net.SubMask[2] = 255;
				Net.SubMask[3] = 0;
				Net.SetAddrInfo(Net.localIp,Net.GatewayIP,Net.SubMask,Net.MacAddr);
				NetManger.Status = NETCREATUSERPORT;
//				SetNetLinkStatus(1);
			}
			if(MsTickDiff(NetManger.Tick) > 5000)//�������ӣ�ʵ����Ҫ2s����ʱ��
				NetManger.Status = NETINIT;
			break;
		case NETCREATUSERPORT:
			SetTaskStatus(TASKID_NETPRO,"�����û��˿�");
			CreatUserPort();
			NetManger.Status = NETRECVCHECK;
			break;
		case NETRECVCHECK:
			SetTaskStatus(TASKID_NETPRO,"���ݽ���");
			if(!Net.WireIsLink())
			{
//				SetNetLinkStatus(0);
				NetManger.Status = NETINIT;
			}
			Net.NetIntDeal();
			NetOfflineDeal();
			break;
	}
}

void UdpPro(void)
{
	switch(Udp.Status)
	{
		case UDPIDLE:
			break;
		
		case UDPRECV:
			if(MsTickDiff(Udp.Tick) >= Net.localIp[3]*2)//���Ӧ��
				Udp.Status = UDPACK;
			break;
			
		case UDPACK:
			Net.SendUdpData(&Net.Sockets[UDPSOCKET1],Udp.ackData,strlen((char*)Udp.ackData),Net.SocketSelect->remoteIP,Udp.remotePort+1);
			myfree(SRAMIN,Udp.ackData);
			if(Udp.cmdType == 0x02)
			{
				Net.CloseAllSocket();//�ر�����Socket
				Sys_Soft_Reset();//������������
			}
			Udp.Status = UDPIDLE;
			break;
	}
}

//�����豸����ָ��
void SerchCmdDeal(void)
{
	cJSON* Ack;
	char ipAddr[30];
	char macAddr[30];
	
	Ack = cJSON_CreateObject();//����Json����
	cJSON_AddStringToObject(Ack,"cmdType","AckSerch");//�������ͣ�Ӧ���豸����
	cJSON_AddStringToObject(Ack,"softVer",Version);//��վ����汾
	sprintf(ipAddr,"%d.%d.%d.%d",Net.localIp[0],Net.localIp[1],Net.localIp[2],Net.localIp[3]);
	cJSON_AddStringToObject(Ack,"ipAddr",ipAddr);//��վIP��ַ
	sprintf(macAddr,"%02x:%02x:%02x:%02x:%02x:%02x",Net.MacAddr[0],Net.MacAddr[1],Net.MacAddr[2],Net.MacAddr[3],Net.MacAddr[4],Net.MacAddr[5]);
	cJSON_AddStringToObject(Ack,"macAddr",macAddr);//��վMAC��ַ
	cJSON_AddNullToObject(Ack,"data");//�������򣬷��ؿ�
	cJSON_AddStringToObject(Ack,"position","������������ۺϷ�վ");//��վ��װλ��
	Udp.ackData = (u8*)cJSON_PrintUnformatted(Ack);//��JSON����ת��Ϊbyte����
	Udp.cmdType = 0x01;//UDP��������
	cJSON_Delete(Ack);//�ͷ�Json����ռ�õ��ڴ�
}

//���������������ָ��
void BootCmdDeal(void)
{
	cJSON* Ack;
	Ack = cJSON_CreateObject();//����Json����
	cJSON_AddStringToObject(Ack,"cmdType","AckEnterBoot");
	cJSON_AddStringToObject(Ack,"softVer",Version);
	cJSON_AddNullToObject(Ack,"data");
	cJSON_AddNullToObject(Ack,"ipAddr");
	cJSON_AddNullToObject(Ack,"macAddr");
	Udp.ackData = (u8*)cJSON_PrintUnformatted(Ack);
	Udp.cmdType = 0x02;
	cJSON_Delete(Ack);//�ͷ�Json����ռ�õ��ڴ�
}
