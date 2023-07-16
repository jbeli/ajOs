/*
 * osCfg.c
 *
 *  Created on: Jul 15, 2023
 *      Author: Ahmed
 */
 

#include "osRoundRobinCfg.h"

#define INVALID_ID 	0xFFU
#define NULL 		(void *)0
	
const uint8_t OsCfg_MAX_NUM_OF_TASKS = 	3U; 
// uint8_t OsCfg_MAX_NUM_OF_PERIODIC_TASKS = 2 ;

/*******************************************************************************
	Task contol block table
*******************************************************************************/
