#ifndef _SIFFONTCREATE_H
#define _SIFFONTCREATE_H
#include "sys.h"
#include "GUI.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//STemwin SIF格式字体显示   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/3/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

extern GUI_FONT SIF12_Font;
extern GUI_FONT SIF16_Font;
extern GUI_FONT SIF24_Font;
extern GUI_FONT SIF36_Font;

void Create_SIF12(u8 *fxpath);
void Create_SIF16(u8 *fxpath);
void Create_SIF24(u8 *fxpath);
void Create_SIF36(u8 *fxpath);
#endif
