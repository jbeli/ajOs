
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
	int32_t *stack_pointer ;
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

static int32_t thread_stack[NUM_OF_THREADS][STACKSIZE];


static uint32_t period_tick ;

/*************** END GLOABAL VARIABLES  *****************/


/*************** START FUNCTIONS SECTION ****************/
void osSchedulerLaunch(void) ;

/*************** END FUNCTIONS SECTION *****************/

/*************** START FUNCTIONS SECTION ****************/

void osKernelStackInit(int thread_i)
{

	/*uint32_t* psp = (uint32_t*)(STACK_START + (thread_i+1)*STACKSIZE);
	*(++psp) = 0x01000000u; // Dummy xPSR, just enable Thumb State bit;
  *(++psp) = (int32_t)(OsCfg_TCBs[thread_i].Task_Ptr); // PC
  *(++psp) = 0xFFFFFFFDu; // LR with EXC_RETURN to return to Thread using PSP
  *(++psp) = 0x12121212u; // Dummy R12
  *(++psp) = 0x03030303u; // Dummy R3
  *(++psp) = 0x02020202u; // Dummy R2
  *(++psp) = 0x01010101u; // Dummy R1
  *(++psp) = 0x00000000u; // Dummy R0
  *(++psp) = 0x11111111u; // Dummy R11
  *(++psp) = 0x10101010u; // Dummy R10
  *(++psp) = 0x09090909u; // Dummy R9
  *(++psp) = 0x08080808u; // Dummy R8
  *(++psp) = 0x07070707u; // Dummy R7
  *(++psp) = 0x06060606u; // Dummy R6
  *(++psp) = 0x05050505u; // Dummy R5
  *(++psp) = 0x04040404u; // Dummy R4
	*/

	tcbs[thread_i].stack_pointer = &thread_stack[thread_i][STACKSIZE -16];
	thread_stack[thread_i][STACKSIZE -1] = 0x01000000 ;
	
	
	thread_stack[thread_i][STACKSIZE -3] = 0xAAAAAAAF ; /* LR R14 */
	thread_stack[thread_i][STACKSIZE -4] = 0xAAAAAAA9 ; /* R12 */
	thread_stack[thread_i][STACKSIZE -5] = 0xAAAAAAA8 ; /* R3 */
	thread_stack[thread_i][STACKSIZE -6] = 0xAAAAAAA7 ; /* R2 */
	thread_stack[thread_i][STACKSIZE -7] = 0xAAAAAAA6 ; /* R1 */
	thread_stack[thread_i][STACKSIZE -8] = 0xAAAAAAA5 ; /* R0 */

	thread_stack[thread_i][STACKSIZE -9] = 0xAAAAAAA4 ; /* R11 */
	thread_stack[thread_i][STACKSIZE -10] = 0xAAAAAAA3 ; /* R10 */
	thread_stack[thread_i][STACKSIZE -11] = 0xAAAAAAA2 ; /* R9 */
	thread_stack[thread_i][STACKSIZE -12] = 0xAAAAAAA1 ; /* R8 */
	thread_stack[thread_i][STACKSIZE -13] = 0xAAAAAAA0 ; /* R7 */
	thread_stack[thread_i][STACKSIZE -14] = 0xAAAAAAAA ; /* R6 */
	thread_stack[thread_i][STACKSIZE -15] = 0xAAAAAAAA ; /* R5 */
	thread_stack[thread_i][STACKSIZE -16] = 0xAAAAAAAA ; /* R4 */
	
}
uint32_t __uCurrentTaskIdx = 0;
uint32_t __puTasksPSP[NUM_OF_THREADS] = {0};

uint32_t get_current_psp() {
  return __puTasksPSP[__uCurrentTaskIdx];
}

void save_current_psp(uint32_t psp) {
  __puTasksPSP[__uCurrentTaskIdx] = psp;
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


#ifdef PERIODIC_SCHEDULER_PRIO


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
	}
	
	return E_OK ;
}
#endif

// this function to replace osKernelAddThread(void (*task0)(void), void (*task1)(void), void(*task2)(void))

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

/*
int osKernelAddThread(void (*task0)(void), void (*task1)(void), void(*task2)(void))
{

    // This is a critical code section that cannot be interrupted. Interrupts
    // are then disabled while the TCB linked list is created.
    __disable_irq();

    // Circular linked list is required; this is done by linking
    // TCB 0 --> 1, TCB 1 --> 2, TCB 2 --> 0
    tcbs[0].next_tcb = &tcbs[1];
    tcbs[1].next_tcb = &tcbs[2];
    tcbs[2].next_tcb = &tcbs[0];

    // Initialize the stack for a thread and copy the function pointer to
    // address that will be copied to the PC. This is so that the thread
    // will begin executing when it is first scheduled to execute.
    osKernelStackInit(0);
    thread_stack[0][STACKSIZE - 2] = (int32_t)(task0); // Type cast from void
                                                         // to numerical addr.

    osKernelStackInit(1);
    thread_stack[1][STACKSIZE - 2] = (int32_t)(task1);

    osKernelStackInit(2);
    thread_stack[2][STACKSIZE - 2] = (int32_t)(task2);

    // Initially points to the first thread:
    current_tcb = &tcbs[0];

    // Critical section is now done so interrupts can be enabled again:
    __enable_irq();

    return 1;


}
*/

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
	
	osSchedulerLaunch() ;
	
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

#ifdef PERIODIC_SCHEDULER_PRIO
void osSchedulerPeriodicPrio(void)
{

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
	__ASM("STR     SP, [R1]") ;
	
	#ifdef PERIODIC_SCHEDULER
	__ASM("PUSH {R0, LR}") ;
	//__ASM("BL osSchedulerRoundRobin");
	__ASM("BL osSchedulerPeriodicRR");
	__ASM("POP {R0, LR}");
	__ASM("LDR R1, [R0]");
	#else
	__ASM("LDR		R1, [R1, #4]") ;
	__ASM("STR		R1, [R0]") ;	
	#endif 
	__ASM("LDR		SP, [R1]") ;
	__ASM("POP 	{R4-R11}") ;
	
	__ASM("CPSIE	I") ;
	__ASM("BX		LR") ;

	
	
	
}

void osSchedulerLaunch(void)
{
	__ASM(" LDR     R0, =current_tcb") ;
	__ASM("LDR     R2, [R0] ") ;
	__ASM(" LDR     SP, [R2]") ;
	__ASM("POP     {R4-R11}") ;
	__ASM("POP     {R0-R3}") ;
	__ASM("POP		{R12}") ;
	__ASM("ADD     SP, SP, #4") ;
	__ASM("POP     {LR}") ;
	__ASM("ADD     SP, SP, #4 ") ;
	
	__ASM("CPSIE	I") ;
	__ASM("BX		LR") ;

}
