#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

void uart485_task(void *arg);
void heater_water_module_test(uint8_t send_case);

//esp_err_t test_app(void);

#ifdef __cplusplus
}
#endif
