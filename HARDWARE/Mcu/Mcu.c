#include "Mcu.h"

vu32 SYS_TICK,SEC_TICK;
vu8 _100msCnt = 0;
extern volatile int OS_TimeMS;
void SystemClockInit(void);
//配置单片机主频、系统时基定时器、单片机片上外设初始化
void McuInit(void)
{
	Stm32_Clock_Init(SYSMAINCLOCK*2,12,2,8);
	SystemClockInit();
}

//1ms系统时基定时器初始化
void SystemClockInit(void)
{
	RCC->APB1ENR|=3<<0;	//TIM2、TIM3时钟使能    
 	TIM2->ARR = 999;  	//设定计数器自动重装值 
	TIM2->PSC = SYSMAINCLOCK/2-1;  	//预分频器	  
	TIM2->DIER |=1<<0;   //允许更新中断	  
	TIM2->CR1|=0x01;    //使能定时器2
	//100ms定时
//	TIM3->ARR = 49999;
//	TIM3->PSC = SYSMAINCLOCK-1;  	//预分频器
//	TIM3->DIER |= 1<<0;
//	TIM3->CR1 |= 1<<0;
	
  	MY_NVIC_Init(0,0,TIM2_IRQn,0);	//最高优先级防止在其他中断中使用延时函数导致死机	
}

//定时器2中断服务程序	 
void TIM2_IRQHandler(void)
{ 		    		  			    
	if(TIM2->SR&0X0001)//溢出中断
	{
		SYS_TICK++;			
		OS_TimeMS++;		
	}
	TIM2->SR&=~(1<<0);//清除中断标志位 	    
}

//定时器3中断服务程序	 
void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//溢出中断
	{
		_100msCnt++;
		if(_100msCnt >= 10)
		{
			_100msCnt = 0;
			SEC_TICK++;	
		}
	}		
	TIM3->SR&=~(1<<0);//清除中断标志位 	    
}

u32 MsTickDiff(u32 tick)
{
	if(tick > SYS_TICK)
		return 0xFFFFFFFF- tick + SYS_TICK;
	else
		return SYS_TICK - tick;
}

u32 SecTickDiff(u32 tick)
{
	if(tick > SEC_TICK)
		return 0xFFFFFFFF- tick + SEC_TICK;
	else
		return SEC_TICK - tick;
}

void delay_ms(u32 nms)
{
	u32 tick = SYS_TICK;
	while(MsTickDiff(tick) <= nms);
}

void delay_us(u32 nus)
{
	u32 i,j;
	for(j=0;j<nus;j++)
	{
		i = SYSMAINCLOCK;
		do{
			__nop();
		}while(--i);
	}
}

void delay_10ns(u32 nNs)
{
	do{
		__nop();
	}while(--nNs);
}
