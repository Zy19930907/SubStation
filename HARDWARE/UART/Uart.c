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
	temp=(float)((SYSMAINCLOCK/2)*1000000)/(baud*16);//�õ�USARTDIV@OVER8=0
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������@OVER8=0 
    mantissa<<=4;
	mantissa+=fraction; 
	
		RCC->AHB1ENR|=1<<0;   	//ʹ��PORTA��ʱ�� 
	RCC->APB2ENR|=1<<4;  	//ʹ�ܴ���1ʱ�� 
	GPIO_Set(GPIOA,PIN9|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//PA9,PA10,���ù���,�������
 	GPIO_AF_Set(GPIOA,9,7);	//PA9,AF7
	GPIO_AF_Set(GPIOA,10,7);//PA10,AF7  	   
	//����������
 	USART1->BRR=mantissa; 	//����������	 
	USART1->CR1&=~(1<<15); 	//����OVER8=0 
	USART1->CR1|=1<<3;  	//���ڷ���ʹ�� 
	//ʹ�ܽ����ж� 
	USART1->CR1|=1<<2;  	//���ڽ���ʹ��
	USART1->CR1 |= 1<<4;    //idle�ж�	   	    	
	MY_NVIC_Init(0,14,USART1_IRQn,0);
	USART1->CR1|=1<<13;  	//����ʹ��
	
	RCC->AHB1RSTR |= 1 << 22;
	RCC->AHB1RSTR &= ~(1<<22);
	RCC->AHB1ENR |= 1 << 22; //����DMA2ʱ��
	USART1->CR3 |= 0xC0; //ʹ��UART1 DMA�շ�
	DMA2_Stream7->CR &= ~0x00000001;//�ر�������
	DMA2_Stream7->CR |= 0x04 << 25;//ѡ��ͨ��4
	DMA2_Stream7->CR |= 0x02 << 16;//���ȼ���
	DMA2_Stream7->CR |= 0x01 << 10;//�洢����ַ����
	DMA2_Stream7->CR |= 0x01 << 6;//���䷽�򣺴洢��������
	DMA2_Stream7->PAR = (u32)&USART1->DR;//���ݰ��Ƶ��������ݼĴ���
	DMA2_Stream7->M0AR = (u32)ComManger.TxBuf[COM1][0];//����Դ��ַ
	
	DMA2_Stream2->CR &= ~0x00000001;//�ر�������
	DMA2_Stream2->CR |= 0x04 << 25;//ѡ��ͨ��4
	DMA2_Stream2->CR |= 0x03 << 16;//���ȼ���
	DMA2_Stream2->CR |= 0x01 << 10;//�洢����ַ����
	DMA2_Stream2->CR |= 0x00 << 6;//���䷽�����赽�洢��
	//DMA2_Stream2->CR |= 0x03 << 3;//ʹ��DMA��������ж�
	DMA2_Stream2->PAR = (u32)&USART1->DR;//�Ӵ������ݼĴ�����ȡ����
	DMA2_Stream2->M0AR = (u32)&ComManger.RxBuf[COM1][0];//���ݰ���Ŀ�ĵ�ַ
	DMA2_Stream2->NDTR = MAXCOMBUFLENN;
	DMA2_Stream2->CR |= 0x00000001;//����������
}

void Uart2Init(u32 baud)
{
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)((SYSMAINCLOCK/4)*1000000)/(baud*16);//�õ�USARTDIV@OVER8=0
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������@OVER8=0 
    mantissa<<=4;
	mantissa+=fraction;
	
	RCC->AHB1ENR|=1<<0;   	//ʹ��PORTA��ʱ��  
	RCC->APB1ENR|=1<<17;  	//ʹ�ܴ���2ʱ�� 
	GPIO_Set(GPIOA,PIN2|PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//PA9,PA10,���ù���,�������
 	GPIO_AF_Set(GPIOA,2,7);	//PA2,AF7
	GPIO_AF_Set(GPIOA,3,7);//PA3,AF7
	//����������
 	USART2->BRR=mantissa; 	//����������	 
	USART2->CR1&=~(1<<15); 	//����OVER8=0 
	USART2->CR1|=1<<3;  	//���ڷ���ʹ�� 
	//ʹ�ܽ����ж� 
	USART2->CR1|=1<<2;  	//���ڽ���ʹ��
	USART2->CR1 |= 1<<4;    //idle�ж�	   	    	
	MY_NVIC_Init(0,5,USART2_IRQn,0);
	USART2->CR1|=1<<13;  	//����ʹ��
	
	RCC->AHB1RSTR |= 1 << 21;
	RCC->AHB1RSTR &= ~(1<<21);
	RCC->AHB1ENR |= 1 << 21; //����DMA2ʱ��
	USART2->CR3 |= 0xC0; //ʹ��UART1 DMA�շ�
	DMA1_Stream6->CR &= ~0x00000001;//�ر�������
	DMA1_Stream6->CR |= 0x04 << 25;//ѡ��ͨ��4
	DMA1_Stream6->CR |= 0x02 << 16;//���ȼ���
	DMA1_Stream6->CR |= 0x01 << 10;//�洢����ַ����
	DMA1_Stream6->CR |= 0x01 << 6;//���䷽�򣺴洢��������
	DMA1_Stream6->PAR = (u32)&USART2->DR;//���ݰ��Ƶ��������ݼĴ���
	DMA1_Stream6->M0AR = (u32)&ComManger.TxBuf[COM2][0];//����Դ��ַ
	
	DMA1_Stream5->CR &= ~0x00000001;//�ر�������
	DMA1_Stream5->CR |= 0x04 << 25;//ѡ��ͨ��4
	DMA1_Stream5->CR |= 0x03 << 16;//���ȼ���
	DMA1_Stream5->CR |= 0x01 << 10;//�洢����ַ����
	DMA1_Stream5->CR |= 0x00 << 6;//���䷽�����赽�洢��
	DMA1_Stream5->PAR = (u32)&USART2->DR;//�Ӵ������ݼĴ�����ȡ����
	DMA1_Stream5->M0AR = (u32)&ComManger.RxBuf[COM2][0];//���ݰ���Ŀ�ĵ�ַ
	DMA1_Stream5->NDTR = MAXCOMBUFLENN;
	DMA1_Stream5->CR |= 0x00000001;//����������
	//DMA2_Stream2->CR |= 0x03 << 3;//ʹ��DMA��������ж�
}

