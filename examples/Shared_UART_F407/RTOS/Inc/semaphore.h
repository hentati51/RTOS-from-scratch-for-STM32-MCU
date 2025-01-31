/******************************************************************************
 * @file    	RTOS/Inc/semaphore.h
 *
 * @Description Header file of semaphore.c
 *
 *
 * @date    	2024-05-21
 * @author  	Achraf Hentati	Embedded Systems Engineer
 * @github  	https://github.com/hentati51
 * @email   	hentati51@gmail.com
 *****************************************************************************/
#ifndef __SEMAPHORE_H_
#define __SEMAPHORE_H_

#include "stdint.h"
#include "osKernel.h"
#include "boardConfig.h"

typedef struct{
	uint8_t value;
	uint8_t maxCount;
	uint32_t WaitStartTimeRecords[NUM_OF_USER_TASKS+1] ;
	uint32_t timeoutRecords[NUM_OF_USER_TASKS+1] ;
	tcbType * blockList[NUM_OF_USER_TASKS+1] ;
	uint32_t numBlockedTask;


}semaphoreT;

typedef semaphoreT* semaphoreHandleT;

extern uint32_t numSems ;

extern semaphoreT *Semaphores;



semaphoreHandleT osSemaphoreCreate(uint8_t maxCount,uint8_t initialCount);


uint8_t osSemaphoreTake(semaphoreHandleT semaphore ,uint32_t timeout) ;


void osSemaphoreGive(semaphoreHandleT semaphore) ;










#endif


