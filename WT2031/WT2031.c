#include "WT2031.h"

_WT2031 WT2031;

u8 pRateControlWord[] = {
	
//	0x7f,0xa0,0x00,0x00,0x00,0x00,0x00,0x00,0x52,0xa0,
	0x90,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x43,0x30, //2400  +  0     RATE_SEL[4:0]=00000
	0x93,0x48,0x00,0x00,0x00,0x00,0x00,0x00,0x6F,0x48, //3600  +  0     RATE_SEL[4:0]=00001
	0xAB,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x39,0x50, //4000  +  0     RATE_SEL[4:0]=01111
	0xAB,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x79,0x60, //4800  +  0     RATE_SEL[4:0]=00011
	0xBF,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x72,0xC0, //9600  +  0     RATE_SEL[4:0]=00100
	0x90,0x2F,0x00,0x00,0x00,0x00,0x00,0x00,0x69,0x30, //2350  +  50    RATE_SEL[4:0]=00101
	0x92,0x3E,0x28,0x00,0x00,0x00,0x00,0x00,0x74,0x60, //3100  +  1700  RATE_SEL[4:0]=01000
	0x92,0x43,0x00,0x80,0x00,0x00,0x00,0x00,0x53,0x48, //3350  +  250   RATE_SEL[4:0]=01011
	0x93,0x48,0x20,0x30,0x00,0x00,0x00,0x00,0x70,0x60, //3600  +  1200  RATE_SEL[4:0]=00010
	0x93,0x4B,0x00,0x80,0x00,0x00,0x00,0x00,0x39,0x50, //3750  +  250   RATE_SEL[4:0]=01110
	0xAB,0x53,0x2C,0x00,0x00,0x00,0x00,0x00,0x56,0x80, //4150  +  2250  RATE_SEL[4:0]=01010
	0xAB,0x58,0x30,0x00,0x00,0x00,0x00,0x00,0x44,0x90, //4400  +  2800  RATE_SEL[4:0]=01001
	0xAB,0x5B,0x00,0x80,0x00,0x00,0x00,0x00,0x68,0x60, //4550  +  250   RATE_SEL[4:0]=00111
	0xAB,0x5D,0x34,0x00,0x00,0x00,0x00,0x00,0x31,0xA0, //4650  +  3350  RATE_SEL[4:0]=01101
	0xAB,0x64,0xE4,0x00,0x00,0x00,0x00,0x00,0x67,0xC0, //4850  +  4750  RATE_SEL[4:0]=00110
	0xBF,0x9B,0x00,0x80,0x00,0x00,0x00,0x00,0x49,0xA0, //7750  +  250   RATE_SEL[4:0]=01100
};//4850  +  4750  RATE_SEL[4:0]=00110

