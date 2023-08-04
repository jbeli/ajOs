#ifndef __OS_KERNEL_H__
#define __OS_KERNEL_H__

#include "stdint.h"
#include "stm32f4xx.h"                  // Device header
#include "osCounter.h"
#include "OsSchedulerCfg.h"
#include "OsCfg.h"


#define RAM_START				(0x20000000u)
#define RAM_SIZE				(128*1024) //128Kb
#define RAM_END					((RAM_START) + (RAM_SIZE))
#define STACK_START			RAM_START

#define NULL 		(void *)0
#define ROUND_ROBIN 

typedef void(*taskT)(void) ;

void osKernelStackInit(int thread_i);
void osKernelInit(void) ;
int osKernelAddThread(void (*task0)(void), void (*task1)(void), void(*task2)(void)) ;
void osKernelLaunch(uint32_t quanta) ;
void osThreadYield(void);
void osStartSheduler(void) ;
StatusType osKernelAddThreaddraft(void) ;
void osSetTaskStateRunning(void) ;


/********************* PERIODIC TASKS*****************/
#ifdef PERIODIC_SCHEDULER
void periodicTask0(void) ;
void periodicTask1(void) ;
StatusType osKernelAddPeriodicThreads() ;
#endif

#endif
