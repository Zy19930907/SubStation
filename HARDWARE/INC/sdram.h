#ifndef _SDRAM_H
#define _SDRAM_H
#include "sys.h"
//升级说明
//V1.1 20180724
//1,修改CAS延迟为2，以提高访问速度
//2,修改SDRAM突发访问长度为1
////////////////////////////////////////////////////////////////////////////////// 	

#define Bank5_SDRAM_ADDR    ((u32)(0XC0000000)) 			//SDRAM开始地址
 
u8 SDRAM_Send_Cmd(u8 bankx,u8 cmd,u8 refresh,u16 regval);
void SDRAM_Init(void);
void FMC_SDRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n);
void FMC_SDRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n);
#endif



