void WT2031Init(void)
{
	RCC->AHB1ENR |= 0x05;//使能PA、PC时钟
	//RST
	WT2031_RSTPROT->MODER |= (0x01 << (WT2031_RSTPIN << 1));//RST引脚输出模式
	WT2031_RSTPROT->OSPEEDR |= (0x02<< (WT2031_RSTPIN << 1));//50MHz
	WT2031_RSTPROT->PUPDR |= (0x01 << (WT2031_RSTPIN << 1));//上拉输出
	//CODECRST
	WT2031_CODECRSTPROT->MODER |= (0x01 << (WT2031_CODECRSTPIN << 1));//RST引脚输出模式
	WT2031_CODECRSTPROT->OSPEEDR |= (0x02<< (WT2031_CODECRSTPIN << 1));//50MHz
	WT2031_CODECRSTPROT->PUPDR |= (0x01 << (WT2031_CODECRSTPIN << 1));//上拉输出
	//EPR
	WT2031_EPRPROT->OSPEEDR |= (0x02 << (WT2031_EPRPIN << 1));//50MHz
	//RX_CLK
	WT2031_RX_CLKPROT->MODER |= (0x01 << (WT2031_RX_CLKPIN << 1));//RX_CLK输出模式
	WT2031_RX_CLKPROT->OSPEEDR |= (0x02 << (WT2031_RX_CLKPIN << 1));//50MHz
	WT2031_RX_CLKPROT->PUPDR |= (0x01 << (WT2031_RX_CLKPIN << 1));//上拉输出
	//RX_STRB
	WT2031_RX_STRBPROT->MODER |= (0x01 << (WT2031_RX_STRBPIN << 1));//RX_CLK输出模式
	WT2031_RX_STRBPROT->OSPEEDR |= (0x02 << (WT2031_RX_STRBPIN << 1));//50MHz
	WT2031_RX_STRBPROT->PUPDR |= (0x01 << (WT2031_RX_STRBPIN << 1));//上拉输出
	//RX_DATA
	WT2031_RX_DATAPROT->MODER |= (0x01 << (WT2031_RX_DATAPIN << 1));//RX_CLK输出模式
	WT2031_RX_DATAPROT->OSPEEDR |= (0x02 << (WT2031_RX_DATAPIN << 1));//50MHz
	WT2031_RX_DATAPROT->PUPDR |= (0x01 << (WT2031_RX_DATAPIN << 1));//上拉输出
	
	//TX_CLK
	WT2031_TX_CLKPROT->MODER |= (0x01 << (WT2031_TX_CLKPIN << 1));//RX_CLK输出模式
	WT2031_TX_CLKPROT->OSPEEDR |= (0x02 << (WT2031_TX_CLKPIN << 1));//50MHz
	WT2031_TX_CLKPROT->PUPDR |= (0x01 << (WT2031_TX_CLKPIN << 1));//上拉输出
	//TX_STRB
	WT2031_TX_STRBPROT->MODER |= (0x01 << (WT2031_TX_STRBPIN << 1));//RX_CLK输出模式
	WT2031_TX_STRBPROT->OSPEEDR |= (0x02 << (WT2031_TX_STRBPIN << 1));//50MHz
	WT2031_TX_STRBPROT->PUPDR |= (0x01 << (WT2031_TX_STRBPIN << 1));//上拉输出
	//TX_DATA
	WT2031_TX_DATAPROT->MODER |= (0x01 << (WT2031_TX_DATAPIN << 1));//RX_CLK输出模式
	WT2031_TX_DATAPROT->OSPEEDR |= (0x02 << (WT2031_TX_DATAPIN << 1));//50MHz
	WT2031_TX_DATAPROT->PUPDR |= (0x01 << (WT2031_TX_DATAPIN << 1));//上拉输出
}


u8 WT2031STB(void)
{
	return WT2031_EPR_VALUE;
}

void LoadConfigInfo(u8 index)
{
	u8 i;
	WT2031.TxFram[0] = 0x13;
	WT2031.TxFram[1] = 0xEC;
	WT2031.TxFram[2] = 0x00;//Power Control
	WT2031.TxFram[3] = 0x00;//Control Word 1
	for(i=0;i<10;i++)
		WT2031.TxFram[4+i] = pRateControlWord[(index*10)+i];
	WT2031.TxFram[14] = 0x00;//Unused
	WT2031.TxFram[15] = 0x00;
	WT2031.TxFram[16] = 0x00;//Unused
	WT2031.TxFram[17] = 0x00;
	WT2031.TxFram[18] = 0x00;//Unused
	WT2031.TxFram[19] = 0x00;
	WT2031.TxFram[21] = 0xFF;
	WT2031.TxFram[22] = 80;
	WT2031.TxFram[23] = 0x20;//VAD=1,EC=0
}

void ResetWT2031(void)
{
	WT2031_CODECRST_SET(1);
	delay_ms(1);
	
	WT2031_CODECRST_SET(0);
	delay_ms(1);
	
	WT2031_CODECRST_SET(1);
	delay_ms(20);
	
	WT2031_RST_SET(1);
	delay_ms(1);
	
	WT2031_RST_SET(0);
	delay_ms(1);
	
	WT2031_RST_SET(1);
	delay_ms(98);
}

