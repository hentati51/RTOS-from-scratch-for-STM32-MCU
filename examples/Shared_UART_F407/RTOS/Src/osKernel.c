/******************************************************************************
 * @file        RTOS/Src/kernel.c
 *
 * @Description This file contains the core implementation of the RTOS kernel for the
 *  		    STM32 MCU(tested on stm32f103 and stm32407).
 *
 *
 * @date        2024-05-04
 * @author      Achraf Hentati
 * @github      https://github.com/hentati51
 * @email       hentati51@gmail.com
 *****************************************************************************/
#include "osKernel.h"
#include "semaphore.h"
#include "main.h"


#define IDLE_TASK_PRIORITY  255








uint32_t nbTick ;
uint32_t kernelIdleCount;

uint32_t MILLIS_PRESCELAR ;
uint32_t systickLoadVal;







void osSchedulerLaunch(void);
void osSchedulerRoundRobin(void);

void periodic_events_execute(void);

void tickTimer_Init(TIM_TypeDef *timer,uint32_t freq, uint8_t priority);
void sem_timeout_handle(void);



tcbType* findFirstHighestPriorityTask(void);


// Provide a weak implementation of the userIdleHook function
__attribute__((weak)) void userIdleHook(void) {
    // Default implementation: do nothing
}



static periodicTaskT	PeriodicTasks[NUM_PERIODIC_TASKS];

static int32_t NumPeriodicTasks =0;
static int32_t NumTasks =0;


tcbType tcbs[NUM_OF_TASKS];
tcbType *currentPt;


int32_t TCB_STACK[NUM_OF_TASKS][STACKSIZE];


void osKernelStackInit(int i){
   tcbs[i].stackPt =  &TCB_STACK[i][STACKSIZE -16];//StackPointer
	 TCB_STACK[i][STACKSIZE -1] =  0x01000000;

	TCB_STACK[i][STACKSIZE-3] = 0x14141414;  //R14
	TCB_STACK[i][STACKSIZE-4] = 0x12121212;  //R12
	TCB_STACK[i][STACKSIZE-5] = 0x03030303;  //R3
	TCB_STACK[i][STACKSIZE-6] = 0x02020202;  //R2
	TCB_STACK[i][STACKSIZE-7] = 0x01010101;  //R1
	TCB_STACK[i][STACKSIZE-8] = 0x00000000;  //R0
	  /*We have to save manually*/
	TCB_STACK[i][STACKSIZE-9] = 0x11111111;  //R11
	TCB_STACK[i][STACKSIZE-10] = 0x10101010; //R10
	TCB_STACK[i][STACKSIZE-11] = 0x09090909; //R9
	TCB_STACK[i][STACKSIZE-12] = 0x08080808; //R8
	TCB_STACK[i][STACKSIZE-13] = 0x07070707; //R7
	TCB_STACK[i][STACKSIZE-14] = 0x06060606; //R6
	TCB_STACK[i][STACKSIZE-15] = 0x05050505; //R5
	TCB_STACK[i][STACKSIZE-16] = 0x04040404; //R4
}

taskHandleT osKernelCreateTask( taskT task,uint32_t priority){
	__disable_irq();

	 if (NumTasks == NUM_OF_TASKS || task == NULL || (priority == 255 && NumTasks > 0) || priority > 255) {
	        return -1;
	    }
	if(NumTasks<NUM_OF_TASKS-1){
		tcbs[NumTasks].nextPt = &tcbs[NumTasks+1];
	}else if(NumTasks == NUM_OF_TASKS-1){
		tcbs[NumTasks].nextPt = &tcbs[0];}

    osKernelStackInit(NumTasks);TCB_STACK[NumTasks][STACKSIZE-2] = (int32_t)(task); /*Init PC*/
	tcbs[NumTasks].priority = priority;
	tcbs[NumTasks].originalPriority = priority;
	tcbs[NumTasks].id = NumTasks;
	tcbs[NumTasks].sleepTime = 0;
	tcbs[NumTasks].sleepTimeLeft = 0;
	tcbs[NumTasks].lastWakeTime = 0;

	NumTasks++;
	__enable_irq();
	return (NumTasks-1);

}


ptaskHandleT osKernelCreatePeriodc_Task(taskT ptask, uint32_t msPeriod){
	  if(NumPeriodicTasks ==  NUM_PERIODIC_TASKS || msPeriod == 0){
		  return -1;
		}

		PeriodicTasks[NumPeriodicTasks].task = ptask;
		if(TICK_TIMER_ENABLE){
		PeriodicTasks[NumPeriodicTasks].period = msPeriod;
		PeriodicTasks[NumPeriodicTasks].timeLeft =  msPeriod-1;}
		else{
			PeriodicTasks[NumPeriodicTasks].period = MS_to_Quantas(msPeriod);
			PeriodicTasks[NumPeriodicTasks].timeLeft =  MS_to_Quantas(msPeriod)-1;

		}
		PeriodicTasks[NumPeriodicTasks].suspended =  0;
		PeriodicTasks[NumPeriodicTasks].id =  NumPeriodicTasks;
		NumPeriodicTasks++;
		return (NumPeriodicTasks-1);
}

