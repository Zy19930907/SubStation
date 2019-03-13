#ifndef __TASKMANGER_H
#define __TASKMANGER_H

#include "Public.h"

#define MAXTASKCNT		32	//最大任务数
#define TASKNAMELEN		15	//任务名最大长度

typedef struct
{
	char *TaskName;
	char *Status;
	u32 Tick;
	u32 UseTime;
	u32 MaxUseTime;
	u32 ExecInterval;
	u32 DelayTime;
	u32 TimeRemain;
	uint64_t ExecCnt;
	void(*TaskFunction)(void);
}_Task;

typedef struct
{
	u32 Flag;
	u32 Tick;
	u32 AllTaskUseTime;
	u32 MaxAllTaskUseTime;
	_Task Tasks[MAXTASKCNT];
	void(*SendTaskInfo)(char* info);
}_TaskManger;

extern _TaskManger TaskManger;
extern cJSON* TaskList;
void TaskListInit(void);
u32 CreateTask(char *TaskName,u64 delayDay,u64 delayH, u64 delayM,u64 delayS,u64 delayMs,void(*taskFunction)(void));
void DelTask(u32 *taskid);
void ExecTask(void);
void ShowTasks(void);
void SetTaskDelay(u32 taskid,u64 delayDay,u64 delayH, u64 delayM,u64 delayS,u64 delayMs);
char *itoa(u32 val, char *buf,u8 radix);
void GetTaskList(void);
void SetTaskStatus(u32 taskid,char* status);
void ShowTaskInfo(u32 id);

#endif
