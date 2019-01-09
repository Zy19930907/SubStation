#include "PageManger.h"
#include "dialog.h"
#include "GUI.h"
#include "LcdSpi.h"


#include "tftlcd.h"
#include "fontupd.h"
#include "JsonTest.h"


_Page *CurPage;

extern void GUI_UC_SetEncodeGBK(void);

void *GuiMalloc(u32 size)  
{
	return mymalloc(SRAMEX,size);
}

void GuiFree(void *ptr)
{
	myfree(SRAMEX,ptr);
}

void SetListViewRowBkColor(u32 listview)
{
	u8 row,rowcnt,i;
	u16 clumncnt;
	GUI_COLOR color; 
	rowcnt = LISTVIEW_GetNumRows(listview);
	clumncnt = LISTVIEW_GetNumColumns(listview);
	for(row=0;row<rowcnt;row++)
	{
		if(row % 2)
			color = GUI_GRAY_C0;
		else
			color = GUI_GRAY_7C;
		for(i=0;i<clumncnt;i++)
			LISTVIEW_SetItemBkColor(listview,i,row,LISTVIEW_CI_UNSEL,color);
	}
}

void LcdInit(void)
{
	RCC->AHB1ENR |= 1<<11;//使能CRC时钟
	
	TFT_Init();
	LCD_Clear(GRAYBLUE);
	font_init();//加载16号字库
	WM_SetCreateFlags(WM_CF_MEMDEV);
	GUI_Init();			//初始化emWin
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX); 
	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
	MENU_SetDefaultSkin(MENU_SKIN_FLEX);
	MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
	SPINBOX_SetDefaultSkin(SPINBOX_SKIN_FLEX);
	GUI_SetBkColor(GUI_DARKCYAN);
	GUI_Clear();
	GUI_SetFont(&GUI_FontHZ24);
	StatusBarInit();
	MainPageInit();
	SysInfoPageInit();
	CurValuePageInit();
	LinkListPageInit();
	HistoryCruvePageInit();
	PowerOffLogPageInit();
	TaskListPageInit();

	MainPage->Id = CreateMainPage();
	CurPage = MainPage;
	WM_Deactivate();
	WM_Activate();
}
void Gui_UpKeyDeal(void)
{
	CurPage->UpKeyDeal();
}

void Gui_DownKeyDeal(void)
{
	CurPage->DownKeyDeal();
}

void Gui_EnterKeyDeal(void)
{
	CurPage->EnterKeyDeal();
}

void Gui_ExitKeyDeal(void)
{
	CurPage->ExitKeyDeal();
}

void Gui_MoveKeyDeal(void)
{
	CurPage->MoveKeyDeal();
}

void Gui_EnterKeyLongDeal(void)
{
	CurPage->EnterKeyLongDeal();
}
//页面跳转
void PageJump(_Page *targetPage)
{
	CurPage->ExitPage();//退出当前页面
	targetPage->EnterPage();//进入目标界面
	targetPage->prePage = CurPage->Id;//目标界面的上一界面设置为当前界面
	CurPage = targetPage;//更新当前界面为目标界面
}
