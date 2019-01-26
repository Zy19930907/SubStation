#include "Can.h"

_Can Can;

void Mcp2515Init(u8 canx);

u32 CanBaudValue(u32 Pclk,u16 Baud)
{
	u32 BtrValue = 0;
	BtrValue += 0x00;
	BtrValue <<= 4;			//��ͬ����Ծ���1tq
	BtrValue += 0x07;
	BtrValue <<= 4;			//ʱ���2 8tq
	BtrValue += 0x07;
	BtrValue <<= 16;		//ʱ���1 8tq
	BtrValue += (((((u32)Pclk*1000000)/17/Baud) -1) & 0x000003FF);
	return BtrValue;
}

void Can1Init(void)
{
	u8 i;
	RCC->AHB1ENR|=1<<0;  	//ʹ��PORTA��ʱ�� 
	GPIOA->MODER &= ~(0x0F << 22);//����PA11��PA12ģʽ����
	GPIOA->MODER |= 0x0A << 22;//PA11��PA12���ù���ģʽ
	GPIOA->OSPEEDR |= 0x0F << 22;//�������80MHz
	GPIOA->PUPDR |= 0x05 << 22;//����
	GPIOA->AFR[1] |= 0x99 << 12;//PA11��PA12���ù���ѡ��CAN
	
	RCC->APB1RSTR |= 1 << 25;//CANʱ�Ӹ�λ
	RCC->APB1RSTR &= ~(1 << 25);//ֹͣ��λ
	RCC->APB1ENR  |= 1 << 25;//CANʱ��ʹ��
    CAN1->MCR |= (1 << 0); //�������CAN����ģʽ
	while((CAN1->MSR & 0x00000001) != 0x00000001)
	{
		if(i++>200)
			return;
	}//�ȴ�����CAN����ģʽ
	i=0;
	CAN1->MCR &= ~(1<<1);//�˳�˯��ģʽ
	
	CAN1->BTR = CanBaudValue(SYSMAINCLOCK/4,CANBAUD);//������
     
	CAN1->FMR |= 1<<0;//����������ģʽ
	
	CAN1->FS1R |= 0x00000003;//��������1��2λ��32λ
	CAN1->FM1R &= ~0x00000003;//��������1��2����λģʽ
	CAN1->FFA1R &= ~0x00000001;
	CAN1->FFA1R |=0x00000002;//��������1����FIFO0����������2����FIFO1
		
	CAN1->sFilterRegister[0].FR1 = 0x00000000;
	CAN1->sFilterRegister[0].FR2 = 0x00000000;
	
	CAN1->sFilterRegister[1].FR1 = 0x00000000;
	CAN1->sFilterRegister[1].FR2 = 0x00000000;
	CAN1->FA1R |= 0x00000003;//���������1��2
	CAN1->FMR &= 0<<0;
	CAN1->MCR |= (1<<6);//����ģʽ�Զ��ָ�
	CAN1->MCR &= ~(1 << 0); //�������CAN����ģʽ
	delay_ms(5);
	while((CAN1->MSR & 0x00000001) != 0x00000000)
	{
		if(i++>200)
			return;
	}//�ȴ�����CAN����ģʽ
}

