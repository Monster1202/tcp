//#pragma once
#ifndef __PARA_LIST_H__
#define __PARA_LIST_H__

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

//esp_err_t test_app_1(void);
uint16_t parameter_read_pressure(void);
void parameter_write_pressure(uint16_t pressure);
void parameter_write_temperature(uint16_t temperature);
uint16_t parameter_read_temperature(void);

typedef struct
{
    uint32_t uuid;
    char topic_subscribe[50];
    char topic_publish[50];
    //char switch_name[10];
    uint8_t nozzle;      //command down
    uint8_t centralizer;
    uint8_t rotation;
    uint8_t status;     //status upload
    uint8_t water;
    uint8_t pressure_alarm;
    uint8_t emergency_stop;
    double timestamp;
    char msg_id[30];
    double temperature;
    uint16_t pressure;
    uint8_t counter_1s;

}PARAMETER_BRUSH;



#ifdef __cplusplus
}
#endif

#endif