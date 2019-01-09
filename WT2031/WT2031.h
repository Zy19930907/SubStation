#ifndef __WT2031_H
#define __WT2031_H

#include "Public.h"

#define VOICEPACKLEN				6
typedef void(*WT2031Handler)(u8 *buf,u8 len);
#define MAXWT2031OBSERVER			3
//WT2031引脚定义
//RST
#define WT2031_RSTPROT				GPIOI
#define WT2031_RSTPIN				6
#define WT2031_RST_SET(val)			{if(val) WT2031_RSTPROT->ODR |= (0x01 << WT2031_RSTPIN); else WT2031_RSTPROT->ODR &= ~(0x01 << WT2031_RSTPIN);}
//CODECRST
#define WT2031_CODECRSTPROT			GPIOI
#define WT2031_CODECRSTPIN			6
#define WT2031_CODECRST_SET(val)	{if(val) WT2031_CODECRSTPROT->ODR |= (0x01 << WT2031_CODECRSTPIN); else WT2031_CODECRSTPROT->ODR &= ~(0x01 << WT2031_CODECRSTPIN);}
//EPR
#define WT2031_EPRPROT				GPIOB
#define WT2031_EPRPIN				8
#define WT2031_EPR_VALUE			(!(((WT2031_EPRPROT->IDR) >> WT2031_EPRPIN) & 0x01))
//RX_CLK
#define WT2031_RX_CLKPROT			GPIOB
#define WT2031_RX_CLKPIN			4
#define WT2031_RX_CLK_SET(val)		{if(val) WT2031_RX_CLKPROT->ODR |= (0x01 << WT2031_RX_CLKPIN); else WT2031_RX_CLKPROT->ODR &= ~(0x01 << WT2031_RX_CLKPIN);}
//RXSTRB
#define WT2031_RX_STRBPROT			GPIOB
#define WT2031_RX_STRBPIN			3
#define WT2031_RX_STRB_SET(val)		{if(val) WT2031_RX_STRBPROT->ODR |= (0x01 << WT2031_RX_STRBPIN); else WT2031_RX_STRBPROT->ODR &= ~(0x01 << WT2031_RX_STRBPIN);}
//RXDATA
#define WT2031_RX_DATAPROT			GPIOB
#define WT2031_RX_DATAPIN			7
#define WT2031_RX_DATA_SET(val)		{if(val) WT2031_RX_DATAPROT->ODR |= (0x01 << WT2031_RX_DATAPIN); else WT2031_RX_DATAPROT->ODR &= ~(0x01 << WT2031_RX_DATAPIN);}
//TX_CLK
#define WT2031_TX_CLKPROT			GPIOB
#define WT2031_TX_CLKPIN			9
#define WT2031_TX_CLK_SET(val)		{if(val) WT2031_TX_CLKPROT->ODR |= (0x01 << WT2031_TX_CLKPIN); else WT2031_TX_CLKPROT->ODR &= ~(0x01 << WT2031_TX_CLKPIN);}
//TXSTRB
#define WT2031_TX_STRBPROT			GPIOI
#define WT2031_TX_STRBPIN			5
#define WT2031_TX_STRB_SET(val)		{if(val) WT2031_TX_STRBPROT->ODR |= (0x01 << WT2031_TX_STRBPIN); else WT2031_TX_STRBPROT->ODR &= ~(0x01 << WT2031_TX_STRBPIN);}
//TXDATA
#define WT2031_TX_DATAPROT			GPIOI
#define WT2031_TX_DATAPIN			7
#define WT2031_TX_DATA_VALUE		((WT2031_TX_DATAPROT->IDR >> WT2031_TX_DATAPIN) & 0x01)

#define WT2031IOCLKEN				RCC->AHB1ENR |= 0x102;//使能PI、PB时钟

typedef enum
{
	WTINIT,
	WTRST,
	WTRDHD,
	WTIDLE,
	WTRECV,
}_WT2031Status;

typedef struct
{
	_WT2031Status Status;
	u8 TxFram[48];
	u8 RxFram[48];
	u32 Tick;
	
	u8 ObserverFlag;
	WT2031Handler WT2031Handlers[MAXWT2031OBSERVER];
}_WT2031;

void WT2031Pro(void);
void WT2031_SendCmd(void);
void SetVoiceData(u8 *buf ,u8 len);
void WT2031RST(void);
void WT2031SetVolum(u8 volum);
void RecvVoiceData(u32 ID,u8 *buf,u8 len);
void WTSETSPEED(u8 index);
void PlayRecord(void);
void PlayTolkData(u8 *buf ,u8 len);

#endif
