#include <stdbool.h>
#include <sys/param.h>
#include "esp_check.h"
#include "esp_log.h"
#include "para_list.h"
#include "esp_system.h"
#include <string.h>
//
PARAMETER_BRUSH bursh_para;
PARAMETER_BLISTER blister_para;
PARAMETER_REMOTE remote_para;

esp_err_t get_chip_id(uint32_t* chip_id){
    esp_err_t status = ESP_OK;
    *chip_id = (REG_READ(0x3FF00050) & 0xFF000000) |
                         (REG_READ(0x3ff0005C) & 0xFFFFFF);
    return status;
}


void para_init(void)
{
    uint32_t id;
    get_chip_id(&id);
    printf("SDK version:%s,chip id:%u\n", esp_get_idf_version(),id);
    #ifdef DEVICE_TYPE_BRUSH
        bursh_para.uuid = id;
        bursh_para.nozzle = 0;
        bursh_para.centralizer = 0;
        bursh_para.rotation = 0;
        bursh_para.status = 1;
        bursh_para.water = 0;
        bursh_para.pressure_alarm = 0;
        bursh_para.emergency_stop = 0;
        bursh_para.timestamp = 1654585625000;
        strcpy(bursh_para.msg_id,"msg_id");
        bursh_para.temperature = 0;
    #else
        #ifdef DEVICE_TYPE_BLISTER
            blister_para.uuid = id;
            blister_para.mode = 0;
            blister_para.heater = 0;
            blister_para.status = 1;
            blister_para.water = 0;
            blister_para.pressure_alarm = 0;
            blister_para.liquid_alarm = 0;
            blister_para.emergency_stop = 0;
            blister_para.timestamp = 1654585625000;
            strcpy(blister_para.msg_id,"msg_id");
            blister_para.temperature = 0;
        #else
            remote_para.uuid = id;
            remote_para.nozzle = 0;
            remote_para.centralizer = 0;
            remote_para.rotation = 0;
            remote_para.status = 1;
            remote_para.mode = 0;
            remote_para.angle = 0;
            remote_para.timestamp = 1654585625000;
            strcpy(remote_para.msg_id,"msg_id");
        #endif
    #endif
}

void parameter_write_water(uint8_t value)
{   
    bursh_para.water = value;
    blister_para.water = value;
}

uint8_t parameter_read_water(void)
{
    return bursh_para.water;
}

void parameter_write_pressure_alarm(uint8_t value)
{   
    bursh_para.pressure_alarm = value;
    blister_para.pressure_alarm = value;
}

uint8_t parameter_read_pressure_alarm(void)
{
    return bursh_para.pressure_alarm;
}

void parameter_write_liquid_alarm(uint8_t value)
{
    blister_para.liquid_alarm = value;
}

uint8_t parameter_read_liquid_alarm(void)
{
    return blister_para.liquid_alarm;
}


void get_parameter(PARAMETER_BRUSH *bursh_t)
{
    memcpy(bursh_t,&bursh_para,sizeof(PARAMETER_BRUSH));
}

void get_blister_parameter(PARAMETER_BLISTER *blister_t)
{
    memcpy(blister_t,&blister_para,sizeof(PARAMETER_BLISTER));
}

void get_remote_parameter(PARAMETER_REMOTE *remote_t)
{
    memcpy(remote_t,&remote_para,sizeof(PARAMETER_REMOTE));
}

void parameter_write_version(char *str_version)
{   
    strcpy(bursh_para.version,str_version);
    strcpy(blister_para.version,str_version);
    strcpy(remote_para.version,str_version);
}


// char *parameter_read_version(void)
// {
//     return bursh_para.version;
// }

void parameter_write_msg_id(char *str_msgid)
{   
    strcpy(bursh_para.msg_id,str_msgid);
    strcpy(blister_para.msg_id,str_msgid);
    strcpy(remote_para.msg_id,str_msgid);
}


char *parameter_read_msg_id(void)
{
    return bursh_para.msg_id;
}

void parameter_write_timestamp(double timestamp)
{   
    bursh_para.timestamp = timestamp;
    blister_para.timestamp = timestamp;
    remote_para.timestamp = timestamp;
}

double parameter_read_timestamp(void)
{
    return bursh_para.timestamp;
}

void parameter_write_emergency_stop(uint8_t value)
{   
    
#ifdef DEVICE_TYPE_BRUSH
bursh_para.emergency_stop = value;
#endif
#ifdef DEVICE_TYPE_BLISTER
blister_para.emergency_stop = value;
#endif
#ifdef DEVICE_TYPE_REMOTE
remote_para.emergency_stop = value;
#endif
}

uint8_t parameter_read_emergency_stop(void)
{
#ifdef DEVICE_TYPE_BRUSH
return bursh_para.emergency_stop;
#endif
#ifdef DEVICE_TYPE_BLISTER
return blister_para.emergency_stop;
#endif
#ifdef DEVICE_TYPE_REMOTE
return remote_para.emergency_stop;
#endif
}

void parameter_write_centralizer(uint8_t value)
{   
    bursh_para.centralizer = value;
    remote_para.centralizer = value;
}

uint8_t parameter_read_centralizer(void)
{
#ifdef DEVICE_TYPE_BRUSH
return bursh_para.centralizer;
#endif
#ifdef DEVICE_TYPE_BLISTER
return 0;
#endif
#ifdef DEVICE_TYPE_REMOTE
return remote_para.centralizer;
#endif
}

void parameter_write_rotation(uint8_t value)
{   
    bursh_para.rotation = value;
    remote_para.rotation = value;
}

uint8_t parameter_read_rotation(void)
{
#ifdef DEVICE_TYPE_BRUSH
return bursh_para.rotation;
#endif
#ifdef DEVICE_TYPE_BLISTER
return 0;
#endif
#ifdef DEVICE_TYPE_REMOTE
return remote_para.rotation;
#endif
}

void parameter_write_nozzle(uint8_t value)
{   
    bursh_para.nozzle = value;
    remote_para.nozzle = value;
}

uint8_t parameter_read_nozzle(void)
{
#ifdef DEVICE_TYPE_BRUSH
return bursh_para.nozzle;
#endif
#ifdef DEVICE_TYPE_BLISTER
return 0;
#endif
#ifdef DEVICE_TYPE_REMOTE
return remote_para.nozzle;
#endif
    
}

void parameter_write_pressure(uint16_t pressure)
{   
    bursh_para.pressure = pressure;
}

uint16_t parameter_read_pressure(void)
{
    return bursh_para.pressure;
}

void parameter_write_temperature(double temperature)
{   
    bursh_para.temperature = temperature;
    blister_para.temperature = temperature;
}

double parameter_read_temperature(void)
{
    return bursh_para.temperature;
}
double blister_read_temperature(void)
{
    return blister_para.temperature;
}

void parameter_write_heater(uint8_t value)
{   
    blister_para.heater = value;
    remote_para.heater = value;
}

uint8_t parameter_read_heater(void)
{
    return blister_para.heater;
}

void parameter_write_mode(uint8_t value)
{   
    blister_para.mode = value;
    remote_para.mode = value;
}

uint8_t parameter_read_mode(void)
{
    return blister_para.mode;
}

// esp_err_t test_app_1(void)
// {
//     printf("para_init\n");
//     return ESP_OK;
// }


