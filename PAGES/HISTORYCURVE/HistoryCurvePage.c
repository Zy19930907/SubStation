#include "HistoryCurvePage.h"
#include "DIALOG.h"
#include "rng.h"

_Page *HistoryCruvePage;

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_GRAPH_0 (GUI_ID_USER + 0x01)
#define ID_LISTWHEEL_0 (GUI_ID_USER + 0x02)

u32 TASKID_FRESHCURVE = 0xFFFFFFFF;
GRAPH_DATA_Handle ahData;
GRAPH_SCALE_Handle hScaleV;
GRAPH_SCALE_Handle hScaleH;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 64, 800, 416, 0, 0x0, 0 },
  { GRAPH_CreateIndirect, "Graph", ID_GRAPH_0, 0, 32, 800, 384, 0, 0x0, 0 },
  { LISTWHEEL_CreateIndirect, "Listwheel", ID_LISTWHEEL_0, 0, 0, 800, 32, 0, 0x0, 0 },
};

void FreshHistoryCurve(void)
{
	GRAPH_DATA_YT_AddValue(ahData, GetSensorValue(3)); //--------------（14）
//	GRAPH_DATA_YT_AddValue(ahData,RNG_Get_RandomRange(0,200)); //--------------（14）
}

static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int NCode;
  int Id;
  switch (pMsg->MsgId) {
	case WM_INIT_DIALOG:
    hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
    GRAPH_SetBorder(hItem, 30, 10, 10, 10);
	GRAPH_SetColor(hItem,GUI_BLACK,GRAPH_CI_BK);
	/* 创建数据对象 ***********************/
	ahData = GRAPH_DATA_YT_Create(GUI_WHITE, 1024, 0, 0); 
	GRAPH_AttachData(hItem, ahData);
	GRAPH_DATA_XY_SetLineStyle(ahData,GUI_LS_SOLID);
	GRAPH_DATA_XY_SetPenSize(ahData,20);
	/* 数据对象添加到图形控件 */
	
	/* 设置X、 Y 轴方向的栅格间距 */
	GRAPH_SetGridDistX(hItem, 100);
	GRAPH_SetGridDistY(hItem, 52);
	/* 固定 X 轴方向的栅格 */
	GRAPH_SetGridFixedX(hItem, 1); //--------------（7）
	/* 设置栅格可见 */
	GRAPH_SetGridVis(hItem, 1); //--------------（8）
	/* 创建刻度对象 ********************/
	hScaleV = GRAPH_SCALE_Create( 30, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 100); //--------------（9）
	/* 将垂直刻度对象添加到图形控件 */
	GRAPH_AttachScale(hItem, hScaleV); //--------------（10）
	/* 用于设置比例刻度的因子 */
	GRAPH_SCALE_SetFactor(hScaleV, 1); //--------------（11）
	/* 设置标签字体颜色 */
	GRAPH_SCALE_SetTextColor(hScaleV, GUI_RED); //--------------（12）
	
	hScaleH = GRAPH_SCALE_Create( 5, GUI_TA_RIGHT, GRAPH_SCALE_CF_HORIZONTAL, 100); //--------------（9）
	/* 将垂直刻度对象添加到图形控件 */
	GRAPH_AttachScale(hItem, hScaleH); //--------------（10）
	/* 用于设置比例刻度的因子 */
	GRAPH_SCALE_SetFactor(hScaleH, 0.1); //--------------（11）
	/* 设置标签字体颜色 */
	GRAPH_SCALE_SetTextColor(hScaleH, GUI_RED); //--------------（12）
	TASKID_FRESHCURVE = CreateTask("FreshCurve",0,0,0,0,5,FreshHistoryCurve);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
	LISTWHEEL_SetFont(hItem,&GUI_FontHZ32);
	LISTWHEEL_AddString(hItem,"1-CO-地面煤仓");
	LISTWHEEL_AddString(hItem,"2-CO-回风巷");
	WM_SetFocus(hItem);
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_LISTWHEEL_0: // Notifications sent by 'Listwheel'
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

WM_HWIN CreateHistoryCruveWindow(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

//-----------------------------------------------------------------------------------页面对按键的处理---------------------------------------------------------//
static WM_HWIN EnterPage(void)
{
	HistoryCruvePage->Id = CreateHistoryCruveWindow();
	return HistoryCruvePage->Id;
}

static void ExitPage(void)
{
	GUI_EndDialog(HistoryCruvePage->Id,1);
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
	DelTask(&TASKID_FRESHCURVE);
	PageJump(MainPage);
}

void HistoryCruvePageInit(void)
{
	HistoryCruvePage = GuiMalloc(sizeof(_Page));
	HistoryCruvePage->EnterPage = EnterPage;
	HistoryCruvePage->ExitPage = ExitPage;
	HistoryCruvePage->UpKeyDeal = UpKeyDeal;
	HistoryCruvePage->DownKeyDeal = DownKeyDeal;
	HistoryCruvePage->MoveKeyDeal = MoveKeyDeal;
	HistoryCruvePage->EnterKeyDeal = EnterKeyDeal;
	HistoryCruvePage->ExitKeyDeal = ExitKeyDeal;
	HistoryCruvePage->EnterKeyLongDeal = EnterKeyLongDeal;
}
/*************************** End of file ****************************/
