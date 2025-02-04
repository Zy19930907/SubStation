#include "TaskManger.h"
#include "string.h"
_TaskManger TaskManger;
cJSON* TaskList;
cJSON* TaskInfo;
extern u32 MemUseHandle;

//创建任务
u32 CreateTask(char *TaskName,u64 delayDay,u64 delayH, u64 delayM,u64 delayS,u64 delayMs,void(*taskFunction)(void))
{
	u8 i;
	for(i=0;i<MAXTASKCNT;i++)
	{
		//任务已创建
		if(TaskManger.Tasks[i].TaskFunction == taskFunction)
			return i;
		//任务格子已占用
		if((TaskManger.Flag >> i) & 0x01)
			continue;
		TaskManger.Tasks[i].TaskName = mymalloc(SRAMEX,TASKNAMELEN);
		TaskManger.Tasks[i].Status = mymalloc(SRAMEX,TASKNAMELEN);
		strcpy(TaskManger.Tasks[i].Status,"正常");
		strcpy((char*)TaskManger.Tasks[i].TaskName,(char*)TaskName);
		
		TaskManger.Tasks[i].DelayTime = ((delayDay * 86400000) + (delayH * 3600000) + (delayM * 60000) + (delayS * 1000) + delayMs);
		TaskManger.Tasks[i].MaxUseTime = 0;
		TaskManger.Tasks[i].Tick = SYS_TICK;
		TaskManger.Tasks[i].ExecInterval = 0;
		TaskManger.Tasks[i].UseTime = 0;
		TaskManger.Tasks[i].ExecCnt = 0;
		TaskManger.Tasks[i].TaskFunction = taskFunction;
		TaskManger.Flag |= (1<<i);
		return i;
	}
	return 0xFFFFFFFF;
}

//删除任务
void DelTask(u32 *taskid)
{
	if(*taskid == 0xFFFFFFFF)
		return;
	TaskManger.Flag &= ~(1 << *taskid);
	TaskManger.Tasks[*taskid].DelayTime = 0;
	TaskManger.Tasks[*taskid].MaxUseTime = 0;
	TaskManger.Tasks[*taskid].Tick = 0;
	TaskManger.Tasks[*taskid].ExecInterval = 0;
	TaskManger.Tasks[*taskid].UseTime = 0;
	TaskManger.Tasks[*taskid].TaskFunction = 0;
	myfree(SRAMEX,TaskManger.Tasks[*taskid].TaskName);
	myfree(SRAMEX,TaskManger.Tasks[*taskid].Status);
	*taskid = 0xFFFFFFFF;
}

//设置任务执行延时
void SetTaskDelay(u32 taskid,u64 delayDay,u64 delayH, u64 delayM,u64 delayS,u64 delayMs)
{
	_Task* task;
	task = &TaskManger.Tasks[taskid];
	task->DelayTime = ((delayDay * 86400000) + (delayH * 3600000) + (delayM * 60000) + (delayS * 1000) + delayMs);
}

//设置任务执行延时
void SetTaskStatus(u32 taskid,char* status)
{
	_Task* task;
	if(taskid == 0xFFFFFFFF)
		return;
	task = &TaskManger.Tasks[taskid];
	strcpy(task->Status,status);
}


//统计当前执行的所有任务
void GetTaskList(void)
{
	u8 i;
	_Task* Task;
	char* taskinfos;
	cJSON* array;
	signed int time;
	cJSON_Delete(TaskList);
	TaskList = cJSON_CreateObject();
	array = cJSON_CreateArray();
	cJSON_AddItemToObject(TaskList, "totalTime",cJSON_CreateNumber(TaskManger.AllTaskUseTime));
	cJSON_AddItemToObject(TaskList,"memUsed",cJSON_CreateNumber(my_mem_perused(SRAMEX)));
	
	for(i=0;i<MAXTASKCNT;i++)
	{
		if(!((TaskManger.Flag >> i) & 0x01))
			continue;
		Task = &TaskManger.Tasks[i];
		TaskInfo = cJSON_CreateObject();
		cJSON_AddItemToObject(TaskInfo, "Id",cJSON_CreateNumber(i));
		cJSON_AddItemToObject(TaskInfo, "Name",cJSON_CreateString(Task->TaskName));
		cJSON_AddItemToObject(TaskInfo, "Status",cJSON_CreateString(Task->Status));
		cJSON_AddItemToObject(TaskInfo, "Delay",cJSON_CreateNumber(Task->DelayTime));
		cJSON_AddItemToObject(TaskInfo, "UseTime",cJSON_CreateNumber(Task->UseTime));
		cJSON_AddItemToObject(TaskInfo, "MaxUseTime",cJSON_CreateNumber(Task->MaxUseTime));
		cJSON_AddItemToObject(TaskInfo, "ExecInterval",cJSON_CreateNumber(Task->ExecInterval));
		time = ((signed int)Task->DelayTime) - ((signed int)MsTickDiff(Task->Tick));
		cJSON_AddItemToObject(TaskInfo, "TimeRemain",cJSON_CreateNumber(time));
		cJSON_AddItemToObject(TaskInfo, "ExecCnt",cJSON_CreateNumber(Task->ExecCnt));
		cJSON_AddItemToArray(array, TaskInfo);
	}
	cJSON_AddItemToObject(TaskList, "Info",array);
	taskinfos = cJSON_PrintUnformatted(TaskList);
	TaskManger.SendTaskInfo(taskinfos);
	myfree(SRAMIN,taskinfos);
}

