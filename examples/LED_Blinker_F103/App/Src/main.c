/******************************************************************************
 * @file        examples/LED_Blinker_F103/Src/main.c
 *
 * @description This code demonstrates the implementation of the developed RTOS for the
 *              STM32F1 microcontroller, tested on the STM32F103C8 Blue Pill Board.
 *              It includes 4 tasks for toggling 4 GPIO pins with different delays
 *              and managing periodic tasks.
 *
 * @date        2024-07-26
 * @author      Achraf Hentati, Embedded Systems Engineer
 * @github      https://github.com/hentati51
 * @contact     hentati51@gmail.com
 *****************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "osKernel.h"

#include "STM32F1_gpio.h"
//#include "STM32F4_uart.h"


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
		 //led_toggle(GREEN);
		 gpio_toggle_pin(GPIOC, 13);
		 osThreadSleep(500);







	 }
}



void Task1(void)
{

	 while(1){
   		count1++;
		//led_toggle(ORANGE);
		 gpio_toggle_pin(GPIOC, 14);
		osThreadSleep(200);



	 }
}

void Task2(void)
{

	 while(1){
		 count2++;
		 gpio_toggle_pin(GPIOC, 15);
   		 osThreadSleep(500);




	 }

}

void Task3(void)
{
	 while(1){

   		 count3++;
 		 //led_toggle(BLUE);
		 gpio_toggle_pin(GPIOC, 16);
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
   	gpio_pin_init(GPIOC, 13, GPIO_MODE_OUTPUT_PP);
   	gpio_pin_init(GPIOC, 14, GPIO_MODE_OUTPUT_PP);
   	gpio_pin_init(GPIOC, 15, GPIO_MODE_OUTPUT_PP);
   	gpio_pin_init(GPIOC, 16, GPIO_MODE_OUTPUT_PP);

	//uart_init(USART2,115200);
	//uart_write_string(USART2, "Hello from my RTOS");


	task0_Handle=osKernelCreateTask(&Task0,10);
	task1_Handle=osKernelCreateTask(&Task1,10);
	task2_Handle=osKernelCreateTask(&Task2,10);
	task3_Handle=osKernelCreateTask(&Task3,10);

	ptask1_Handle= osKernelCreatePeriodc_Task(&periodicTask1,3000);
	ptask2_Handle= osKernelCreatePeriodc_Task(&periodicTask2,5000);

	osKernelLaunch();

}






