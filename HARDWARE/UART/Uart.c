#include "Uart.h"
#include "malloc.h"	

typedef void(*UartInit)(u32 baud);
typedef void(*UartSendBuf)(void);
typedef void(*UartRecvFunction)(u8 *buf,u16 len);
void ComInit(u8 comx,u32 baud,UartRecvFunction function);
_ComManger ComManger;

void Uart1Init(u32 baud);
void Uart2Init(u32 baud);
void Uart3Init(u32 baud);

void Uart1SendBuf(void);
void Uart2SendBuf(void);
void Uart3SendBuf(void);
void UART1SendData(u8 *buf,u16 len);
void UART2SendData(u8 *buf,u16 len);
void UART3SendData(u8 *buf,u16 len);

UartInit 	UartInitFunctions[COMCNT] = {Uart1Init,Uart2Init,Uart3Init};
UartSendBuf	UartSendFunctions[COMCNT] = {Uart1SendBuf,Uart2SendBuf,Uart3SendBuf};
UartRecvFunction UartRecvFunctions[COMCNT];

void Uart1Init(u32 baud)
{
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)((SYSMAINCLOCK/2)*1000000)/(baud*16);//得到USARTDIV@OVER8=0
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分@OVER8=0 
    mantissa<<=4;
	mantissa+=fraction; 
	
		RCC->AHB1ENR|=1<<0;   	//使能PORTA口时钟 
	RCC->APB2ENR|=1<<4;  	//使能串口1时钟 
	GPIO_Set(GPIOA,PIN9|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
 	GPIO_AF_Set(GPIOA,9,7);	//PA9,AF7
	GPIO_AF_Set(GPIOA,10,7);//PA10,AF7  	   
	//波特率设置
 	USART1->BRR=mantissa; 	//波特率设置	 
	USART1->CR1&=~(1<<15); 	//设置OVER8=0 
	USART1->CR1|=1<<3;  	//串口发送使能 
	//使能接收中断 
	USART1->CR1|=1<<2;  	//串口接收使能
	USART1->CR1 |= 1<<4;    //idle中断	   	    	
	MY_NVIC_Init(0,14,USART1_IRQn,0);
	USART1->CR1|=1<<13;  	//串口使能
	
	RCC->AHB1RSTR |= 1 << 22;
	RCC->AHB1RSTR &= ~(1<<22);
	RCC->AHB1ENR |= 1 << 22; //开启DMA2时钟
	USART1->CR3 |= 0xC0; //使能UART1 DMA收发
	DMA2_Stream7->CR &= ~0x00000001;//关闭数据流
	DMA2_Stream7->CR |= 0x04 << 25;//选择通道4
	DMA2_Stream7->CR |= 0x02 << 16;//优先级高
	DMA2_Stream7->CR |= 0x01 << 10;//存储器地址递增
	DMA2_Stream7->CR |= 0x01 << 6;//传输方向：存储器到外设
	DMA2_Stream7->PAR = (u32)&USART1->DR;//数据搬移到串口数据寄存器
	DMA2_Stream7->M0AR = (u32)ComManger.TxBuf[COM1][0];//数据源地址
	
	DMA2_Stream2->CR &= ~0x00000001;//关闭数据流
	DMA2_Stream2->CR |= 0x04 << 25;//选择通道4
	DMA2_Stream2->CR |= 0x03 << 16;//优先级高
	DMA2_Stream2->CR |= 0x01 << 10;//存储器地址递增
	DMA2_Stream2->CR |= 0x00 << 6;//传输方向：外设到存储器
	//DMA2_Stream2->CR |= 0x03 << 3;//使能DMA传输完成中断
	DMA2_Stream2->PAR = (u32)&USART1->DR;//从串口数据寄存器读取数据
	DMA2_Stream2->M0AR = (u32)&ComManger.RxBuf[COM1][0];//数据搬移目的地址
	DMA2_Stream2->NDTR = MAXCOMBUFLENN;
	DMA2_Stream2->CR |= 0x00000001;//开启数据流
}