u8 WT_DATA_STB(void)
{
	u8 i;
	u16 Head = 0;
	
	delay_10ns(40);
	WT2031_TX_CLK_SET(1);
	delay_10ns(40);
	WT2031_TX_CLK_SET(0);
	delay_10ns(40);
	WT2031_TX_CLK_SET(1);
	delay_10ns(40);
	
	WT2031_TX_STRB_SET(1);
	delay_10ns(40);
	
	WT2031_TX_CLK_SET(0);
	delay_10ns(40);
	WT2031_TX_CLK_SET(1);
	delay_10ns(40);
	WT2031_TX_CLK_SET(0);
	delay_10ns(40);
	
	WT2031_TX_STRB_SET(0);
	
	for(i=0;i<16;i++)
	{
		WT2031_TX_CLK_SET(1);
		delay_10ns(40);
		Head <<= 1;
		Head += WT2031_TX_DATA_VALUE;	
		WT2031_TX_CLK_SET(0);
		delay_10ns(40);
	}

	if(Head == 0x13EC)
	{
		WT2031.RxFram[0] = Head >> 8;
		WT2031.RxFram[1] = Head;
		return 1;
	}
	return 0;
}

void WTReadData(u8 *dest)
{
	u8 i,j;
	u16 DATA = 0;
	for(j=0;j<23;j++)
	{
		delay_10ns(50);
		WT2031_TX_CLK_SET(1);
		delay_10ns(50);
		WT2031_TX_CLK_SET(0);
		delay_10ns(50);
		WT2031_TX_CLK_SET(1);
		delay_10ns(50);
		
		WT2031_TX_STRB_SET(1);
		delay_10ns(50);
		
		WT2031_TX_CLK_SET(0);
		delay_10ns(50);
		WT2031_TX_CLK_SET(1);
		delay_10ns(50);
		
		WT2031_TX_STRB_SET(0);
		delay_10ns(50);
		
		WT2031_TX_CLK_SET(0);
		delay_10ns(40);
	
		for(i=0;i<16;i++)
		{
			WT2031_TX_CLK_SET(1);
			delay_10ns(50);
			DATA <<= 1;
			DATA += WT2031_TX_DATA_VALUE;	
			WT2031_TX_CLK_SET(0);
			delay_10ns(50);
		}
		*(dest+2*j) = (u8)(DATA >> 8);
		*(dest+2*j+1) = (u8)(DATA & 0x00FF);
	}
}

void WT2KReadData(void)
{
	u8 i,j;
	u16 DATA = 0;
	
	delay_10ns(50);
	WT2031_TX_CLK_SET(1);
	delay_10ns(50);
	WT2031_TX_CLK_SET(0);
	delay_10ns(50);
	WT2031_TX_CLK_SET(1);
	delay_10ns(50);
	WT2031_TX_STRB_SET(1);
	delay_10ns(50);
	WT2031_TX_CLK_SET(0);
	delay_10ns(50);
	WT2031_TX_CLK_SET(1);
	delay_10ns(50);
		
	WT2031_TX_STRB_SET(0);
	delay_10ns(50);
		
	WT2031_TX_CLK_SET(0);
	delay_10ns(40);
	for(i=0;i<16;i++)
	{
			WT2031_TX_CLK_SET(1);
			delay_10ns(50);
			DATA <<= 1;
			DATA += WT2031_TX_DATA_VALUE;
			WT2031_TX_CLK_SET(0);
			delay_10ns(50);
	}
	for(j=0;j<24;j++)
	{
		delay_10ns(50);
		WT2031_TX_CLK_SET(1);
		delay_10ns(50);
		WT2031_TX_CLK_SET(0);
		delay_10ns(50);
		WT2031_TX_CLK_SET(1);
		delay_10ns(50);
		
		WT2031_TX_STRB_SET(1);
		delay_10ns(50);
		
		WT2031_TX_CLK_SET(0);
		delay_10ns(50);
		WT2031_TX_CLK_SET(1);
		delay_10ns(50);
		
		WT2031_TX_STRB_SET(0);
		delay_10ns(50);
		
		WT2031_TX_CLK_SET(0);
		delay_10ns(40);
	
		for(i=0;i<16;i++)
		{
			WT2031_TX_CLK_SET(1);
			delay_10ns(50);
			DATA <<= 1;
			DATA += WT2031_TX_DATA_VALUE;
			WT2031_TX_CLK_SET(0);
			delay_10ns(50);
		}
		WT2031.RxFram[2*j] = (u8)(DATA >> 8);
		WT2031.RxFram[2*j+1] = (u8)(DATA & 0x00FF);
	}
}

