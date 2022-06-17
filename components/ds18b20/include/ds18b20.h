#pragma once

#include "esp_err.h"
#include "driver/gpio.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif

esp_err_t test_app(void);
void ReadTemperature(void);
 #define GPIO_IO_DS18B20    14
// #define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_IO_DS18B20)

// #define DS18B20_DQSet()		gpio_set_level(GPIO_IO_DS18B20, 1)		//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)
// #define DS18B20_DQReset()	gpio_set_level(GPIO_IO_DS18B20, 0)		//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)
// #define DS18B20_DQModeOutput()	gpio_set_direction(GPIO_IO_DS18B20,GPIO_MODE_DEF_OUTPUT)	//	Port_SetMode(GPIOB, GPIO_PIN_6, GPIO_MODE_OUTPUT_PP)
// #define DS18B20_DQModeInput()	gpio_set_direction(GPIO_IO_DS18B20,GPIO_MODE_DEF_INPUT)
// #define DS18B20_DIORead()		gpio_get_level(GPIO_IO_DS18B20)	//	HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)

// void DS18B20_Start(void);
// uint8_t DS18B20_ReadByte(void);
#ifdef __cplusplus
}
#endif
