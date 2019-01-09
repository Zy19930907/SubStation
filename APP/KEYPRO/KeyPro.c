#include "KeyPro.h"
#include "Key.h"

_Key Key;
void KeyReset(void)
{
	Key.ScanCnt = 0;
	Key.Value[0] = 0;
	Key.Value[1] = 0;
}
//�����̰��¼�����
void KeyShortPressDeal(void)
{
	switch(Key.Value[0])
	{
		//��
		case 0x01:
			Gui_DownKeyDeal();
			break;
		//ȷ��
		case 0x02:
			Gui_EnterKeyDeal();
			break;
		//��
		case 0x04:
			Gui_UpKeyDeal();
			break;
		//�˳�
		case 0x08:
			Gui_ExitKeyDeal();
			break;
		//MOVE
		case 0x10:
			Gui_MoveKeyDeal();
			break;
	}
}
//���������¼�����
void KeyLongPressDeal(void)
{
	switch(Key.Value[0])
	{
		//��
		case 0x01:
			break;
		//ȷ��
		case 0x02:
			Gui_EnterKeyLongDeal();
			break;
		//��
		case 0x04:
			break;
		//�˳�
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
			if(val)//�а�ť����
				Key.Value[Key.ScanCnt++] =  val;//��¼��ֵ
			if(!val)//û�а�������
				KeyReset();
			//��������ɨ���ֵ��ͬ��Ϊ��������
			if((Key.ScanCnt >= 2) && (Key.Value[0] == Key.Value[1]))
			{
				Key.ScanCnt = 0;
				Key.Status = KEYPRESS;
				Key.Tick = SYS_TICK;
			}
			break;
		
		case KEYPRESS:
			if(!val)//��ֵΪ0����Ϊ�����ɿ�
				Key.Status = KEYUP;
			else if((val != Key.Value[0]) && (val != 0))//��ϼ����º�ֻ�ɿ����ְ���������Ϊ�����
				Key.Status = KEYIDLE;
			else if(MsTickDiff(Key.Tick) >= 1500)
			{
				Key.Status = KEYLONGPRESS;
				Key.LTick = SYS_TICK;
			}
			break;
			
		case KEYLONGPRESS://��������״̬
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
			if(MsTickDiff(Key.Tick) < 300)//����ʱ��С��1�룬�̰�
				Key.Status = KEYSHORT;
			else if(MsTickDiff(Key.Tick) >= 300 && MsTickDiff(Key.Tick) < 1500)
				Key.Status = KEYLONG;
			else //����ʱ�䳬��10�룬����Ϊ�����
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