void Uart3Init(u32 baud)
{
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)((SYSMAINCLOCK/4)*1000000)/(baud*16);//�õ�USARTDIV@OVER8=0
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������@OVER8=0 
    mantissa<<=4;
	mantissa+=fraction;
	
	RCC->AHB1ENR|=1<<1;   	//ʹ��PORTB��ʱ��  
	RCC->APB1ENR|=1<<18;  	//ʹ�ܴ���3ʱ�� 
	GPIO_Set(GPIOB,PIN10|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//PA9,PA10,���ù���,�������
 	GPIO_AF_Set(GPIOB,10,7);	//PA2,AF7
	GPIO_AF_Set(GPIOB,11,7);//PA3,AF7
	//����������
 	USART3->BRR=mantissa; 	//����������	 
	USART3->CR1&=~(1<<15); 	//����OVER8=0 
	USART3->CR1|=1<<3;  	//���ڷ���ʹ�� 
	//ʹ�ܽ����ж� 
	USART3->CR1|=1<<2;  	//���ڽ���ʹ��
	USART3->CR1 |= 1<<4;    //idle�ж�	   	    	
	MY_NVIC_Init(0,13,USART3_IRQn,0);
	USART3->CR1|=1<<13;   //����ʹ��
	
	RCC->AHB1RSTR |= 1 << 21;
	RCC->AHB1RSTR &= ~(1<<21);
	RCC->AHB1ENR |= 1 << 21; //����DMA2ʱ��
	USART3->CR3 |= 0xC0; //ʹ��UART1 DMA�շ�
	DMA1_Stream3->CR &= ~0x00000001;//�ر�������
	DMA1_Stream3->CR |= 0x04 << 25;//ѡ��ͨ��4
	DMA1_Stream3->CR |= 0x02 << 16;//���ȼ���
	DMA1_Stream3->CR |= 0x01 << 10;//�洢����ַ����
	DMA1_Stream3->CR |= 0x01 << 6;//���䷽�򣺴洢��������
	DMA1_Stream3->PAR = (u32)&USART3->DR;//���ݰ��Ƶ��������ݼĴ���
	DMA1_Stream3->M0AR = (u32)&ComManger.TxBuf[COM3][0];//����Դ��ַ
	
	DMA1_Stream1->CR &= ~0x00000001;//�ر�������
	DMA1_Stream1->CR |= 0x04 << 25;//ѡ��ͨ��4
	DMA1_Stream1->CR |= 0x03 << 16;//���ȼ���
	DMA1_Stream1->CR |= 0x01 << 10;//�洢����ַ����
	DMA1_Stream1->CR |= 0x00 << 6;//���䷽�����赽�洢��
	DMA1_Stream1->PAR = (u32)&USART3->DR;//�Ӵ������ݼĴ�����ȡ����
	DMA1_Stream1->M0AR = (u32)&ComManger.RxBuf[COM3][0];//���ݰ���Ŀ�ĵ�ַ
	DMA1_Stream1->NDTR = MAXCOMBUFLENN;
	DMA1_Stream1->CR |= 0x00000001;//����������
	//DMA2_Stream2->CR |= 0x03 << 3;//ʹ��DMA��������ж�
}
//����1�����ж�
void USART1_IRQHandler(void)
{
	u16 temp;
	if(USART1 -> SR & 0x10)    // �����ж� , �ȹر�DMAͨ��������������ڴ�
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
//����2�����ж�
void USART2_IRQHandler(void)
{
	u16 temp;
	if(USART2 -> SR & 0x10)    // �����ж� , �ȹر�DMAͨ��������������ڴ�
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

//����3�����ж�
void USART3_IRQHandler(void)
{
	u16 temp;
	if(USART3 -> SR & 0x10)    // �����ж� , �ȹر�DMAͨ��������������ڴ�
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

//���ڷ�������
void ComSendBuf(u8 comx,u8 *buf,u16 len)
{
	u16 i;
	//�������ݵ��������ݷ��ͻ��棬��ֹDMA���͹��������ݱ��޸�
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

//��ʼ�����ڣ�ע�ᴮ�ڽ��ջص�����
void ComInit(u8 comx,u32 baud,UartRecvFunction function)
{
	myfree(SRAMEX,ComManger.RxBuf[comx]);
	myfree(SRAMEX,ComManger.TxBuf[comx]);
	ComManger.RxBuf[comx] = mymalloc(SRAMEX,MAXCOMBUFLENN);
	ComManger.TxBuf[comx] = mymalloc(SRAMEX,MAXCOMBUFLENN);
	ComManger.Baud[comx] = baud;
	//ע�ᴮ�ڽ��ջص�����
	UartRecvFunctions[comx] = function;
	//��ʼ������
	UartInitFunctions[comx](baud);
}

void SendData(char* buf)
{
	u16 i,len = strlen(buf);
	//�������ݵ��������ݷ��ͻ��棬��ֹDMA���͹��������ݱ��޸�
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
