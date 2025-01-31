/******************************************************************************
 * @file    	examples/SharedUART_F407/Inc/main.h
 *
 * @Description This file include macros, and configuration settings needed for the RTOS implementation
 *
 * @attention
 *          	- The stack size (STACKSIZE) should be adjusted according to the task requirements
 *          	- Modify the number of user tasks (NUM_OF_USER_TASKS) and periodic tasks
 *            	  (NUM_PERIODIC_TASKS) as needed based on your application.
 *
 *
 * @date    	2024-05-10
 * @author  	Achraf Hentati	Embedded Systems Engineer
 * @github  	https://github.com/hentati51
 * @email   	hentati51@gmail.com
 *****************************************************************************/



#ifndef __MAIN_H__
#define __MAIN_H_

#define	MS_QUANTA	10

#define STACKSIZE   200 // 200 byte

#define NUM_OF_USER_TASKS		3
#define NUM_PERIODIC_TASKS  	0


#define USER_IDLE_TASK_ENABLE   1

#define TICK_TIMER_ENABLE    1
#define TICK_TIMER		     TIM2  //when enable the tick timer you should use the timer used (tim2,tim3,tim4)


#endif
