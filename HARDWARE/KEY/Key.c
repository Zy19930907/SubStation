#include "Key.h"

void KeyInit(void)
{
	KEYCLKEN;
	//�ϼ�
	IoInit(UPKEYPORT,UPKEYPIN,0);//����ģʽ
	//�¼�
	IoInit(DOWNKEYPORT,DOWNKEYPIN,0);//����ģʽ
	//ȷ�ϼ�
	IoInit(CONFIRMKEYPORT,CONFIRMKEYPIN,0);//����ģʽ
	//�˳���
	IoInit(EXITKEYPORT,EXITKEYPIN,0);//����ģʽ
	//MOVE��
	IoInit(MOVEKEYPORT,MOVEKEYPIN,0);//����ģʽ
	
	//�Խ�����
	IoInit(TOLKKEYPORT,TOLKKEYPIN,0);//����ģʽ
	//�㲥����
	IoInit(BOARDCASTPORT,BOARDCASTPIN,0);//����ģʽ
}
