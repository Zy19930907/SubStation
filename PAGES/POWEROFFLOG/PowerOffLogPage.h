#ifndef __POWEROFFLOGPAGE_H
#define __POWEROFFLOGPAGE_H

#include "PageManger.h"

extern _Page *PowerOffLogPage;
typedef enum
{
	BREAKLOGFRESHINIT,
	BREAKLOGFRESHITEM,
	BREAKLOGFRESHEND,
}BreakLogFreshStatus;

typedef struct
{
	BreakLogFreshStatus Status;
	u32 logFileSize;
	u32 readSize;
	u16 logCnt;
}_BreakLogFresh;

void PowerOffLogPageInit(void);

#endif
