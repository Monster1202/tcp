#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

//esp_err_t test_app(void);
void timer_app(void);
void timer_periodic(void);
// void FTC533_cycle(void);
// void FTC533_process(void);
void timer_FTC533(void);
#ifdef __cplusplus
}
#endif