void Can2Init(void)
{
	u16 i;
	//CAN���ų�ʼ��
	RCC->AHB1ENR|=1<<1;  	//ʹ��PB��ʱ�� 
	GPIOB->MODER &= ~(0x0F << 10);//����PB5��PB6ģʽ����
	GPIOB->MODER |= 0x0A << 10;//PB5��PB6���ù���ģʽ
	GPIOB->OSPEEDR |= 0x0F << 10;//�������80MHz
	GPIOB->PUPDR |= 0x05 << 10;//����
	GPIOB->AFR[0] |= 0x99 << 20;//PB5��PB6���ù���ѡ��CAN
	
	RCC->APB1RSTR |= 1 << 26;//CANʱ�Ӹ�λ
	RCC->APB1RSTR &= ~(1 << 26);
	RCC->APB1ENR  |= 1 << 26;//CANʱ��ʹ��
	delay_ms(1);
	CAN2->MCR |= (1 << 0); //�������CAN����ģʽ
	while((CAN2->MSR & 0x00000001) != 0x00000001)
	{
		if(i++>200)
			return;
	}//�ȴ�����CAN����ģʽ
	i=0;
	
	CAN2->MCR &= ~(1<<1);//�˳�˯��ģʽ
	CAN2->BTR = CanBaudValue(SYSMAINCLOCK/4,CANBAUD);//������
     
	CAN1->FMR |= 1<<0;//����������ģʽ
	delay_ms(1);
	CAN1->FMR |= (14 << 8);
	CAN1->FS1R |= (0x03 << 14);//��������1��2λ��32λ
	CAN1->FM1R &= ~(0x03 << 14);//��������1��2����λģʽ
	CAN1->FFA1R |= (0x01 << 15);//��������1����FIFO0����������2����FIFO1
	
	//������������
	CAN1->sFilterRegister[14].FR1 = 0;
	CAN1->sFilterRegister[14].FR2 =	0;
	CAN1->sFilterRegister[15].FR1 = 0;
	CAN1->sFilterRegister[15].FR2 = 0;
	
	CAN1->FA1R |= (0x03 << 14);//���������14��15
	CAN1->FMR &= ~(0x01 << 0);//�˳�����������ģʽ
	
	CAN2->MCR |= (1<<6);//����ģʽ�Զ��ָ�
	CAN2->MCR &= ~(1 << 0); //�������CAN����ģʽ
	delay_ms(1);
	while((CAN2->MSR & 0x00000001) != 0x00000000)
	{
		if(i++ > 500)
			return;
	}//�ȴ�����CAN����ģʽ
}

void CanSpiInit(void)
{
	EXCANIOCLKEN;
	//EXCAN---MOSI
	IoInit(EXCANMOSIPORT,EXCANMOSIPIN,CANIOMODEOUT);
	//EXCAN---MISO
	IoInit(EXCANMISOPORT,EXCANMISOPIN,CANIOMODEIN);
	//EXCAN---SCK
	IoInit(EXCANSCKPORT,EXCANSCKPIN,CANIOMODEOUT);
}

void Can3IoInit(void)
{
	CanSpiInit();
	IoInit(CAN3CSPORT,CAN3CSPIN,CANIOMODEOUT);
	SetIoValue(CAN3CSPORT,CAN3CSPIN,CANIOVALUEHIGH);//Ƭѡ���߲�ѡ��
	Mcp2515Init(CAN_3);
}

void Can4IoInit(void)
{
	CanSpiInit();
	IoInit(CAN4CSPORT,CAN4CSPIN,CANIOMODEOUT);
	SetIoValue(CAN4CSPORT,CAN4CSPIN,CANIOVALUEHIGH);//Ƭѡ���߲�ѡ��
	Mcp2515Init(CAN_4);
}

void Can5IoInit(void)
{
	CanSpiInit();
	IoInit(CAN5CSPORT,CAN5CSPIN,CANIOMODEOUT);
	SetIoValue(CAN5CSPORT,CAN5CSPIN,CANIOVALUEHIGH);//Ƭѡ���߲�ѡ��
	Mcp2515Init(CAN_5);
}

void Can6IoInit(void)
{
	CanSpiInit();
	IoInit(CAN6CSPORT,CAN6CSPIN,CANIOMODEOUT);
	SetIoValue(CAN6CSPORT,CAN6CSPIN,CANIOVALUEHIGH);//Ƭѡ���߲�ѡ��
	Mcp2515Init(CAN_6);
}

u32 CanIdSwitch(u32 CanId, u8 SwitchDir)
{
	u32 temp = 0;
	switch (SwitchDir)
	{
	case CANIDSWITCH_B2F:
		CanId >>= 3;
		temp = (CanId >> 18)&0x000003FF;
		temp |= (CanId << 11)&0x1FFFF800;
		break;
	case CANIDSWITCH_F2B:
		temp = ((CanId & 0x000007FF) << 18);
		temp |= ((CanId & 0x7FFFF800) >> 11);
		temp <<= 3; 
		temp|= 0x00000004;
		break;
	}
	return temp;
}

