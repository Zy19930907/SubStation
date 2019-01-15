#include "VS1003.h"

static u32 Vs_delay;

void SetSpiSpeed(u32 speed)
{
	Vs_delay = 100000000 / speed;
}

void VS1003_IO_Init(void)
{
	
	VS1003IOCLKEN;
	
	//VOICE_CHOICE
	VS1003_EN_PORT->MODER &= ~((u32)0x03 << (VS1003_EN_PIN << 1));//清除原有配置
	VS1003_EN_PORT->MODER |=  ((u32)0x01 << (VS1003_EN_PIN << 1));//通用输出模式
	VS1003_EN_PORT->OSPEEDR |= ((u32)0x03 << (VS1003_EN_PIN << 1));//80MHz
	VS1003_EN_PORT->PUPDR |= ((u32)0x01 << (VS1003_EN_PIN << 1));//上拉
	//RST
	VS1003_RST_PORT->MODER &= ~((u32)0x03 << (VS1003_RST_PIN << 1));//清除原有配置
	VS1003_RST_PORT->MODER |=  ((u32)0x01 << (VS1003_RST_PIN << 1));//通用输出模式
	VS1003_RST_PORT->OSPEEDR |= ((u32)0x03 << (VS1003_RST_PIN << 1));//80MHz
	VS1003_RST_PORT->PUPDR |= ((u32)0x01 << (VS1003_RST_PIN << 1));//上拉
	//DREQ
	VS1003_DREQ_PORT->MODER &= ~((u32)0x03 << (VS1003_DREQ_PIN << 1));//清除原有配置(输入模式)
	//VS1003_DREQ_PORT->PUPDR |= (0x01 << (VS1003_DREQ_PIN << 1));//上拉
	//MISO
	VS1003_MISO_PORT->MODER &= ~((u32)0x03 << (VS1003_MISO_PIN << 1));//清除原有配置(输入模式)
	VS1003_MISO_PORT->PUPDR |= ((u32)0x01 << (VS1003_MISO_PIN << 1));//上拉
	//MOSI
	VS1003_MOSI_PORT->MODER	&= ~((u32)0x03 << (VS1003_MOSI_PIN << 1));//清除原有配置
	VS1003_MOSI_PORT->MODER |=  ((u32)0x01 << (VS1003_MOSI_PIN << 1));//通用输出模式
	VS1003_MOSI_PORT->OSPEEDR |= ((u32)0x03 << (VS1003_MOSI_PIN << 1));//80MHz
	VS1003_MOSI_PORT->PUPDR |= ((u32)0x01 << (VS1003_MOSI_PIN << 1));//上拉
	//SCLK
	VS1003_SCLK_PORT->MODER	&= ~((u32)0x03 << (VS1003_SCLK_PIN << 1));//清除原有配置
	VS1003_SCLK_PORT->MODER |=  ((u32)0x01 << (VS1003_SCLK_PIN << 1));//通用输出模式
	VS1003_SCLK_PORT->OSPEEDR |= ((u32)0x03 << (VS1003_SCLK_PIN << 1));//80MHz
	VS1003_SCLK_PORT->PUPDR |= ((u32)0x01 << (VS1003_SCLK_PIN << 1));//上拉
	//XDCS
	VS1003_XDCS_PORT->MODER	&= ~((u32)0x03 << (VS1003_XDCS_PIN << 1));//清除原有配置
	VS1003_XDCS_PORT->MODER |=  ((u32)0x01 << (VS1003_XDCS_PIN << 1));//通用输出模式
	VS1003_XDCS_PORT->OSPEEDR |= ((u32)0x03 << (VS1003_XDCS_PIN << 1));//80MHz
	VS1003_XDCS_PORT->PUPDR |= ((u32)0x01 << (VS1003_XDCS_PIN << 1));//上拉
	//XCS
	VS1003_XCS_PORT->MODER	&= ~((u32)0x03 << (VS1003_XCS_PIN << 1));//清除原有配置
	VS1003_XCS_PORT->MODER |=  ((u32)0x01 << (VS1003_XCS_PIN << 1));//通用输出模式
	VS1003_XCS_PORT->OSPEEDR |= ((u32)0x03 << (VS1003_XCS_PIN << 1));//80MHz
	VS1003_XCS_PORT->PUPDR |= ((u32)0x01 << (VS1003_XCS_PIN << 1));//上拉
}

u8 VS1003SendByte(u8 dat)
{
	unsigned char i, returnData;
    returnData = 0;
    SET_VS_SCLK(0);
    for(i = 0; i < 8; i ++)
    {
		SET_VS_MOSI((dat >> 7) & 0x01);
        dat <<= 1;
        SET_VS_SCLK(1);
        returnData <<= 1;
        if(VS1003_MISO)
           returnData++;
        SET_VS_SCLK(0);
		delay_10ns(Vs_delay);//控制模拟SPI速率
    }
	return returnData;
}

