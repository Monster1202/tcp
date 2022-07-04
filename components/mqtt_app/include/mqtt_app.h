#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

void mqtt_init(void);
void data_process(char *data);
void data_publish(char *data,uint8_t case_pub);

#ifdef __cplusplus
}
#endif
