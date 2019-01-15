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
	VS1003_EN_PORT->MODER &= ~((u32)0x03 << (VS1003_EN_PIN << 1));//���ԭ������
	VS1003_EN_PORT->MODER |=  ((u32)0x01 << (VS1003_EN_PIN << 1));//ͨ�����ģʽ
	VS1003_EN_PORT->OSPEEDR |= ((u32)0x03 << (VS1003_EN_PIN << 1));//80MHz
	VS1003_EN_PORT->PUPDR |= ((u32)0x01 << (VS1003_EN_PIN << 1));//����
	//RST
	VS1003_RST_PORT->MODER &= ~((u32)0x03 << (VS1003_RST_PIN << 1));//���ԭ������
	VS1003_RST_PORT->MODER |=  ((u32)0x01 << (VS1003_RST_PIN << 1));//ͨ�����ģʽ
	VS1003_RST_PORT->OSPEEDR |= ((u32)0x03 << (VS1003_RST_PIN << 1));//80MHz
	VS1003_RST_PORT->PUPDR |= ((u32)0x01 << (VS1003_RST_PIN << 1));//����
	//DREQ
	VS1003_DREQ_PORT->MODER &= ~((u32)0x03 << (VS1003_DREQ_PIN << 1));//���ԭ������(����ģʽ)
	//VS1003_DREQ_PORT->PUPDR |= (0x01 << (VS1003_DREQ_PIN << 1));//����
	//MISO
	VS1003_MISO_PORT->MODER &= ~((u32)0x03 << (VS1003_MISO_PIN << 1));//���ԭ������(����ģʽ)
	VS1003_MISO_PORT->PUPDR |= ((u32)0x01 << (VS1003_MISO_PIN << 1));//����
	//MOSI
	VS1003_MOSI_PORT->MODER	&= ~((u32)0x03 << (VS1003_MOSI_PIN << 1));//���ԭ������
	VS1003_MOSI_PORT->MODER |=  ((u32)0x01 << (VS1003_MOSI_PIN << 1));//ͨ�����ģʽ
	VS1003_MOSI_PORT->OSPEEDR |= ((u32)0x03 << (VS1003_MOSI_PIN << 1));//80MHz
	VS1003_MOSI_PORT->PUPDR |= ((u32)0x01 << (VS1003_MOSI_PIN << 1));//����
	//SCLK
	VS1003_SCLK_PORT->MODER	&= ~((u32)0x03 << (VS1003_SCLK_PIN << 1));//���ԭ������
	VS1003_SCLK_PORT->MODER |=  ((u32)0x01 << (VS1003_SCLK_PIN << 1));//ͨ�����ģʽ
	VS1003_SCLK_PORT->OSPEEDR |= ((u32)0x03 << (VS1003_SCLK_PIN << 1));//80MHz
	VS1003_SCLK_PORT->PUPDR |= ((u32)0x01 << (VS1003_SCLK_PIN << 1));//����
	//XDCS
	VS1003_XDCS_PORT->MODER	&= ~((u32)0x03 << (VS1003_XDCS_PIN << 1));//���ԭ������
	VS1003_XDCS_PORT->MODER |=  ((u32)0x01 << (VS1003_XDCS_PIN << 1));//ͨ�����ģʽ
	VS1003_XDCS_PORT->OSPEEDR |= ((u32)0x03 << (VS1003_XDCS_PIN << 1));//80MHz
	VS1003_XDCS_PORT->PUPDR |= ((u32)0x01 << (VS1003_XDCS_PIN << 1));//����
	//XCS
	VS1003_XCS_PORT->MODER	&= ~((u32)0x03 << (VS1003_XCS_PIN << 1));//���ԭ������
	VS1003_XCS_PORT->MODER |=  ((u32)0x01 << (VS1003_XCS_PIN << 1));//ͨ�����ģʽ
	VS1003_XCS_PORT->OSPEEDR |= ((u32)0x03 << (VS1003_XCS_PIN << 1));//80MHz
	VS1003_XCS_PORT->PUPDR |= ((u32)0x01 << (VS1003_XCS_PIN << 1));//����
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
		delay_10ns(Vs_delay);//����ģ��SPI����
    }
	return returnData;
}

