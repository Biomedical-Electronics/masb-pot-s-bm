/*
 * stm32main.h
 *
 *  Created on: 15 may. 2021
 *      Author: mariamor
 */

#ifndef INC_COMPONENTS_STM32MAIN_H_
#define INC_COMPONENTS_STM32MAIN_H_

#include "stm32f4xx_hal.h"

struct Handles_S {
	UART_HandleTypeDef *huart;
	ADC_HandleTypeDef *hadc1;
	I2C_HandleTypeDef *hi2c1;
	TIM_HandleTypeDef *htim3;
};

// Prototypes.
void setup(struct Handles_S *handles);
void loop(void);

#endif /* INC_COMPONENTS_STM32MAIN_H_ */
