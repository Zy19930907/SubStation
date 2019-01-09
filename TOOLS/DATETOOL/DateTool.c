#include "DateTool.h"
#include <string.h>

char Date[6];

void GetDate(void)
{
	ReadTime();
	Date[0] = SD2068.Hex[0];//年
	Date[1] = SD2068.Hex[1];//月
	Date[2] = SD2068.Hex[2];//年
	Date[3] = SD2068.Hex[4];//时
	Date[4] = SD2068.Hex[5];//分
	Date[5] = SD2068.Hex[6];//秒
}

//生成时间戳---（年-月-日）
void GetDateY_M_D(char *buf)
{
	GetDate();
	sprintf(buf,"20%02x-%02x-%02x",Date[0],Date[1],Date[2]);//年-月-日
}

//生成时间戳---（时:分:秒）
void GetDateH_M_S(char *buf)
{
	GetDate();
	sprintf(buf,"%02x:%02x:%02x",Date[3],Date[4],Date[5]);
}

//生成时间戳---（年-月-日 时）
void GetDateY_M_D_H(char *buf)
{
	GetDate();
	sprintf(buf,"20%02x-%02x-%02x %02x",Date[0],Date[1],Date[2],Date[3]);//年-月-日 时
}

//生成时间戳---（年-月-日 时:分：秒）
void GetDateY_M_D_H_M_S(char *buf)
{
	GetDate();
	sprintf(buf,"20%02x-%02x-%02x %02x:%02x:%02x",Date[0],Date[1],Date[2],Date[3],Date[4],Date[5]);//年-月-日 时
}