//硬复位MP3
//返回1:复位失败
//返回0:复位成功
u8 VS_HD_Reset(void)
{
	u8 retry=0;
	SET_VS_XDCS(1);	//取消数据传输
	SET_VS_XCS(1);	//取消指令传输
	SET_VS_RST(0);			    
	delay_ms(100);  
	SET_VS_RST(1);	   
	while((VS1003_DREQ)==0 && (retry<200))//等待DREQ为高
	{
		retry++;
		delay_ms(1);
	}; 	   
	delay_ms(20);	
	if(retry>=200)return 1;
	else return 0;	    		 
}

//软复位VS10XX
//注意,在这里设置完后,系统SPI时钟被修改成9M左右                     
void VS_Soft_Reset(void)
{	 
	u8 retry; 
	SetSpiSpeed(281250);
	while((VS1003_DREQ==0)&&(retry<200))			//等待DREQ为高
	{
		retry++;
		delay_ms(1);
	} 	
	VS1003SendByte(0X00);       	//启动传输	 
	retry=0; 					
	VS_WR_Cmd(SPI_MODE,0x0804);  //软件复位,新模式

	while((VS1003_DREQ==0)&&(retry<200))			//等待DREQ为高
	{
		retry++;
		delay_ms(1);
	} 	
	retry=0;
	VS_WR_Cmd(SPI_CLOCKF,0X9800);
	delay_ms(1);
//	VS_WR_Cmd(SPI_AUDATA,0xbb81);		   				
// 	VS_Rst_DecodeTime();				//复位解码时间	    
    //向VS10XX发送4个字节无效数据，用以启动SPI发送
    SetSpiSpeed(9000000);
	SET_VS_XDCS(0);							//选中数据传输 记得,这里一定要传送0X00
	VS1003SendByte(0x00);
	VS1003SendByte(0x00);
	VS1003SendByte(0x00);
	VS1003SendByte(0x00);
	SET_VS_XDCS(1);							//取消数据传输   								    
} 

//向VS10XX写命令
//address:命令地址
//data:命令数据
void VS_WR_Cmd(u8 address,u16 data)
{  
	u16 retry=0;
    while((VS1003_DREQ==0)&&(retry++)<0xfffe)retry++;//等待空闲      
	SetSpiSpeed(1125000);
	SET_VS_XDCS(1);
	SET_VS_XCS(0);
	VS1003SendByte(VS_WRITE_COMMAND);//发送VS10XX的写命令
	VS1003SendByte(address); //地址
	VS1003SendByte(data>>8); //发送高八位
	VS1003SendByte(data);	 //第八位
	SET_VS_XCS(1);
	SetSpiSpeed(9000000);
} 
//向VS10XX写数据
void VS_WR_Data(u8 data)
{
	SET_VS_XDCS(0);    
	VS1003SendByte(data);
	SET_VS_XDCS(1);      
}   
//读VS10XX的寄存器     
//注意不要用倍速读取,会出错
u16 VS_RD_Reg(u8 address)
{ 
	u16 temp=0; 
	u8 retry=0;
    while((VS1003_DREQ==0)&&(retry++)<0XFE);	//等待空闲	  
	if(retry>=0XFE)return 0;	 
	SetSpiSpeed(1125000);	
	SET_VS_XDCS(1);
	SET_VS_XCS(0);      
	VS1003SendByte(VS_READ_COMMAND);//发送VS10XX的读命令
	VS1003SendByte(address);        //地址
	temp=VS1003SendByte(0xff);		//读取高字节
	temp=temp<<8;
	temp+=VS1003SendByte(0xff); 	//读取低字节
	SET_VS_XCS(1);  
	SetSpiSpeed(9000000);    
    return temp;
} 
//读取VS10XX的RAM
u16 VS_RD_Wram(u16 addr)
{ 			   	  
	VS_WR_Cmd(SPI_WRAMADDR, addr); 
	return VS_RD_Reg(SPI_WRAM);  
} 
//重设解码时间                          
void VS_Rst_DecodeTime(void)
{
	VS_WR_Cmd(SPI_DECODE_TIME,0x0000);
	VS_WR_Cmd(SPI_DECODE_TIME,0x0000);//操作两次
}

//设定VS1003播放的音量和高低音
void VS1003_Voice(unsigned int voice)
{   
	VS_WR_Cmd(SPI_BASS,0x0e0e);//BASS   
	VS_WR_Cmd(SPI_VOL,voice); //设音量
}

//初始化VS1003
void VS1003Init(void)
{
	VS1003_IO_Init();
	VS_HD_Reset();
	VS_Soft_Reset();
	VS_WR_Cmd(SPI_AUDATA,0xffff);
}

u8 VS1003Idle(void)
{
	if(VS1003_DREQ)
		return 1;
	else
		return 0;
}
