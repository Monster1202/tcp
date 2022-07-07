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
#include "para_list.h"





void sw_key_read(uint8_t io_num);
uint8_t KEY_READ(uint8_t io_num);
void gpio_init(void);

void centralizer_io_out(uint8_t value);
void rotation_io_out(uint8_t value);
void nozzle_io_out(uint8_t value);
void emergency_stop_io_out(uint8_t value);


#ifdef __cplusplus
}
#endif
