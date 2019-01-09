#include "LinkListPage.h"
#include "DIALOG.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_LISTVIEW_0 (GUI_ID_USER + 0x01)
u8 linkShowCnt = 0;
u32 linkListView;
u32	TASKID_RELATIONSHOW = 0xFFFFFFFF;
_Page *LinkListPage;

_RelationShow RelationShow;
void ShowRelationInfo(void)
{
	_Excute* Exc;
	char index[3];
	char TriggerInfo[100];
	char ExcuterInfo[100];
	char TypeInfo[100];
	GUI_COLOR color; 
	Exc = &Excute[RelationShow.showCnt];
	
	switch(RelationShow.Status)
	{
		case RELATIONSHOWINIT:
			LISTVIEW_DeleteAllRows(linkListView);
			RelationShow.showCnt = 0;
			RelationShow.Status = RELATIONSHOWITEM;
			break;
		
		case RELATIONSHOWITEM:
			if((Exc->ExcuteAddr == 0) || RelationShow.showCnt>= MaxTriggerNum)
				RelationShow.Status = RELATIONSHOWEND;
			else
			{
				if(GetLinkInfo(RelationShow.showCnt,TriggerInfo,ExcuterInfo,TypeInfo))
				{
					LISTVIEW_AddRow(linkListView,NULL);
					sprintf(index,"%d",RelationShow.showCnt+1);
					if(RelationShow.showCnt % 2)
						color = GUI_GRAY_C0;
					else
						color = GUI_GRAY_7C;
					LISTVIEW_SetItemBkColor(linkListView,0,RelationShow.showCnt,LISTVIEW_CI_UNSEL,color);
					LISTVIEW_SetItemBkColor(linkListView,1,RelationShow.showCnt,LISTVIEW_CI_UNSEL,color);
					LISTVIEW_SetItemBkColor(linkListView,2,RelationShow.showCnt,LISTVIEW_CI_UNSEL,color);
					LISTVIEW_SetItemBkColor(linkListView,3,RelationShow.showCnt,LISTVIEW_CI_UNSEL,color);
					LISTVIEW_SetItemText(linkListView,0,RelationShow.showCnt,index);
					LISTVIEW_SetItemText(linkListView,1,RelationShow.showCnt,TriggerInfo);
					LISTVIEW_SetItemText(linkListView,2,RelationShow.showCnt,ExcuterInfo);
					LISTVIEW_SetItemText(linkListView,3,RelationShow.showCnt,TypeInfo);
					RelationShow.showCnt++;
				}
				else
					RelationShow.Status = RELATIONSHOWEND;
			}
			break;
		
		case RELATIONSHOWEND:
			RelationShow.Status = RELATIONSHOWINIT;
			DelTask(&TASKID_RELATIONSHOW);
			break;
	}
}

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 64, 800, 416, 0, 0x0, 0 },
  { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0, 0, 0, 800, 416, 0, 0x0, 0 },
};

static void _cbDialog(WM_MESSAGE * pMsg) {
	WM_HWIN hItem;
	int     NCode;
	int     Id;
	HEADER_Handle hHeader;
 
	switch (pMsg->MsgId) {
	case WM_INIT_DIALOG:
	linkShowCnt = 0;
	hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
	LISTVIEW_SetFont(hItem,&GUI_FontHZ16);
	  
	hHeader = LISTVIEW_GetHeader(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0));
	HEADER_SetFont(hHeader, &GUI_FontHZ16);

	LISTVIEW_AddColumn(hItem, 50, "序号", GUI_TA_HCENTER | GUI_TA_VCENTER);
	LISTVIEW_AddColumn(hItem, 150, "触发设备", GUI_TA_HCENTER | GUI_TA_VCENTER);
	LISTVIEW_AddColumn(hItem, 150, "执行设备", GUI_TA_HCENTER | GUI_TA_VCENTER);
	LISTVIEW_AddColumn(hItem, 450, "触发类型", GUI_TA_HCENTER | GUI_TA_VCENTER);
	LISTVIEW_SetGridVis(hItem, 1);
	   
	LISTVIEW_SetRowHeight(hItem, 32);
	LISTVIEW_SetHeaderHeight(hItem, 32);
		
		
	linkListView = hItem;
	WM_SetFocus(hItem);
	TASKID_RELATIONSHOW = CreateTask("ShowLinkList",0,0,0,0,5,ShowRelationInfo);
	  
	break;
	case WM_NOTIFY_PARENT:
		Id    = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch(Id) {
		case ID_LISTVIEW_0: // Notifications sent by 'Listview'
		  switch(NCode) {
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

WM_HWIN CreateLinkListWindow(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}
//-----------------------------------------------------------------------------------页面对按键的处理---------------------------------------------------------//
static WM_HWIN EnterPage(void)
{
	LinkListPage->Id = CreateLinkListWindow();
	return LinkListPage->Id;
}

static void ExitPage(void)
{
	GUI_EndDialog(LinkListPage->Id,1);
	DelTask(&TASKID_RELATIONSHOW);
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
static void EnterKeyLongDeal()
{

}
//退出
static void ExitKeyDeal(void)
{
	DelTask(&TASKID_RELATIONSHOW);
	PageJump(MainPage);
}

void LinkListPageInit(void)
{
	LinkListPage = GuiMalloc(sizeof(_Page));
	LinkListPage->EnterPage = EnterPage;
	LinkListPage->ExitPage = ExitPage;
	LinkListPage->UpKeyDeal = UpKeyDeal;
	LinkListPage->DownKeyDeal = DownKeyDeal;
	LinkListPage->MoveKeyDeal = MoveKeyDeal;
	LinkListPage->EnterKeyDeal = EnterKeyDeal;
	LinkListPage->ExitKeyDeal = ExitKeyDeal;
	LinkListPage->EnterKeyLongDeal = EnterKeyLongDeal;
}
/*************************** End of file ****************************/
