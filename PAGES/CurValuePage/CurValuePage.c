#include "CurValuePage.h"
#include "DIALOG.h"

void FreshSensorValues(void);

#define DEVNAMELABELSIZE		130
#define DEVVALEDITSIZE			120

#define DEVROWY1START			10
#define DEVROWY2START			170
#define DEVYSIZE				25

#define DEVCLOUMN1X1			10
#define DEVCLOUMN1X2			DEVCLOUMN1X1 + DEVNAMELABELSIZE
#define DEVCLOUMN1RECTXEND		DEVCLOUMN1X2 + DEVVALEDITSIZE + 5

#define DEVCLOUMN2X1			DEVCLOUMN1X1 + DEVNAMELABELSIZE + DEVVALEDITSIZE + 15
#define DEVCLOUMN2X2			DEVCLOUMN2X1 + DEVNAMELABELSIZE
#define DEVCLOUMN2RECTXEND		DEVCLOUMN2X2 + DEVVALEDITSIZE + 5

#define DEVCLOUMN3X1			DEVCLOUMN2X1 + DEVNAMELABELSIZE + DEVVALEDITSIZE + 15
#define DEVCLOUMN3X2			DEVCLOUMN3X1 + DEVNAMELABELSIZE
#define DEVCLOUMN3RECTXEND		DEVCLOUMN3X2 + DEVVALEDITSIZE + 5

GUI_RECT CANSRECT[6];

#define SensorInfoFont &GUI_FontHZ16
_Page *CurValuePage;

u32 TASKID_FRESHSENSORSVALUE = 0xFFFFFFFF;

typedef struct
{
	WM_HWIN type;
	WM_HWIN value;
}_SensorInfo;

_SensorInfo SensorInfos[sizeof(_SensorInfo) * 36];

#define ID_WINDOW_0 (GUI_ID_USER + 0x3E)
#define ID_TEXT_0 (GUI_ID_USER + 0x3F)
#define ID_EDIT_0 (GUI_ID_USER + 0x40)
#define ID_TEXT_1 (GUI_ID_USER + 0x41)
#define ID_EDIT_1 (GUI_ID_USER + 0x42)
#define ID_TEXT_2 (GUI_ID_USER + 0x43)
#define ID_EDIT_2 (GUI_ID_USER + 0x44)
#define ID_TEXT_3 (GUI_ID_USER + 0x45)
#define ID_EDIT_3 (GUI_ID_USER + 0x46)
#define ID_TEXT_4 (GUI_ID_USER + 0x47)
#define ID_EDIT_4 (GUI_ID_USER + 0x48)
#define ID_TEXT_5 (GUI_ID_USER + 0x49)
#define ID_EDIT_5 (GUI_ID_USER + 0x4A)

#define ID_TEXT_6 (GUI_ID_USER + 0x4B)
#define ID_EDIT_6 (GUI_ID_USER + 0x4C)
#define ID_TEXT_7 (GUI_ID_USER + 0x4D)
#define ID_EDIT_7 (GUI_ID_USER + 0x4E)
#define ID_TEXT_8 (GUI_ID_USER + 0x4F)
#define ID_EDIT_8 (GUI_ID_USER + 0x50)
#define ID_TEXT_9 (GUI_ID_USER + 0x51)
#define ID_EDIT_9 (GUI_ID_USER + 0x52)
#define ID_TEXT_10 (GUI_ID_USER + 0x53)
#define ID_EDIT_10 (GUI_ID_USER + 0x54)
#define ID_TEXT_11 (GUI_ID_USER + 0x55)
#define ID_EDIT_11 (GUI_ID_USER + 0x56)

#define ID_TEXT_12 (GUI_ID_USER + 0x57)
#define ID_EDIT_12 (GUI_ID_USER + 0x58)
#define ID_TEXT_13 (GUI_ID_USER + 0x59)
#define ID_EDIT_13 (GUI_ID_USER + 0x5A)
#define ID_TEXT_14 (GUI_ID_USER + 0x5B)
#define ID_EDIT_14 (GUI_ID_USER + 0x5C)
#define ID_TEXT_15 (GUI_ID_USER + 0x5D)
#define ID_EDIT_15 (GUI_ID_USER + 0x5E)
#define ID_TEXT_16 (GUI_ID_USER + 0x5F)
#define ID_EDIT_16 (GUI_ID_USER + 0x60)
#define ID_TEXT_17 (GUI_ID_USER + 0x61)
#define ID_EDIT_17 (GUI_ID_USER + 0x62)

