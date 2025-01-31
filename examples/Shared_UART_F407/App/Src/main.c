/******************************************************************************
 * @file    	examples/SharedUART_F407/Src/main.c
 *
 * @Description This file include a simple Demo application for for the STM32F4 mcu (tested on
 * 				the STM32F407VG Discovery board) to demonstrate the semaphore implementation of
 * 				the developed RTOS.
 * 				It includes 3 tasks that shares the same UART resource to sends a string message.
 * 				A semaphore is used to manage UART access to ensure that messages are not
 * 				corrupted due to simultaneous access
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
#include "semaphore.h"

#include "STM32F4_led.h"
#include "STM32F4_uart.h"


uint32_t count0,count1,count2, count3;


uint32_t IdleHookcount;


 char *myString0 = "hello from task 0\n\r";
 char *myString1 = "hello from task 1\n\r";
 char *myString2 = "hello from task 2\n\r";
 char *myString3 = "hello from task 3\n\r";
 char *myString4 = "hello from task 4\n\r";


taskHandleT task0_Handle,task1_Handle,task2_Handle ;

semaphoreHandleT uartSemaphore_handle ;






void Task0(void)
{
	 while(1){
		 count0++;
		 osSemaphoreTake(uartSemaphore_handle, MAX_DELAY);
		 uart_write_string(USART2, myString0);
		 osSemaphoreGive(uartSemaphore_handle);


	 }
}



void Task1(void)
{

	 while(1){
   		count1++;
		 osSemaphoreTake(uartSemaphore_handle, MAX_DELAY);
		 uart_write_string(USART2, myString1);
		 osSemaphoreGive(uartSemaphore_handle);



	 }
}

void Task2(void)
{

	 while(1){
		 count2++;
		 osSemaphoreTake(uartSemaphore_handle, MAX_DELAY);
		 uart_write_string(USART2, myString2);
		 osSemaphoreGive(uartSemaphore_handle);

	 }

}


void userIdleHook(void){

	IdleHookcount++;

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

	uartSemaphore_handle=osSemaphoreCreate(1, 1);

	osKernelLaunch();

}






