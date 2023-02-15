#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

void timer_app(void);
void timer_periodic(void);

void timer_FTC533(void);
void timer_heater_init(void);
#ifdef __cplusplus
}
#endif
