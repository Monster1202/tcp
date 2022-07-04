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
            blister_para.nozzle = 0;
            blister_para.centralizer = 0;
            blister_para.rotation = 0;
            blister_para.status = 1;
            blister_para.water = 0;
            blister_para.pressure_alarm = 0;
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



void parameter_write_pressure(uint16_t pressure)
{   
    bursh_para.pressure = pressure;
}

uint16_t parameter_read_pressure(void)
{
    return bursh_para.pressure;
}

void parameter_write_temperature(uint16_t temperature)
{   
    bursh_para.temperature = temperature;
}

uint16_t parameter_read_temperature(void)
{
    return bursh_para.temperature;
}

// esp_err_t test_app_1(void)
// {
//     printf("para_init\n");
//     return ESP_OK;
// }


