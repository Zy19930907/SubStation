#ifndef __NETPRO_H
#define __NETPRO_H

#include "Public.h"

#define MAXSOCKETCNT			6
#define NETOFFLINETIME			15000
#define TCPSOCKET0				0
#define TCPSOCKET1				1
#define TCPSOCKET2				2
#define UDPSOCKET0				3
#define UDPSOCKET1				4
#define DEBUGSOCKET				5

typedef enum
{
	UDPIDLE,
	UDPRECV,
	UDPACK,
}_UdpStatus;

typedef struct
{
	_UdpStatus Status;
	u8* ackData;
	u32 Tick;
	u8 cmdType;
	u16 remotePort;
}_Udp;

typedef enum
{
	NETINIT,
	NETWAITHARDLINK,
	NETCREATUSERPORT,
	NETRECVCHECK,
}_NetStatus;

typedef struct
{
	_NetStatus Status;
	u8 Flag;
	u32 SocketTick[MAXSOCKETCNT];
	u32 Tick;
	void(*SendData)(u8 Scoketn,u8 *buf,u16 len);
}_NetManger;

extern _NetManger NetManger;
extern u32 TASKID_NETPRO;

void NetPro(void);

#endif