void SetVoiceData(u8 *buf ,u8 len)
{
	u8 i;
	SET_VS_EN(0);
	for(i=0;i<len;i++)
		WT2031.TxFram[24+i] = *(buf+i);
	WT2031_SendCmd();
}

void PlayTolkData(u8 *buf ,u8 len)
{
	u8 i;
	SET_VS_EN(0);
	for(i=0;i<len;i++)
		WT2031.TxFram[24+i] = *(buf+i);
	WT2031_SendCmd();
}

void WT2031_SendCmd(void)
{
	u8 i,j;
	u16 data;
	for(i=0;i<24;i++)
	{
		delay_10ns(50);
		WT2031_RX_CLK_SET(1);
		delay_10ns(50);
		
		WT2031_RX_STRB_SET(1);
		delay_10ns(50);	

		WT2031_RX_CLK_SET(0);
		delay_10ns(50);
		
		WT2031_RX_CLK_SET(1);
		delay_10ns(50);		
		
		WT2031_RX_STRB_SET(0);
		delay_10ns(50);
		
		data = WT2031.TxFram[2*i];
		data <<= 8;
		data += WT2031.TxFram[2*i+1];
		
		for(j=0;j<=15;j++)
		{
			WT2031_RX_CLK_SET(1);
			WT2031_RX_DATA_SET((data >> (15-j)) & 0x01);
			delay_10ns(50);
			WT2031_RX_CLK_SET(0);
			delay_10ns(50);
		}
	}
}

void WT2031RST(void)
{
	WT2031.Status = WTINIT;
}

void WT2031SetVolum(u8 volum)
{
	WT2031.TxFram[22] = volum;
}


void SendWT2031Data(void)
{
//	u8 i;
//	LED2CHANGE;
//	for(i=0;i<3;i++)
//	{
//		Can.ID = 0;
//		Can.ID |= i<<24;
//		Can.ID |= 0x01;
//		BufCopy(Can.Buf,&WT2031.RxFram[24+i*8],8);
//		CanSendData(Can.ID,Can.Buf,8);
//	}
	Can.ID = 0x01;
	CanSendData(CAN_1,Can.ID,&WT2031.RxFram[24],6);
}

u8 testVoice[2048];
u16 Cnt=0;
u16 PlayCnt=0;

void WT2031Pro(void)
{
	u8 i;
	switch(WT2031.Status)
	{
		case WTINIT:
			WT2031Init();
			ResetWT2031();
			WT2031.Status = WTRST;
			break;
		
		case WTRST:
			if(WT2031_EPR_VALUE)
				WT2031.Status = WTRDHD;
			break;
		case WTRDHD:
			if(WT_DATA_STB())
			{
				WTReadData(&WT2031.RxFram[2]);
				LoadConfigInfo(0);
				WT2031_SendCmd();
				WT2031.Status = WTIDLE;
			}
			break;
			
		case WTIDLE:
			if(WT2031_EPR_VALUE)
			{
				WT2KReadData();
				WT2031.Status = WTRECV;
			}
			break;
		case WTRECV:	
			switch(TOLKVALUE & 0x03)
			{
				case 0x01:
//					TolkManger.Status = TOLKING;
//					TolkManger.Dir = TOLKUP;
//					memcpy(TolkManger.VoiceSend,&WT2031.RxFram[24],6);
					break;
				case 0x02:
					SET_VS_EN(0);
					for(i=0;i<48;i++)
						WT2031.TxFram[i] = WT2031.RxFram[i];
					WT2031_SendCmd();
//					TolkManger.Status = TOLKING;
//					TolkManger.Dir = TOLKUP;
//					memcpy(TolkManger.VoiceSend,&WT2031.RxFram[24],6);
//					TolkManger.VoiceSendLen = 6;
					break;
				default:
					break;
			}
			WT2031.Status = WTIDLE;
			break;
	}
}

void WTSETSPEED(u8 index)
{
	LoadConfigInfo(index);
	WT2031_SendCmd();
}

void PlayRecord(void)
{
	SetVoiceData(&testVoice[PlayCnt*6],6);
	PlayCnt++;
	if(PlayCnt>=500)
		PlayCnt = 0;
}
