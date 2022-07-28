#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

void mqtt_init(void);
void data_process(char *data);
void data_publish(char *data,uint8_t case_pub);
void device_states_publish(uint8_t button);

void mqtt_app_start(void);
#ifdef __cplusplus
}
#endif