//Ӳ��λMP3
//����1:��λʧ��
//����0:��λ�ɹ�
u8 VS_HD_Reset(void)
{
	u8 retry=0;
	SET_VS_XDCS(1);	//ȡ�����ݴ���
	SET_VS_XCS(1);	//ȡ��ָ���
	SET_VS_RST(0);			    
	delay_ms(100);  
	SET_VS_RST(1);	   
	while((VS1003_DREQ)==0 && (retry<200))//�ȴ�DREQΪ��
	{
		retry++;
		delay_ms(1);
	}; 	   
	delay_ms(20);	
	if(retry>=200)return 1;
	else return 0;	    		 
}

//��λVS10XX
//ע��,�������������,ϵͳSPIʱ�ӱ��޸ĳ�9M����                     
void VS_Soft_Reset(void)
{	 
	u8 retry; 
	SetSpiSpeed(281250);
	while((VS1003_DREQ==0)&&(retry<200))			//�ȴ�DREQΪ��
	{
		retry++;
		delay_ms(1);
	} 	
	VS1003SendByte(0X00);       	//��������	 
	retry=0; 					
	VS_WR_Cmd(SPI_MODE,0x0804);  //�����λ,��ģʽ

	while((VS1003_DREQ==0)&&(retry<200))			//�ȴ�DREQΪ��
	{
		retry++;
		delay_ms(1);
	} 	
	retry=0;
	VS_WR_Cmd(SPI_CLOCKF,0X9800);
	delay_ms(1);
//	VS_WR_Cmd(SPI_AUDATA,0xbb81);		   				
// 	VS_Rst_DecodeTime();				//��λ����ʱ��	    
    //��VS10XX����4���ֽ���Ч���ݣ���������SPI����
    SetSpiSpeed(9000000);
	SET_VS_XDCS(0);							//ѡ�����ݴ��� �ǵ�,����һ��Ҫ����0X00
	VS1003SendByte(0x00);
	VS1003SendByte(0x00);
	VS1003SendByte(0x00);
	VS1003SendByte(0x00);
	SET_VS_XDCS(1);							//ȡ�����ݴ���   								    
} 

//��VS10XXд����
//address:�����ַ
//data:��������
void VS_WR_Cmd(u8 address,u16 data)
{  
	u16 retry=0;
    while((VS1003_DREQ==0)&&(retry++)<0xfffe)retry++;//�ȴ�����      
	SetSpiSpeed(1125000);
	SET_VS_XDCS(1);
	SET_VS_XCS(0);
	VS1003SendByte(VS_WRITE_COMMAND);//����VS10XX��д����
	VS1003SendByte(address); //��ַ
	VS1003SendByte(data>>8); //���͸߰�λ
	VS1003SendByte(data);	 //�ڰ�λ
	SET_VS_XCS(1);
	SetSpiSpeed(9000000);
} 
//��VS10XXд����
void VS_WR_Data(u8 data)
{
	SET_VS_XDCS(0);    
	VS1003SendByte(data);
	SET_VS_XDCS(1);      
}   
//��VS10XX�ļĴ���     
//ע�ⲻҪ�ñ��ٶ�ȡ,�����
u16 VS_RD_Reg(u8 address)
{ 
	u16 temp=0; 
	u8 retry=0;
    while((VS1003_DREQ==0)&&(retry++)<0XFE);	//�ȴ�����	  
	if(retry>=0XFE)return 0;	 
	SetSpiSpeed(1125000);	
	SET_VS_XDCS(1);
	SET_VS_XCS(0);      
	VS1003SendByte(VS_READ_COMMAND);//����VS10XX�Ķ�����
	VS1003SendByte(address);        //��ַ
	temp=VS1003SendByte(0xff);		//��ȡ���ֽ�
	temp=temp<<8;
	temp+=VS1003SendByte(0xff); 	//��ȡ���ֽ�
	SET_VS_XCS(1);  
	SetSpiSpeed(9000000);    
    return temp;
} 
//��ȡVS10XX��RAM
u16 VS_RD_Wram(u16 addr)
{ 			   	  
	VS_WR_Cmd(SPI_WRAMADDR, addr); 
	return VS_RD_Reg(SPI_WRAM);  
} 
//�������ʱ��                          
void VS_Rst_DecodeTime(void)
{
	VS_WR_Cmd(SPI_DECODE_TIME,0x0000);
	VS_WR_Cmd(SPI_DECODE_TIME,0x0000);//��������
}

//�趨VS1003���ŵ������͸ߵ���
void VS1003_Voice(unsigned int voice)
{   
	VS_WR_Cmd(SPI_BASS,0x0e0e);//BASS   
	VS_WR_Cmd(SPI_VOL,voice); //������
}

//��ʼ��VS1003
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
