
#include "osKernel.h"


/*************** START DEFINE SECTION ****************/

#define NUM_OF_THREADS	3
#define STACKSIZE				50
#define BUS_FREQ				16000000
#define SYSPRI3					  (*((volatile uint32_t *)0xE000ED20))
#define NVIC_INT_CTRL_R		(*((volatile uint32_t *)0xE000ED04))

#define PERIOD				100


#ifdef PERIODIC_SCHEDULER
#define OsCfg_MAX_NUM_OF_PERIODIC_TASKS 	2
#endif

/*************** END DEFINE SECTION *****************/




/*************** START GLOBAL VARIABLES  ****************/


static uint32_t MILLIS_PRESCALER ;
#ifdef PERIODIC_SCHEDULER
typedef struct 
{
	taskT task ;
	uint32_t period ;
}periodicTaskT ;

static periodicTaskT PeriodicTasks[OsCfg_MAX_NUM_OF_PERIODIC_TASKS] ;
static uint32_t TimeMsec ;
static uint32_t MaxPeriod ;

#endif
struct tcb_def {
	uint32_t *stack_pointer ;
	struct tcb_def *next_tcb ;
	#ifdef PERIODIC_SCHEDULER_PRIO
	uint32_t sleepTime ;
	uint32_t blocked ;
	uint32_t priority ;
	#endif

	
} ;


typedef struct tcb_def tcbType ;

static tcbType tcbs[NUM_OF_THREADS] ;
static tcbType *current_tcb ;

static uint32_t thread_stack[NUM_OF_THREADS][STACKSIZE];


static uint32_t period_tick ;

static uint32_t __uCurrentTaskIdx = 0;
static uint32_t __puTasksPSP[OsCfg_MAX_NUM_OF_TASKS] = {0};
uint32_t pt ;

uint32_t get_current_psp() {
  return __puTasksPSP[__uCurrentTaskIdx];
}

void save_current_psp(uint32_t psp) {
  __puTasksPSP[__uCurrentTaskIdx] = psp;
}


/*************** END GLOABAL VARIABLES  *****************/


/*************** START FUNCTIONS SECTION ****************/
void osSchedulerLaunch(void) ;

/*************** END FUNCTIONS SECTION *****************/

/*************** START FUNCTIONS SECTION ****************/

void osKernelStackInit(int thread_i)
{

	
	__uCurrentTaskIdx = thread_i ;
	uint32_t *_psp ;
	_psp = &thread_stack[thread_i][STACKSIZE -16]; 
	save_current_psp((uint32_t) (&thread_stack[thread_i][STACKSIZE -16])) ;
	*(--_psp) = 0x01000000 ; // xPSR
	*(--_psp) = (int32_t)(OsCfg_TCBs[thread_i].Task_Ptr); ; // handler PC R15
	*(--_psp) = 0xAAAAAAAF ; // LR R14
	*(--_psp) = 0xAAAAAAA9 ; // SP R12
	*(--_psp) = 0xAAAAAAA8 ; // R3
	*(--_psp) = 0xAAAAAAA7 ; // R2
	*(--_psp) = 0xAAAAAAA6 ;  // R1
	*(--_psp) = 0xAAAAAAA5 ;// R0

	*(--_psp) = 0xAAAAAAA4 ; //R11
	*(--_psp) = 0xAAAAAAA3 ;
	*(--_psp) = 0xAAAAAAA2 ;
	*(--_psp) = 0xAAAAAAA1 ;
	*(--_psp) = 0xAAAAAAA0 ;
	*(--_psp) = 0xAAAAAAAA ;
	*(--_psp) = 0xAAAAAAAA ;
	*(--_psp) = 0xAAAAAAAA ; //R4
	__puTasksPSP[thread_i] = (uint32_t)_psp ;
	OsCfg_TCBs[thread_i].Stack_Pointer = _psp ;
	
	
	

}

void osInitTaskStack(uint8_t threadID)
{
	
}


#ifdef PERIODIC_SCHEDULER

