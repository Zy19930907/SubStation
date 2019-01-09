#include "SysInfoPage.h"
#include "DIALOG.h"

_Page *SysInfoPage;

#define ID_WINDOW_0 (GUI_ID_USER + 0x40)
#define ID_IMAGE_0 (GUI_ID_USER + 0x41)
#define ID_TEXT_0 (GUI_ID_USER + 0x42)
#define ID_LISTVIEW_0 (GUI_ID_USER + 0x43)
u32 sysInfoListView;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = 
{
  { WINDOW_CreateIndirect, "SysInfoPage", ID_WINDOW_0, 0, 64, 800, 416, 0, 0x0, 0},
  { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0,	0, 0, 800, 416, 0, 0x0, 0 },
};

void ShowSysInfo(void)
{
	char infoList[2][50];
	char *showInfo[2];
	LISTVIEW_DeleteAllRows(sysInfoListView);
	strcpy(infoList[0],"IP地址");
	sprintf(infoList[1],"%d.%d.%d.%d",Net.localIp[0],Net.localIp[1],Net.localIp[2],Net.localIp[3]);
	showInfo[0] = infoList[0];
	showInfo[1] = infoList[1];
	LISTVIEW_AddRow(sysInfoListView,(GUI_ConstString*)showInfo);
	
	strcpy(infoList[0],"MAC地址");
	sprintf(infoList[1],"%02X:%02X:%02X:%02X:%02X:%02X",Net.MacAddr[0],Net.MacAddr[1],Net.MacAddr[2],Net.MacAddr[3],Net.MacAddr[4],Net.MacAddr[5]);
	showInfo[0] = infoList[0];
	showInfo[1] = infoList[1];
	LISTVIEW_AddRow(sysInfoListView,(GUI_ConstString*)showInfo);
	SetListViewRowBkColor(sysInfoListView);
}

static void _cbDialog(WM_MESSAGE * pMsg) 
{
	WM_HWIN hItem;
	int     NCode;
	int     Id;
	
	switch (pMsg->MsgId) 
	{
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
		LISTVIEW_SetFont(hItem,&GUI_FontHZ32);
		LISTVIEW_AddColumn(hItem, 400, "", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_SetGridVis(hItem, 1);
		LISTVIEW_AddColumn(hItem, 400, "", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_SetRowHeight(hItem, 32);
		LISTVIEW_SetHeaderHeight(hItem,0);
		sysInfoListView = hItem;
		ShowSysInfo();
    break;
	case WM_NOTIFY_PARENT:
		Id    = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch(Id) 
		{
		case ID_LISTVIEW_0: //Notifications sent by 'Listview'
			switch(NCode) 
			{
			case WM_NOTIFICATION_CLICKED:

			break;
			case WM_NOTIFICATION_RELEASED:

			break;
			case WM_NOTIFICATION_SEL_CHANGED:

			break;
			}
		break;
		}
	break;
	default:
		WM_DefaultProc(pMsg);
	break;
	}
}

WM_HWIN CreateSysInfoPage(void) 
{
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

//------------------------------------------------------页面对按键的处理---------------------------------------------------------//
static WM_HWIN EnterPage(void)
{
	SysInfoPage->Id = CreateSysInfoPage();
	return SysInfoPage->Id;
}

static void ExitPage(void)
{
	GUI_EndDialog(SysInfoPage->Id,1);
}
//上
static void UpKeyDeal(void)
{
	GUI_SendKeyMsg(GUI_KEY_UP,1);
}
//下
static void DownKeyDeal(void)
{
	GUI_SendKeyMsg(GUI_KEY_DOWN,1);
}
//确定
static void EnterKeyDeal(void)
{

}
//MOVE
static void MoveKeyDeal(void)
{

}
//确认
static void EnterKeyLongDeal()
{

}
//退出
static void ExitKeyDeal(void)
{
	PageJump(MainPage);
}

void SysInfoPageInit(void)
{
	SysInfoPage = GuiMalloc(sizeof(_Page));
	SysInfoPage->EnterPage = EnterPage;
	SysInfoPage->ExitPage = ExitPage;
	SysInfoPage->UpKeyDeal = UpKeyDeal;
	SysInfoPage->DownKeyDeal = DownKeyDeal;
	SysInfoPage->MoveKeyDeal = MoveKeyDeal;
	SysInfoPage->EnterKeyDeal = EnterKeyDeal;
	SysInfoPage->ExitKeyDeal = ExitKeyDeal;
	SysInfoPage->EnterKeyLongDeal = EnterKeyLongDeal;
}
/*************************** End of file ****************************/