// ������0�̶����ڷ��ʹ�����ʵʱ���ݣ�������1,2���ڻظ���Ϣ
u8 CheckCan1TxBuf(void)
{
     u8 t;
     t = 0xFF;
     //if((CAN1->TSR >> 26) & 0x01)   //���ͻ�����1��
     //{
         // t = 0;
         // return t;
     //}
     if((CAN1->TSR >> 27) & 0x01)   //���ͻ�����2��
     {
          t = 1;
          return t;
     }
     if((CAN1->TSR >> 28) & 0x01)   //���ͻ�����3��
     {
          t = 2;
          return t;
     }
     return t;
}

// ������0�̶����ڷ��ʹ�����ʵʱ���ݣ�������1,2���ڻظ���Ϣ
u8 CheckCan2TxBuf(void)
{
     u8 t;
     t = 0xFF;
     //if((CAN1->TSR >> 26) & 0x01)   //���ͻ�����1��
     //{
         // t = 0;
         // return t;
     //}
     if((CAN1->TSR >> 27) & 0x01)   //���ͻ�����2��
     {
          t = 1;
          return t;
     }
     if((CAN1->TSR >> 28) & 0x01)   //���ͻ�����3��
     {
          t = 2;
          return t;
     }
     return t;
}

u8 SendDataCan1(u32 ID,u8 *buf,u8 len)
{
      u8 i;
	i= CheckCan1TxBuf();
	if(i > 2)
		return 0xFF;

	CAN1->sTxMailBox[i].TIR = CanIdSwitch(ID,CANIDSWITCH_F2B);
	CAN1->sTxMailBox[i].TDTR = len;
	CAN1->sTxMailBox[i].TDHR=(((u32)buf[7]<<24)|
                                ((u32)buf[6]<<16)|
                                ((u32)buf[5]<<8)|
                                ((u32)buf[4]));
	CAN1->sTxMailBox[i].TDLR=(((u32)buf[3]<<24)|
                                ((u32)buf[2]<<16)|
                                ((u32)buf[1]<<8)|
                                ((u32)buf[0]));
	CAN1->sTxMailBox[i].TIR |= 0x00000001; //��������
	return i;
}


u8 SendDataCan2(u32 ID,u8 *buf,u8 len)
{
      u8 i;
	i= CheckCan2TxBuf();
	if(i > 2)
		return 0xFF;

	CAN2->sTxMailBox[i].TIR = CanIdSwitch(ID,CANIDSWITCH_F2B);
	CAN2->sTxMailBox[i].TDTR = len;
	CAN2->sTxMailBox[i].TDHR=(((u32)buf[7]<<24)|
                                ((u32)buf[6]<<16)|
                                ((u32)buf[5]<<8)|
                                ((u32)buf[4]));
	CAN2->sTxMailBox[i].TDLR=(((u32)buf[3]<<24)|
                                ((u32)buf[2]<<16)|
                                ((u32)buf[1]<<8)|
                                ((u32)buf[0]));
	CAN2->sTxMailBox[i].TIR |= 0x00000001; //��������
	return i;
}

void SetCanxCs(u8 canx,u8 val)
{
	switch(canx)
	{
		case CAN_3:SETCAN3CS(val);break;
		case CAN_4:SETCAN4CS(val);break;
		case CAN_5:SETCAN5CS(val);break;
		case CAN_6:SETCAN6CS(val);break;
	}
}

u8 SendByteCan(u8 index,u8 val)
{
	unsigned char i, dat;
	dat = 0;
	SETCANSCK(0);
	for(i = 0; i < 8; i ++)
	{
		SETCANMOSI((val & 0x80));
		val <<= 1;
		SETCANSCK(1);
		dat <<= 1;
		if(GETCANMISO)
			dat++; 
		SETCANSCK(0);
	}
	return dat;
}