void KernelIdleTask(void){
	while(1){
		kernelIdleCount++;	}

}

void userIdleTask(void){
	while(1){
		userIdleHook();	}

}

void osKernelInit(){

	MILLIS_PRESCELAR = (BUS_FREQ/1000);

	if(!USER_IDLE_TASK_ENABLE){
	osKernelCreateTask(&KernelIdleTask, IDLE_TASK_PRIORITY);
	}else{
	osKernelCreateTask(&userIdleTask, IDLE_TASK_PRIORITY);


	}
}

void osKernelLaunch(){

	currentPt=findFirstHighestPriorityTask();
	//reset systick
	SysTick -> CTRL = 0;
	//clear current value register
	SysTick -> VAL = 0;

	//load quanta value
	systickLoadVal=(MS_QUANTA*MILLIS_PRESCELAR) - 1;

	SysTick -> LOAD = (MS_QUANTA*MILLIS_PRESCELAR) - 1;


	NVIC_SetPriority(SysTick_IRQn, 7/* priority value */);

	// Set PendSV priority
	NVIC_SetPriority(PendSV_IRQn, 15/* lowest priority value */);

	// enable systick and select internal clock
	//SysTick -> CTRL = CTRL_ENABLE | CTRL_CLCKSRC;
	SysTick -> CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;


	//enable systick interrupt
	//SysTick -> CTRL |= CTRL_TICKINT ;
	SysTick -> CTRL |= SysTick_CTRL_TICKINT_Msk ;

	if(TICK_TIMER_ENABLE) tickTimer_Init(TICK_TIMER,1000,6);

	osSchedulerLaunch();

}

__attribute__((naked)) void PendSV_Handler(void)
{
	/* Rq: when an exception occurs the r0,r1,r2,r3,r12,lr,pc,psr :(the stack frame)
	 are saved automatically in the stack*/

	//**suspend current thread

		//disable global interrupt
		__asm("CPSID	I");

		//save r4,r5,r6,r7,r8,r9,r10,r11
		__asm("PUSH	{R4-R11}");

		//load adresse of currentPt int r0
		__asm("LDR R0,=currentPt");

		//load r1 from address equal r0 ==>( r1=currentPt)
		__asm("LDR R1,[R0]");

		//Store the Cortex-M SP at address equal r1 ==> (save the MCU SP into tcb)
		__asm("STR SP,[R1]");

	//** chose next thread

		/* the next thread can be the periodic thread so in this
		   section we will check if the period of the periodic thread
		   has elapsed and execute it
		 */

		__asm("PUSH  {R0,LR}" );
		__asm("BL	osPriorityScheduler");
		__asm("POP {R0,LR}");

//=====================================================================================================
		 __asm("LDR R1,[R0]");


//=======================================================================================================
		//load cortex-m SP from address equal r1 ==>( SP=currentPt->stackPt)
		__asm("LDR SP,[R1]");

		//restore r4,r5,r6,r7,r8,r9,r10,r11
		__asm("POP {R4-R11}");

		//enable globale interrupt
		__asm("CPSIE	I");

		/* return from exception and restore r0,r1,r2,r3,r12,lr,pc,psr */
		__asm("BX       LR");

}

