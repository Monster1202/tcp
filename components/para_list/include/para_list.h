//#pragma once
#ifndef __PARA_LIST_H__
#define __PARA_LIST_H__

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_TYPE_BRUSH 
//#define DEVICE_TYPE_BLISTER
//#define DEVICE_TYPE_REMOTE 

#define GPIO_IO_DS18B20    4//9

#define I2C_MASTER_SCL_IO           1      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           2      /*!< GPIO number used for I2C master data  */

uint16_t parameter_read_pressure(void);
void parameter_write_pressure(uint16_t pressure);
void parameter_write_temperature(uint16_t temperature);
uint16_t parameter_read_temperature(void);
void para_init(void);
esp_err_t get_chip_id(uint32_t* chip_id);

void parameter_write_centralizer(uint8_t value);
uint8_t parameter_read_centralizer(void);
void parameter_write_rotation(uint8_t value);
uint8_t parameter_read_rotation(void);
void parameter_write_nozzle(uint8_t value);
uint8_t parameter_read_nozzle(void);

typedef struct
{
    uint32_t uuid;
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
//    uint8_t counter_1s;

}PARAMETER_BRUSH;

typedef struct
{
    uint32_t uuid;
    uint8_t mode;      //command down
    uint8_t status;     //status upload
    uint8_t water;
    uint8_t pressure_alarm;
    uint8_t emergency_stop;
    double timestamp;
    char msg_id[30];
    double temperature;
    uint16_t pressure;
}PARAMETER_BLISTER;

typedef struct
{
    uint32_t uuid;
    uint8_t status;     //status upload
    double timestamp;
    char msg_id[30];
    uint8_t nozzle;      //command down
    uint8_t centralizer;
    uint8_t rotation;
    uint8_t mode;
    uint8_t angle;
}PARAMETER_REMOTE;

#ifdef __cplusplus
}
#endif

#endif