void WriteRegCan(u8 canx,u8 addr, u8 value)
{
	SetCanxCs(canx,0);
	SendByteCan(canx,CAN_WRITE);
	SendByteCan(canx,addr);
	SendByteCan(canx,value);
	SetCanxCs(canx,1);
}

void MCP2515Reset(u8 canx)
{
	SetCanxCs(canx,0);
    SendByteCan(canx,CAN_RESET);
	SetCanxCs(canx,1);
}

void Mcp2515Init(u8 canx)
{
     u16 tick;
     MCP2515Reset(canx);
     delay_ms(10);
// ���������¸�λ�����ϵ��ʱ�򣬶����Զ���������ģʽ���˾����ʡ��     
     WriteRegCan(canx,CANCTRL,CONFIG_MODE);    // CANCTRL �Ĵ���������Ϊ����ģʽ
     
     WriteRegCan(canx,CNF1,0x18);      // ���ò����ʣ�10K
//	 WriteRegCan(canx,CNF1,0x31);      // ���ò����ʣ�5K
     WriteRegCan(canx,CNF2,0xA4);
     WriteRegCan(canx,CNF3, 0x04);
    
     WriteRegCan(canx,CANINTE,0x00);        // �ж�ʹ�ܼĴ�����CANINTE,3�����ͻ�����+2�����ջ�����
     WriteRegCan(canx,CANINTF,0x00);
     WriteRegCan(canx,RXB0CTRL,0x40);       // ������չ֡����ֹ�����洢
     WriteRegCan(canx,RXB1CTRL,0x40);       // ֻ������չ֡
     // ֻ��עEID3
     WriteRegCan(canx,RXM0EIDH,0x00);  
     WriteRegCan(canx,RXM0EIDL,0x08);  
     WriteRegCan(canx,RXM0SIDH,0x00);  
     WriteRegCan(canx,RXM0SIDL,0x00);  
     
     WriteRegCan(canx,RXF0EIDH,0x00);  
     WriteRegCan(canx,RXF0EIDL,0x08);  
     WriteRegCan(canx,RXF0SIDH,0x00);  
     WriteRegCan(canx,RXF0SIDL,0x08);  
     
     WriteRegCan(canx,TXRTSCTRL,0);
     WriteRegCan(canx,BFPCTRL,0);       //BFPCTRL_RXnBF ���ſ��ƼĴ�����״̬�Ĵ���
     WriteRegCan(canx,CANCTRL,NORMAL_MODE);    //����ģʽ
}

u8 ReadRegCan(u8 canx,u8 addr)
{
	u8 value;
	SetCanxCs(canx,0);
	SendByteCan(canx,CAN_READ);
	SendByteCan(canx,addr);
	value = SendByteCan(canx,0x00); 
	SetCanxCs(canx,1);
	return value;
}

void ReadBurstRegCan(u8 canx,u8 addr,u8 *buf,u8 len)
{
	u8 i;
	SetCanxCs(canx,0);
	SendByteCan(canx,CAN_READ);
	SendByteCan(canx,addr);
	for(i = 0;i < len;i ++)
		buf[i] = SendByteCan(canx,0x00);
	SetCanxCs(canx,0);
}

u8 CheckCanTxBufMcp2515(u8 canx)
{
     u8 flag;
     flag = ReadRegCan(canx,TXB0CTRL);
     if(!(flag & 0x08))
          return 0;
     flag = ReadRegCan(canx,TXB1CTRL);
     if(!(flag & 0x08))
          return 1;
     flag = ReadRegCan(canx,TXB2CTRL);
     if(!(flag & 0x08))
          return 2;
     return 0xFF;
}

void WriteBurstRegCan(u8 canx,u8 addr, u8 *buf, u8 len)
{
	u8 i;
	SetCanxCs(canx,0);
	SendByteCan(canx,CAN_WRITE);
	SendByteCan(canx,addr);
	for(i = 0;i < len;i ++)
		SendByteCan(canx,buf[i]);
	SetCanxCs(canx,1);
}

