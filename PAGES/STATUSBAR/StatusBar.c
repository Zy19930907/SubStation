#include "DIALOG.h"
#include "StatusBar.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_IMAGE_0 (GUI_ID_USER + 0x01)
#define ID_ICONVIEW_0 (GUI_ID_USER + 0x02)
#define ID_TEXT_0 (GUI_ID_USER + 0x03)
#define ID_TEXT_1 (GUI_ID_USER + 0x04)
#define ID_PROGBAR_0 (GUI_ID_USER + 0x05)
#define ID_TEXT_2 (GUI_ID_USER + 0x06)
#define ID_TEXT_3 (GUI_ID_USER + 0x07)

_Page *StatusBar;

u32 titleHandle; 
u32 statusIconHandle;
u32 memUsedHandle;

u8 *BmpBuf_Logo;
u32 logoSize;

u8 *BmpBuf_NetBreak;
u32 netBreakSize;

u8 *BmpBuf_NetLink;
u32 netLinkSize;

u8 *BmpBuf_PcBreak;
u32 pcBreakSize;

u8 *BmpBuf_PcLink;
u32 pcLinkSize;

u8 *BmpBuf_SD;
u32 sdSize;

_IconView *StatusIconView;
u32 titleText;
u32 verText;
u32 memUsedText;
u32 memprogBar;
u32 rtcText;
u8 memCnt = 0;

u32 TASKID_FRESHUSED = 0xFFFFFFFF;

void FreshMemUsed(void)
{
	char used[50];
	u32 i;
	if(memCnt)
	{
		memCnt = 0;
		i = my_mem_perused(1);
		PROGBAR_SetMinMax(memprogBar,0,28912);
		PROGBAR_SetValue(memprogBar,i>>10);
		sprintf(used,"外部RAM已使用%d字节",i);
	}
	else
	{
		memCnt = 1;
		i = my_mem_perused(0);
		PROGBAR_SetMinMax(memprogBar,0,MEM1_MAX_SIZE);
		PROGBAR_SetValue(memprogBar,i);
		sprintf(used,"内部RAM已使用%d字节",i);
	}
	
	TEXT_SetText(memUsedText,used);
	TEXT_SetText(rtcText,Sys.RtcStr);
}
		
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = 
{
	{ WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 0, 800, 64, 0, 0x0, 0 },
	{ IMAGE_CreateIndirect, "Image", ID_IMAGE_0, 0, 0, 64, 64, 0, 0, 0 },
	{ ICONVIEW_CreateIndirect, "Iconview", ID_ICONVIEW_0, 64, 0, 736, 16, 0, 0x00100010, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_0, 64, 16, 736, 32, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_1, 64, 48, 180, 16, 0, 0x0, 0 },
	{ PROGBAR_CreateIndirect, "Progbar", ID_PROGBAR_0, 244, 48, 356, 16, 0, 0x0, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_2, 600, 48, 200, 16, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_3, 600, 0, 200, 16, 0, 0x0, 0 },
};

