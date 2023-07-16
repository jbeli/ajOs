#ifndef __OS_KERNEL_H__
#define __OS_KERNEL_H__

#include "stdint.h"
#include "stm32f4xx.h"                  // Device header
#include "osCounter.h"

#define PERIODIC_SCHEDULER
//#define PERIODIC_SCHEDULER_PRIO

#ifdef PERIODIC_SCHEDULER
#include "osPeriodicScheduler.h"
#else
#include "osRoundRobinCfg.h"
#endif
#define NULL 		(void *)0
	
typedef void(*taskT)(void) ;

void osKernelStackInit(int thread_i);
void osKernelInit(void) ;
int osKernelAddThread(void (*task0)(void), void (*task1)(void), void(*task2)(void)) ;
void osKernelLaunch(uint32_t quanta) ;
void osThreadYield(void);
int osKernelAddThreaddraft(void) ;

/********************* PERIODIC TASKS*****************/

void periodicTask0(void) ;
void periodicTask1(void) ;
StatusType osKernelAddPeriodicThreads() ;

#endif
