#ifndef __PAGEMANGER_H
#define __PAGEMANGER_H

#include "Public.h"

typedef struct
{
	WM_HWIN prePage;
	WM_HWIN Id;
	GUI_HWIN FocusItem;
	void(*UpKeyDeal)(void);
	void(*DownKeyDeal)(void);
	void(*EnterKeyDeal)(void);
	void(*ExitKeyDeal)(void);
	void(*MoveKeyDeal)(void);
	void(*EnterKeyLongDeal)(void);
	WM_HWIN(*EnterPage)(void);
	void(*ExitPage)(void);
}_Page;

#include "MainPage.h"
#include "StatusBar.h"
#include "SysInfoPage.h"
#include "CurValuePage.h"
#include "LinkListPage.h"
#include "HistoryCurvePage.h"
#include "PowerOffLogPage.h"
#include "TaskListPage.h"
typedef struct
{
	int Id;
	u8 SelIndex;
	u8 MaxIcon;
}_IconView;

extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ12;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ16;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ24;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ32;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ16x2;
void LcdInit(void);
void *GuiMalloc(u32 size);
void GuiFree(void *ptr);
void PageJump(_Page *targetPage);
void Gui_EnterKeyDeal(void);
void Gui_ExitKeyDeal(void);
void Gui_UpKeyDeal(void);
void Gui_DownKeyDeal(void);
void Gui_MoveKeyDeal(void);
void Gui_EnterKeyLongDeal(void);
void SetListViewRowBkColor(u32 listview);

#endif
