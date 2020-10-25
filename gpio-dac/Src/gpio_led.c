/*
 * gpio_led.c
 *
 *  Created on: Sep. 14, 2020
 *      Author: ocayci
 */
#include "main.h"
#include "gpio_led.h"

void toggleLED() {
	if (HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin) == GPIO_PIN_RESET) {
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	}
}