#define ID_TEXT_18 (GUI_ID_USER + 0x63)
#define ID_EDIT_18 (GUI_ID_USER + 0x64)
#define ID_TEXT_19 (GUI_ID_USER + 0x65)
#define ID_EDIT_19 (GUI_ID_USER + 0x66)
#define ID_TEXT_20 (GUI_ID_USER + 0x67)
#define ID_EDIT_20 (GUI_ID_USER + 0x68)
#define ID_TEXT_21 (GUI_ID_USER + 0x69)
#define ID_EDIT_21 (GUI_ID_USER + 0x6A)
#define ID_TEXT_22 (GUI_ID_USER + 0x6B)
#define ID_EDIT_22 (GUI_ID_USER + 0x6C)
#define ID_TEXT_23 (GUI_ID_USER + 0x6D)
#define ID_EDIT_23 (GUI_ID_USER + 0x6E)

#define ID_TEXT_24 (GUI_ID_USER + 0x6F)
#define ID_EDIT_24 (GUI_ID_USER + 0x70)
#define ID_TEXT_25 (GUI_ID_USER + 0x71)
#define ID_EDIT_25 (GUI_ID_USER + 0x72)
#define ID_TEXT_26 (GUI_ID_USER + 0x73)
#define ID_EDIT_26 (GUI_ID_USER + 0x74)
#define ID_TEXT_27 (GUI_ID_USER + 0x75)
#define ID_EDIT_27 (GUI_ID_USER + 0x76)
#define ID_TEXT_28 (GUI_ID_USER + 0x77)
#define ID_EDIT_28 (GUI_ID_USER + 0x78)
#define ID_TEXT_29 (GUI_ID_USER + 0x79)
#define ID_EDIT_29 (GUI_ID_USER + 0x7A)

#define ID_TEXT_30 (GUI_ID_USER + 0x7B)
#define ID_EDIT_30 (GUI_ID_USER + 0x7C)
#define ID_TEXT_31 (GUI_ID_USER + 0x7D)
#define ID_EDIT_31 (GUI_ID_USER + 0x7E)
#define ID_TEXT_32 (GUI_ID_USER + 0x7F)
#define ID_EDIT_32 (GUI_ID_USER + 0x80)
#define ID_TEXT_33 (GUI_ID_USER + 0x81)
#define ID_EDIT_33 (GUI_ID_USER + 0x82)
#define ID_TEXT_34 (GUI_ID_USER + 0x83)
#define ID_EDIT_34 (GUI_ID_USER + 0x84)
#define ID_TEXT_35 (GUI_ID_USER + 0x85)
#define ID_EDIT_35 (GUI_ID_USER + 0x86)
#define ID_LISTVIEW_0 (GUI_ID_USER + 0x87)