StatusType osKernelAddPeriodicThreads()
{
	uint8_t i, minPeriod  ;
	MaxPeriod = OsPSCfg_TCBs[0].periodicity ;
	minPeriod = OsPSCfg_TCBs[0].periodicity ;
	for (i=0 ; i<OsCfg_MAX_NUM_OF_PERIODIC_TASKS ; i++)
	{
		if(OsPSCfg_TCBs[i].periodicity > MaxPeriod) 
		{
			MaxPeriod = OsPSCfg_TCBs[i].periodicity ;
		}
	}
	
	for (i=0 ; i<OsCfg_MAX_NUM_OF_PERIODIC_TASKS ; i++)
	{
		if(OsPSCfg_TCBs[i].periodicity < MaxPeriod) 
		{
			minPeriod = OsPSCfg_TCBs[i].periodicity ;
		}
	}
	if (MaxPeriod % minPeriod != 0 ) return 0 ;
	for (i=0 ; i<OsCfg_MAX_NUM_OF_PERIODIC_TASKS ; i++)
	{
		PeriodicTasks[i].task = OsPSCfg_TCBs[i].Task_Ptr ;
		PeriodicTasks[i].period = OsPSCfg_TCBs[i].periodicity ;
	}
	return E_OK ;
}

#endif


#ifdef  PERIODIC_SCHEDULER_PRIO


StatusType osKernelAddThreads(void)
{
	uint8_t i ;
	__disable_irq() ;
	
	for (i= 0 ; i< OsCfg_MAX_NUM_OF_TASKS ; i++) 
	{
		tcbs[i].next_tcb = &tcbs[i+1] ;
		if(i ==(OsCfg_MAX_NUM_OF_TASKS -1)) 
		{
			tcbs[i].next_tcb = &tcbs[0] ;
		}
	}
	// this already defined in osPeriodicScheduler.c file need some clean up TO DO
	for (i=0 ; i< OsCfg_MAX_NUM_OF_TASKS ; i++)
	{
		tcbs[i].blocked = 0 ;
		tcbs[i].sleepTime = 0 ;
		tcbs[i].priority = (int32_t)(OsCfg_TCBs[i].priority) ;
	}
	
	__enable_irq() ;
	
	return E_OK ;
}

void osSchedulerPeriodicPrio(void)
{
	tcbType * _currentPt = current_tcb ;
	tcbType * nextThreadToRun = _currentPt ;
	uint8_t highestPrioound = 255 ;
	do
	{
		_currentPt = _currentPt->next_tcb ;
		if ((_currentPt->priority < highestPrioound) &&
			(_currentPt->sleepTime == 0 ))
		{
			nextThreadToRun = _currentPt ;
			highestPrioound = _currentPt->priority ;
		}
	}while(_currentPt !=current_tcb);
	current_tcb = nextThreadToRun ;

}
#endif

#ifdef ROUND_ROBIN

StatusType osKernelAddThreaddraft()
{
	uint8_t i ;
	// Enter Critical Section 
	__disable_irq() ;
	
	for (i =0 ; i< OsCfg_MAX_NUM_OF_TASKS ; i++)
	{
		tcbs[i].next_tcb = &tcbs[i+1] ;
		if(i ==(OsCfg_MAX_NUM_OF_TASKS -1)) 
		{
			tcbs[i].next_tcb = &tcbs[0] ;
		}
		osKernelStackInit(i);
		thread_stack[i][STACKSIZE - 2] = (int32_t)(OsCfg_TCBs[i].Task_Ptr);
	}
	// Initially points to the first thread:
    current_tcb = &tcbs[0];
    // Critical section is now done so interrupts can be enabled again:
    __enable_irq();	
	return E_OK ;
}
#endif

void osKernelInit(void)
{
	// 16 000 000 per second 
	// for an action to occur every 1 ms
	// 16 000 000 /1000 --> 16 000 value of reload register we start decrementing from 15 999 (16 000 -1 ) to so each 
	// 0.0000625ms  and then SysTick_Handler will occur 
	MILLIS_PRESCALER = (BUS_FREQ/1000) ;
}

/*
	quanta =x nunmber of milliseconds 
*/
void osKernelLaunch(uint32_t quanta)
{
	SysTick->CTRL = 0x00 ;
	SysTick->VAL  = 0x00 ;
	SysTick->LOAD = (quanta * MILLIS_PRESCALER) -1 ;
	SYSPRI3 = (SYSPRI3 &0x00FFFFFF) | 0xE0000000 ;
	SysTick->CTRL = 0x00000007 ;
	
	//osSchedulerLaunch() ;
	osStartSheduler() ;
	
}

