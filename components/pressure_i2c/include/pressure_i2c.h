#pragma once


#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

void pressure_read(void* arg);
//esp_err_t test_app(void);
void pressure_i2c(void);
#ifdef __cplusplus
}
#endif
