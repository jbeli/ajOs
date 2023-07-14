#include "gpio.h"

// User LD4: green LED is a user LED connected to the I/O PD12 

// User LD3: orange LED is a user LED connected to the I/O PD13

// User LD5: red LED is a user LED connected to the I/O PD14

// User LD6: blue LED is a user LED connected to the I/O PD15 
void init_clock_portD(void)
{
	RCC->AHB1ENR |= (1 <<3) ;
	

	
	GPIOD->MODER |= (1<<24) ;
	
	GPIOD->MODER |= (1<<26) ;
	
	GPIOD->MODER |= (1<<28) ;
	
	GPIOD->MODER |= (1<<30) ;
	
	
}

void turnOnLed(uint32_t ledId)
{
	GPIOD->BSRR |= (1<<ledId) ;
}

void turnOffLed(uint32_t ledId)
{
	GPIOD->BSRR |=(1<< (ledId+16) ) ;
}

void toggleLDE(uint32_t ledID)
{
	GPIOD->ODR ^= (1UL << ledID);
}
