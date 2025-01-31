/******************************************************************************
 * @file    	RTOS/Src/semaphore.c
 *
 * @Description This file provides the implementation of semaphore services
 *				It includes the creation, acquisition, and release semaphores API's
 *
 *
 * @date    	2024-05-21
 * @author  	Achraf Hentati	Embedded Systems Engineer
 * @github  	https://github.com/hentati51
 * @email   	hentati51@gmail.com
 *****************************************************************************/
#include "semaphore.h"
#include "osKernel.h"
#include <stdlib.h>



uint32_t numSems=0;

semaphoreT *Semaphores=NULL;


semaphoreHandleT osSemaphoreCreate(uint8_t maxCount, uint8_t initialCount) {
    __disable_irq();
    if (Semaphores == NULL && numSems == 0) {
        // Allocate initial memory for Semaphores array
        Semaphores = malloc(sizeof(semaphoreT));
        if (Semaphores == NULL) {
            // Error handling for memory allocation failure
            __enable_irq();
            return NULL;
        }
    } else {
        // Reallocate memory for additional semaphore
        semaphoreT *temp = realloc(Semaphores, (numSems + 1) * sizeof(semaphoreT));
        if (temp == NULL) {
            // Error handling for memory allocation failure
            __enable_irq();
            return NULL;
        }
        Semaphores = temp;
    }

    // Initialize the new semaphore
    Semaphores[numSems].value = initialCount;
    Semaphores[numSems].maxCount = maxCount;
    for (int i = 0; i < NUM_OF_TASKS; i++) {
        Semaphores[numSems].WaitStartTimeRecords[i] = 0;
        Semaphores[numSems].blockList[i] = NULL;
        Semaphores[numSems].timeoutRecords[i] = 0;
    }
    Semaphores[numSems].numBlockedTask = 0;
    numSems++;
    __enable_irq();
    return &Semaphores[numSems - 1];
}



uint8_t osSemaphoreTake(semaphoreHandleT semaphore ,uint32_t timeout) {
    __disable_irq();
   semaphore ->WaitStartTimeRecords[currentPt->id]=nbTick;
   semaphore->timeoutRecords[currentPt->id]=timeout;
    while(semaphore->value <= 0) {

    	if(semaphore->timeoutRecords[currentPt->id]==0){
    		if(semaphore->blockList[currentPt->id]!=NULL){
    		semaphore->blockList[currentPt->id]=NULL;
    		semaphore->numBlockedTask--;}
    		__enable_irq();
      		 return(0);

    	}


        currentPt->blocked=1;
        semaphore->blockList[currentPt->id]=currentPt;

        semaphore -> numBlockedTask++;
        __enable_irq();
         osThredYield();
         __disable_irq();

    }
    semaphore->value--;
    __enable_irq();
    return(1);

}

void osSemaphoreGive(semaphoreHandleT semaphore) {
	__disable_irq();
    if (semaphore->value < semaphore->maxCount) {
        semaphore->value++;
    }
	    uint32_t _numBlockedTask=semaphore->numBlockedTask;
	    for (int i=0;i<NUM_OF_TASKS;i++){
	    		semaphore->blockList[i]->blocked=0;
	    		semaphore->blockList[i]=NULL;
	    		semaphore->numBlockedTask=0;

	    }

	    if (_numBlockedTask >0)  {
	        __enable_irq();
	        osThredYield();
	    }

	    __enable_irq();
}
