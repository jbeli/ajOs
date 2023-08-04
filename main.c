#include "osKernel.h"
#include "gpio.h"

#define QUANTA 10 // 1000 ms = 1 s

#define LD5_red		  14 
#define LD4_green		12
#define LD6_blue		15
#define LD3_orange   13
static   uint32_t count0, count1, count2, pcount0, pcount1 ;
void Task0(void) ;
void Task1(void) ;
void Task2(void) ;

__attribute__((noreturn)) void OsTask_TaskA(void) 
{
	while(1)
	{
		count0 ++ ;
		toggleLDE(LD6_blue) ;
		if(count0 == 5000)
		{
			count0 =0 ;
			osThreadYield() ;
			
		}
		
	}

}

__attribute__((noreturn)) void OsTask_TaskB(void)
{
	while(1)
	{
		count1 ++ ;
		toggleLDE(LD4_green) ;
	}

}

__attribute__((noreturn)) void OsTask_TaskC(void)
{
	while(1)
	{
		count2 ++ ;
		toggleLDE(LD5_red) ;
	}

}

__attribute__((noreturn)) void OsTask_IdleMechanism(void)
{
	while(1)
	{
		count2 ++ ;
		toggleLDE(LD5_red) ;
	}

}

#ifdef PERIODIC_SCHEDULER

void OsTask_TaskA(void)
{
	pcount0++ ;
}
void OsTask_TaskB(void)
{
	pcount1++ ;
}
void periodicTask0(void)
{
	pcount0++ ;
}
void OsTask_IdleMechanism(void)
{
}

void periodicTask1(void)
{
	pcount1++ ;
}

#endif

int main(void)
{
	init_clock_portD() ;
	#ifdef PERIODIC_SCHEDULER
	osKernelAddPeriodicThreads() ;
	#endif
	osKernelInit();
	#ifdef ROUND_ROBIN
	osKernelAddThreaddraft() ;
	#endif
	#ifdef PERIODIC_SCHEDULER_PRIO
	
	#endif
	//osKernelAddThread(&OsTask_TaskA, &OsTask_TaskB, &OsTask_TaskC ) ;
	//osKernelAddPeriodicThreads();
	osKernelLaunch(QUANTA) ;
	
}

