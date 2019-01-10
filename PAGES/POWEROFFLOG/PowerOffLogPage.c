#include "PowerOffLogPage.h"
#include "DIALOG.h"
#include "PowerOffLogBean.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_TREEVIEW_0 (GUI_ID_USER + 0x01)
#define ID_LISTVIEW_0 (GUI_ID_USER + 0x03)

_Page *PowerOffLogPage;
TREEVIEW_ITEM_Handle  rootNode;
_BreakLogFresh BreakLogFresh;
u32 TASKID_FRESHBREAKLOG = 0xFFFFFFFF;
GUI_HWIN poweroffLogListView;
GUI_HWIN logPathTree;
TREEVIEW_ITEM_Handle dateNodes[7];
const char pathList[3][50] = {"0:/Logs/BreakerLogs/","",""};

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 64, 800, 416, 0, 0x0, 0 },
  { TREEVIEW_CreateIndirect, "BreakLogPath", ID_TREEVIEW_0, 0, 0, 200, 416, 0, 0x0, 0 },
  { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0, 200, 0, 600, 416, 0, 0x0, 0 },
};

void FreshBreakPowerLog(void)
{
	char log[2000];
	u32 logLen;
	char time[30];
	char triggerInfo[100];
	char type[100];
	
	switch(BreakLogFresh.Status)
	{
		case BREAKLOGFRESHINIT:
			BreakLogFresh.logCnt = 0;
			LISTVIEW_DeleteAllRows(poweroffLogListView);
			BreakLogFresh.Status = BREAKLOGFRESHITEM;
			break;
		//刷新断电日志
		case BREAKLOGFRESHITEM:
			logLen = ReadBreakPowerLog(log);
			if((logLen != 0) && (BreakLogFresh.readSize < BreakLogFresh.logFileSize))
			{
				BreakLogFresh.readSize += logLen;
				LISTVIEW_AddRow(poweroffLogListView,NULL);
				JsonBytesToLogBean(log);
				GetPowerOffInfo(time,triggerInfo,type);
				LISTVIEW_SetItemText(poweroffLogListView,0,BreakLogFresh.logCnt,time);
				LISTVIEW_SetItemText(poweroffLogListView,1,BreakLogFresh.logCnt,triggerInfo);
				LISTVIEW_SetItemText(poweroffLogListView,2,BreakLogFresh.logCnt,type);
				DelatePowerOffLogBean();
				BreakLogFresh.logCnt++;
			}
			else
				BreakLogFresh.Status = BREAKLOGFRESHEND;
			break;
		//断电日志刷新完成	
		case BREAKLOGFRESHEND:
			SetListViewRowBkColor(poweroffLogListView);
			BreakLogFresh.Status = BREAKLOGFRESHINIT;
			DelTask(&TASKID_FRESHBREAKLOG);//删除刷新任务
			break;
	}
}

static void _cbDialog(WM_MESSAGE * pMsg) {
	WM_HWIN hItem;
	u32 	userData;
	HEADER_Handle hHeader;
	TREEVIEW_ITEM_Handle  hNode0,hNode1;
	DIR dir0,dir1;
	
	char datePath[50];
	char breakerPath[200];
	char logFilePath[50];
	
	u8 i=0,k=0;

	switch (pMsg->MsgId) {
	case WM_INIT_DIALOG:
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
		hHeader = LISTVIEW_GetHeader(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0));
		HEADER_SetFont(hHeader, &GUI_FontHZ16);
		LISTVIEW_SetFont(hItem,&GUI_FontHZ16);
		LISTVIEW_AddColumn(hItem, 80, "断电时间", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 100, "触发设备", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 420, "断电类型", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddRow(hItem, NULL);
		LISTVIEW_SetGridVis(hItem, 1);
		LISTVIEW_SetRowHeight(hItem, 32);
		LISTVIEW_SetHeaderHeight(hItem, 32);
		poweroffLogListView = hItem;
	
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TREEVIEW_0);
		TREEVIEW_SetFont(hItem,&GUI_FontHZ16);
		TREEVIEW_SetIndent(hItem,10);
		rootNode = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE,"断电记录",0);
		mf_opendirex(&dir0,BREAKERLOGPATH);
		TREEVIEW_AttachItem(hItem,rootNode,0,TREEVIEW_INSERT_FIRST_CHILD);
		TREEVIEW_ITEM_SetUserData(rootNode,0);//根节点标签---0
		TREEVIEW_SetSelMode(hItem,TREEVIEW_SELMODE_ROW);
		while(mf_getnextfilenameex(datePath,&dir0))
		{
			k = 0;
			userData = 0;
			dateNodes[i] = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE,datePath,0);
			if(i==0)
				TREEVIEW_AttachItem(hItem,dateNodes[i],rootNode,TREEVIEW_INSERT_FIRST_CHILD);
			else
				TREEVIEW_AttachItem(hItem,dateNodes[i],dateNodes[i-1],TREEVIEW_INSERT_BELOW);
			TREEVIEW_ITEM_SetUserData(dateNodes[i],1);//目录节点标签---1
			strcpy(breakerPath,pathList[0]);
			strcat(breakerPath,datePath);
			mf_opendirex(&dir1,breakerPath);
			while(mf_getnextfilenameex(logFilePath,&dir1))
			{
				hNode0 = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_LEAF,logFilePath,0);
				if(k==0)
					TREEVIEW_AttachItem(hItem,hNode0,dateNodes[i],TREEVIEW_INSERT_FIRST_CHILD);
				else
					TREEVIEW_AttachItem(hItem,hNode0,hNode1,TREEVIEW_INSERT_BELOW);
				userData |= ((i<<5) | 2);//标记日志文件父节点
				TREEVIEW_ITEM_SetUserData(hNode0,userData);//日志文件节点标签---2
				hNode1 = hNode0;
				k++;
			}
			i++;
		}
		WM_SetFocus(hItem);
		PowerOffLogPage->FocusItem = hItem;
		logPathTree = hItem;
    break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

