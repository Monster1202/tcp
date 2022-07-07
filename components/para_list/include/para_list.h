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

#define GPIO_OUTPUT_IO_STRETCH    14//39
#define GPIO_OUTPUT_IO_DRAW       13//40
#define GPIO_OUTPUT_IO_ROTATEX    12//41
#define GPIO_OUTPUT_IO_ROTATEY    11//42
#define GPIO_OUTPUT_IO_WATER      10//19
#define GPIO_OUTPUT_IO_BUBBLE     9//20
#define GPIO_OUTPUT_IO_7     46
#define GPIO_OUTPUT_IO_8     3

#define GPIO_OUTPUT_LED_1         18
#define GPIO_OUTPUT_LED_2         17
#define GPIO_OUTPUT_LED_3         16
#define GPIO_OUTPUT_LED_4         15
#define GPIO_OUTPUT_LED_5         7
#define GPIO_OUTPUT_LED_6         6

#define GPIO_SYS_LED         0
#define GPIO_BEEP            8

#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_SYS_LED) |(1ULL<<GPIO_BEEP) |(1ULL<<GPIO_OUTPUT_IO_STRETCH) | (1ULL<<GPIO_OUTPUT_IO_DRAW)| (1ULL<<GPIO_OUTPUT_IO_ROTATEX)| (1ULL<<GPIO_OUTPUT_IO_ROTATEY)| (1ULL<<GPIO_OUTPUT_IO_WATER)| (1ULL<<GPIO_OUTPUT_IO_BUBBLE)| (1ULL<<GPIO_OUTPUT_IO_7)| (1ULL<<GPIO_OUTPUT_IO_8)| (1ULL<<GPIO_OUTPUT_LED_1)| (1ULL<<GPIO_OUTPUT_LED_2)| (1ULL<<GPIO_OUTPUT_LED_3)| (1ULL<<GPIO_OUTPUT_LED_4)| (1ULL<<GPIO_OUTPUT_LED_5)| (1ULL<<GPIO_OUTPUT_LED_6))


//#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_LED_1) | (1ULL<<GPIO_OUTPUT_LED_2)| (1ULL<<GPIO_OUTPUT_LED_3)| (1ULL<<GPIO_OUTPUT_LED_4)| (1ULL<<GPIO_OUTPUT_LED_5)| (1ULL<<GPIO_OUTPUT_LED_6))

#define GPIO_INPUT_IO_1     35
#define GPIO_INPUT_IO_2     36
#define GPIO_INPUT_IO_3     37
#define GPIO_INPUT_IO_4     38
#define GPIO_INPUT_IO_5     39
#define GPIO_INPUT_IO_6     40
#define GPIO_INPUT_IO_7     41
#define GPIO_INPUT_IO_STOP     42
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_1)|(1ULL<<GPIO_INPUT_IO_2)|(1ULL<<GPIO_INPUT_IO_3)|(1ULL<<GPIO_INPUT_IO_4)|(1ULL<<GPIO_INPUT_IO_5)|(1ULL<<GPIO_INPUT_IO_6)|(1ULL<<GPIO_INPUT_IO_7)|(1ULL<<GPIO_INPUT_IO_STOP))
#define ESP_INTR_FLAG_DEFAULT 0
#define KEY_SPEED_LONG 200 //long press debug time(ms)
#define KEY_SPEED_DOUBLE 10 //double press debug time(ms)
#define KEY_ONCE 1
#define KEY_TWICE 2
#define KEY_LONG 3




uint16_t parameter_read_pressure(void);
void parameter_write_pressure(uint16_t pressure);
void parameter_write_temperature(double temperature);
double parameter_read_temperature(void);
void para_init(void);
esp_err_t get_chip_id(uint32_t* chip_id);

void parameter_write_centralizer(uint8_t value);
uint8_t parameter_read_centralizer(void);
void parameter_write_rotation(uint8_t value);
uint8_t parameter_read_rotation(void);
void parameter_write_nozzle(uint8_t value);
uint8_t parameter_read_nozzle(void);
void parameter_write_emergency_stop(uint8_t value);
uint8_t parameter_read_emergency_stop(void);

void parameter_write_msg_id(char *str_msgid);
char *parameter_read_msg_id(void);
void parameter_write_timestamp(double timestamp);
double parameter_read_timestamp(void);


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
    uint8_t heater;    
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

void get_parameter(PARAMETER_BRUSH *bursh_t);

#ifdef __cplusplus
}
#endif

#endif

