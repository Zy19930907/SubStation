#ifndef __STATUSBAR_H
#define __STATUSBAR_H

#include "PageManger.h"

extern _Page *StatusBar;

void StatusBarInit(void);
void SetStatusBarTitle(char *title);
void SetPcLinkStatus(u8 index,u8 linkFlag);
void SetNetLinkStatus(u8 netFlag);

#endif
