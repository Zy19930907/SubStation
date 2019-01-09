#include "Socket.h"
#include "malloc.h"

_Net Net;
extern _W5500 W5500;
void NetIntProcess(void);
u8 WireIsLink(void);
void SetAddrInfo(u8 *ip,u8 *gwip,u8 *mask,u8 *mac);
void Sendto(W5500SOCKET* s,u8 *buf,u16 len,u8 *receverIp,u16 receverPort);
void SendSocketData(W5500SOCKET* s,u8 *buf,u16 len);
W5500SOCKET* newTcpServer(u16 port,void (*RecvFunction)(u8 *buf,u16 len));
W5500SOCKET* newTcpClient(u8 *remoteIp,u16 remotePort,u16 localport,void (*RecvFunction)(u8 *buf,u16 len));
W5500SOCKET* newUdpSocket(u16 port,void (*RecvFunction)(u8 *buf,u16 len));
void DisableSocket(W5500SOCKET* s);
void CloseAllSocket(void);
void NetIntProcess(void);
void ReCreatSocket(W5500SOCKET* s);

void W5500Init(	void(*SetRst)(u8 val),
				void(*SetCs)(u8 val),
				void(*delay)(u32 nms),
				u8(*spi_sentbyte)(u8 byte),
				u8(*checkIntIo)(void))
{	
	u8 i;
	W5500_Init(SetRst,SetCs,delay,spi_sentbyte);
	W5500Reset();
	for(i=0;i<MAXSOCKET;i++)
		Net.Sockets[i].socketn = i;
	Net.CheckIntIo = checkIntIo;
	Net.CheckNetData = NetIntProcess;
	Net.CloseAllSocket = CloseAllSocket;
	Net.ReCreatSocket = ReCreatSocket;
	Net.CloseSocket = DisableSocket;
	Net.newTcpClient = newTcpClient;
	Net.newTcpServer = newTcpServer;
	Net.newUdpSocket = newUdpSocket;
	Net.SetAddrInfo = SetAddrInfo;
	Net.WireIsLink = WireIsLink;
	Net.NetIntDeal = NetIntProcess;
	Net.SendTcpData = SendSocketData;
	Net.SendUdpData = Sendto;
}

u8 NetAsc2Hex(u8 in)
{
	u8 inlist[16] = "0123456789ABCDEF",i;
	for(i=0;i<16;i++)
	{
		if(in == *(inlist+i))
			return i;
	}
	return 0;
}

u8 WireIsLink(void)
{
	if(ReadReg1Byte(PHYCFGR) & LINK)
		return TRUE;
	else 
		return FALSE;
}

void SetString2Ip(u8 *buf,u8 *ipString)
{
	u8 i=0,j=0,ip=0;
	for(j=0;j<3;j++)
	{
		while(*(ipString+(i)) != '.')
		{
			ip *=10;
			ip+= NetAsc2Hex(*(ipString+(i++)));
		}
		*(buf++) = ip;
		ip=0;
		i++;
	}
	while(*(ipString+i) != '\0')
	{
		ip *=10;
		ip+= NetAsc2Hex(*(ipString+(i++)));
	}
	*(buf++) = ip;
}

W5500SOCKET* GetIntSocket(u8 flag)
{
	u8 i;
	for(i=0;i<MAXSOCKET;i++)
	{
		if(flag & 0x01)
			return &Net.Sockets[i];
		flag >>= 1;
	}
	return NULL;		
}
//��ȡ����socket
W5500SOCKET* GetIdleSocket(void)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		if(Net.Sockets[i].Mode == MODE_IDLE)
			return &Net.Sockets[i];
	}
	return NULL;
}

void SetAddrInfo(u8 *ip,u8 *gwip,u8 *mask,u8 *mac)
{
	u8 i;
	for(i=0;i<6;i++)
	{
		if(i < 4)
		{
			*(Net.localIp + i) = *(ip + i);
			*(Net.GatewayIP + i) = *(gwip + i);
			*(Net.SubMask + i) = *(mask + i);
		}
		*(Net.MacAddr + i) = *(mac + i);
	}
	WriteReg1Byte(MR, RST);
	W5500.Delay(200);
	
	WriteBurstReg(GAR,Net.GatewayIP,4);
	WriteBurstReg(SUBR,Net.SubMask,4);
	WriteBurstReg(SHAR,Net.MacAddr,6);
	WriteBurstReg(SIPR,Net.localIp,4);

     for(i = 0;i < 8;i ++)
     {
          WriteSocket1Byte(i,Sn_RXBUF_SIZE,0x02);
          WriteSocket1Byte(i,Sn_TXBUF_SIZE,0x02);
     }
}