void osSchedulerLaunch(void){

	// load address of currentPt into r0
	__asm("LDR R0,=currentPt");

	//load r2 from address equal r0 ,r2=currentPt
	__asm("LDR R2,[R0]");

	//load cortex-M SP from address equals r2 , SP=currentPt
	__asm("LDR SP,[R2]");

	//Restore r4 -- r11
	__asm("POP {R4-R11}");

	//Restore r12
	__asm("POP {R12}");

	//Restore r0,r1,r2,r3
	__asm("POP {R0-R3}");

	//skip LR
	__asm("ADD  SP,SP,#4");

	//create a new start location by popping LR
	__asm("POP {LR}");

	//skip PSR by adding 4 to SP
	__asm("ADD  SP,SP,#4");

	//enable global interrupt
	__asm("CPSIE	 I");

	//return from exception
	__asm("BX    LR");}



	void osThredYield(void){
		//this function trigger the systick exception

		//clear systick current val
		//SysTick -> VAL = 0 ;
		if (TICK_TIMER_ENABLE)  SysTick -> VAL = systickLoadVal;


		//triger systick
		//ICSR = PENDSTSET;

		//SCB -> ICSR = PENDSTSET;

		//triger PendSV

		//*****SCB -> ICSR |= PENDSVSET;
		SCB -> ICSR |= SCB_ICSR_PENDSVSET_Msk;


	}

	void SysTick_Handler(void){
		//the systick handler will be responsible for trigger the PendSv exeption
		if(!TICK_TIMER_ENABLE){
		nbTick++;}
		//*****SCB -> ICSR |= PENDSVSET;
		SCB -> ICSR |= SCB_ICSR_PENDSVSET_Msk;
		//fromSystick = 1;

	}


	void osPriorityScheduler(void)
	{
		sem_timeout_handle();
		  periodic_events_execute();
		  tcbType *_currentPt = currentPt;
	  	tcbType *nextThreadToRun = _currentPt;
		  uint16_t highestPriorityFound = 256;


		 do{
			  _currentPt =_currentPt->nextPt;
			  if(_currentPt->priority < highestPriorityFound &&
					  !_currentPt->suspended &&
					  /*_currentPt->sleepTimeLeft <= 0 &&*/
					  !_currentPt -> blocked
					  ){

					 nextThreadToRun =_currentPt;
						highestPriorityFound = _currentPt->priority;
			  }
		  }while(_currentPt != currentPt );


		  currentPt  =  nextThreadToRun;




	}

	tcbType* findFirstHighestPriorityTask(void){
		uint8_t highestPriority = tcbs[0].priority;
		tcbType *firstHighestPriorityTask = &tcbs[0] ;

		// Find the task with the highest priority (lowest integer value)
		for (int i = 0; i < NumTasks; i++) {
			if (tcbs[i].priority < highestPriority) {
				highestPriority = tcbs[i].priority;
				firstHighestPriorityTask = &tcbs[i];
			}
		}

		// return the task with the first highest priority found
		return(firstHighestPriorityTask);
}



	uint32_t MS_to_Quantas(uint32_t ms) {
	    // Calculate the number of quanta required for the given milliseconds
		if(TICK_TIMER_ENABLE){
			return(ms);
		}
 		if(ms == 0)	return 0;

		else if(ms <MS_QUANTA) return 1;

		return ((ms + MS_QUANTA / 2) / MS_QUANTA); // Calculate the "rounded" number of quanta


	}

	void periodic_events_execute(void) {
	    for (int i = 0; i < NumPeriodicTasks; i++) {
	        if (PeriodicTasks[i].timeLeft <= 0 && !PeriodicTasks[i].suspended) {
	            PeriodicTasks[i].task();
	            PeriodicTasks[i].timeLeft = PeriodicTasks[i].period - 1;
	            PeriodicTasks[i].lastWakeTime=nbTick;
	        } else if (!PeriodicTasks[i].suspended ) {

	            PeriodicTasks[i].timeLeft= PeriodicTasks[i].period -(nbTick-PeriodicTasks[i].lastWakeTime);

	        }
	    }

	    for (int i = 0; i < NumTasks; i++) {
	        if (tcbs[i].sleepTimeLeft > 0 && !tcbs[i].suspended) {
	            tcbs[i].sleepTimeLeft=tcbs[i].sleepTime-(nbTick-tcbs[i].lastWakeTime);

	        if(tcbs[i].sleepTimeLeft <= 0){
	            tcbs[i].sleepTimeLeft = 0;  // Ensure non-negative sleep time
	        	tcbs[i].blocked=0;
	        }
	      }
	    }


	}

///////////////////////////////////////////////

//APIs to suspend and resume normal tasks

	uint8_t osKernelSuspendTask(taskHandleT taskHandle) {
    // Search for the task with the given ID and suspend it
    for (int i = 0; i <= NumTasks; i++) {
        if (tcbs[i].id == taskHandle && !tcbs[i].suspended ) {
            // Suspend the task by setting its priority to a special value
            // that indicates it is suspended
            //tcbs[i].priority = TASK_SUSPENDED_PRIORITY; // Define TASK_SUSPENDED_PRIORITY
            tcbs[i].suspended = 1;
            return 1; // Return success
        }
    }
    // If task not found, return failure
    return 0;
}


