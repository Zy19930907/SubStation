#ifndef __VS1003_H
#define __VS1003_H

#include "Public.h"

#define VS1003_EN_PORT					GPIOC
#define VS1003_EN_PIN					6
#define SET_VS_EN(val)					{if(val) VS1003_EN_PORT->ODR |= (0x01 << VS1003_EN_PIN);else VS1003_EN_PORT->ODR &= ~(0x01 << VS1003_EN_PIN);}

#define VS1003_RST_PORT					GPIOH
#define VS1003_RST_PIN					13
#define SET_VS_RST(val)					{if(val) VS1003_RST_PORT->ODR |= (0x01 << VS1003_RST_PIN);else VS1003_RST_PORT->ODR &= ~(0x01 << VS1003_RST_PIN);}

#define VS1003_DREQ_PORT				GPIOH
#define VS1003_DREQ_PIN					14
#define VS1003_DREQ						((VS1003_DREQ_PORT->IDR >> VS1003_DREQ_PIN) & 0x01)

#define VS1003_MISO_PORT				GPIOI
#define VS1003_MISO_PIN					0
#define VS1003_MISO						((VS1003_DREQ_PORT->IDR >> VS1003_MISO_PIN) & 0x01)

#define VS1003_MOSI_PORT				GPIOI
#define VS1003_MOSI_PIN					1
#define SET_VS_MOSI(val)				{if(val) VS1003_MOSI_PORT->ODR |= (0x01 << VS1003_MOSI_PIN);else VS1003_MOSI_PORT->ODR &= ~(0x01 << VS1003_MOSI_PIN);}		

#define VS1003_SCLK_PORT				GPIOI
#define VS1003_SCLK_PIN					2
#define SET_VS_SCLK(val)				{if(val) VS1003_SCLK_PORT->ODR |= (0x01 << VS1003_SCLK_PIN);else VS1003_SCLK_PORT->ODR &= ~(0x01 << VS1003_SCLK_PIN);}

#define VS1003_XDCS_PORT				GPIOH
#define VS1003_XDCS_PIN					15
#define SET_VS_XDCS(val)				{if(val) VS1003_XDCS_PORT->ODR |= (0x01 << VS1003_XDCS_PIN);else VS1003_XDCS_PORT->ODR &= ~(0x01 << VS1003_XDCS_PIN);}

#define VS1003_XCS_PORT					GPIOI
#define VS1003_XCS_PIN					3
#define SET_VS_XCS(val)					{if(val) VS1003_XCS_PORT->ODR |= (0x01 << VS1003_XCS_PIN);else VS1003_XCS_PORT->ODR &= ~(0x01 << VS1003_XCS_PIN);}

#define VS1003IOCLKEN					RCC->AHB1ENR |= 0x184;//使能PI、PH、PC时钟

//VS1003的XX(ID列表)
#define	 VS1001 	0
#define	 VS1011 	1
#define	 VS1002 	2
#define	 VS1003 	3
#define	 VS1053 	4
#define	 VS1033 	5
#define	 VS1103 	7
extern u8 VS10XX_ID;	  		  //VS10XX的真实型号
//命令					  
#define VS_WRITE_COMMAND 	0x02  //写命令
#define VS_READ_COMMAND 	0x03  //读命令
//VS10XX寄存器定义
#define SPI_MODE        	0x00  //模式控制   
#define SPI_STATUS      	0x01  //VS10XX的状态寄存器 
#define SPI_BASS        	0x02  //高低音控制 
#define SPI_CLOCKF      	0x03  //时钟频率倍乘寄存器 
#define SPI_DECODE_TIME 	0x04  //解码时间长度 
#define SPI_AUDATA      	0x05  //各种音频数据 
#define SPI_WRAM        	0x06  //RAM 写/读 
#define SPI_WRAMADDR    	0x07  //RAM 写/读的起始地址 
#define SPI_HDAT0       	0x08  //流数据标头0 
#define SPI_HDAT1       	0x09  //流数据标头1	 
   
#define SPI_AIADDR      	0x0a  //应用程序的起始地址 
#define SPI_VOL         	0x0b  //音量控制 
#define SPI_AICTRL0     	0x0c  //应用程序控制寄存器0 
#define SPI_AICTRL1     	0x0d  //应用程序控制寄存器1 
#define SPI_AICTRL2     	0x0e  //应用程序控制寄存器2 
#define SPI_AICTRL3     	0x0f  //应用程序控制寄存器3
//SPI_MODE的各位功能,下表根据VS1053的手册标注
#define SM_DIFF         	0x01  //差分 
#define SM_LAYER12         	0x02  //允许MPEG 1,2解码 FOR vs1053 
#define SM_RESET        	0x04  //软件复位 
#define SM_CANCEL       	0x08  //取消当前解码 
#define SM_EARSPEAKER_LO  	0x10  //EarSpeaker低设定 
#define SM_TESTS        	0x20  //允许SDI测试 
#define SM_STREAM       	0x40  //流模式 
#define SM_EARSPEAKER_HI   	0x80  //EarSpeaker高设定  
#define SM_DACT         	0x100 //DCLK的有效边沿  
#define SM_SDIORD       	0x200 //SDI位顺序  
#define SM_SDISHARE     	0x400 //共享SPI片选  
#define SM_SDINEW       	0x800 //VS1002 本地SPI模式  
#define SM_ADPCM        	0x1000//ADPCM录音激活   
#define SM_LINE1         	0x4000//咪/线路1 选择 		 
#define SM_CLK_RANGE     	0x8000//输入时钟范围

u16  VS_RD_Reg(u8 address);			//读寄存器
void VS_WR_Cmd(u8 address,u16 data);//写指令
void VS_WR_Data(u8 data);			//写数据  
u16  VS_RD_Wram(u16 addr);			//读WRAM	
  
void VS1003Init(void);				//初始化VS10XX	  			    
u8  VS_HD_Reset(void);				//硬复位
void VS_Soft_Reset(void);     		//软复位
void VS_Rst_DecodeTime(void);		//重设解码时间
												    
void VS1003_Voice(unsigned int voice);        			//设置VS1003
u8 VS1003Idle(void);
u8 VS1003SendByte(u8 dat);
void SetSpiSpeed(u32 speed);

#endif
