#ifndef __SOCKET_H
#define __SOCKET_H

#include "W5500.h"

#define NULL 0
#define MAXSOCKET			8
#define SOCKETBUFLEN		1500

typedef enum
{
	MODE_IDLE 		= 0x00,
	MODE_TCPSERVER 	= 0x01,
	MODE_TCPCLIENT 	= 0x02,
	MODE_UDP 		= 0x03,
	MODE_TCPSLINK 	= 0x04,
	MODE_TCPCLINK 	= 0x05,
}SOCKETMODE;

typedef struct
{
	SOCKETMODE Mode;
	u8 socketn;
	u8  remoteIP[4];
	u16 remotePort;
	u16 localPort;
	u16 Len;
	u8 Buf[SOCKETBUFLEN];
	void(*UdpRecv)(u8 *buf,u16 len);
	void(*TcpSRecv)(u8 *buf,u16 len);
	void(*TcpCRecv)(u8 *buf,u16 len);	
}W5500SOCKET;

typedef enum
{
	NET_INIT = 0x00,
	NET_LINK = 0x01,
	NET_MODE = 0x02,
	NET_INT  = 0x03,
	NET_IDLE = 0x04,
}NetStatus;

typedef struct
{
	NetStatus Status;
	u8 	IntFlag;
	u16	Tick;
	u16   port;
	u8 Buf[SOCKETBUFLEN];
	W5500SOCKET*  SocketSelect;
	W5500SOCKET Sockets[MAXSOCKET];
	u8  MacAddr[6];//Mac地址	
	u8  localIp[4];//本地IP地址
	u8  SubMask[4];//子网掩码
	u8  GatewayIP[4];//默认网关
	u8  (*CheckIntIo)(void);
	
	void(*SetAddrInfo)(u8 *ip,u8 *gwip,u8 *mask,u8 *mac);
	u8(*WireIsLink)(void);
	W5500SOCKET* (*newTcpServer)(u16 port,void (*RecvFunction)(u8 *buf,u16 len));
	W5500SOCKET* (*newTcpClient)(u8 *remoteIp,u16 remotePort,u16 localport,void (*RecvFunction)(u8 *buf,u16 len));
	W5500SOCKET* (*newUdpSocket)(u16 port,void (*RecvFunction)(u8 *buf,u16 len));
	void(*SendTcpData)(W5500SOCKET* s,u8 *buf,u16 len);
	void(*SendUdpData)(W5500SOCKET* s,u8 *buf,u16 len,u8 *receverIp,u16 receverPort);
    void(*NetIntDeal)(void);
	void(*CheckNetData)(void);
	void(*CloseSocket)(W5500SOCKET* s);
	void(*CloseAllSocket)(void);
	void(*ReCreatSocket)(W5500SOCKET* s);
}_Net;

extern _Net Net;

void W5500Init(	void(*SetRst)(u8 val),
				void(*SetCs)(u8 val),
				void(*delay)(u32 nms),
				u8(*spi_sentbyte)(u8 byte),
				u8(*checkIntIo)(void));
#endif
