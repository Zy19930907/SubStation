#include "KeyPro.h"
#include "Key.h"

_Key Key;
void KeyReset(void)
{
	Key.ScanCnt = 0;
	Key.Value[0] = 0;
	Key.Value[1] = 0;
}
//按键短按事件处理
void KeyShortPressDeal(void)
{
	switch(Key.Value[0])
	{
		//下
		case 0x01:
			Gui_DownKeyDeal();
			break;
		//确认
		case 0x02:
			Gui_EnterKeyDeal();
			break;
		//上
		case 0x04:
			Gui_UpKeyDeal();
			break;
		//退出
		case 0x08:
			Gui_ExitKeyDeal();
			break;
		//MOVE
		case 0x10:
			Gui_MoveKeyDeal();
			break;
	}
}
//按键长按事件处理
void KeyLongPressDeal(void)
{
	switch(Key.Value[0])
	{
		//下
		case 0x01:
			break;
		//确认
		case 0x02:
			Gui_EnterKeyLongDeal();
			break;
		//上
		case 0x04:
			break;
		//退出
		case 0x08:
			break;
	}
}

void KeyPro(void)
{
	u8 val;
	val = KEYSVALUE;
	switch(Key.Status)
	{
		case KEYINIT:
			KeyInit();
			Key.Status = KEYIDLE;
			break;
		
		case KEYIDLE:
			if(val)//有按钮按下
				Key.Value[Key.ScanCnt++] =  val;//记录键值
			if(!val)//没有按键按下
				KeyReset();
			//连续两次扫描键值相同认为按键按下
			if((Key.ScanCnt >= 2) && (Key.Value[0] == Key.Value[1]))
			{
				Key.ScanCnt = 0;
				Key.Status = KEYPRESS;
				Key.Tick = SYS_TICK;
			}
			break;
		
		case KEYPRESS:
			if(!val)//键值为0则认为按键松开
				Key.Status = KEYUP;
			else if((val != Key.Value[0]) && (val != 0))//组合键按下后只松开部分按键，处理为误操作
				Key.Status = KEYIDLE;
			else if(MsTickDiff(Key.Tick) >= 1500)
			{
				Key.Status = KEYLONGPRESS;
				Key.LTick = SYS_TICK;
			}
			break;
			
		case KEYLONGPRESS://长按保持状态
			if(val == Key.Value[0])
			{
				if(MsTickDiff(Key.LTick) >= 50)
				{
					KeyShortPressDeal();
					Key.LTick = SYS_TICK;
				}
			}else
				Key.Status = KEYIDLE;
			break;
		
		case KEYUP:
			if(MsTickDiff(Key.Tick) < 300)//按下时间小于1秒，短按
				Key.Status = KEYSHORT;
			else if(MsTickDiff(Key.Tick) >= 300 && MsTickDiff(Key.Tick) < 1500)
				Key.Status = KEYLONG;
			else //按下时间超过10秒，处理为误操作
				Key.Status = KEYIDLE;
			break;
		
		case KEYSHORT:
			KeyShortPressDeal();
			Key.Status = KEYIDLE;
			break;
		
		case KEYLONG:
			KeyLongPressDeal();
			Key.Status = KEYIDLE;
			break;
	}
}
