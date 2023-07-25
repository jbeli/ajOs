#ifndef TASK_TYPES_H__
#define TASK_TYPES_H__

#include <stdint.h>
//#include

#define TASK(TaskName)	extern void Func ##TaskName(void)
#define PTASK(TaskName) &Func ## TaskName


typedef uint16_t TaskId ;
typedef uint16_t TaskPrio ;
typedef enum e_OsTaskType
{
	BASIC = 0 ,
	EXTENDED
}OsTaskType ;

typedef enum e_OsTaskSchedule
{
	PREEMPTIVE = 0,
	NON_PREEMPTIVE
}OsTaskSchedule ;

typedef enum e_TaskState
{
  RUNNING,
  WAITING,
  READY,
  SUSPENDED,
  INVALID_STATE
} OsTaskStateType;

typedef OsTaskStateType* TaskStateRefType;

typedef enum e_OsTaskAutoStart
{
	NO_AUTOSART = 0,
	AUTOSTART
}OsTaskAutoStart ;

typedef struct 
{
	TaskId TaskID;
	TaskPrio TaskPRIO ;
	int32_t *Stack ;
	const uint16_t stackSize;
	OsTaskType TaskType;
	OsTaskSchedule TaskSchedule;
	
	
	
}Task_TCB ;

#endif