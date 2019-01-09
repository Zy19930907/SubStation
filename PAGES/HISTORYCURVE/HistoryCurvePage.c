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
	GRAPH_DATA_YT_AddValue(ahData, GetSensorValue(3)); //--------------��14��
//	GRAPH_DATA_YT_AddValue(ahData,RNG_Get_RandomRange(0,200)); //--------------��14��
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
	/* �������ݶ��� ***********************/
	ahData = GRAPH_DATA_YT_Create(GUI_WHITE, 1024, 0, 0); 
	GRAPH_AttachData(hItem, ahData);
	GRAPH_DATA_XY_SetLineStyle(ahData,GUI_LS_SOLID);
	GRAPH_DATA_XY_SetPenSize(ahData,20);
	/* ���ݶ�����ӵ�ͼ�οؼ� */
	
	/* ����X�� Y �᷽���դ���� */
	GRAPH_SetGridDistX(hItem, 100);
	GRAPH_SetGridDistY(hItem, 52);
	/* �̶� X �᷽���դ�� */
	GRAPH_SetGridFixedX(hItem, 1); //--------------��7��
	/* ����դ��ɼ� */
	GRAPH_SetGridVis(hItem, 1); //--------------��8��
	/* �����̶ȶ��� ********************/
	hScaleV = GRAPH_SCALE_Create( 30, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 100); //--------------��9��
	/* ����ֱ�̶ȶ�����ӵ�ͼ�οؼ� */
	GRAPH_AttachScale(hItem, hScaleV); //--------------��10��
	/* �������ñ����̶ȵ����� */
	GRAPH_SCALE_SetFactor(hScaleV, 1); //--------------��11��
	/* ���ñ�ǩ������ɫ */
	GRAPH_SCALE_SetTextColor(hScaleV, GUI_RED); //--------------��12��
	
	hScaleH = GRAPH_SCALE_Create( 5, GUI_TA_RIGHT, GRAPH_SCALE_CF_HORIZONTAL, 100); //--------------��9��
	/* ����ֱ�̶ȶ�����ӵ�ͼ�οؼ� */
	GRAPH_AttachScale(hItem, hScaleH); //--------------��10��
	/* �������ñ����̶ȵ����� */
	GRAPH_SCALE_SetFactor(hScaleH, 0.1); //--------------��11��
	/* ���ñ�ǩ������ɫ */
	GRAPH_SCALE_SetTextColor(hScaleH, GUI_RED); //--------------��12��
	TASKID_FRESHCURVE = CreateTask("FreshCurve",0,0,0,0,5,FreshHistoryCurve);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
	LISTWHEEL_SetFont(hItem,&GUI_FontHZ32);
	LISTWHEEL_AddString(hItem,"1-CO-����ú��");
	LISTWHEEL_AddString(hItem,"2-CO-�ط���");
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

//-----------------------------------------------------------------------------------ҳ��԰����Ĵ���---------------------------------------------------------//
static WM_HWIN EnterPage(void)
{
	HistoryCruvePage->Id = CreateHistoryCruveWindow();
	return HistoryCruvePage->Id;
}

static void ExitPage(void)
{
	GUI_EndDialog(HistoryCruvePage->Id,1);
}
//��
static void UpKeyDeal(void)
{
	GUI_SendKeyMsg(GUI_KEY_UP,1);
}
//��
static void DownKeyDeal(void)
{
	GUI_SendKeyMsg(GUI_KEY_DOWN,1);
}
//ȷ��
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
//�˳�
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