W5500SOCKET* newTcpServer(u16 port,void (*RecvFunction)(u8 *buf,u16 len))
{
	u8 Cnt = 0;
	
	if((Net.SocketSelect = GetIdleSocket()) == NULL) return NULL;
	
TcpServerCreatFail:	
	Net.SocketSelect->localPort = port;
	WriteReg2Byte(RTR,2000);    // 0.1MS Ϊһ����λ���ش���ʱʱ��
	WriteReg1Byte(RCR,8);       // �ش�����
	WriteReg1Byte(IMR,IM_IR7 | IM_IR6);   //IP��ַ��ͻ��Ŀ�ĵ�ַ���ܵ����ж�
	Net.IntFlag |= (1 << Net.SocketSelect->socketn);
	WriteReg1Byte(SIMR,Net.IntFlag);//������Ӧsocket�ж�           
	WriteSocket1Byte(Net.SocketSelect->socketn,Sn_IMR, IMR_SENDOK | IMR_TIMEOUT | IMR_RECV | IMR_DISCON | IMR_CON);
	WriteSocket2Byte(Net.SocketSelect->socketn,Sn_PORT,port);   //���ö˿�s��Դ��ַ
	WriteSocket1Byte(Net.SocketSelect->socketn,Sn_MR,MR_TCP);//����socketΪTCPģʽ 
	WriteSocket1Byte(Net.SocketSelect->socketn,Sn_CR,OPENNET);//��Socket
	
	W5500.Delay(10);	
	if(ReadSocket1Byte(Net.SocketSelect->socketn,Sn_SR)!= SOCK_INIT)//���socket��ʧ��
	{
		WriteSocket1Byte(Net.SocketSelect->socketn,Sn_CR,CLOSENET);//�򿪲��ɹ�,�ر�Socket
		Cnt++;
		if(Cnt<3)
			goto TcpServerCreatFail;
		return NULL;
	}	
	WriteSocket1Byte(Net.SocketSelect->socketn,Sn_CR,LISTEN);//����SocketΪ����ģʽ
	Net.SocketSelect->Mode = MODE_TCPSLINK;
	Net.SocketSelect->TcpSRecv = RecvFunction;
	return Net.SocketSelect;
}

W5500SOCKET* newTcpClient(u8 *remoteIp,
					  u16 remotePort,
					  u16 localport,
					  void (*RecvFunction)(u8 *buf,u16 len))
{
	u8 Cnt = 0,i=0;
	
	if((Net.SocketSelect = GetIdleSocket()) == NULL) return NULL;
	for(i=0;i<4;i++)
		Net.SocketSelect->remoteIP[i] = *(remoteIp+i);
	Net.SocketSelect->remotePort = remotePort;
	Net.SocketSelect->localPort = localport;
	
TcpClientCreatFail:

	WriteReg2Byte(RTR,2000);    // 0.1MS Ϊһ����λ���ش���ʱʱ��
	WriteReg1Byte(RCR,8);       // �ش�����
	WriteReg1Byte(IMR,IM_IR7 | IM_IR6);   //IP��ַ��ͻ��Ŀ�ĵ�ַ���ܵ����ж�
	Net.IntFlag |= (1 << Net.SocketSelect->socketn);
	WriteReg1Byte(SIMR,Net.IntFlag);          
	WriteSocket1Byte(Net.SocketSelect->socketn, Sn_IMR, IMR_SENDOK | IMR_TIMEOUT | IMR_RECV | IMR_DISCON | IMR_CON);
	WriteSocket2Byte(Net.SocketSelect->socketn,Sn_PORT,localport);   //���ö˿�s��Դ��ַ
	WriteSocket2Byte(Net.SocketSelect->socketn,Sn_DPORTR,remotePort);   //���ö˿�s��Դ��ַ
	WriteSocket4Byte(Net.SocketSelect->socketn,Sn_DIPR,remoteIp);
	WriteSocket1Byte(Net.SocketSelect->socketn,Sn_MR,MR_TCP);//����socketΪTCPģʽ 
	WriteSocket1Byte(Net.SocketSelect->socketn,Sn_CR,OPENNET);//��Socket
	
	W5500.Delay(10);
	if(ReadSocket1Byte(Net.SocketSelect->socketn,Sn_SR)!= SOCK_INIT)//���socket��ʧ��
	{
		WriteSocket1Byte(Net.SocketSelect->socketn,Sn_CR,CLOSENET);//�򿪲��ɹ�,�ر�Socket
		Cnt++;
		if(Cnt<3)
			goto TcpClientCreatFail;
		return NULL;
	}
	WriteSocket1Byte(Net.SocketSelect->socketn,Sn_CR,CONNECT);
	Net.SocketSelect->Mode = MODE_TCPCLINK;
	Net.SocketSelect->TcpCRecv = RecvFunction;
	return Net.SocketSelect;
}

