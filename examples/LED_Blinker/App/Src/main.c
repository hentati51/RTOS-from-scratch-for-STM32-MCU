/******************************************************************************
 * @file    	examples/SharedUART/Src/main.c
 *
 * @Description This code demonstrates a simple RTOS implementation on the STM32F407 discovery board
 *          	microcontroller. It includes 4 tasks for toggling discovery board users LEDs with different
 *          	delays and also managing periodic tasks.
 *
 *
 * @date    	2024-07-22
 * @author  	Achraf Hentati	Embedded Systems Engineer
 * @github  	https://github.com/hentati51
 * @email   	hentati51@gmail.com
 *****************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "osKernel.h"

#include "STM32F4_led.h"
#include "STM32F4_uart.h"


uint32_t count0,count1,count2, count3;


uint32_t pcount1,pcount2;

uint32_t IdleHookcount;


 char *myString0 = "hello from task 0\n\r";
 char *myString1 = "hello from task 1\n\r";
 char *myString2 = "hello from task 2\n\r";
 char *myString3 = "hello from task 3\n\r";
 char *myString4 = "hello from task 4\n\r";


taskHandleT task0_Handle,task1_Handle,task2_Handle,task3_Handle,task4_Handle,task5_Handle ;

taskHandleT ptask1_Handle,ptask2_Handle;


void Task0(void)
{
	 while(1){
		 count0++;
		 led_toggle(GREEN);
		 osThreadSleep(100);







	 }
}



void Task1(void)
{

	 while(1){
   		count1++;
		led_toggle(ORANGE);
		osThreadSleep(200);



	 }
}

void Task2(void)
{

	 while(1){
		 count2++;
		 led_toggle(RED);
   		 osThreadSleep(500);




	 }

}

void Task3(void)
{
	 while(1){

   		 count3++;
 		 led_toggle(BLUE);
   		 osThreadSleep(1000);
	 }
}





void userIdleHook(void){

	IdleHookcount++;

}


void periodicTask1(void){

	pcount1++;

}

void periodicTask2(void){

	pcount2++;

}






int main(void)
{
   	osKernelInit();
	leds_init();
	uart_init(USART2,115200);
	uart_write_string(USART2, "Hello from my RTOS");

	task0_Handle=osKernelCreateTask(&Task0,10);
	task1_Handle=osKernelCreateTask(&Task1,10);
	task2_Handle=osKernelCreateTask(&Task2,10);
	task3_Handle=osKernelCreateTask(&Task3,10);

	ptask1_Handle= osKernelCreatePeriodc_Task(&periodicTask1,3000);
	ptask2_Handle= osKernelCreatePeriodc_Task(&periodicTask2,5000);

	osKernelLaunch();

}






