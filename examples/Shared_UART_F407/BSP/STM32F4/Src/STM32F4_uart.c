/******************************************************************************
 * @file    	STM32F4_uart.c
 *
 * @Description Drivers for the STM32F4 MCU UART peripherals
 *
 *
 * @date    	2024-04-27
 * @author  	Achraf Hentati	Embedded Systems Engineer
 * @github  	https://github.com/hentati51
 * @email   	hentati51@gmail.com
 *****************************************************************************/
#include <stdio.h>
#include <stdint.h>
 #include <stdarg.h>
#include <STM32F4_uart.h>

#define SYSFREQ  16000000
#define AHB1_CLK  SYSFREQ

//USART2 TX:PA2 RX:PA3
//USART1 TX:PA9 RX:PA10

static uint16_t compute_baud_rate(uint32_t prephi_clk,uint32_t baudrate);
static void uart_set_baudrate (uint32_t prephi_clk,uint32_t baudrate,USART_TypeDef* UART);

void uart_write(USART_TypeDef* UART ,int ch);

void USART2_IRQHandler(void);

// Provide a weak implementation of the callback function
__attribute__((weak)) void uart_rxlntCallback(void) {
    // Default implementation: do nothing
}




void uart_tx_init(uint32_t  BaudRate ){
	//set_sysclk_to_168();
	//** enable bus clock access for PORTA because usart2_tx is connected to PA2

	RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // RCC -> APB2ENR |= (1U <<0);

	//** enable bus clock access for usart2

	RCC -> APB1ENR |= RCC_APB1ENR_USART2EN; //(1U<17)

	//** set PA2 mode to alternate function mode
	 GPIOA -> MODER |=  GPIO_MODER_MODER2_1;

	 GPIOA -> MODER &= ~ (GPIO_MODER_MODER2_0);

	    //GPIOA->OSPEEDR |= 0x000000A0; // Set pin 2/3 to high speed mode (0b10)


	//configure the alternate function register : F7 for usart2

	 GPIOA -> AFR[0] |= 0X00000700;

	 // ** configure the baudrate

	 uart_set_baudrate(AHB1_CLK,BaudRate,USART2);

	 //**configure transfert direction tx

	 USART2 -> CR1 |= USART_CR1_TE ;//(1U<<3)

	 //** enable uart2

	 USART2 -> CR1 |= USART_CR1_UE ;//(1U<<13)




}





//Initialize RX and TX of UART peripheral
void uart_init(USART_TypeDef* UART,uint32_t  BaudRate ){
	//enable bus clock access for PORTA because usart2_tx_rx is connected to PA2-PA3
	if (UART == USART2){
			//enable bus clock access for PORTA because usart2_tx_rx is connected to PA2-PA3
			RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // RCC -> APB2ENR |= (1U <<0);

			//enable bus clock access for usart2

			RCC -> APB1ENR |= RCC_APB1ENR_USART2EN; //(1U<17)

			//set PA2 mode to alternate function mode
			 GPIOA -> MODER |=  GPIO_MODER_MODER2_1;

			 GPIOA -> MODER &= ~ (GPIO_MODER_MODER2_0);

				//GPIOA->OSPEEDR |= 0x000000A0; // Set pin 2/3 to high speed mode (0b10)

			//set PA3 mode to alternate function mode
			 GPIOA -> MODER |=  GPIO_MODER_MODER3_1;

			 GPIOA -> MODER &= ~ (GPIO_MODER_MODER3_0);

			//set PA2 alternate function type to UART_TX (AF07)
			// AFR[0] is AFR LOW REGISTRE
			 GPIOA -> AFR[0] |= 0X00000000;

			 GPIOA -> AFR[0] |= 0X00000700;

			//set PA3 alternate function type to UART_RX (AF07)
			 GPIOA -> AFR[0] |= 0X00007000;}
	else if(UART == USART1){
			RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
			RCC -> APB2ENR |= RCC_APB2ENR_USART1EN;

			 GPIOA -> MODER |=  GPIO_MODER_MODER9_1;
			 GPIOA -> MODER &= ~ (GPIO_MODER_MODER9_0);

			 GPIOA -> MODER |=  GPIO_MODER_MODER10_1;
			 GPIOA -> MODER &= ~ (GPIO_MODER_MODER10_0);

			 GPIOA -> AFR[1] |= 0X00000700;
			 GPIOA -> AFR[1] |= 0X00007000;


		}else if(UART == USART3){
			RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
			RCC -> APB1ENR |= RCC_APB1ENR_USART3EN;

			 GPIOB -> MODER |=  GPIO_MODER_MODER10_1;
				 GPIOB -> MODER &= ~ (GPIO_MODER_MODER10_0);

				 GPIOB -> MODER |=  GPIO_MODER_MODER11_1;
				 GPIOB -> MODER &= ~ (GPIO_MODER_MODER11_0);

				 GPIOB -> AFR[1] |= 0X00000700;
				 GPIOB -> AFR[1] |= 0X00007000;

		}


	 //configure the baudrate


    	uart_set_baudrate(AHB1_CLK,BaudRate,UART);


	 //configure transfert direction tx and rx

	 UART -> CR1 |= USART_CR1_TE | USART_CR1_RE ;//(1U<<3) | (1U<<2)

	 //enable uart

	 UART -> CR1 |= USART_CR1_UE ;//(1U<<13)



}



 void uart_write(USART_TypeDef* UART, int ch){
	//*Make sure that the transmit data registre is empty
	while(!(UART -> SR & USART_SR_TXE)){}

	//* write to the transmit data registre
	UART -> DR = (ch & 0xFF);
}

