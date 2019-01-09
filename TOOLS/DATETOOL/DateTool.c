#include "DateTool.h"
#include <string.h>

char Date[6];

void GetDate(void)
{
	ReadTime();
	Date[0] = SD2068.Hex[0];//��
	Date[1] = SD2068.Hex[1];//��
	Date[2] = SD2068.Hex[2];//��
	Date[3] = SD2068.Hex[4];//ʱ
	Date[4] = SD2068.Hex[5];//��
	Date[5] = SD2068.Hex[6];//��
}

//����ʱ���---����-��-�գ�
void GetDateY_M_D(char *buf)
{
	GetDate();
	sprintf(buf,"20%02x-%02x-%02x",Date[0],Date[1],Date[2]);//��-��-��
}

//����ʱ���---��ʱ:��:�룩
void GetDateH_M_S(char *buf)
{
	GetDate();
	sprintf(buf,"%02x:%02x:%02x",Date[3],Date[4],Date[5]);
}

//����ʱ���---����-��-�� ʱ��
void GetDateY_M_D_H(char *buf)
{
	GetDate();
	sprintf(buf,"20%02x-%02x-%02x %02x",Date[0],Date[1],Date[2],Date[3]);//��-��-�� ʱ
}

//����ʱ���---����-��-�� ʱ:�֣��룩
void GetDateY_M_D_H_M_S(char *buf)
{
	GetDate();
	sprintf(buf,"20%02x-%02x-%02x %02x:%02x:%02x",Date[0],Date[1],Date[2],Date[3],Date[4],Date[5]);//��-��-�� ʱ
}

