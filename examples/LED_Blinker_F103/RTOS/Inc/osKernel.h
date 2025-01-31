/******************************************************************************
 * @file    	RTOS/Inc/osKernel.h
 *
 * @Description Header file of osKernel.c
 *
 *
 * @date    	2024-05-04
 * @author  	Achraf Hentati	Embedded Systems Engineer
 * @github  	https://github.com/hentati51
 * @email   	hentati51@gmail.com
 *****************************************************************************/
#ifndef __OSKERNEL_H__
#define __OSKERNEL_H__

#include "boardConfig.h"
#include "main.h"


#define NUM_OF_TASKS		NUM_OF_USER_TASKS+1

#define NULL  (void*) 0



typedef void(*taskT)(void);


typedef uint8_t taskHandleT ;
typedef uint8_t ptaskHandleT ;

extern uint32_t nbTick ;



#define MAX_DELAY  (uint32_t)0xffffffffUL


typedef struct tcb{

  int32_t *stackPt;
  struct tcb *nextPt;
  uint8_t priority;
  uint8_t originalPriority;
  taskHandleT id;
  uint8_t suspended;
  uint32_t sleepTime;
  int32_t sleepTimeLeft;
  uint32_t lastWakeTime;
	uint8_t blocked;

}tcbType;

typedef struct{
  taskT task;
	uint32_t period;
	int32_t  timeLeft;
	uint8_t id;
	uint8_t suspended;
	uint32_t lastWakeTime;
	uint32_t blocked;



}periodicTaskT;

extern tcbType *currentPt;





taskHandleT osKernelCreateTask( taskT task,uint32_t priority);


ptaskHandleT osKernelCreatePeriodc_Task(taskT ptask, uint32_t period);

uint8_t osKernelSuspendTask(taskHandleT taskHandle) ;
uint8_t osKernelResumeTask(ptaskHandleT ptaskHandle) ;

uint8_t osKernelSuspendPeriodicTask(taskHandleT taskHandle) ;
uint8_t osKernelResumePeriodicTask(ptaskHandleT taskHandle) ;


uint8_t osKernelSetPriority(taskHandleT taskHandle,uint8_t newPriority);


void osKernelLaunch();
void osKernelInit();
void osThredYield(void);
void osThreadSleep(uint32_t ms);

uint32_t MS_to_Quantas(uint32_t ms);


void userIdleHook(void);

#endif
