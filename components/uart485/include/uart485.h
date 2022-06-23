#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

void uart485_task(void *arg);


esp_err_t test_app(void);

#ifdef __cplusplus
}
#endif
