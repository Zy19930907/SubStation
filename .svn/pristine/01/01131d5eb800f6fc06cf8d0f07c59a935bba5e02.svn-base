#ifndef __SD2068_H
#define __SD2068_H

#include "Public.h"

#define SD2068IOMODEIN					0
#define SD2068IOMODEOUT					1

#define SD2068IOVALUELOW				0
#define SD2068IOVALUEHIGH				1

#define SD2068SDAPORT					GPIOG
#define SD2068SDAPIN					14
#define SETSD2068SDA(val)				{if(val) SD2068SDAPORT->ODR |= (1<<SD2068SDAPIN);else SD2068SDAPORT->ODR &= ~(1<<SD2068SDAPIN);}

#define SD2068SCLPORT					GPIOG
#define SD2068SCLPIN					13
#define SETSD2068SCL(val)				{if(val) SD2068SCLPORT->ODR |= (1<<SD2068SCLPIN);else SD2068SCLPORT->ODR &= ~(1<<SD2068SCLPIN);}

#define SD2068INTPORT					GPIOG
#define SD2068INTPIN					12

#define SD2068SDAOUT					{SD2068SDAPORT->MODER &= ~(0x03 << (SD2068SDAPIN << 1));SD2068SDAPORT->MODER |= (0x01 << (SD2068SDAPIN << 1));}
#define SD2068SDAIN						SD2068SDAPORT->MODER &= ~(0x03 << (SD2068SDAPIN << 1))

#define GETSD2068SDA					((SD2068SDAPORT->IDR >> SD2068SDAPIN) & 0x01)
#define GETSD2068INT					((SD2068INTPORT->IDR >> SD2068INTPIN) & 0x01)

#define SD2068PORTCLKEN					RCC->AHB1ENR |= (1<<6)

typedef struct
{
	u8 Buf[7];
    u8 Hex[7];
    u16 AllSecond;
}_SD2068;

void SD2068Init(void);
	
extern _SD2068 SD2068;
void TimeChange(u8 *buf);
void ReadTime(void);
void WriteTime(void);

#endif