void ModifyReg(u8 canx,u8 addr,u8 mask,u8 val)
{
	SetCanxCs(canx,0);
	SendByteCan(canx,CAN_BIT_MODIFY);
	SendByteCan(canx,addr);
	SendByteCan(canx,mask);
	SendByteCan(canx,val);
	SetCanxCs(canx,1);
}

void SendDataCanEx(u8 index,u32 ID,u8 *buf,u8 len)
{
	u8 t,RegShift;
	t = CheckCanTxBufMcp2515(index);
	if(t == 0xFF)
		return;
	switch(t)
	{
		case 0: RegShift = 0x00;break;
		case 1: RegShift = 0x10;break;
		case 2: RegShift = 0x20;break;
	}
	WriteRegCan(index,TXB0EIDH+RegShift, ID>>19);
	WriteRegCan(index,TXB0EIDL+RegShift, ID>>11);
	WriteRegCan(index,TXB0SIDH+RegShift, ID>>3);
	WriteRegCan(index,TXB0SIDL+RegShift, (ID<<5)+0x08+(ID >> 27));
	WriteRegCan(index,TXB0DLC+RegShift, len); //?????0?????
	WriteBurstRegCan(index,TXB0D0+RegShift,buf,len);
	ModifyReg(index,TXB0CTRL+RegShift,0x08,0x08);
}

u8 ReadStatus(u8 canx)
{
	u8 temp;
	SetCanxCs(canx,0);
	SendByteCan(canx,CAN_RD_STATUS);
	temp = SendByteCan(canx,0x00);
	SetCanxCs(canx,1);
	return temp;
}
void Can1RecvFunc(void)
{
	if(CAN1->RF0R & 0x00000003)
     {
		do{
			Can.Len = (CAN1->sFIFOMailBox[0].RDTR & 0x0000000F);
			Can.ID = (u32)0x1FFFFFFF & (CAN1->sFIFOMailBox[0].RIR >> 3);
			Can.ID = (Can.ID >> 18) + ((Can.ID&0x3FFFF)<<11);
			Can.Buf[0] = CAN1->sFIFOMailBox[0].RDLR&0xFF;
			Can.Buf[1] = (CAN1->sFIFOMailBox[0].RDLR>>8)&0xFF;
			Can.Buf[2] = (CAN1->sFIFOMailBox[0].RDLR>>16)&0xFF;
			Can.Buf[3] = (CAN1->sFIFOMailBox[0].RDLR>>24)&0xFF;    
			Can.Buf[4] = CAN1->sFIFOMailBox[0].RDHR&0xFF;
			Can.Buf[5] = (CAN1->sFIFOMailBox[0].RDHR>>8)&0xFF;
			Can.Buf[6] = (CAN1->sFIFOMailBox[0].RDHR>>16)&0xFF;
			Can.Buf[7] = (CAN1->sFIFOMailBox[0].RDHR>>24)&0xFF;
			HandleCanData(Can.ID,CAN_1);			
			CAN1->RF0R |= (1 << 5);//�ͷ�FIFO0
			while(CAN1->RF0R  & 0x00000020);
		}while(CAN1->RF0R & 0x00000003);
		if(CAN1->RF0R & 0x00000010)
			CAN1->RF0R &= ~(1 << 4);
		if(CAN1->RF0R & 0x00000008)
			CAN1->RF0R &= ~(1 << 3);
     }
     if(CAN1->RF1R & 0x00000003)
     {
		do{
			Can.Len = (CAN1->sFIFOMailBox[1].RDTR & 0x0000000F);
			Can.ID = (u32)(0x1FFFFFFF & (CAN1->sFIFOMailBox[1].RIR >> 3));
			Can.ID = (Can.ID >> 18) + ((Can.ID&0x3FFFF)<<11);
			Can.Buf[0] = CAN1->sFIFOMailBox[1].RDLR&0xFF;
			Can.Buf[1] = (CAN1->sFIFOMailBox[1].RDLR>>8)&0xFF;
			Can.Buf[2] = (CAN1->sFIFOMailBox[1].RDLR>>16)&0xFF;
			Can.Buf[3] = (CAN1->sFIFOMailBox[1].RDLR>>24)&0xFF;    
			Can.Buf[4] = CAN1->sFIFOMailBox[1].RDHR&0xFF;
			Can.Buf[5] = (CAN1->sFIFOMailBox[1].RDHR>>8)&0xFF;
			Can.Buf[6] = (CAN1->sFIFOMailBox[1].RDHR>>16)&0xFF;
			Can.Buf[7] = (CAN1->sFIFOMailBox[1].RDHR>>24)&0xFF;
			HandleCanData(Can.ID,CAN_1);			
			CAN1->RF1R |= (1 << 5);//�ͷ�FIFO1
			while(CAN1->RF1R & 0x00000020);
		}while(CAN1->RF1R & 0x00000003);
		if(CAN1->RF1R & 0x00000010)
			CAN1->RF1R &= ~(1 << 4);
		if(CAN1->RF1R & 0x00000008)
			CAN1->RF1R &= ~(1 << 3);
     }
}