WM_HWIN CreatePowerOffLogWindow(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

//-----------------------------------------------------------------------------------页面对按键的处理---------------------------------------------------------//
static WM_HWIN EnterPage(void)
{
	PowerOffLogPage->Id = CreatePowerOffLogWindow();
	return PowerOffLogPage->Id;
}

static void ExitPage(void)
{
	DelTask(&TASKID_FRESHBREAKLOG);
	GUI_EndDialog(PowerOffLogPage->Id,1);
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

	TREEVIEW_ITEM_Handle  hNode;
	u32 nodeType;
	u8 nodeIndex;
	char logPath[100];
	char date[50];
	hNode = TREEVIEW_GetSel(logPathTree);
	nodeType = TREEVIEW_ITEM_GetUserData(hNode);
	switch(nodeType & 0x0F)
	{
		case 0:
		case 1:
			if(nodeType & 0x10)//节点当前为展开状态
			{
				TREEVIEW_ITEM_Collapse(hNode);
				nodeType &= ~(1<<4);
			}
			else//
			{
				TREEVIEW_ITEM_Expand(hNode);
				nodeType |= (1<<4);
			}
			TREEVIEW_ITEM_SetUserData(hNode,nodeType);//目录节点节点标签---2
			break;
		case 2:
			strcpy(logPath,pathList[0]);
			nodeIndex = nodeType >> 5;
			TREEVIEW_ITEM_GetText(dateNodes[nodeIndex],(u8*)date,50);
			strcat(logPath,date);
			strcat(logPath,"/");
			TREEVIEW_ITEM_GetText(hNode,(u8*)date,50);
			strcat(logPath,date);
			OpenLogFile(logPath);
			BreakLogFresh.logFileSize = mf_size();
			BreakLogFresh.readSize = 0;
			BreakLogFresh.Status = BREAKLOGFRESHINIT;
			TASKID_FRESHBREAKLOG = CreateTask("FreshBreakLog",0,0,0,0,0,FreshBreakPowerLog);
			break;
	}
}
//MOVE
static void MoveKeyDeal(void)
{
	if(PowerOffLogPage->FocusItem == logPathTree)
	{
		TREEVIEW_SetBkColor(logPathTree,TREEVIEW_CI_SEL,GUI_GRAY_60);
		PowerOffLogPage->FocusItem = poweroffLogListView;
	}
	else if(PowerOffLogPage->FocusItem == poweroffLogListView)
	{
		TREEVIEW_SetBkColor(logPathTree,TREEVIEW_CI_SEL,GUI_BLUE);
		PowerOffLogPage->FocusItem = logPathTree;
	}
	WM_SetFocus(PowerOffLogPage->FocusItem);
}
static void EnterKeyLongDeal()
{
	if(PowerOffLogPage->FocusItem == logPathTree)
	{
		LISTVIEW_SetSel(poweroffLogListView,0);
		TREEVIEW_SetBkColor(logPathTree,TREEVIEW_CI_SEL,GUI_GRAY_60);
		PowerOffLogPage->FocusItem = poweroffLogListView;
	}
	else if(PowerOffLogPage->FocusItem == poweroffLogListView)
	{
		TREEVIEW_SetBkColor(logPathTree,TREEVIEW_CI_SEL,GUI_BLUE);
		PowerOffLogPage->FocusItem = logPathTree;
	}
	WM_SetFocus(PowerOffLogPage->FocusItem);
}
//退出
static void ExitKeyDeal(void)
{
	PageJump(MainPage);
}

void PowerOffLogPageInit(void)
{
	PowerOffLogPage = GuiMalloc(sizeof(_Page));
	PowerOffLogPage->EnterPage = EnterPage;
	PowerOffLogPage->ExitPage = ExitPage;
	PowerOffLogPage->UpKeyDeal = UpKeyDeal;
	PowerOffLogPage->DownKeyDeal = DownKeyDeal;
	PowerOffLogPage->MoveKeyDeal = MoveKeyDeal;
	PowerOffLogPage->EnterKeyDeal = EnterKeyDeal;
	PowerOffLogPage->ExitKeyDeal = ExitKeyDeal;
	PowerOffLogPage->EnterKeyLongDeal = EnterKeyLongDeal;
}
/*************************** End of file ****************************/
