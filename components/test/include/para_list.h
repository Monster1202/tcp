#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t test_app_1(void);

typedef struct
{
    char topic_subscribe[50];
    char topic_publish[50];
    uint8_t nozzle;      //command down
    uint8_t centralizer;
    uint8_t rotation;
    uint8_t status;     //status upload
    uint8_t water;
    uint8_t pressure_alarm;

}PARAMETER_BRUSH;



#ifdef __cplusplus
}
#endif
