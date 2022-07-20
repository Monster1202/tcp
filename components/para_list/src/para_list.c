#include <stdbool.h>
#include <sys/param.h>
#include "esp_check.h"
#include "esp_log.h"
#include "para_list.h"
#include "esp_system.h"
#include <string.h>
//
PARAMETER_BRUSH brush_para;
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
        brush_para.uuid = id;
        brush_para.nozzle = 0;
        brush_para.centralizer = 0;
        brush_para.rotation = 0;
        brush_para.status = 1;
        brush_para.water = 0;
        brush_para.pressure_alarm = 0;
        brush_para.emergency_stop = 0;
        brush_para.timestamp = 1654585625000;
        strcpy(brush_para.msg_id,"msg_id");
        brush_para.temperature = 0;
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
    brush_para.water = value;
    blister_para.water = value;
}

uint8_t parameter_read_water(void)
{
    return brush_para.water;
}

void parameter_write_pressure_alarm(uint8_t value)
{   
    brush_para.pressure_alarm = value;
    blister_para.pressure_alarm = value;
}

uint8_t parameter_read_pressure_alarm(void)
{
    return brush_para.pressure_alarm;
}

void parameter_write_liquid_alarm(uint8_t value)
{
    blister_para.liquid_alarm = value;
}

uint8_t parameter_read_liquid_alarm(void)
{
    return blister_para.liquid_alarm;
}


void get_parameter(PARAMETER_BRUSH *brush_t)
{
    memcpy(brush_t,&brush_para,sizeof(PARAMETER_BRUSH));
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
    strcpy(brush_para.version,str_version);
    strcpy(blister_para.version,str_version);
    strcpy(remote_para.version,str_version);
}


// char *parameter_read_version(void)
// {
//     return brush_para.version;
// }

void parameter_write_msg_id(char *str_msgid)
{   
    strcpy(brush_para.msg_id,str_msgid);
    strcpy(blister_para.msg_id,str_msgid);
    strcpy(remote_para.msg_id,str_msgid);
}


char *parameter_read_msg_id(void)
{
    return brush_para.msg_id;
}

void parameter_write_timestamp(double timestamp)
{   
    brush_para.timestamp = timestamp;
    blister_para.timestamp = timestamp;
    remote_para.timestamp = timestamp;
}

double parameter_read_timestamp(void)
{
    return brush_para.timestamp;
}

void parameter_write_emergency_stop(uint8_t value)
{   
    
#ifdef DEVICE_TYPE_BRUSH
brush_para.emergency_stop = value;
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
return brush_para.emergency_stop;
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
    brush_para.centralizer = value;
    remote_para.centralizer = value;
}

uint8_t parameter_read_centralizer(void)
{
#ifdef DEVICE_TYPE_BRUSH
return brush_para.centralizer;
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
    brush_para.rotation = value;
    remote_para.rotation = value;
}

uint8_t parameter_read_rotation(void)
{
#ifdef DEVICE_TYPE_BRUSH
return brush_para.rotation;
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
    brush_para.nozzle = value;
    remote_para.nozzle = value;
}

uint8_t parameter_read_nozzle(void)
{
#ifdef DEVICE_TYPE_BRUSH
return brush_para.nozzle;
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
    brush_para.pressure = pressure;
}

uint16_t parameter_read_pressure(void)
{
    return brush_para.pressure;
}

void parameter_write_temperature(double temperature)
{   
    brush_para.temperature = temperature;
    blister_para.temperature = temperature;
}

double parameter_read_temperature(void)
{
    return brush_para.temperature;
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

void parameter_write_rssi(int8_t value)
{  
    brush_para.rssi = value; 
    blister_para.rssi = value;
    remote_para.rssi = value;
}
// esp_err_t test_app_1(void)
// {
//     printf("para_init\n");
//     return ESP_OK;
// }