W5500SOCKET* newUdpSocket(u16 port,void (*RecvFunction)(u8 *buf,u16 len))
{
	u8 Cnt = 0;
	
	if((Net.SocketSelect = GetIdleSocket()) == NULL) return NULL;
	
UdpCreatFail:
	Net.SocketSelect->localPort = port;
	WriteReg1Byte(IMR,IM_IR7 | IM_IR6);   //IP��ַ��ͻ��Ŀ�ĵ�ַ���ܵ����ж�
	
	Net.IntFlag |= (1 << Net.SocketSelect->socketn);
	WriteReg1Byte(SIMR,Net.IntFlag);         
	WriteSocket1Byte(Net.SocketSelect->socketn, Sn_IMR, IMR_SENDOK| IMR_RECV);
	
	WriteSocket2Byte(Net.SocketSelect->socketn,Sn_PORT,port);   //���ö˿�s��Դ��ַ
	WriteSocket1Byte(Net.SocketSelect->socketn,Sn_MR,MR_UDP);//����socketΪTCPģʽ 
	WriteSocket1Byte(Net.SocketSelect->socketn,Sn_CR,OPENNET);//��Socket	
	
	W5500.Delay(10);
	if(ReadSocket1Byte(Net.SocketSelect->socketn,Sn_SR)!= SOCK_UDP)//���socket��ʧ��
	{
		WriteSocket1Byte(Net.SocketSelect->socketn,Sn_CR,CLOSENET);//�򿪲��ɹ�,�ر�Socket
		Cnt++;
		if(Cnt<3)
			goto UdpCreatFail;
		return NULL;
	}
	Net.SocketSelect->UdpRecv = RecvFunction;
	Net.SocketSelect->Mode = MODE_UDP;
	return Net.SocketSelect;
}

void Sendto(W5500SOCKET* s,u8 *buf,u16 len,u8 *receverIp,u16 receverPort)
{
	u16 offset,offset1,i;
	
	if(s == NULL) return;
	//SetString2Ip(s->remoteIP,receverIp);
	for(i=0;i<4;i++)
		s->remoteIP[i] = receverIp[i];
	s->remotePort = receverPort;
	WriteSocket4Byte(s->socketn,Sn_DIPR,Net.SocketSelect->remoteIP);
	WriteSocket2Byte(s->socketn,Sn_DPORTR,receverPort);
	offset=ReadSocket2Byte(s->socketn,Sn_TX_WR);
	offset1=offset;
	offset&=(S_TX_SIZE-1);//����ʵ�ʵ������ַ

	W5500.SetCs(0);
	W5500.SendByte(offset>>8);//д16λ��ַ
	W5500.SendByte(offset);
	W5500.SendByte(VDM|RWB_WRITE|((s->socketn<<5)+0x10));

	if((offset+len)<S_TX_SIZE)//�������ַδ����W5500���ͻ������Ĵ���������ַ
	{
		for(i=0;i<len;i++)
			W5500.SendByte(buf[i]);		
	}
	else//�������ַ����W5500���ͻ������Ĵ���������ַ
	{
		offset=S_TX_SIZE-offset;
		for(i=0;i<offset;i++)
		W5500.SendByte(buf[i]);
		W5500.SetCs(1);

		W5500.SetCs(0);         //��ַ������ת
		W5500.SendByte(0x00);     //д16λ��ַ
		W5500.SendByte(0x00);
		W5500.SendByte(VDM|RWB_WRITE|((s->socketn<<5)+0x10));
		for(;i<len;i++)
			W5500.SendByte(buf[i]);
	}
	W5500.SetCs(1);
	offset1+=len;//����ʵ�������ַ,���´�д���������ݵ��������ݻ���������ʼ��ַ
	WriteSocket2Byte(s->socketn, Sn_TX_WR, offset1);
	WriteSocket1Byte(s->socketn, Sn_CR, SEND);//����������������
}
/*
 �������ݻ��������յ����ݳ���
 */