uint8_t osKernelResumeTask(taskHandleT taskHandle) {
    // Search for the suspended task with the given ID and resume it
    for (int i = 0; i <= NumTasks; i++) {
        if (tcbs[i].id == taskHandle && tcbs[i].suspended == 1 ) {
            // Resume the suspended task by restoring its original priority
            //tcbs[i].priority = tcbs[i].originalPriority; // Define originalPriority in tcbType
        	tcbs[i].suspended=0;
            return 1; // Return success
        }
    }
    // If task not found, return failure
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

// APIs to suspend and resume periodic tsk

uint8_t osKernelSuspendPeriodicTask(ptaskHandleT ptaskHandle) {
// Search for the task with the given ID and suspend it
for (int i = 0; i <= NumPeriodicTasks; i++) {
    if (PeriodicTasks[i].id == ptaskHandle && PeriodicTasks[i].suspended == 0) {
        // Suspend the task by setting its priority to a special value
        // that indicates it is suspended
    	PeriodicTasks[i].suspended = 1;
        return 1; // Return success
    }
}
// If task not found, return failure
return 0;
}

uint8_t osKernelResumePeriodicTask(ptaskHandleT ptaskHandle) {
    // Search for the suspended task with the given ID and resume it
    for (int i = 0; i <= NumPeriodicTasks; i++) {
        if (PeriodicTasks[i].id == ptaskHandle && PeriodicTasks[i].suspended == 1 ) {
            // Resume the suspended task by restoring its original priority
        	PeriodicTasks[i].suspended = 0; // Define originalPriority in tcbType
            return 1; // Return success
        }
    }
    // If task not found, return failure
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////


uint8_t osKernelSetPriority(taskHandleT taskHandle , uint8_t newPriority) {
    // Search for the suspended task with the given ID and resume it
     for (int i = 0; i <= NumTasks; i++) {
         if (tcbs[i].id == taskHandle) {
            // Set the task's priority to the new priority
            tcbs[i].priority = newPriority; // Define originalPriority in tcbType
            tcbs[i].originalPriority = newPriority; // Define originalPriority in tcbType

            return 1; // Return success
        }
    }
    // If task not found, return failure
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

	void osThreadSleep(uint32_t ms){
	__disable_irq();
	if(TICK_TIMER_ENABLE){
	currentPt->sleepTime = ms;
	currentPt->sleepTimeLeft=ms;}
	else{
		currentPt->sleepTime = MS_to_Quantas(ms);
		currentPt->sleepTimeLeft= MS_to_Quantas(ms);

	}
	currentPt->lastWakeTime=nbTick;
	currentPt->blocked=1;
	__enable_irq();
	osThredYield();

}
/////////////////////////////////////////////////////////////////////////////

	void tickTimer_Init(TIM_TypeDef *timer,uint32_t freq, uint8_t priority){

		__disable_irq();

	   	if(timer == TIM2){
			   RCC->APB1ENR |=RCC_APB1ENR_TIM2EN;
				 NVIC_SetPriority(TIM2_IRQn,priority);
				 NVIC_EnableIRQ(TIM2_IRQn);

		}else if (timer == TIM3){
			   RCC->APB1ENR |=RCC_APB1ENR_TIM3EN;
				 NVIC_SetPriority(TIM3_IRQn,priority);
				 NVIC_EnableIRQ(TIM3_IRQn);

		}else if (timer == TIM4){
			   RCC->APB1ENR |=RCC_APB1ENR_TIM4EN;
				 NVIC_SetPriority(TIM4_IRQn,priority);
				 NVIC_EnableIRQ(TIM4_IRQn);

		}
	   timer->PSC =  16-1;    /* 16 000 000 / 16 = 1 000 000*/
	   timer->ARR =  (1000000/freq)-1;
	   timer->CR1 =1;
	   timer->DIER |=1;




		__enable_irq();

	}

	////////////////////////////////////////////////////////////////////////////////////////


	void TIM2_IRQHandler(void){
	  TIM2->SR  =0;
	  nbTick++;
	}
	void TIM3_IRQHandler(void){
	  TIM3->SR  =0;
	  nbTick++;
	}	void TIM4_IRQHandler(void){
		  TIM4->SR  =0;
		  nbTick++;
		}	void TIM5_IRQHandler(void){
			  TIM5->SR  =0;
			  nbTick++;
			}

		////////////////////////////////////////////////////////////////

		  void sem_timeout_handle(void){
			    if (Semaphores == NULL) {
			        return; // No semaphores to process
			    }

			  for(int i=0;i<numSems;i++){
			        semaphoreT* sem = &Semaphores[i]; // Cache semaphore reference to avoid repeated array access
				  for(int j=0;j<NumTasks;j++){
					  if((sem->timeoutRecords[j]>0) &&
						((nbTick - sem->WaitStartTimeRecords[j])>=sem->timeoutRecords[j])){
						  tcbs[j].blocked=0;
						  sem->timeoutRecords[j]=0;
					  }

				  }
			  }
			  }