void osThreadYield(void)
{
	SysTick->VAL = 0;
	NVIC_INT_CTRL_R = 0x04000000 ;
}

#ifdef PERIODIC_SCHEDULER

void osSchedulerPeriodicRR(void)
{
	if(TimeMsec < MaxPeriod) TimeMsec ++ ;
	else
	{
		TimeMsec = 1 ;
	}
	uint8_t i ;
	for (i = 0 ; i< OsCfg_MAX_NUM_OF_PERIODIC_TASKS ; i ++ )
	{
		if((TimeMsec % PeriodicTasks[i].period) == 0 && PeriodicTasks[i].task !=NULL)
		{
			PeriodicTasks[i].task() ;
		}
	}
	current_tcb = current_tcb->next_tcb ;
}
#endif

#ifndef PERIODIC_SCHEDULER
void osSchedulerRoundRobin(void)
{
	/*OsCnt_IncrSystemCounter() ;
	if((OsCnt_GetSystemCounter() % 100) == 1)
	{
		(*periodicTask0)() ;
		
	}
	
	if((OsCnt_GetSystemCounter() % 200) == 1)
	{
		(*periodicTask1)() ;
		
	}
	*/
	current_tcb = current_tcb->next_tcb ;
}

#endif

__attribute__((naked)) void SysTick_Handler(void)
{
	__ASM("CPSID I") ;
	__ASM("PUSH    {R4-R11}") ; // R0, R1, R2, R3, R12, LR, PC, PSR are automatically saved in the stack memory
	__ASM("LDR     R0, =current_tcb") ;
	__ASM("LDR     R1, [R0]") ;
	__ASM("STR     SP, [R1]") ; // store the value of the address saved in SP to R1
	
	#ifdef PERIODIC_SCHEDULER
	__ASM("PUSH {R0, LR}") ;
	//__ASM("BL osSchedulerRoundRobin");
	__ASM("BL osSchedulerPeriodicRR");
	__ASM("POP {R0, LR}");
	__ASM("LDR R1, [R0]");
	#else
	__ASM("LDR		R1, [R1, #4]") ; // @R1 <- *(R1+4)
	__ASM("STR		R1, [R0]") ;	
	#endif 
	__ASM("LDR		SP, [R1]") ;
	__ASM("POP 	{R4-R11}") ;
	
	__ASM("CPSIE	I") ;
	__ASM("BX		LR") ;

	
	
	
}

void osSchedulerLaunch(void)
{
	__ASM(" LDR     R0, =current_tcb") ; // load the address of current_tcb
	__ASM("LDR     R2, [R0] ") ; // load the value stored at the address in R0
	__ASM(" LDR     SP, [R2]") ; // load SP with the value stored in address saved in R2 

	__ASM("POP     {R4-R11}") ;
	__ASM("POP     {R0-R3}") ;
	__ASM("POP		{R12}") ;
	__ASM("ADD     SP, SP, #4") ;
	__ASM("POP     {LR}") ;
	__ASM("ADD     SP, SP, #4 ") ;
	
	__ASM("CPSIE	I") ;
	__ASM("BX		LR") ;

}

void osStartSheduler(void)
{
	__uCurrentTaskIdx =0 ;
	 pt = get_current_psp() ;
	__ASM volatile("LDR  R0, =pt") ; // save the address of pt to R0 
	__ASM volatile("LDR SP, [R0]");  // load the value stored in the address saved in R0 
	
	__ASM volatile("POP     {R4-R11}") ;
	__ASM volatile("POP     {R0-R3}") ;
	__ASM volatile("POP		{R12}") ;
	__ASM volatile("ADD     SP, SP, #4") ;
	__ASM volatile("POP     {LR}") ;
	__ASM volatile("ADD     SP, SP, #4 ") ;
	
	//__ASM("BL osSetTaskStateRunning");
	__ASM volatile("CPSIE	I") ;
	__ASM volatile("BX		LR") ;	
	__ASM("BL osSetTaskStateRunning");
}

void osSetTaskStateRunning()
{
	OsCfg_TCBs[__uCurrentTaskIdx].state = RUNNING ;
}