void Uart2Init(u32 baud)
{
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)((SYSMAINCLOCK/4)*1000000)/(baud*16);//得到USARTDIV@OVER8=0
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分@OVER8=0 
    mantissa<<=4;
	mantissa+=fraction;
	
	RCC->AHB1ENR|=1<<0;   	//使能PORTA口时钟  
	RCC->APB1ENR|=1<<17;  	//使能串口2时钟 
	GPIO_Set(GPIOA,PIN2|PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
 	GPIO_AF_Set(GPIOA,2,7);	//PA2,AF7
	GPIO_AF_Set(GPIOA,3,7);//PA3,AF7
	//波特率设置
 	USART2->BRR=mantissa; 	//波特率设置	 
	USART2->CR1&=~(1<<15); 	//设置OVER8=0 
	USART2->CR1|=1<<3;  	//串口发送使能 
	//使能接收中断 
	USART2->CR1|=1<<2;  	//串口接收使能
	USART2->CR1 |= 1<<4;    //idle中断	   	    	
	MY_NVIC_Init(0,5,USART2_IRQn,0);
	USART2->CR1|=1<<13;  	//串口使能
	
	RCC->AHB1RSTR |= 1 << 21;
	RCC->AHB1RSTR &= ~(1<<21);
	RCC->AHB1ENR |= 1 << 21; //开启DMA2时钟
	USART2->CR3 |= 0xC0; //使能UART1 DMA收发
	DMA1_Stream6->CR &= ~0x00000001;//关闭数据流
	DMA1_Stream6->CR |= 0x04 << 25;//选择通道4
	DMA1_Stream6->CR |= 0x02 << 16;//优先级高
	DMA1_Stream6->CR |= 0x01 << 10;//存储器地址递增
	DMA1_Stream6->CR |= 0x01 << 6;//传输方向：存储器到外设
	DMA1_Stream6->PAR = (u32)&USART2->DR;//数据搬移到串口数据寄存器
	DMA1_Stream6->M0AR = (u32)&ComManger.TxBuf[COM2][0];//数据源地址
	
	DMA1_Stream5->CR &= ~0x00000001;//关闭数据流
	DMA1_Stream5->CR |= 0x04 << 25;//选择通道4
	DMA1_Stream5->CR |= 0x03 << 16;//优先级高
	DMA1_Stream5->CR |= 0x01 << 10;//存储器地址递增
	DMA1_Stream5->CR |= 0x00 << 6;//传输方向：外设到存储器
	DMA1_Stream5->PAR = (u32)&USART2->DR;//从串口数据寄存器读取数据
	DMA1_Stream5->M0AR = (u32)&ComManger.RxBuf[COM2][0];//数据搬移目的地址
	DMA1_Stream5->NDTR = MAXCOMBUFLENN;
	DMA1_Stream5->CR |= 0x00000001;//开启数据流
	//DMA2_Stream2->CR |= 0x03 << 3;//使能DMA传输完成中断
}

