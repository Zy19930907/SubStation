#include "Key.h"

void KeyInit(void)
{
	KEYCLKEN;
	//上键
	IoInit(UPKEYPORT,UPKEYPIN,0);//输入模式
	//下键
	IoInit(DOWNKEYPORT,DOWNKEYPIN,0);//输入模式
	//确认键
	IoInit(CONFIRMKEYPORT,CONFIRMKEYPIN,0);//输入模式
	//退出键
	IoInit(EXITKEYPORT,EXITKEYPIN,0);//输入模式
	//MOVE键
	IoInit(MOVEKEYPORT,MOVEKEYPIN,0);//输入模式
	
	//对讲按键
	IoInit(TOLKKEYPORT,TOLKKEYPIN,0);//输入模式
	//广播按键
	IoInit(BOARDCASTPORT,BOARDCASTPIN,0);//输入模式
}