void Can2RecvFunc(void)
{
	if(CAN2->RF0R & 0x00000003)
     {
		do{
			Can.Len = (CAN2->sFIFOMailBox[0].RDTR & 0x0000000F);
			Can.ID = (u32)0x1FFFFFFF & (CAN2->sFIFOMailBox[0].RIR >> 3);
			Can.ID = (Can.ID >> 18) + ((Can.ID&0x3FFFF)<<11);
			Can.Buf[0] = CAN2->sFIFOMailBox[0].RDLR&0xFF;
			Can.Buf[1] = (CAN2->sFIFOMailBox[0].RDLR>>8)&0xFF;
			Can.Buf[2] = (CAN2->sFIFOMailBox[0].RDLR>>16)&0xFF;
			Can.Buf[3] = (CAN2->sFIFOMailBox[0].RDLR>>24)&0xFF;    
			Can.Buf[4] = CAN2->sFIFOMailBox[0].RDHR&0xFF;
			Can.Buf[5] = (CAN2->sFIFOMailBox[0].RDHR>>8)&0xFF;
			Can.Buf[6] = (CAN2->sFIFOMailBox[0].RDHR>>16)&0xFF;
			Can.Buf[7] = (CAN2->sFIFOMailBox[0].RDHR>>24)&0xFF;		
			HandleCanData(Can.ID,CAN_2);			
			CAN2->RF0R |= (1 << 5);//�ͷ�FIFO0
			while(CAN2->RF0R  & 0x00000020);
		}while(CAN2->RF0R & 0x00000003);
		if(CAN2->RF0R & 0x00000010)
			CAN2->RF0R &= ~(1 << 4);
		if(CAN2->RF0R & 0x00000008)
			CAN2->RF0R &= ~(1 << 3);
     }
     if(CAN2->RF1R & 0x00000003)
     {
		do{
			Can.Len = (CAN2->sFIFOMailBox[1].RDTR & 0x0000000F);
			Can.ID = (u32)(0x1FFFFFFF & (CAN2->sFIFOMailBox[1].RIR >> 3));
			Can.ID = (Can.ID >> 18) + ((Can.ID&0x3FFFF)<<11);
			Can.Buf[0] = CAN2->sFIFOMailBox[1].RDLR&0xFF;
			Can.Buf[1] = (CAN2->sFIFOMailBox[1].RDLR>>8)&0xFF;
			Can.Buf[2] = (CAN2->sFIFOMailBox[1].RDLR>>16)&0xFF;
			Can.Buf[3] = (CAN2->sFIFOMailBox[1].RDLR>>24)&0xFF;    
			Can.Buf[4] = CAN2->sFIFOMailBox[1].RDHR&0xFF;
			Can.Buf[5] = (CAN2->sFIFOMailBox[1].RDHR>>8)&0xFF;
			Can.Buf[6] = (CAN2->sFIFOMailBox[1].RDHR>>16)&0xFF;
			Can.Buf[7] = (CAN2->sFIFOMailBox[1].RDHR>>24)&0xFF; 
			HandleCanData(Can.ID,CAN_2);
			CAN2->RF1R |= (1 << 5);//�ͷ�FIFO1
			while(CAN2->RF1R & 0x00000020);
		}while(CAN2->RF1R & 0x00000003);
		if(CAN2->RF1R & 0x00000010)
			CAN2->RF1R &= ~(1 << 4);
		if(CAN2->RF1R & 0x00000008)
			CAN2->RF1R &= ~(1 << 3);
     }
}