u16 ReadSocketData(W5500SOCKET* s,u8 *buf)
{
	u16 rx_size,offset,offset1,i;
	
	if(s == NULL) return 0;
	
	rx_size = ReadSocket2Byte(s->socketn,Sn_RX_RSR);
	
	if(rx_size == 0) 
		return 0;//û���յ������򷵻�
	if(rx_size > 1460) 
		rx_size = 1460;

	offset = ReadSocket2Byte(s->socketn,Sn_RX_RD);
	offset1 = offset;
	offset &= (S_RX_SIZE-1);//����ʵ�ʵ������ַ

	W5500.SetCs(0);
	W5500.SendByte(offset >> 8);//д16λ��ַ
	W5500.SendByte(offset);
	W5500.SendByte(VDM|RWB_READ|((s->socketn<<5)+0x18));//д�����ֽ�,N���ֽ����ݳ���,������,ѡ��˿�s�ļĴ���
	if((offset + rx_size) < S_RX_SIZE)//�������ַδ����W5500���ջ������Ĵ���������ַ
	{
		for(i = 0;i < rx_size;i ++)
			buf[i] = W5500.SendByte(0x00);
	}
	else//�������ַ����W5500���ջ������Ĵ���������ַ
	{
		offset = S_RX_SIZE - offset;
		for(i = 0;i < offset;i ++)
			buf[i] = W5500.SendByte(0x00);
		W5500.SetCs(1);

		W5500.SetCs(0);
		W5500.SendByte(0x00);//д��ת�ĵ�ַ
		W5500.SendByte(0x00);
		W5500.SendByte(VDM|RWB_READ|((s->socketn<<5)+0x18));
		for(;i < rx_size;i ++)
			buf[i] = W5500.SendByte(0x00);
	}
		W5500.SetCs(1);
	offset1 += rx_size;//����ʵ�������ַ,���´ζ�ȡ���յ������ݵ���ʼ��ַ
	WriteSocket2Byte(s->socketn, Sn_RX_RD, offset1);
	WriteSocket1Byte(s->socketn, Sn_CR, RECV);          //������ձ�־λ
	return rx_size;
}

void SendSocketData(W5500SOCKET* s,u8 *buf,u16 len)
{
	u16 offset,offset1,i;
	if(s == NULL) return;
	
	offset = ReadSocket2Byte(s->socketn,Sn_TX_WR);
	offset1 = offset;
	offset &= (S_TX_SIZE-1);//����ʵ�ʵ������ַ

	W5500.SetCs(0);
	W5500.SendByte(offset>>8);//д16λ��ַ
	W5500.SendByte(offset);
	W5500.SendByte(VDM|RWB_WRITE|((s->socketn<<5)+0x10));

	if((offset+len)<S_TX_SIZE)//�������ַδ����W5500���ͻ������Ĵ���������ַ
	{
		for(i=0;i<len;i++)
			W5500.SendByte(buf[i]);		
	}
	else//�������ַ����W5500���ͻ������Ĵ���������ַ
	{
		offset=S_TX_SIZE-offset;
		for(i=0;i<offset;i++)
			W5500.SendByte(buf[i]);
		W5500.SetCs(1);
		W5500.SetCs(0);         //��ַ������ת
		W5500.SendByte(0x00);     //д16λ��ַ
		W5500.SendByte(0x00);
		W5500.SendByte(VDM|RWB_WRITE|((s->socketn<<5)+0x10));
		for(;i<len;i++)
			W5500.SendByte(buf[i]);
	}
	W5500.SetCs(1);
	offset1+=len;//����ʵ�������ַ,���´�д���������ݵ��������ݻ���������ʼ��ַ
	WriteSocket2Byte(s->socketn, Sn_TX_WR, offset1);
	WriteSocket1Byte(s->socketn, Sn_CR, SEND);//����������������
}

void DisableSocket(W5500SOCKET* s)
{
	Net.IntFlag &= ~(1 << s->socketn);
	WriteReg1Byte(SIMR,Net.IntFlag);//�رն�Ӧsocket�ж�
	WriteSocket1Byte(s->socketn, Sn_CR, CLOSENET);
	s->Mode = MODE_IDLE;
}

void CloseAllSocket(void)
{
	u8 i;
	for(i=0;i<MAXSOCKET;i++)
	{
		if(Net.Sockets[i].Mode != MODE_IDLE)
		{
			DisableSocket(&Net.Sockets[i]);
		}
	}
}

void ReCreatSocket(W5500SOCKET* s)
{
	DisableSocket(s);
	newTcpServer(s->localPort,s->TcpSRecv);
}