static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
	//主页面标题
	hItem = WM_GetDialogItem(pMsg->hWin,ID_TEXT_0);
	TEXT_SetFont(hItem,&GUI_FontHZ32);
	TEXT_SetTextColor(hItem,GUI_BLACK);
	TEXT_SetBkColor(hItem,GUI_DARKORANGE);
	TEXT_SetTextAlign(hItem,GUI_TA_HCENTER | GUI_TA_VCENTER);
	TEXT_SetText(hItem,"矿用本安型综合分站");
	titleText = hItem;
	//显示公司logo
	hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
	IMAGE_SetDTA(hItem,BmpBuf_Logo,logoSize);
	//状态栏Icon控件
	hItem = WM_GetDialogItem(pMsg->hWin,ID_ICONVIEW_0);
	ICONVIEW_SetFrame(hItem, GUI_COORD_X, 0);//设置图标到IconView边框的间距
	ICONVIEW_SetFrame(hItem, GUI_COORD_Y, 0);
	ICONVIEW_SetSpace(hItem, GUI_COORD_X, 0);//设置图标和图标之间的间距
	ICONVIEW_SetSpace(hItem, GUI_COORD_Y, 0);
	ICONVIEW_AddStreamedBitmapItem(hItem,BmpBuf_NetBreak,"");
	ICONVIEW_AddStreamedBitmapItem(hItem,BmpBuf_PcBreak,"");
	ICONVIEW_AddStreamedBitmapItem(hItem,BmpBuf_PcBreak,"");
	ICONVIEW_AddStreamedBitmapItem(hItem,BmpBuf_PcBreak,"");
	ICONVIEW_AddStreamedBitmapItem(hItem,BmpBuf_SD,"");
	StatusIconView->Id = hItem;
	
	//内存使用显示
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
	TEXT_SetBkColor(hItem,GUI_ORANGE);
	TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
	TEXT_SetFont(hItem,&GUI_FontHZ12);
	memUsedText = hItem;

	//软件版本
	hItem = WM_GetDialogItem(pMsg->hWin,ID_TEXT_2);
	TEXT_SetFont(hItem,&GUI_FontHZ12);
	TEXT_SetTextAlign(hItem,GUI_TA_HCENTER | GUI_TA_VCENTER);
	TEXT_SetBkColor(hItem,GUI_ORANGE);
	TEXT_SetText(hItem,Version);
	verText = hItem;
	//时钟显示控件
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
	TEXT_SetBkColor(hItem,GUI_BROWN);
	TEXT_SetTextColor(hItem,GUI_WHITE);
	TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
	TEXT_SetFont(hItem,&GUI_FontHZ16);
	rtcText = hItem;
	
	hItem = WM_GetDialogItem(pMsg->hWin,ID_PROGBAR_0);
	PROGBAR_SetFont(hItem,&GUI_FontHZ12);
	memprogBar = hItem;
	
	TASKID_FRESHUSED = CreateTask("状态栏刷新",0,0,0,1,0,FreshMemUsed);
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_ICONVIEW_0: // Notifications sent by 'statusicon'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_MOVED_OUT:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SCROLL_CHANGED:
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

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateWindow
*/
static WM_HWIN CreateStatusBar(void) {
  WM_HWIN hWin;
	unsigned int br;
	FIL *bmpfile;
	
	StatusIconView = GuiMalloc(sizeof(_IconView));
	bmpfile = GuiMalloc(sizeof(FIL));
	//公司logo
	f_open(bmpfile,"0:/综合分站图片/主界面/logo.dta",FA_READ);
	logoSize = bmpfile->obj.objsize;
	BmpBuf_Logo = GuiMalloc(logoSize);
	f_read(bmpfile,BmpBuf_Logo,logoSize,&br);
	f_close(bmpfile);
	//局域网断开图标
	f_open(bmpfile,"0:/综合分站图片/主界面/netbreak.dta",FA_READ);
	netBreakSize = bmpfile->obj.objsize;
	BmpBuf_NetBreak = GuiMalloc(netBreakSize);
	f_read(bmpfile,BmpBuf_NetBreak,netBreakSize,&br);
	f_close(bmpfile);
	//局域网连接图标
	f_open(bmpfile,"0:/综合分站图片/主界面/netlink.dta",FA_READ);
	netLinkSize = bmpfile->obj.objsize;
	BmpBuf_NetLink = GuiMalloc(netLinkSize);
	f_read(bmpfile,BmpBuf_NetLink,netLinkSize,&br);
	f_close(bmpfile);
	//SD卡状态图标
	f_open(bmpfile,"0:/综合分站图片/主界面/sdcard.dta",FA_READ);
	sdSize = bmpfile->obj.objsize;
	BmpBuf_SD = GuiMalloc(sdSize);
	f_read(bmpfile,BmpBuf_SD,sdSize,&br);
	f_close(bmpfile);
	//上位机连接状态图标--未连接
	f_open(bmpfile,"0:/综合分站图片/主界面/pcbreak.dta",FA_READ);
	pcBreakSize = bmpfile->obj.objsize;
	BmpBuf_PcBreak = GuiMalloc(pcBreakSize);
	f_read(bmpfile,BmpBuf_PcBreak,pcBreakSize,&br);
	f_close(bmpfile);
	
	//上位机连接状态图标--已连接
	f_open(bmpfile,"0:/综合分站图片/主界面/pc.dta",FA_READ);
	pcLinkSize = bmpfile->obj.objsize;
	BmpBuf_PcLink = GuiMalloc(pcLinkSize);
	f_read(bmpfile,BmpBuf_PcLink,pcLinkSize,&br);
	f_close(bmpfile);
	
	GuiFree(bmpfile);
	
	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
	return hWin;
}

void StatusBarInit(void)
{
	StatusBar = GuiMalloc(sizeof(_Page));
	StatusBar->Id = CreateStatusBar();
}

void SetStatusBarTitle(char *title)
{
	TEXT_SetText(titleText,title);
}

void SetNetLinkStatus(u8 netFlag)
{
	if(netFlag)
		ICONVIEW_SetStreamedBitmapItem(StatusIconView->Id,0,BmpBuf_NetLink);
	else
		ICONVIEW_SetStreamedBitmapItem(StatusIconView->Id,0,BmpBuf_NetBreak);
}

void SetPcLinkStatus(u8 index,u8 linkFlag)
{
	if(linkFlag)
		ICONVIEW_SetStreamedBitmapItem(StatusIconView->Id,index+1,BmpBuf_PcLink);
	else
		ICONVIEW_SetStreamedBitmapItem(StatusIconView->Id,index+1,BmpBuf_PcBreak);
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
