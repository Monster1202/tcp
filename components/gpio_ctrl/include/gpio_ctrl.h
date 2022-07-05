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
#define GPIO_OUTPUT_IO_STRETCH    14//39
#define GPIO_OUTPUT_IO_DRAW       13//40
#define GPIO_OUTPUT_IO_ROTATEX    12//41
#define GPIO_OUTPUT_IO_ROTATEY    11//42
#define GPIO_OUTPUT_IO_WATER      10//19
#define GPIO_OUTPUT_IO_BUBBLE     9//20
#define GPIO_OUTPUT_IO_7     46
#define GPIO_OUTPUT_IO_8     3
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_STRETCH) | (1ULL<<GPIO_OUTPUT_IO_DRAW)| (1ULL<<GPIO_OUTPUT_IO_ROTATEX)| (1ULL<<GPIO_OUTPUT_IO_ROTATEY)| (1ULL<<GPIO_OUTPUT_IO_WATER)| (1ULL<<GPIO_OUTPUT_IO_BUBBLE)| (1ULL<<GPIO_OUTPUT_IO_7)| (1ULL<<GPIO_OUTPUT_IO_8))

#define GPIO_OUTPUT_LED_1         18
#define GPIO_OUTPUT_LED_2         17
#define GPIO_OUTPUT_LED_3         16
#define GPIO_OUTPUT_LED_4         15
#define GPIO_OUTPUT_LED_5         7
#define GPIO_OUTPUT_LED_6         6
//#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_LED_1) | (1ULL<<GPIO_OUTPUT_LED_2)| (1ULL<<GPIO_OUTPUT_LED_3)| (1ULL<<GPIO_OUTPUT_LED_4)| (1ULL<<GPIO_OUTPUT_LED_5)| (1ULL<<GPIO_OUTPUT_LED_6))

#define GPIO_INPUT_IO_1  35
#define GPIO_INPUT_IO_2     36
#define GPIO_INPUT_IO_3     37
#define GPIO_INPUT_IO_4     38
#define GPIO_INPUT_IO_5     39
#define GPIO_INPUT_IO_6     40
#define GPIO_INPUT_IO_7     41
#define GPIO_INPUT_IO_STOP     42
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_1)|(1ULL<<GPIO_INPUT_IO_2)|(1ULL<<GPIO_INPUT_IO_3)|(1ULL<<GPIO_INPUT_IO_4)|(1ULL<<GPIO_INPUT_IO_5)|(1ULL<<GPIO_INPUT_IO_6)|(1ULL<<GPIO_INPUT_IO_7)|(1ULL<<GPIO_INPUT_IO_STOP))
#define ESP_INTR_FLAG_DEFAULT 0
#define KEY_SPEED_LONG 200 //long press debug time(ms)
#define KEY_SPEED_DOUBLE 10 //double press debug time(ms)
#define KEY_ONCE 1
#define KEY_TWICE 2
#define KEY_LONG 3


void sw_key_read(uint8_t io_num);
uint8_t KEY_READ(uint8_t io_num);
void gpio_init(void);



#ifdef __cplusplus
}
#endif