// �����жϴ��������жϷ�������Щ�ж�
void NetIntProcess(void)
{
	u8 flag;
	if(Net.CheckIntIo())
		return;
	do
	{
		flag = ReadReg1Byte(IR);   // IR �жϼĴ���,���ж���IP��ַ��ͻ����Ŀ�겻�ɵִ�
		if(flag)
		{
			WriteReg1Byte(IR,(flag & 0xf0));
			if((flag & CONFLICT) == CONFLICT)   //IP ��ͻ
				Net.Status = NET_INIT;
			if((flag & UNREACH) == UNREACH)   //Ŀ�겻�ɴ�
				Net.Status = NET_INIT;
		}
		flag = ReadReg1Byte(SIR);        //��ȡsocket�˿��жϼĴ�����������socket0---7��һ���ж�
		
		Net.SocketSelect = GetIntSocket(flag);
		if(Net.SocketSelect == NULL)
			return;
		
		flag = ReadSocket1Byte(Net.SocketSelect->socketn,Sn_IR);//��ȡSocket0�жϱ�־�Ĵ���
		WriteSocket1Byte(Net.SocketSelect->socketn,Sn_IR,flag);//����жϱ�־
		
		if(flag & IR_CON)         // �˿ں����ӳɹ�
		{
			switch(Net.SocketSelect->Mode)
			{
				case MODE_TCPSLINK://�пͻ������ӵ����ط�����socket
					Net.SocketSelect->Mode = MODE_TCPSERVER;
					ReadSocket4Byte(Net.SocketSelect->socketn,Sn_DIPR,Net.SocketSelect->remoteIP);//��ȡ�ͻ���IP��ַ
					Net.SocketSelect->remotePort = ReadSocket2Byte(Net.SocketSelect->socketn,Sn_DPORTR);//��ȡ�ͻ��˶˿ں�
				break;
				
				case MODE_TCPCLINK:
					Net.SocketSelect->Mode = MODE_TCPCLIENT;
				break;
				
				default:
					break;
			}
		 }
		
		if((flag & IR_DISCON) || (flag & IR_TIMEOUT)) // ���ӶϿ����߷��ͳ�ʱ
		{
			WriteSocket1Byte(Net.SocketSelect->socketn,Sn_CR,CLOSENET);
			switch(Net.SocketSelect->Mode)
			{

				case MODE_TCPSLINK:
				case MODE_TCPSERVER:
					DisableSocket(Net.SocketSelect);
					newTcpServer(Net.SocketSelect->localPort,Net.SocketSelect->TcpSRecv);
				break;
				
				case MODE_UDP:
					DisableSocket(Net.SocketSelect);
					newUdpSocket(Net.SocketSelect->localPort,Net.SocketSelect->UdpRecv);
				break;
				case MODE_TCPCLINK:
				case MODE_TCPCLIENT:
					DisableSocket(Net.SocketSelect);
					break;
				
				default:
				break;
			}
		}
		if(flag & IR_RECV)         //Socket���յ�����
		{
			switch(Net.SocketSelect->Mode)
			{
				case MODE_TCPSERVER:
					if((Net.SocketSelect->Len = ReadSocketData(Net.SocketSelect,Net.SocketSelect->Buf))>0)
						Net.SocketSelect->TcpSRecv(Net.SocketSelect->Buf,Net.SocketSelect->Len);	
				break;
				
				case MODE_UDP:
					if((Net.SocketSelect->Len = ReadSocketData(Net.SocketSelect,Net.SocketSelect->Buf))>0)
					{
						Net.SocketSelect->remoteIP[0] = Net.SocketSelect->Buf[0];
						Net.SocketSelect->remoteIP[1] = Net.SocketSelect->Buf[1];
						Net.SocketSelect->remoteIP[2] = Net.SocketSelect->Buf[2];
						Net.SocketSelect->remoteIP[3] = Net.SocketSelect->Buf[3];
						Net.SocketSelect->remotePort = Net.SocketSelect->Buf[4];
						Net.SocketSelect->remotePort <<= 8;
						Net.SocketSelect->remotePort += Net.SocketSelect->Buf[5];
						Net.SocketSelect->Len = Net.SocketSelect->Buf[6];
						Net.SocketSelect->Len <<= 8;
						Net.SocketSelect->Len += Net.SocketSelect->Buf[7];
						Net.SocketSelect->UdpRecv(&Net.SocketSelect->Buf[8],Net.SocketSelect->Len);
					}
				break;
				
				case MODE_TCPCLIENT:
					if((Net.SocketSelect->Len = ReadSocketData(Net.SocketSelect,Net.SocketSelect->Buf))>0)
						Net.SocketSelect->TcpCRecv(Net.SocketSelect->Buf,Net.SocketSelect->Len);
				break;
				
				default:
					break;
			}
		}
	}while(ReadReg1Byte(SIR) != 0);    //������������ж�
}

