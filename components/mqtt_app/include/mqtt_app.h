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
void mqtt_reset(void);
void ESP32_LOG_publish(char *log_buffer);
void log_file_clear(char *filename);
void log_write_send(const char *format,...);
void log_read_send(const char *format,...);
void log_process(void);
void spiff_init(void);
#ifdef __cplusplus
}
#endif