void Uart3Init(u32 baud)
{
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)((SYSMAINCLOCK/4)*1000000)/(baud*16);//得到USARTDIV@OVER8=0
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分@OVER8=0 
    mantissa<<=4;
	mantissa+=fraction;
	
	RCC->AHB1ENR|=1<<1;   	//使能PORTB口时钟  
	RCC->APB1ENR|=1<<18;  	//使能串口3时钟 
	GPIO_Set(GPIOB,PIN10|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
 	GPIO_AF_Set(GPIOB,10,7);	//PA2,AF7
	GPIO_AF_Set(GPIOB,11,7);//PA3,AF7
	//波特率设置
 	USART3->BRR=mantissa; 	//波特率设置	 
	USART3->CR1&=~(1<<15); 	//设置OVER8=0 
	USART3->CR1|=1<<3;  	//串口发送使能 
	//使能接收中断 
	USART3->CR1|=1<<2;  	//串口接收使能
	USART3->CR1 |= 1<<4;    //idle中断	   	    	
	MY_NVIC_Init(0,13,USART3_IRQn,0);
	USART3->CR1|=1<<13;   //串口使能
	
	RCC->AHB1RSTR |= 1 << 21;
	RCC->AHB1RSTR &= ~(1<<21);
	RCC->AHB1ENR |= 1 << 21; //开启DMA2时钟
	USART3->CR3 |= 0xC0; //使能UART1 DMA收发
	DMA1_Stream3->CR &= ~0x00000001;//关闭数据流
	DMA1_Stream3->CR |= 0x04 << 25;//选择通道4
	DMA1_Stream3->CR |= 0x02 << 16;//优先级高
	DMA1_Stream3->CR |= 0x01 << 10;//存储器地址递增
	DMA1_Stream3->CR |= 0x01 << 6;//传输方向：存储器到外设
	DMA1_Stream3->PAR = (u32)&USART3->DR;//数据搬移到串口数据寄存器
	DMA1_Stream3->M0AR = (u32)&ComManger.TxBuf[COM3][0];//数据源地址
	
	DMA1_Stream1->CR &= ~0x00000001;//关闭数据流
	DMA1_Stream1->CR |= 0x04 << 25;//选择通道4
	DMA1_Stream1->CR |= 0x03 << 16;//优先级高
	DMA1_Stream1->CR |= 0x01 << 10;//存储器地址递增
	DMA1_Stream1->CR |= 0x00 << 6;//传输方向：外设到存储器
	DMA1_Stream1->PAR = (u32)&USART3->DR;//从串口数据寄存器读取数据
	DMA1_Stream1->M0AR = (u32)&ComManger.RxBuf[COM3][0];//数据搬移目的地址
	DMA1_Stream1->NDTR = MAXCOMBUFLENN;
	DMA1_Stream1->CR |= 0x00000001;//开启数据流
	//DMA2_Stream2->CR |= 0x03 << 3;//使能DMA传输完成中断
}
//串口1接收中断
void USART1_IRQHandler(void)
{
	u16 temp;
	if(USART1 -> SR & 0x10)    // 空闲中断 , 先关闭DMA通道，清零计数，在打开
	{
		temp = MAXCOMBUFLENN - DMA2_Stream2->NDTR;
		ComManger.RxLen[COM1] = temp;
		temp = USART1->SR;
		temp |= USART1->DR;
		DMA2->LIFCR |= 0x3D << 16;
		DMA2_Stream2->CR &= ~(1<<0);
		DMA2_Stream2->NDTR = MAXCOMBUFLENN;
		DMA2_Stream2->CR |= (1<<0);
		Uart1Init(ComManger.Baud[COM1]);
		UartRecvFunctions[COM1](ComManger.RxBuf[COM1],ComManger.RxLen[COM1]);
	}
}
//串口2接收中断
void USART2_IRQHandler(void)
{
	u16 temp;
	if(USART2 -> SR & 0x10)    // 空闲中断 , 先关闭DMA通道，清零计数，在打开
	{
		temp = MAXCOMBUFLENN - DMA1_Stream5->NDTR;
		ComManger.RxLen[COM2] = temp;
		temp = USART2->SR;
		temp |= USART2->DR;
		DMA1_Stream5->CR &= ~(1<<0);
		DMA1_Stream5->NDTR = MAXCOMBUFLENN;
		DMA1_Stream5->CR |= (1<<0);
		Uart2Init(ComManger.Baud[COM2]);
		UartRecvFunctions[COM2](ComManger.RxBuf[COM2],ComManger.RxLen[COM2]);
	}
}

//串口3接收中断
void USART3_IRQHandler(void)
{
	u16 temp;
	if(USART3 -> SR & 0x10)    // 空闲中断 , 先关闭DMA通道，清零计数，在打开
	{
		temp = MAXCOMBUFLENN - DMA1_Stream1->NDTR;
		ComManger.RxLen[COM2] = temp;
		temp = USART3->SR;
		temp |= USART3->DR;
		DMA1_Stream1->CR &= ~(1<<0);
		DMA1_Stream1->NDTR = MAXCOMBUFLENN;
		DMA1_Stream1->CR |= (1<<0);
		Uart3Init(ComManger.Baud[COM3]);
		UartRecvFunctions[COM3](ComManger.RxBuf[COM3],ComManger.RxLen[COM3]);
	}
}

void Uart1SendBuf(void)
{
	DMA2->HIFCR |= 0x3D << 22;
	DMA2_Stream7->CR &= ~(1<<0);
	DMA2_Stream7->M0AR = (u32)&ComManger.TxBuf[COM1][0];
	DMA2_Stream7->NDTR = ComManger.TxLen[COM1];
	DMA2_Stream7->CR |= (1<<0);
}