char uart_read(void){
	//Make sure that the recive data registre is NOT empty
	while(!(USART2 -> SR & USART_SR_RXNE)){}

	//Read data
	return USART2 -> DR;



}


// Function to read a string from UART
void uart_read_string(char *buffer, uint32_t buffer_size) {
    uint32_t i = 0;

    // Read characters until buffer is full or termination character is received
    while (i < buffer_size - 1) {  // Leave space for null terminator
        char c = uart_read();

        // Check for termination character
        if (c == '\n' || c == '\0') {
            break;
        }

        // Store character in buffer
        buffer[i++] = c;
    }

    // Null-terminate the string
    buffer[i] = '\0';
}



 void uart_write_string(USART_TypeDef* UART , char *str) {
    while (*str != '\0') {
        uart_write(UART,*str);
        str++;
    }
}

 void uart_write_int(USART_TypeDef* UART ,int num) {
     char buffer[20]; // Assuming a maximum of 20 digits for the integer

     // Convert integer to string
     sprintf(buffer, "%d", num);

     // Call uart_write_string to transmit the string
     uart_write_string(UART,buffer);
 }


 static void uart_set_baudrate (uint32_t prephi_clk,uint32_t baudrate,USART_TypeDef* UART){
	 UART -> BRR = compute_baud_rate(prephi_clk, baudrate);

 }

 static uint16_t compute_baud_rate(uint32_t prephi_clk,uint32_t baudrate){
		 /*  USART_BRR
		     	 Bits 3:0 DIV_Fraction[3:0]
		     	 Bits 15:4 DIV_Mantissa[11:0]
		     	 Bits 31:16 Reserved
		     USARTDIV = clk/(16*baudrate)
		     DIV_Fraction[3:0]: the fraction part of USARTDIV
		     DIV_Mantissa[11:0]: the decimal part of USARTDIV
		     ===>
		     BRR = (clk/(16*baudrate) )*16 - we multiply by 16 to shift it by four bits
		         = (clk/baudrate)  or (clk/baudrate)+(1/2) for rounding
		  */

		 return (prephi_clk+(baudrate/2U))/baudrate;

 	 }

 /////////////////////////////////////////////////////////////////////////////////////////

 volatile uint8_t *rxBuffer;
 volatile uint32_t rxBufferSize;
 volatile uint32_t rxIndex = 0;
 volatile uint8_t rxInProgress = 0;



 void uart2_enableInterrupt(uint32_t priority) {
     NVIC_SetPriority(USART2_IRQn, priority);  // Set priority
     NVIC_EnableIRQ(USART2_IRQn);  // Enable USART2 interrupt in NVIC
 }

 int uart_interrupt_receive(uint8_t *buffer, uint32_t bufferSize) {
     if (rxInProgress) {
         return -1;  // Return error if a reception is already in progress
     }

     rxBuffer = buffer;
     rxBufferSize = bufferSize;
     rxIndex = 0;
     rxInProgress = 1;

     USART2->CR1 |= USART_CR1_RXNEIE;  // Enable RXNE (Receive Not Empty) interrupt

     return 1;  // Return success
 }


 void USART2_IRQHandler(void) {
     if (USART2->SR & USART_SR_RXNE) {  // Check if data is received
         if (rxInProgress && rxIndex < rxBufferSize) {
             rxBuffer[rxIndex++] = USART2->DR;  // Read data from data register
             if (rxIndex >= rxBufferSize) {
                 rxInProgress = 0;  // Reception complete
                 USART2->CR1 &= ~USART_CR1_RXNEIE;  // Disable RXNE interrupt
                 uart_rxlntCallback();  // Call user-defined callback function
             }
         } else {
             volatile uint8_t temp = USART2->DR;  // Read data to clear the RXNE flag
         }
     }
 }









