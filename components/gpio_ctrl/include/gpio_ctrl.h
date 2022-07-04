#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

//gpio
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

//esp_err_t test_app(void);
#define GPIO_OUTPUT_IO_STRETCH    39
#define GPIO_OUTPUT_IO_DRAW    40
#define GPIO_OUTPUT_IO_ROTATEX    41
#define GPIO_OUTPUT_IO_ROTATEY    42
#define GPIO_OUTPUT_IO_WATER    19
#define GPIO_OUTPUT_IO_BUBBLE    20
#define GPIO_OUTPUT_IO_STOP    21
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_STRETCH) | (1ULL<<GPIO_OUTPUT_IO_DRAW)| (1ULL<<GPIO_OUTPUT_IO_ROTATEX)| (1ULL<<GPIO_OUTPUT_IO_ROTATEY)| (1ULL<<GPIO_OUTPUT_IO_WATER)| (1ULL<<GPIO_OUTPUT_IO_BUBBLE)| (1ULL<<GPIO_OUTPUT_IO_STOP))
// #define GPIO_OUTPUT_IO_0 17
// #define GPIO_OUTPUT_IO_1 18
// #define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))
#define GPIO_INPUT_IO_0     6
#define GPIO_INPUT_IO_1     7
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))
#define ESP_INTR_FLAG_DEFAULT 0
#define KEY_SPEED_LONG 200 //long press debug time(ms)
#define KEY_SPEED_DOUBLE 10 //double press debug time(ms)
//#define KEY_PRESS gpio_get_level(GPIO_INPUT_IO_0)
#define KEY_ONCE 1
#define KEY_TWICE 2
#define KEY_LONG 3


void sw_key_read(uint8_t io_num);
uint8_t KEY_READ(uint8_t io_num);
void gpio_init(void);



#ifdef __cplusplus
}
#endif
