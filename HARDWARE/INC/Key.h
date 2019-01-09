#ifndef __KEY_H
#define __KEY_H

#include "Public.h"

//---------------------------------------------------------------界面控制按钮------------------------------------------------------------------
#define UPKEYPORT					GPIOC
#define UPKEYPIN					14

#define DOWNKEYPORT					GPIOI
#define DOWNKEYPIN					9

#define CONFIRMKEYPORT				GPIOC
#define CONFIRMKEYPIN				15

#define EXITKEYPORT					GPIOI
#define EXITKEYPIN					10

#define MOVEKEYPORT					GPIOI
#define MOVEKEYPIN					11

#define UPKEYVALUE					((UPKEYPORT->IDR >> UPKEYPIN) & 0x01)
#define DOWNKEYVALUE				((DOWNKEYPORT->IDR >> DOWNKEYPIN) & 0x01)
#define CONFIRMKEYVALUE				((CONFIRMKEYPORT->IDR >> CONFIRMKEYPIN) & 0x01)
#define EXITKEYVALUE				((EXITKEYPORT->IDR >> EXITKEYPIN) & 0x01)
#define	MOVEKEYVALUE				((MOVEKEYPORT->IDR >> MOVEKEYPIN) & 0x01)
#define KEYSVALUE					(MOVEKEYVALUE<<4) | ((EXITKEYVALUE << 3) | ((UPKEYVALUE << 2) | (CONFIRMKEYVALUE << 1) |  DOWNKEYVALUE))
//----------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------对讲按钮---------------------------------------------------------------------
#define TOLKKEYPORT					GPIOE
#define TOLKKEYPIN					3

#define BOARDCASTPORT				GPIOE
#define BOARDCASTPIN				2

#define TOLKKEYVALUE				((TOLKKEYPORT->IDR >> TOLKKEYPIN) & 0x01)
#define BOARDCASTKEYVALUE			((BOARDCASTPORT->IDR >> BOARDCASTPIN) & 0x01)
#define TOLKVALUE					BOARDCASTKEYVALUE | (TOLKKEYVALUE << 1)
//----------------------------------------------------------------------------------------------------------------------------------------------
#define KEYCLKEN					RCC->AHB1ENR |= 0x114 //使能PC、PE、PI时钟

void KeyInit(void);

#endif
