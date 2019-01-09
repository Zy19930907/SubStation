#ifndef __LINKLISTPAGE_H
#define __LINKLISTPAGE_H

#include "PageManger.h"

typedef enum
{
	RELATIONSHOWINIT,
	RELATIONSHOWITEM,
	RELATIONSHOWEND,
}RelationShowStatus;

typedef struct
{
	RelationShowStatus Status;
	u8 showCnt;
}_RelationShow;

extern _Page *LinkListPage;
void LinkListPageInit(void);

#endif
