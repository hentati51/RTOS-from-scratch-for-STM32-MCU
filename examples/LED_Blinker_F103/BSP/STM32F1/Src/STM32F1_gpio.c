/******************************************************************************
 * @file    	STM32F1_gpio.c
 *
 * @Description Drivers for the STM32F1 GPIO
 *
 *
 * @date    	2024-07-26
 * @author  	Achraf Hentati	Embedded Systems Engineer
 * @github  	https://github.com/hentati51
 * @email   	hentati51@gmail.com
 *****************************************************************************/
#include "STM32F1_gpio.h"

void gpio_pin_init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIOMode_TypeDef direction) {
    // Enable clock for the GPIO port
    if (GPIOx == GPIOA) {
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    } else if (GPIOx == GPIOB) {
        RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    } else if (GPIOx == GPIOC) {
        RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    } else if (GPIOx == GPIOD) {
        RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
    } // Add other GPIO ports as needed

    // Configure the pin as input or output
    if (direction == GPIO_MODE_OUTPUT_PP) {
        if (GPIO_Pin < 8) {
            GPIOx->CRL &= ~(0xF << (GPIO_Pin * 4));
            GPIOx->CRL |= (0x2 << (GPIO_Pin * 4)); // Set CNF as 00 and MODE as 10 (output, 2 MHz)
        } else {
            GPIOx->CRH &= ~(0xF << ((GPIO_Pin - 8) * 4));
            GPIOx->CRH |= (0x2 << ((GPIO_Pin - 8) * 4)); // Set CNF as 00 and MODE as 10 (output, 2 MHz)
        }
    } else if (direction == GPIO_MODE_INPUT) {
        if (GPIO_Pin < 8) {
            GPIOx->CRL &= ~(0xF << (GPIO_Pin * 4));
            GPIOx->CRL |= (0x4 << (GPIO_Pin * 4)); // Set CNF as 01 and MODE as 00 (input floating)
        } else {
            GPIOx->CRH &= ~(0xF << ((GPIO_Pin - 8) * 4));
            GPIOx->CRH |= (0x4 << ((GPIO_Pin - 8) * 4)); // Set CNF as 01 and MODE as 00 (input floating)
        }
    }
}

void gpio_write_pin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIOState_TypeDef pin_state) {
    if (pin_state == HIGH) {
        GPIOx->BSRR = (1U << GPIO_Pin); // Set the pin
    } else {
        GPIOx->BRR = (1U << GPIO_Pin); // Reset the pin
    }
}

void gpio_toggle_pin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    GPIOx->ODR ^= (1U << GPIO_Pin);
}
