#ifndef __OS_KERNEL_H__
#define __OS_KERNEL_H__

#include "stdint.h"
#include "stm32f4xx.h"                  // Device header


void osKernelStackInit(int thread_i);
void osKernelInit(void) ;
int osKernelAddThread(void (*task0)(void), void (*task1)(void), void(*task2)(void)) ;
void osKernelLaunch(uint32_t quanta) ;


#endif