////统计当前执行的所有任务
//void ShowTaskInfo(u32 id)
//{
//	u8 i;
//	_Task* Task;
//	signed int time;
//	char info[500];
//	GUI_COLOR color; 
//	
//	LISTVIEW_DeleteAllRows(id);
//	for(i=0;i<MAXTASKCNT;i++)
//	{
//		if(!((TaskManger.Flag >> i) & 0x01))
//			continue;
//		
//		Task = &TaskManger.Tasks[i];
//		if(i % 2)
//			color = GUI_GRAY_C0;
//		else
//			color = GUI_GRAY_7C;

//		sprintf(info,"%d",i);
//		LISTVIEW_AddRow(id,NULL);
//		LISTVIEW_SetItemText(id,0,i,info);
//		LISTVIEW_SetItemText(id,1,i,Task->TaskName);
//		LISTVIEW_SetItemText(id,2,i,Task->Status);
//		sprintf(info,"%d",Task->DelayTime);
//		LISTVIEW_SetItemText(id,3,i,info);
//		sprintf(info,"%d",Task->UseTime);
//		LISTVIEW_SetItemText(id,4,i,info);
//		sprintf(info,"%d",Task->MaxUseTime);
//		LISTVIEW_SetItemText(id,5,i,info);
//		sprintf(info,"%d",Task->ExecInterval);
//		LISTVIEW_SetItemText(id,6,i,info);
//		time = ((signed int)Task->DelayTime) - ((signed int)MsTickDiff(Task->Tick));
//		sprintf(info,"%d",time);
//		LISTVIEW_SetItemText(id,7,i,info);
//		LISTVIEW_SetItemBkColor(id,0,i,LISTVIEW_CI_UNSEL,color);
//		LISTVIEW_SetItemBkColor(id,1,i,LISTVIEW_CI_UNSEL,color);
//		LISTVIEW_SetItemBkColor(id,2,i,LISTVIEW_CI_UNSEL,color);
//		LISTVIEW_SetItemBkColor(id,3,i,LISTVIEW_CI_UNSEL,color);
//		LISTVIEW_SetItemBkColor(id,4,i,LISTVIEW_CI_UNSEL,color);
//		LISTVIEW_SetItemBkColor(id,5,i,LISTVIEW_CI_UNSEL,color);
//		LISTVIEW_SetItemBkColor(id,6,i,LISTVIEW_CI_UNSEL,color);
//		LISTVIEW_SetItemBkColor(id,7,i,LISTVIEW_CI_UNSEL,color);
//	}
//}

//统计当前执行的所有任务
void ShowTaskInfo(u32 id)
{
	u8 i,j;
	_Task* Task;
	signed int time;
	char *infostr[7];
	char info[7][50];
	
	LISTVIEW_DeleteAllRows(id);
	for(i=0;i<MAXTASKCNT;i++)
	{
		if(!((TaskManger.Flag >> i) & 0x01))
			continue;
		j=0;
		Task = &TaskManger.Tasks[i];
		
		sprintf(info[j],"%s",Task->TaskName);
		infostr[j] = info[j];
		j++;
		sprintf(info[j],"%s",Task->Status);
		infostr[j] = info[j];
		j++;
		sprintf(info[j],"%d",Task->DelayTime);
		infostr[j] = info[j];
		j++;
		sprintf(info[j],"%d",Task->UseTime);
		infostr[j] = info[j];
		j++;
		sprintf(info[j],"%d",Task->MaxUseTime);
		infostr[j] = info[j];
		j++;
		sprintf(info[j],"%d",Task->ExecInterval);
		infostr[j] = info[j];
		j++;
		time = ((signed int)Task->DelayTime) - ((signed int)MsTickDiff(Task->Tick));
		sprintf(info[j],"%d",time);
		infostr[j] = info[j];
		LISTVIEW_AddRow(id,(GUI_ConstString*)infostr);
	}
	SetListViewRowBkColor(id);
}

//执行任务列表中的任务
void ExecTask(void)
{
	u8 i;
	_Task* Task;
	for(;;)
	{
//		TaskManger.Tick = SYS_TICK;
		for(i=0;i<MAXTASKCNT;i++)
		{
			if(!((TaskManger.Flag >> i) & 0x01))					//未添加任务
				continue;
			
			Task = &TaskManger.Tasks[i];
			if(MsTickDiff(Task->Tick) >= Task->DelayTime)
			{
//				Task->ExecInterval = MsTickDiff(Task->Tick);		//计算上次执行任务到本次执行任务的时间间隔
				Task->Tick = SYS_TICK;
				Task->TaskFunction();								//执行任务函数
//				Task->UseTime = MsTickDiff(Task->Tick);				//计算本次执行任务用时
//				if(Task->UseTime > Task->MaxUseTime)
//					Task->MaxUseTime = Task->UseTime;
//				Task->ExecCnt++;
			}
		}
//		TaskManger.AllTaskUseTime = MsTickDiff(TaskManger.Tick);	//计算所有任务执行耗时 
	}
}