void Uart2SendBuf(void)
{
	DMA1->HIFCR |= 0xFFFFFFFF;
	DMA1_Stream6->CR &= ~(1<<0);
	DMA1_Stream6->M0AR = (u32)&ComManger.TxBuf[COM2][0];
	DMA1_Stream6->NDTR = ComManger.TxLen[COM2];
	DMA1_Stream6->CR |= (1<<0);
}

void Uart3SendBuf(void)
{
	DMA1->LIFCR |= 0xFFFFFFFF;
	DMA1_Stream3->CR &= ~(1<<0);
	DMA1_Stream3->M0AR = (u32)&ComManger.TxBuf[COM3][0];
	DMA1_Stream3->NDTR = ComManger.TxLen[COM3];
	DMA1_Stream3->CR |= (1<<0);
}

//串口发送数据
void ComSendBuf(u8 comx,u8 *buf,u16 len)
{
	u16 i;
	//拷贝数据到串口数据发送缓存，防止DMA发送工程中数据被修改
	for(i=0;i<len;i++)
		ComManger.TxBuf[comx][i]= *(buf+i);
	ComManger.TxLen[comx] = len;
	UartSendFunctions[comx]();
}

void ComMangerInit(void)
{
	ComManger.ComInit = ComInit;
	ComManger.ComSendBuf = ComSendBuf;
	ComManger.Com1Send = UART1SendData;
	ComManger.Com2Send = UART2SendData;
	ComManger.Com3Send = UART3SendData;
}

//初始化串口，注册串口接收回调函数
void ComInit(u8 comx,u32 baud,UartRecvFunction function)
{
	myfree(SRAMEX,ComManger.RxBuf[comx]);
	myfree(SRAMEX,ComManger.TxBuf[comx]);
	ComManger.RxBuf[comx] = mymalloc(SRAMEX,MAXCOMBUFLENN);
	ComManger.TxBuf[comx] = mymalloc(SRAMEX,MAXCOMBUFLENN);
	ComManger.Baud[comx] = baud;
	//注册串口接收回调函数
	UartRecvFunctions[comx] = function;
	//初始化串口
	UartInitFunctions[comx](baud);
}

void SendData(char* buf)
{
	u16 i,len = strlen(buf);
	//拷贝数据到串口数据发送缓存，防止DMA发送工程中数据被修改
	for(i=0;i<len;i++)
		ComManger.TxBuf[COM1][i]= *(buf+i);
	ComManger.TxLen[COM1] = len;
	Uart1SendBuf();
}

void UART1SendData(u8 *buf,u16 len)
{
	u16 i;
	for(i=0;i<len;i++)
		ComManger.TxBuf[COM1][i]= *(buf+i);
	DMA2->HIFCR |= 0x3D << 22;
	DMA2_Stream7->CR &= ~(1<<0);
	DMA2_Stream7->M0AR = (u32)&ComManger.TxBuf[COM1][0];
	DMA2_Stream7->NDTR = len;
	DMA2_Stream7->CR |= (1<<0);
}

void UART2SendData(u8 *buf,u16 len)
{
	u16 i;
	for(i=0;i<len;i++)
		ComManger.TxBuf[COM2][i]= *(buf+i);
	DMA1->HIFCR |= 0xFFFFFFFF;
	DMA1_Stream6->CR &= ~(1<<0);
	DMA1_Stream6->M0AR = (u32)&ComManger.TxBuf[COM2][0];
	DMA1_Stream6->NDTR = len;
	DMA1_Stream6->CR |= (1<<0);
}

void UART3SendData(u8 *buf,u16 len)
{
	u16 i;
	for(i=0;i<len;i++)
		ComManger.TxBuf[COM3][i]= *(buf+i);
	DMA1->LIFCR |= 0xFFFFFFFF;
	DMA1_Stream3->CR &= ~(1<<0);
	DMA1_Stream3->M0AR = (u32)&ComManger.TxBuf[COM3][0];
	DMA1_Stream3->NDTR = len;
	DMA1_Stream3->CR |= (1<<0);
}
