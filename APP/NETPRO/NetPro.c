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

//TCP端口0接收到数据
void TcpSocket0Recv(u8 *buf,u16 len)
{	
//	SetPcLinkStatus(TCPSOCKET0,1);
	NetManger.SocketTick[TCPSOCKET0] = SYS_TICK;
	HandleNetData(TCPSOCKET0,buf,len,0);
}
//TCP端口1接收到数据
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
//TCP端口2接收到数据
void TcpSocket2Recv(u8 *buf,u16 len)
{
//	SetPcLinkStatus(TCPSOCKET2,1);
	NetManger.SocketTick[TCPSOCKET2] = SYS_TICK;
	HandleNetData(TCPSOCKET2,buf,len,0);
}
//UDP端口0接收到数据
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
//TCP端口1接收到数据
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
		if(BufCmp((u8*)pItem->valuestring,(u8*)"SERCHDEV",7))//搜索设备指令
			SerchCmdDeal();
		else if(BufCmp((u8*)pItem->valuestring,(u8*)"ENTERBOOT",8))//进入引导程序指令
		{
			pItem = cJSON_GetObjectItem(BootCmd,"data");//获取数据域
			pArray = pItem->child;
			for(i=0;i<6;i++)//获取固件更新服务器地址
			{
				iapServerIp[i] = (pArray->valueint & 0xFF);
				pArray = pArray->next;
			}
			UpdateBootLoader(iapServerIp);//固件更新服务器地址存入EEPROM，引导程序读取后连接
			BootCmdDeal();
		}
		Udp.Tick = SYS_TICK;
		Udp.Status = UDPRECV;
	}
	cJSON_Delete(BootCmd);
}

void CreatUserPort(void)
{
	Sockets[TCPSOCKET0] = Net.newTcpServer(5000,TcpSocket0Recv);//创建TCPSERVER端口1
	Sockets[TCPSOCKET1] = Net.newTcpServer(5001,TcpSocket1Recv);//创建TCPSERVER端口2
	Sockets[TCPSOCKET2] = Net.newTcpServer(5002,TcpSocket2Recv);//创建TCPSERVER端口3
	Sockets[UDPSOCKET0] = Net.newUdpSocket(5003,UdpSocket0Recv);//创建UDP端口0
	Sockets[UDPSOCKET1] = Net.newUdpSocket(5005,UdpSocket1Recv);//创建UDP端口1
	Sockets[DEBUGSOCKET] = Net.newTcpServer(5004,DebugSocketRecv);//创建TCPSERVER端口4
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
			SetTaskStatus(TASKID_NETPRO,"W5500初始化");
			NetManger.SendData = NetSendData;
			W5500Spi_Init();//初始化W5500数据接口
			//注册W5500操作函数
			W5500Init(W5500SetRstValue,W5500SetCsValue,delay_ms,W5500Spi_SendByte,W5500GetIntIoState);
			NetManger.Tick   = SYS_TICK;
			NetManger.Status = NETWAITHARDLINK;
			break;
		case NETWAITHARDLINK:
			SetTaskStatus(TASKID_NETPRO,"W5500硬件连接");
			if(Net.WireIsLink())//等待以太网连接完成
			{
				Net.SubMask[0] = 255;
				Net.SubMask[1] = 255;
				Net.SubMask[2] = 255;
				Net.SubMask[3] = 0;
				Net.SetAddrInfo(Net.localIp,Net.GatewayIP,Net.SubMask,Net.MacAddr);
				NetManger.Status = NETCREATUSERPORT;
//				SetNetLinkStatus(1);
			}
			if(MsTickDiff(NetManger.Tick) > 5000)//物理连接，实测需要2s左右时间
				NetManger.Status = NETINIT;
			break;
		case NETCREATUSERPORT:
			SetTaskStatus(TASKID_NETPRO,"创建用户端口");
			CreatUserPort();
			NetManger.Status = NETRECVCHECK;
			break;
		case NETRECVCHECK:
			SetTaskStatus(TASKID_NETPRO,"数据接收");
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
			if(MsTickDiff(Udp.Tick) >= Net.localIp[3]*2)//错峰应答
				Udp.Status = UDPACK;
			break;
			
		case UDPACK:
			Net.SendUdpData(&Net.Sockets[UDPSOCKET1],Udp.ackData,strlen((char*)Udp.ackData),Net.SocketSelect->remoteIP,Udp.remotePort+1);
			myfree(SRAMIN,Udp.ackData);
			if(Udp.cmdType == 0x02)
			{
				Net.CloseAllSocket();//关闭所有Socket
				Sys_Soft_Reset();//启动引导程序
			}
			Udp.Status = UDPIDLE;
			break;
	}
}

//处理设备搜索指令
void SerchCmdDeal(void)
{
	cJSON* Ack;
	char ipAddr[30];
	char macAddr[30];
	
	Ack = cJSON_CreateObject();//创建Json对象
	cJSON_AddStringToObject(Ack,"cmdType","AckSerch");//数据类型，应答设备搜索
	cJSON_AddStringToObject(Ack,"softVer",Version);//分站软件版本
	sprintf(ipAddr,"%d.%d.%d.%d",Net.localIp[0],Net.localIp[1],Net.localIp[2],Net.localIp[3]);
	cJSON_AddStringToObject(Ack,"ipAddr",ipAddr);//分站IP地址
	sprintf(macAddr,"%02x:%02x:%02x:%02x:%02x:%02x",Net.MacAddr[0],Net.MacAddr[1],Net.MacAddr[2],Net.MacAddr[3],Net.MacAddr[4],Net.MacAddr[5]);
	cJSON_AddStringToObject(Ack,"macAddr",macAddr);//分站MAC地址
	cJSON_AddNullToObject(Ack,"data");//无数据域，返回空
	cJSON_AddStringToObject(Ack,"position","三采区变电所综合分站");//分站安装位置
	Udp.ackData = (u8*)cJSON_PrintUnformatted(Ack);//将JSON对象转换为byte数组
	Udp.cmdType = 0x01;//UDP命令类型
	cJSON_Delete(Ack);//释放Json对象占用的内存
}

//处理进入引导程序指令
void BootCmdDeal(void)
{
	cJSON* Ack;
	Ack = cJSON_CreateObject();//创建Json对象
	cJSON_AddStringToObject(Ack,"cmdType","AckEnterBoot");
	cJSON_AddStringToObject(Ack,"softVer",Version);
	cJSON_AddNullToObject(Ack,"data");
	cJSON_AddNullToObject(Ack,"ipAddr");
	cJSON_AddNullToObject(Ack,"macAddr");
	Udp.ackData = (u8*)cJSON_PrintUnformatted(Ack);
	Udp.cmdType = 0x02;
	cJSON_Delete(Ack);//释放Json对象占用的内存
}
