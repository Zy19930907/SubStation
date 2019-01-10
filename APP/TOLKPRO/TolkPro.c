#include "TolkPro.h"

_TolkManger TolkManger;
u32 TASKID_WT2031TASK = 0xFFFFFFFF;

void TolkPro(void)
{
	switch(TolkManger.Status)
	{
		case TOLKINIT:
			TolkManger.PlayTolk = PlayTolkData;
			TolkManger.SetVolum = WT2031SetVolum;
			WT2031RST();
			TASKID_WT2031TASK = CreateTask("WT2031TASK",0,0,0,0,0,WT2031Pro);
			TolkManger.DevFlag |= 0x01;
			TolkManger.Status = TOLKING;
			break;
		
		case TOLKIDLE:

			break;
		
		case TOLKING:
			TolkManger.Tick = SYS_TICK;
			switch(TolkManger.Dir)
			{
				case TOLKDOWN:
					TolkManger.PlayTolk(TolkManger.VoiceRecv,TolkManger.VoiceLen);
					break;
				
				case TOLKUP:
					
					break;
			}
			TolkManger.Status = TOLKIDLE;
			break;
	}
}


