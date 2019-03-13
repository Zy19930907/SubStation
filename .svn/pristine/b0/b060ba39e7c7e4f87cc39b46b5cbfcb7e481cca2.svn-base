#ifndef __LCDSPI_H
#define __LCDSPI_H

#include "Public.h"

// SPI总线速度设置 
#define SPI_SPEED_2   		0
#define SPI_SPEED_4   		1
#define SPI_SPEED_8   		2
#define SPI_SPEED_16  		3
#define SPI_SPEED_32 		4
#define SPI_SPEED_64 		5
#define SPI_SPEED_128 		6
#define SPI_SPEED_256 		7
						  	    													  
void LcdSpiInit(void);			 //初始化SPI口
void Lcd_SetSpiSpeed(u8 SpeedSet); //设置SPI速度   
u16 Lcd_ReadWriteByte(u16 TxData);//SPI总线读写一个字节

#endif
