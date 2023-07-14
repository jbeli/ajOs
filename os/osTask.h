#include "osTypes.h"

StatusType ActivateTask(TaskType task) ;
StatusType TerminateTask(void) ;
StatusType ChainTask(TaskType TaskID) ;
StatusType GetTaskID(TaskRefType TaskID);
StatusType GetTaskState(TaskType TaskID, TaskStateRefType State) ;
