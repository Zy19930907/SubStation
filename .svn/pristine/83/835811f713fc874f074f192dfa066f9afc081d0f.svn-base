#ifndef __UART_H
#define __UART_H

#include "sys.h"

#define COMCNT				3

#define COM1				0
#define COM2				1
#define COM3				2

#define MAXCOMBUFLENN		1024

typedef struct
{
	u16 RxLen[COMCNT];
	u16 TxLen[COMCNT];
//	u8 RxBuf[COMCNT][MAXCOMBUFLENN];
//	u8 TxBuf[COMCNT][MAXCOMBUFLENN];
	u8 *RxBuf[COMCNT];
	u8 *TxBuf[COMCNT];
	u32 Baud[COMCNT];
	void(*ComInit)(u8 comx,u32 baud,void(*RecvFunction)(u8 *buf,u16 len));
	void(*ComSendBuf)(u8 comx,u8 *buf,u16 len);
	void(*Com1Send)(u8 *buf,u16 len);
	void(*Com2Send)(u8 *buf,u16 len);
	void(*Com3Send)(u8 *buf,u16 len);
}_ComManger;
extern _ComManger ComManger;

void ComMangerInit(void);
void SendData(char* buf);
void UART3SendData(u8 *buf,u16 len);
#endif

