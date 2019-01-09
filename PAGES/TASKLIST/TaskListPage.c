#include "TaskListPage.h"
#include "DIALOG.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_LISTVIEW_0 (GUI_ID_USER + 0x01)
_Page *TaskListPage;
u32 taskListView;
u32 TASKID_FRESHTASKLIST = 0xFFFFFFFF;

void FreshTaskList(void)
{
	ShowTaskInfo(taskListView);
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
    //
    // Initialization of 'Listview'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
	hHeader = LISTVIEW_GetHeader(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0));
	HEADER_SetFont(hHeader, &GUI_FontHZ16);
	LISTVIEW_SetFont(hItem, &GUI_FontHZ16);
    LISTVIEW_AddColumn(hItem, 240, "任务名", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 160, "任务状态", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_SetGridVis(hItem, 1);
    LISTVIEW_AddColumn(hItem, 80, "任务延时", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 80, "执行时间", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 110, "执行时间(MAX)", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 65, "执行间隔", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 65, "下次执行", GUI_TA_HCENTER | GUI_TA_VCENTER);
	LISTVIEW_SetRowHeight(hItem,18);
	taskListView = hItem;
	TASKID_FRESHTASKLIST = CreateTask("刷新任务列表",0,0,0,1,0,FreshTaskList);
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_LISTVIEW_0: // Notifications sent by 'Listview'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  // USER START (Optionally insert additional message handling)
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}


static WM_HWIN CreateWindow(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}
//-----------------------------------------------------------------------------------页面对按键的处理---------------------------------------------------------//
static WM_HWIN EnterPage(void)
{
	TaskListPage->Id = CreateWindow();
	return TaskListPage->Id;
}

static void ExitPage(void)
{
	GUI_EndDialog(TaskListPage->Id,1);
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
	GUI_EndDialog(TaskListPage->Id,1);
	DelTask(&TASKID_FRESHTASKLIST);
	PageJump(MainPage);
}

void TaskListPageInit(void)
{
	TaskListPage = GuiMalloc(sizeof(_Page));
	TaskListPage->EnterPage = EnterPage;
	TaskListPage->ExitPage = ExitPage;
	TaskListPage->UpKeyDeal = UpKeyDeal;
	TaskListPage->DownKeyDeal = DownKeyDeal;
	TaskListPage->MoveKeyDeal = MoveKeyDeal;
	TaskListPage->EnterKeyDeal = EnterKeyDeal;
	TaskListPage->EnterKeyLongDeal = EnterKeyLongDeal;
	TaskListPage->ExitKeyDeal = ExitKeyDeal;
}
/*************************** End of file ****************************/
