/*
 * osPeriodicScheduler.c
 *
 *  Created on: Jul 15, 2023
 *      Author: Ahmed
 *                   DON'T UPDATE THIS FILE MANUALLY.
 *           THIS FILE IS GENERATED FROM OIL DESCRIPTION FILE .
 */
 

#include "osPeriodicScheduler.h"

#define INVALID_ID 	0xFFU
#define NULL 		(void *)0
	

// uint8_t OsCfg_MAX_NUM_OF_PERIODIC_TASKS = 2 ;



/*******************************************************************************
	Task contol block table
*******************************************************************************/
tcb_t OsPSCfg_TCBs[2/*num of app tasks*/+1/*for Idle mechanism */] = 
{
	{/*SP*/0u, /*basic SP*/0u , /*task pointer*/OsTask_TaskA, 
	/*task state*/READY, /*DeadBeefLoc*/NULL, /*priority*/10,/* periodicity*/ 100, /*task model*/BASIC,
	/*set events*/0u, /*wait events*/0u, /*res occupation*/0u, 
	/*preemptability*/1u, /*schedule requested */0u},

	{/*SP*/0u, /*basic SP*/0u , /*task pointer*/OsTask_TaskB, 
	/*task state*/SUSPENDED, /*DeadBeefLoc*/NULL, /*priority*/11, /*periodicity*/ 100 , /*task model*/BASIC,
	/*set events*/0u, /*wait events*/0u, /*res occupation*/0u, 
	/*preemptability*/1u, /*schedule requested */0u},

	{0u, 0u, OsTask_IdleMechanism,     READY, NULL, 255u, 10, BASIC, 0u, 0u,
	 0u, 1u, 0u }
};