void CanMcp2515RecFunc(u8 canx)
{
     u8 flag;
     flag = ReadRegCan(canx,CANINTF);
     if(flag & 0x01)
     {
		Can.Len = ReadRegCan(canx,RXB0DLC);
		if(Can.Len >= 8)
			Can.Len = 8;
		Can.ID = (ReadRegCan(canx,RXB0SIDL) & 0x03);
		Can.ID <<=8;
		Can.ID += ReadRegCan(canx,RXB0EIDH);
		Can.ID <<= 8;
		Can.ID += ReadRegCan(canx,RXB0EIDL);
		Can.ID <<= 8;
		Can.ID += ReadRegCan(canx,RXB0SIDH);
		Can.ID <<= 3;
		Can.ID += (ReadRegCan(canx,RXB0SIDL) >> 5);
		ReadBurstRegCan(canx,RXB0D0,Can.Buf,Can.Len);
		HandleCanData(Can.ID,canx);
		ModifyReg(canx,CANINTF,0x21,0x00);
    }
    if(flag & 0x02)
    {
		Can.Len = ReadRegCan(canx,RXB0DLC);
		if(Can.Len >= 8)
			Can.Len = 8;
		Can.ID = (ReadRegCan(canx,RXB1SIDL) & 0x03);
		Can.ID <<=8;
		Can.ID += ReadRegCan(canx,RXB1EIDH);
		Can.ID <<= 8;
		Can.ID += ReadRegCan(canx,RXB1EIDL);
		Can.ID <<= 8;
		Can.ID += ReadRegCan(canx,RXB1SIDH);
		Can.ID <<= 3;
		Can.ID += (ReadRegCan(canx,RXB1SIDL) >> 5);
		ReadBurstRegCan(canx,RXB1D0,Can.Buf,Can.Len);
		HandleCanData(Can.ID,canx);
		ModifyReg(canx,CANINTF,0x22,0x00);
    }
}

void CanxInit(u8 canx)
{
	switch(canx)
	{
		case CAN_1:
			Can1Init();
			break;
		case CAN_2:
			Can2Init();
			break;
		case CAN_3:
			Can3IoInit();
			break;
		case CAN_4:
			Can4IoInit();
			break;
		case CAN_5:
			Can5IoInit();
			break;
		case CAN_6:
			Can6IoInit();
			break;
	}
}

void CanSendData(u8 Canx,u32 ID,u8 *buf,u8 len)
{
	switch(Canx)
	{
		case CAN_1:
			SendDataCan1(ID,buf,len);
			break;
		case CAN_2:
			SendDataCan2(ID,buf,len);
			break;
		case CAN_3:
		case CAN_4:
		case CAN_5:
		case CAN_6:
			SendDataCanEx(Canx,ID,buf,len);
			break;
	}
}

void CanRecvData(u8 Canx)
{
	switch(Canx)
	{
		case CAN_1:
			Can1RecvFunc();
			break;
		case CAN_2:
			Can2RecvFunc();
			break;
		case CAN_3:
		case CAN_4:
		case CAN_5:
		case CAN_6:
			CanMcp2515RecFunc(Canx);
			break;
	}
}


