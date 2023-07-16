/*
 * osCounter.h
 *
 *  Created on: Jul 1, 2023
 *      Author: Ahmed
 */

#ifndef OSKERNEL_OSCOUNTER_H_
#define OSKERNEL_OSCOUNTER_H_

#include <stm32f407xx.h>

extern uint32_t OsCnt_SystemCounter ;
//extern void SysTick_Init(void);
extern void OsCnt_IncrSystemCounter(void);
extern uint32_t OsCnt_GetSystemCounter(void);

#endif /* OSKERNEL_OSCOUNTER_H_ */
