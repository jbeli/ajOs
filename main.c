#include "osKernel.h"
#include "gpio.h"

#define QUANTA 1000

#define LD5_red		  14 
#define LD4_green		12
#define LD6_blue		15
#define LD3_orange   13
static uint32_t count0, count1, count2 ;
void Task0(void) ;
void Task1(void) ;
void Task2(void) ;

__attribute__((noreturn)) void Task0(void) 
{
	while(1)
	{
		count0 ++ ;
		toggleLDE(LD6_blue) ;
		if(count0 == 50)
		{
			osThreadYield() ;
		}
	}

}

__attribute__((noreturn)) void Task1(void)
{
	while(1)
	{
		count1 ++ ;
		toggleLDE(LD4_green) ;
	}

}

__attribute__((noreturn)) void Task2(void)
{
	while(1)
	{
		count2 ++ ;
		toggleLDE(LD5_red) ;
	}

}

/*void periodicTask(void)
{
	pcount++ ;
	toggleLDE(LD3_orange);
	
}
*/
int main(void)
{
	init_clock_portD() ;
	osKernelInit();
	osKernelAddThread(&Task0, &Task1, &Task2 ) ;
	osKernelLaunch(QUANTA) ;
	
}