static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 64, 800, 416, 0, 0x0, 0 },

  { TEXT_CreateIndirect, "Text", ID_TEXT_0, DEVCLOUMN1X1, DEVROWY1START, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_0, DEVCLOUMN1X2, DEVROWY1START, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_1, DEVCLOUMN1X1, DEVROWY1START + DEVYSIZE*1, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_1, DEVCLOUMN1X2, DEVROWY1START + DEVYSIZE*1, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_2, DEVCLOUMN1X1, DEVROWY1START + DEVYSIZE*2, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_2, DEVCLOUMN1X2, DEVROWY1START + DEVYSIZE*2, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_3, DEVCLOUMN1X1, DEVROWY1START + DEVYSIZE*3, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_3, DEVCLOUMN1X2, DEVROWY1START + DEVYSIZE*3, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_4, DEVCLOUMN1X1, DEVROWY1START + DEVYSIZE*4, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_4, DEVCLOUMN1X2, DEVROWY1START + DEVYSIZE*4, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_5, DEVCLOUMN1X1, DEVROWY1START + DEVYSIZE*5, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_5, DEVCLOUMN1X2, DEVROWY1START + DEVYSIZE*5, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  
  { TEXT_CreateIndirect, "Text", ID_TEXT_6, DEVCLOUMN2X1, DEVROWY1START, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_6, DEVCLOUMN2X2, DEVROWY1START, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_7, DEVCLOUMN2X1, DEVROWY1START + DEVYSIZE*1, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_7, DEVCLOUMN2X2, DEVROWY1START + DEVYSIZE*1, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_8, DEVCLOUMN2X1, DEVROWY1START + DEVYSIZE*2, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_8, DEVCLOUMN2X2, DEVROWY1START + DEVYSIZE*2, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_9, DEVCLOUMN2X1, DEVROWY1START + DEVYSIZE*3, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_9, DEVCLOUMN2X2, DEVROWY1START + DEVYSIZE*3, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_10, DEVCLOUMN2X1, DEVROWY1START + DEVYSIZE*4, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_10, DEVCLOUMN2X2, DEVROWY1START + DEVYSIZE*4, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_11, DEVCLOUMN2X1, DEVROWY1START + DEVYSIZE*5, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_11, DEVCLOUMN2X2, DEVROWY1START + DEVYSIZE*5, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  
  { TEXT_CreateIndirect, "Text", ID_TEXT_12, DEVCLOUMN3X1, DEVROWY1START, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_12, DEVCLOUMN3X2, DEVROWY1START, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_13, DEVCLOUMN3X1, DEVROWY1START + DEVYSIZE*1, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_13, DEVCLOUMN3X2, DEVROWY1START + DEVYSIZE*1, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_14, DEVCLOUMN3X1, DEVROWY1START + DEVYSIZE*2, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_14, DEVCLOUMN3X2, DEVROWY1START + DEVYSIZE*2, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_15, DEVCLOUMN3X1, DEVROWY1START + DEVYSIZE*3, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_15, DEVCLOUMN3X2, DEVROWY1START + DEVYSIZE*3, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_16, DEVCLOUMN3X1, DEVROWY1START + DEVYSIZE*4, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_16, DEVCLOUMN3X2, DEVROWY1START + DEVYSIZE*4, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_17, DEVCLOUMN3X1, DEVROWY1START + DEVYSIZE*5, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_17, DEVCLOUMN3X2, DEVROWY1START + DEVYSIZE*5, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  
  { TEXT_CreateIndirect, "Text", ID_TEXT_18, DEVCLOUMN1X1, DEVROWY2START, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_18, DEVCLOUMN1X2, DEVROWY2START, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_19, DEVCLOUMN1X1, DEVROWY2START + DEVYSIZE*1, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_19, DEVCLOUMN1X2, DEVROWY2START + DEVYSIZE*1, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_20, DEVCLOUMN1X1, DEVROWY2START + DEVYSIZE*2, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_20, DEVCLOUMN1X2, DEVROWY2START + DEVYSIZE*2, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_21, DEVCLOUMN1X1, DEVROWY2START + DEVYSIZE*3, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_21, DEVCLOUMN1X2, DEVROWY2START + DEVYSIZE*3, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_22, DEVCLOUMN1X1, DEVROWY2START + DEVYSIZE*4, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_22, DEVCLOUMN1X2, DEVROWY2START + DEVYSIZE*4, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_23, DEVCLOUMN1X1, DEVROWY2START + DEVYSIZE*5, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_23, DEVCLOUMN1X2, DEVROWY2START + DEVYSIZE*5, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  
  { TEXT_CreateIndirect, "Text", ID_TEXT_24, DEVCLOUMN2X1, DEVROWY2START, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_24, DEVCLOUMN2X2, DEVROWY2START, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_25, DEVCLOUMN2X1, DEVROWY2START + DEVYSIZE*1, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_25, DEVCLOUMN2X2, DEVROWY2START + DEVYSIZE*1, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_26, DEVCLOUMN2X1, DEVROWY2START + DEVYSIZE*2, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_26, DEVCLOUMN2X2, DEVROWY2START + DEVYSIZE*2, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_27, DEVCLOUMN2X1, DEVROWY2START + DEVYSIZE*3, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_27, DEVCLOUMN2X2, DEVROWY2START + DEVYSIZE*3, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_28, DEVCLOUMN2X1, DEVROWY2START + DEVYSIZE*4, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_28, DEVCLOUMN2X2, DEVROWY2START + DEVYSIZE*4, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_29, DEVCLOUMN2X1, DEVROWY2START + DEVYSIZE*5, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_29, DEVCLOUMN2X2, DEVROWY2START + DEVYSIZE*5, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  
  { TEXT_CreateIndirect, "Text", ID_TEXT_30, DEVCLOUMN3X1, DEVROWY2START, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_30, DEVCLOUMN3X2, DEVROWY2START, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_31, DEVCLOUMN3X1, DEVROWY2START + DEVYSIZE*1, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_31, DEVCLOUMN3X2, DEVROWY2START + DEVYSIZE*1, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_32, DEVCLOUMN3X1, DEVROWY2START + DEVYSIZE*2, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_32, DEVCLOUMN3X2, DEVROWY2START + DEVYSIZE*2, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_33, DEVCLOUMN3X1, DEVROWY2START + DEVYSIZE*3, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_33, DEVCLOUMN3X2, DEVROWY2START + DEVYSIZE*3, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_34, DEVCLOUMN3X1, DEVROWY2START + DEVYSIZE*4, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_34, DEVCLOUMN3X2, DEVROWY2START + DEVYSIZE*4, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_35, DEVCLOUMN3X1, DEVROWY2START + DEVYSIZE*5, DEVNAMELABELSIZE, 20, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_35, DEVCLOUMN3X2, DEVROWY2START + DEVYSIZE*5, DEVVALEDITSIZE, 20, 0, 0x64, 0 },
  { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0, 5, 325, 791, 90, 0, 0x0, 0 },
};
static void _cbDialog(WM_MESSAGE * pMsg) {
    WM_HWIN hItem;
	u8		i;
	HEADER_Handle hHeader;
	_SensorInfo *info;
	
    switch (pMsg->MsgId) 
	{
    case WM_INIT_DIALOG:
        hItem = pMsg->hWin;
        FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		
		CANSRECT[0].x0 = DEVCLOUMN1X1 - 5;
		CANSRECT[0].y0 = DEVROWY1START - 5;
		CANSRECT[0].x1 = DEVCLOUMN1RECTXEND;
		CANSRECT[0].y1 = DEVROWY2START - 10;
	
		CANSRECT[1].x0 = DEVCLOUMN2X1 - 5;
		CANSRECT[1].y0 = DEVROWY1START - 5;
		CANSRECT[1].x1 = DEVCLOUMN2RECTXEND;
		CANSRECT[1].y1 = DEVROWY2START - 10;
	
		CANSRECT[2].x0 = DEVCLOUMN3X1 - 5;
		CANSRECT[2].y0 = DEVROWY1START - 5;
		CANSRECT[2].x1 = DEVCLOUMN3RECTXEND;
		CANSRECT[2].y1 = DEVROWY2START-10;
		
		CANSRECT[3].x0 = DEVCLOUMN1X1 - 5;
		CANSRECT[3].y0 = DEVROWY2START - 5;
		CANSRECT[3].x1 = DEVCLOUMN1RECTXEND;
		CANSRECT[3].y1 = 320;
	
		CANSRECT[4].x0 = DEVCLOUMN2X1 - 5;
		CANSRECT[4].y0 = DEVROWY2START - 5;
		CANSRECT[4].x1 = DEVCLOUMN2RECTXEND;
		CANSRECT[4].y1 = 320;
	
		CANSRECT[5].x0 = DEVCLOUMN3X1 - 5;
		CANSRECT[5].y0 = DEVROWY2START - 5;
		CANSRECT[5].x1 = DEVCLOUMN3RECTXEND;
		CANSRECT[5].y1 = 320;
		
		for(i=0;i<36;i++)
		{
			info = (_SensorInfo*)&SensorInfos[i*sizeof(_SensorInfo)];
			//传感器类型
			hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_USER + 2*i +0x3F);
			TEXT_SetFont(hItem,SensorInfoFont);
			TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
			info->type = hItem;
			//传感器监测值
			hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_USER + 2*i+1 + 0x3F);
			EDIT_SetTextColor(hItem,EDIT_CI_ENABELD,GUI_WHITE);
			EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
			EDIT_SetFont(hItem,SensorInfoFont);
			EDIT_SetFocusable(hItem,0);
			info->value = hItem;
		}
		FreshSensorValues();
		TASKID_FRESHSENSORSVALUE = CreateTask("FreshCurValue",0,0,0,1,0,FreshSensorValues);
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
		hHeader = LISTVIEW_GetHeader(hItem);
		HEADER_SetFont(hHeader, &GUI_FontHZ12);
		HEADER_SetTextColor(hHeader,GUI_RED);
		LISTVIEW_SetFont(hItem, SensorInfoFont);
		LISTVIEW_AddColumn(hItem, 60, "序号", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 120, "异常类型", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 611, "异常设备安装地址", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_SetGridVis(hItem, 1);
	   
		LISTVIEW_SetRowHeight(hItem, 22);
		LISTVIEW_SetHeaderHeight(hItem, 20);
		LISTVIEW_AddRow(hItem,NULL);
		LISTVIEW_AddRow(hItem,NULL);
		
		LISTVIEW_SetTextColor(hItem,LISTVIEW_CI_UNSEL,GUI_RED);
		LISTVIEW_SetItemText(hItem,0,0,"1");
		LISTVIEW_SetItemText(hItem,1,0,"设备断线");
		LISTVIEW_SetItemText(hItem,2,0,"机头变电所2号电源");
		
		LISTVIEW_SetItemText(hItem,0,1,"2");
		LISTVIEW_SetItemText(hItem,1,1,"设备超限");
		LISTVIEW_SetItemText(hItem,2,1,"掘进工作面3号激光甲烷");
        break;
	case WM_PAINT:
		GUI_SetColor(GUI_BLACK);
		for(i=0;i<6;i++)
			GUI_DrawRectEx(&CANSRECT[i]);
//		GUI_SetFont(&GUI_FontHZ12);
//		GUI_DispStringAt("总线1",DEVCLOUMN1X1+5,5);
//		GUI_DispStringAt("总线2",DEVCLOUMN2X1+5,5);
//		GUI_DispStringAt("总线3",DEVCLOUMN3X1+5,5);
//		GUI_DispStringAt("总线4",DEVCLOUMN1X1+5,175);
//		GUI_DispStringAt("总线5",DEVCLOUMN2X1+5,175);
//		GUI_DispStringAt("总线6",DEVCLOUMN3X1+5,175);
	
//		GUI_DispStringAt("CAN-1",DEVCLOUMN1X1+5,5);
//		GUI_DispStringAt("CAN-2",DEVCLOUMN2X1+5,5);
//		GUI_DispStringAt("CAN-3",DEVCLOUMN3X1+5,5);
//		GUI_DispStringAt("CAN-4",DEVCLOUMN1X1+5,175);
//		GUI_DispStringAt("CAN-5",DEVCLOUMN2X1+5,175);
//		GUI_DispStringAt("CAN-6",DEVCLOUMN3X1+5,175);
		break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

WM_HWIN CreateCurValuePage(void) 
{
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    return hWin;
}

void FreshSensorValues(void)
{
	u8 i;
	u8 color;
	_SensorInfo *info;
	
	char name[20];
	char value[50];

	for(i=0;i<36;i++)
	{
		color = 0xFF;
		info = (_SensorInfo*)&SensorInfos[i*sizeof(_SensorInfo)];
		MakeSensorInfo_Name_Val(i,name,value,&color);
		switch(color)
		{
			case 0x00://上位机已定义但从未收到传感器信息
				EDIT_SetTextColor(info->value,EDIT_CI_ENABLED,GUI_WHITE);//文字白色
				EDIT_SetBkColor(info->value,EDIT_CI_ENABLED,GUI_DARKYELLOW);//背景橘黄色
				break;
			case 0x01://传感器断线
				EDIT_SetTextColor(info->value,EDIT_CI_ENABLED,GUI_BLACK);//文字黑色
				EDIT_SetBkColor(info->value,EDIT_CI_ENABLED,GUI_LIGHTRED);//背景浅红色
				break;
			case 0x02://传感器通讯正常且未超限
				EDIT_SetTextColor(info->value,EDIT_CI_ENABLED,GUI_WHITE);//文字白色
				EDIT_SetBkColor(info->value,EDIT_CI_ENABLED,GUI_DARKGREEN);//背景绿色
				break;
			case 0x03://传感器通信正常但超限
				EDIT_SetTextColor(info->value,EDIT_CI_ENABLED,GUI_WHITE);//文字白色
				EDIT_SetBkColor(info->value,EDIT_CI_ENABLED,GUI_DARKRED);//背景深红色
				strcat(value,"↑");
				break;
			case 0x04://上位机未定义
				EDIT_SetTextColor(info->value,EDIT_CI_ENABLED,GUI_BLACK);//文字黑色
				EDIT_SetBkColor(info->value,EDIT_CI_ENABLED,GUI_DARKGRAY);//背景灰色
				break;
		}
		TEXT_SetText(info->type,name);
		EDIT_SetText(info->value,value);
	}
}
//-----------------------------------------------------------------------------------页面对按键的处理---------------------------------------------------------//
static WM_HWIN EnterPage(void)
{
	CurValuePage->Id = CreateCurValuePage();
	return CurValuePage->Id;
}

static void ExitPage(void)
{
	GUI_EndDialog(CurValuePage->Id,1);
	DelTask(&TASKID_FRESHSENSORSVALUE);
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
	PageJump(MainPage);
}

void CurValuePageInit(void)
{
	CurValuePage = GuiMalloc(sizeof(_Page));
	CurValuePage->EnterPage = EnterPage;
	CurValuePage->ExitPage = ExitPage;
	CurValuePage->UpKeyDeal = UpKeyDeal;
	CurValuePage->DownKeyDeal = DownKeyDeal;
	CurValuePage->MoveKeyDeal = MoveKeyDeal;
	CurValuePage->EnterKeyDeal = EnterKeyDeal;
	CurValuePage->ExitKeyDeal = ExitKeyDeal;
	CurValuePage->EnterKeyLongDeal = EnterKeyLongDeal;
}
/*************************** End of file ****************************/
