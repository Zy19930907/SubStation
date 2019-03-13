#ifndef FRAMINIT_H
#define FRAMINIT_H

#include "Public.h"

#define EEIOMODEIN					0
#define EEIOMODEOUT					1

#define EEIOVALUELOW				0
#define EEIOVALUEHIGH				1

#define EEWPPORT					GPIOF
#define EEWPPIN						8

#define EESDAPORT					GPIOF
#define EESDAPIN					6

#define EESCLPORT					GPIOF
#define	EESCLPIN					7

#define EEIOPORTCLK					RCC->AHB1ENR |= 1<<5

#define EEWPOUT						{EEWPPORT->MODER &= ~(0x03 << (EEWPPIN << 1));EEWPPORT->MODER |= (0x01 << (EEWPPIN << 1));}
#define EESDAOUT					{EESDAPORT->MODER &= ~(0x03 << (EESDAPIN << 1));EESDAPORT->MODER |= (0x01 << (EESDAPIN << 1));}
#define EESDAIN						 EESDAPORT->MODER &= ~(0x03 << (EESDAPIN << 1))
#define EESCLOUT					{EESCLPORT->MODER &= ~(0x03 << (EESCLPIN << 1));EESCLPORT->MODER |= (0x01 << (EESCLPIN << 1));}

#define SETEESDA(val)				{if(val)EESDAPORT->ODR |= (1<<EESDAPIN);else EESDAPORT->ODR &= ~(1<<EESDAPIN);}
#define SETEESCL(val)				{if(val)EESCLPORT->ODR |= (1<<EESCLPIN);else EESCLPORT->ODR &= ~(1<<EESCLPIN);}
#define GETEESDA					((EESDAPORT->IDR >> EESDAPIN) & 0x01)

#define EEDISWP						EEWPPORT->ODR &= ~(1<<EEWPPIN)//失能写保护，执行后可写
#define EEENWP						EEWPPORT->ODR |=  (1<<EEWPPIN)//失能写保护，执行后不可写

void FmMemoryInit(void);
void FramReadBurstByte(u16 addr, u8 *buf, u16 len);
void FramWriteBurstByte(u16 addr, u8 *buf, u16 len);
					
#endif
